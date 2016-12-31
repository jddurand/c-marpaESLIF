package org.parser.marpa;

import java.nio.ByteBuffer;

public class ESLIFRecognizer {
	private ESLIFGrammar             eslifGrammar                 = null;
	private ESLIFRecognizerInterface eslifRecognizerInterface     = null;
	private ByteBuffer               marpaESLIFRecognizerp        = null;
	private ByteBuffer               marpaESLIFRecognizerContextp = null;
	private native void              jniNew(ESLIFGrammar eslifGrammar);
	private native void              jniFree();

	public ESLIFRecognizer(ESLIFGrammar eslifGrammar, ESLIFRecognizerInterface eslifRecognizerInterface) {
		if (eslifGrammar == null) {
			throw new IllegalArgumentException("eslifGrammar must not be null");
		}
		if (eslifRecognizerInterface == null) {
			throw new IllegalArgumentException("eslifRecognizerInterface must not be null");
		}
		setEslifGrammar(eslifGrammar);
		setEslifRecognizerInterface(eslifRecognizerInterface);
		jniNew(eslifGrammar);
	}
	
	public void free() {
		jniFree();
	}

	/*
	 * ********************************************
	 * Private methods - used by the JNI
	 * ********************************************
	 */
	private ESLIFRecognizerInterface getEslifRecognizerInterface() {
		return eslifRecognizerInterface;
	}
	private void setEslifRecognizerInterface(ESLIFRecognizerInterface eslifRecognizerInterface) {
		this.eslifRecognizerInterface = eslifRecognizerInterface;
	}
	protected ESLIFLoggerInterface getLoggerInterface() {
		return (eslifGrammar != null) ? eslifGrammar.getLoggerInterface() : null;
	}
	private ESLIFGrammar getEslifGrammar() {
		return eslifGrammar;
	}
	private void setEslifGrammar(ESLIFGrammar eslifGrammar) {
		this.eslifGrammar = eslifGrammar;
	}
	private ByteBuffer getMarpaESLIFRecognizerp() {
		return marpaESLIFRecognizerp;
	}
	private void setMarpaESLIFRecognizerp(ByteBuffer marpaESLIFRecognizerp) {
		this.marpaESLIFRecognizerp = marpaESLIFRecognizerp;
	}
	private ByteBuffer getMarpaESLIFRecognizerContextp() {
		return marpaESLIFRecognizerContextp;
	}
	private void setMarpaESLIFRecognizerContextp(ByteBuffer marpaESLIFRecognizerContextp) {
		this.marpaESLIFRecognizerContextp = marpaESLIFRecognizerContextp;
	}
}
