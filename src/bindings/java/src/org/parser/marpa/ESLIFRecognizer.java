package org.parser.marpa;

import java.nio.ByteBuffer;

public class ESLIFRecognizer {
	private ESLIFGrammar             eslifGrammar                 = null;
	private ESLIFRecognizerInterface eslifRecognizerInterface     = null;
	private ByteBuffer               marpaESLIFRecognizerp        = null;
	private ByteBuffer               marpaESLIFRecognizerContextp = null;
	private boolean                  canContinue    = false;
	private boolean                  exhausted      = false;
	private native void              jniNew(ESLIFGrammar eslifGrammar);
	private native void              jniFree();
	private native void              jniScan(boolean initialEvents);
	private native void              jniResume();
	private native ESLIFEvent[]      jniEvent();
	private native void              jniEventOnOff(String symbol, ESLIFEventType[] eventTypes, boolean onOff);
	private native void              jniLexemeAlternativeLength(int length);
	private native void              jniLexemeAlternative(String name);
	private native void              jniLexemeComplete();
	private native void              jniLexemeRead(String name, int length);
	private native String[]          jniLexemeExpected();
	private native void              jniEof();
	private native boolean           jniIsEof();
	private native void              jniRead();

	public ESLIFRecognizer(ESLIFGrammar eslifGrammar, ESLIFRecognizerInterface eslifRecognizerInterface) throws ESLIFException {
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
	
	public synchronized void free() throws ESLIFException {
		jniFree();
	}

	public synchronized boolean isCanContinue() throws ESLIFException {
		return canContinue;
	}

	public synchronized boolean isExhausted() throws ESLIFException {
		return exhausted;
	}
	
	public synchronized void scan(boolean initialEvents) throws ESLIFException {
		jniScan(initialEvents);
	}

	public synchronized void resume() throws ESLIFException {
		jniResume();
	}
	
	public synchronized ESLIFEvent[] events() throws ESLIFException {
		return jniEvent();
	}

	public synchronized void eventOnOff(String symbol, ESLIFEventType[] eventTypes, boolean onOff) {
		jniEventOnOff(symbol, eventTypes, onOff);
	}

	public synchronized void lexemeAlternativeLength(int length) throws ESLIFException {
		jniLexemeAlternativeLength(length);
	}
	
	public synchronized void lexemeAlternative(String name) throws ESLIFException {
		jniLexemeAlternative(name);
	}
	
	public synchronized void lexemeComplete() throws ESLIFException {
		jniLexemeComplete();
	}
	
	public synchronized void lexemeRead(String name, int length) throws ESLIFException {
		jniLexemeRead(name, length);
	}
	
	public synchronized String[] lexemeExpected() throws ESLIFException {
		return jniLexemeExpected();
	}
	
	public synchronized void eof() throws ESLIFException {
		jniEof();
	}

	public synchronized boolean isEof() throws ESLIFException {
		return jniIsEof();
	}

	public synchronized void read() throws ESLIFException {
		jniRead();
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
	private void setCanContinue(boolean canContinue) {
		this.canContinue = canContinue;
	}
	private void setExhausted(boolean exhausted) {
		this.exhausted = exhausted;
	}
}
