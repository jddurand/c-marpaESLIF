package org.parser.marpa;

import java.nio.ByteBuffer;

public class ESLIFValue  {
	private ESLIFRecognizer      eslifRecognizer         = null;
	private ByteBuffer           marpaESLIFValuep        = null;
	private ByteBuffer           marpaESLIFValueContextp = null;
	private ESLIFValueInterface  eslifValueInterface     = null;
	private native void          jniNew(ESLIFRecognizer recognizer);
	private native void          jniFree();
	
	public ESLIFValue(ESLIFRecognizer recognizer, ESLIFValueInterface eslifValueInterface) {
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

	public void free() {
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
