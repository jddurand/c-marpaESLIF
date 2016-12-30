package org.parser.marpa;

import java.nio.ByteBuffer;

public class ESLIFRecognizer implements ESLIFRecognizerInterface {
	private ESLIFGrammar         eslifGrammar          = null;
	private ByteBuffer           marpaESLIFRecognizerp = null;
	private ESLIFReaderInterface readerInterface       = null;
	private boolean              withDisableThreshold  = false;
	private boolean              withExhaustion        = false;
	private boolean              withNewline           = false;
	private native void          jniNew(ESLIFGrammar eslifGrammar);
	private native void          jniFree();

	public ESLIFRecognizer(ESLIFGrammar eslifGrammar, ESLIFReaderInterface reader) {
		if (eslifGrammar == null) {
			throw new IllegalArgumentException("eslifGrammar must not be null");
		}
		if (reader == null) {
			throw new IllegalArgumentException("reader must not be null");
		}
		setEslifGrammar(eslifGrammar);
		setReaderInterface(reader);
		jniNew(eslifGrammar);
	}
	
	public void free() {
		jniFree();
	}

	private ESLIFReaderInterface getReaderInterface() {
		return readerInterface;
	}
	private void setReaderInterface(ESLIFReaderInterface reader) {
		this.readerInterface = reader;
	}
	public short read() {
		return getReaderInterface().read();
	}
	public boolean isEof() {
		return getReaderInterface().isEof();
	}
	public boolean isCharacterStream() {
		return getReaderInterface().isCharacterStream();
	}
	public String encoding() {
		return getReaderInterface().encoding();
	}
	public byte[] data() {
		return getReaderInterface().data();
	}
	public boolean isWithDisableThreshold() {
		return withDisableThreshold;
	}
	protected void setWithDisableThreshold(boolean withDisableThreshold) {
		this.withDisableThreshold = withDisableThreshold;
	}
	public boolean isWithExhaustion() {
		return withExhaustion;
	}
	protected void setWithExhaustion(boolean withExhaustion) {
		this.withExhaustion = withExhaustion;
	}
	public boolean isWithNewline() {
		return withNewline;
	}
	protected void setWithNewline(boolean withNewline) {
		this.withNewline = withNewline;
	}
	/*
	 * ********************************************
	 * Private methods - used by the JNI
	 * ********************************************
	 */
	private ESLIFLoggerInterface getLoggerInterface() {
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
}
