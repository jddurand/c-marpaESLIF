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
	private native void              jniResume(int deltaLength);
	private native ESLIFEvent[]      jniEvent();
	private native void              jniEventOnOff(String symbol, ESLIFEventType[] eventTypes, boolean onOff);
	private native void              jniLexemeAlternative(String name, Object object, int grammarLength);
	private native void              jniLexemeComplete(int length);
	private native void              jniLexemeRead(String name, Object object, int grammarLength, int length);
	private native boolean           jniLexemeTry(String name);
	private native String[]          jniLexemeExpected();
	private native byte[]            jniLexemeLastPause(String lexeme);
	private native boolean           jniIsEof();
	private native byte[]            jniRead();
	private native byte[]            jniInput();
	private native void              jniProgressLog(int start, int end, ESLIFLoggerLevel level);
	private native int               jniLastCompletedOffset(String name);
	private native int               jniLastCompletedLength(String name);

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

	public synchronized void resume(int deltaLength) throws ESLIFException {
		jniResume(deltaLength);
	}
	
	public synchronized ESLIFEvent[] events() throws ESLIFException {
		return jniEvent();
	}

	public synchronized void eventOnOff(String symbol, ESLIFEventType[] eventTypes, boolean onOff) {
		jniEventOnOff(symbol, eventTypes, onOff);
	}

	public synchronized void lexemeAlternative(String name, Object object, int grammarLength) throws ESLIFException {
		jniLexemeAlternative(name, object, grammarLength);
	}
	
	public synchronized void lexemeComplete(int length) throws ESLIFException {
		jniLexemeComplete(length);
	}
	
	public synchronized void lexemeRead(String name, Object object, int grammarLength, int length) throws ESLIFException {
		jniLexemeRead(name, object, grammarLength, length);
	}
	
	public synchronized boolean lexemeTry(String name) throws ESLIFException {
		return jniLexemeTry(name);
	}
	
	public synchronized String[] lexemeExpected() throws ESLIFException {
		return jniLexemeExpected();
	}
	
	public synchronized byte[] lexemeLastPause(String lexeme) throws ESLIFException {
		return jniLexemeLastPause(lexeme);
	}

	public synchronized boolean isEof() throws ESLIFException {
		return jniIsEof();
	}

	public synchronized byte[] read() throws ESLIFException {
		return jniRead();
	}

	public synchronized byte[] input() throws ESLIFException {
		return jniInput();
	}

	public synchronized void progressLog(int start, int end, ESLIFLoggerLevel level) throws ESLIFException {
		jniProgressLog(start, end, level);
	}
	
	public synchronized int lastCompletedOffset(String name) throws ESLIFException {
		return jniLastCompletedOffset(name);
	}

	public synchronized int lastCompletedLength(String name) throws ESLIFException {
		return jniLastCompletedLength(name);
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
