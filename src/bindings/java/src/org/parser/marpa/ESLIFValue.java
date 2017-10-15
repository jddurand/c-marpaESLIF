package org.parser.marpa;

import java.nio.ByteBuffer;

/**
 * ESLIFValue is the step after getting an ESLIF recognizer. Caller must dispose resources in reverse of object creation:
 * 
 * <pre>
 * ESLIF eslif = new ESLIF(...)
 * ESLIFGrammar eslifGrammar = new ESLIFGrammar(...);
 * ESLIFRecognizer eslifRecognizer = new ESLIFRecognizer(...);
 * ESLIFValue eslifValue = new ESLIFValue(...);
 * ...
 * eslifValue.free();
 * eslifRecognizer.free();
 * eslifGrammar.free();
 * eslif.free()
 * </pre>
 * 
 * A typical recognizer usage is:
 * <pre>
 * while (eslifValue.value() {@code >} 0) &#123;
 *   Object result = eslifValueInterface.getResult();
 * &#125;
 * </pre>
 * 
 * During valuation (methods mentioned using the "action" adverb) and in the setResult method, it is very important
 * to be able to distinguish the arguments that come directly from the input v.s. those coming from user-defined
 * actions:
 * <ul>
 *   <li>An argument that come directly from the input is guaranteed to be a instance of ByteBuffer. User should not
 *   mind about the lifetime of the byte buffer content in methods called during valuation. On the other hand, in the
 *   setResult method, an explicit copy and/or conversion must be done because lifetime of the byte buffer will expire
 *   just after this call.
 *   <li>Any other argument is seen as an "Object" from the interface point of view, up to the developer to be able to
 *   distinguish eventual ambiguity if the output of one of its action would be also a ByteBuffer, that is discouraged.
 * </ul>
 */
public class ESLIFValue {
	private ESLIFRecognizer      eslifRecognizer         = null;
	private ByteBuffer           marpaESLIFValuep        = null;
	private ByteBuffer           marpaESLIFValueContextp = null;
	private ESLIFValueInterface  eslifValueInterface     = null;
	private native void          jniNew(ESLIFRecognizer recognizer) throws ESLIFException;
	private native void          jniFree() throws ESLIFException;
	private native boolean       jniValue() throws ESLIFException;
	
	/**
	 * 
	 * @param recognizer the recognizer instance
	 * @param eslifValueInterface the value interface
	 * @throws ESLIFException when the interface fails
	 */
	public ESLIFValue(ESLIFRecognizer recognizer, ESLIFValueInterface eslifValueInterface) throws ESLIFException {
		if (recognizer == null) {
			throw new IllegalAccessError("recognizer must not be null");
		}
		if (eslifValueInterface == null) {
			throw new IllegalAccessError("eslifValueInterface must not be null");
		}
		setEslifRecognizer(recognizer);
		setEslifValueInterface(eslifValueInterface);
		jniNew(recognizer);
	}

	/**
	 * Returns true when there is a value, false otherwise. Ambiguous parsing typically return more than one value.
	 * 
	 * @return a boolean indicating if there a value to retrieve via the valueInterface's getResult() method
	 * @throws ESLIFException when the interface fails
	 */
	public boolean value() throws ESLIFException {
		return jniValue();
	}
	/**
	 * Dispose of valuation object resources.
	 * 
	 * @throws ESLIFException when the interface fails
	 */
	public synchronized void free() throws ESLIFException {
		jniFree();
	}
	/*
	 * ********************************************
	 * Private methods - used by the JNI
	 * ********************************************
	 */
	private ESLIFLoggerInterface getLoggerInterface() {
		return (eslifRecognizer != null) ? eslifRecognizer.getLoggerInterface() : null;
	}
	private ESLIFRecognizer getEslifRecognizer() {
		return eslifRecognizer;
	}
	private void setEslifRecognizer(ESLIFRecognizer eslifRecognizer) {
		this.eslifRecognizer = eslifRecognizer;
	}
	private ByteBuffer getMarpaESLIFValuep() {
		return marpaESLIFValuep;
	}
	private void setMarpaESLIFValuep(ByteBuffer marpaESLIFValuep) {
		this.marpaESLIFValuep = marpaESLIFValuep;
	}
	private ByteBuffer getMarpaESLIFValueContextp() {
		return marpaESLIFValueContextp;
	}
	private void setMarpaESLIFValueContextp(ByteBuffer marpaESLIFValueContextp) {
		this.marpaESLIFValueContextp = marpaESLIFValueContextp;
	}
	private ESLIFValueInterface getEslifValueInterface() {
		return eslifValueInterface;
	}
	private void setEslifValueInterface(ESLIFValueInterface eslifValueInterface) {
		this.eslifValueInterface = eslifValueInterface;
	}
}
