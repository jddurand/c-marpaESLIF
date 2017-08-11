package org.parser.marpa;

import java.nio.ByteBuffer;

/**
 * ESLIFRecognizer is the step after getting an ESLIF grammar. As many recognizers as wanted
 * can be created using the same ESLIF grammar, dispose of resources must follow the reverse order
 * of creation, i.e.:
 * 
 * <pre>
 * ESLIF eslif = new ESLIF(...)
 * ESLIFGrammar eslifGrammar = new ESLIFGrammar(...);
 * ESLIFRecognizer eslifRecognizer = new ESLIFRecognizer(...);
 * ...
 * eslifRecognizer.free();
 * eslifGrammar.free();
 * eslif.free()
 * </pre>
 * 
 * A typical recognizer usage is:
 * <pre>
 * eslifRecognizer.scan();
 * // Check for events
 * while (eslifRecognizer.isCanContinue()) &#123;
 *   eslifRecognizer.resume();
 *   // Check for events
 * &#125;
 * </pre>
 * 
 * The recognizer interface is used to read chunks of data, that the internal recognizer will keep in its internal
 * buffers, until it is consumed. The recognizer internal buffer may not be an exact duplicate of the external data
 * that was read: in case of a character stream, the external data is systematically converted to UTF-8 sequence of bytes.
 * If the user is pushing alternatives, he will have to know how many bytes this represent: native number of bytes
 * in case of a binary stream, number of bytes in the UTF-8 encoding in case of a character stream.
 */
public class ESLIFRecognizer {
	private ESLIFGrammar             eslifGrammar                 = null;
	private ESLIFRecognizerInterface eslifRecognizerInterface     = null;
	private ByteBuffer               marpaESLIFRecognizerp        = null;
	private ByteBuffer               marpaESLIFRecognizerContextp = null;
	private boolean                  canContinue    = false;
	private boolean                  exhausted      = false;
	private native void              jniNew(ESLIFGrammar eslifGrammar) throws ESLIFException;
	private native void              jniFree() throws ESLIFException;
	private native boolean			 jniScan(boolean initialEvents) throws ESLIFException;
	private native boolean 			 jniResume(int deltaLength) throws ESLIFException;
	private native ESLIFEvent[]      jniEvent() throws ESLIFException;
	private native void              jniEventOnOff(String symbol, ESLIFEventType[] eventTypes, boolean onOff) throws ESLIFException;
	private native boolean       	 jniLexemeAlternative(String name, Object object, int grammarLength) throws ESLIFException;
	private native boolean			 jniLexemeComplete(int length) throws ESLIFException;
	private native boolean           jniLexemeRead(String name, Object object, int grammarLength, int length) throws ESLIFException;
	private native boolean           jniLexemeTry(String name) throws ESLIFException;
	private native boolean           jniDiscardTry() throws ESLIFException;
	private native String[]          jniLexemeExpected() throws ESLIFException;
	private native byte[]            jniLexemeLastPause(String lexeme) throws ESLIFException;
	private native byte[]            jniLexemeLastTry(String lexeme) throws ESLIFException;
	private native byte[]            jniDiscardLastTry() throws ESLIFException;
	private native boolean           jniIsEof() throws ESLIFException;
	private native boolean           jniRead() throws ESLIFException;
	private native byte[]            jniInput() throws ESLIFException;
	private native void              jniProgressLog(int start, int end, ESLIFLoggerLevel level) throws ESLIFException;
	private native long              jniLastCompletedOffset(String name) throws ESLIFException;
	private native long              jniLastCompletedLength(String name) throws ESLIFException;
	private native long              jniLine() throws ESLIFException;
	private native long              jniColumn() throws ESLIFException;

	/**
	 * 
	 * @param eslifGrammar the ESLIFGrammar instance
	 * @param eslifRecognizerInterface the recognizer interface
	 * @throws ESLIFException if the interface failed
	 */
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
	
	/**
	 * Dispose of a recognizer resources
	 * 
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized void free() throws ESLIFException {
		jniFree();
	}

	/**
	 * 
	 * @return a boolean indicating if recognizing can continue
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean isCanContinue() throws ESLIFException {
		return canContinue;
	}

	/**
	 * 
	 * @return a boolean indicating if parsing is exhausted, always set even without exhaustion event
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean isExhausted() throws ESLIFException {
		return exhausted;
	}
	
	/**
	 * Start a recognizer scanning. This call is allowed once in recognizer lifetime.
	 * This method can generate events.
	 * Initial events are those that are happening at the very first step, and can be only prediction events.
	 * This may be annoying, and most applications do not want that - but some can use this to get the control
	 * before the first data read.
	 * 
	 * @param initialEvents a boolean indicating if initial events should happen.
	 * @return a boolean indicating if the call was successful or not
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean scan(boolean initialEvents) throws ESLIFException {
		return jniScan(initialEvents);
	}

	/**
	 * This method tell the recognizer to continue.
	 * Events can be generate after resume completion.
	 * 
	 * @param deltaLength the number of bytes to skip before resume goes on, must be positive or greater than 0.  In case of a character stream, user will have to compute the number of bytes as if the input was in the UTF-8 encoding.
	 * @return a boolean indicating if the call was successful or not
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean resume(int deltaLength) throws ESLIFException {
		return jniResume(deltaLength);
	}
	
	/**
	 * When control is given back to the end-user, he can always ask what are the current events.
	 * 
	 * @return the array of events, eventually empty if there is none
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized ESLIFEvent[] events() throws ESLIFException {
		return jniEvent();
	}

	/**
	 * Events can be switched on or off. For performance reasons, if you know that you do not need an event, it can be a good idea to switch if off
	 * 
	 * @param symbol the symbol name to which the event is associated
	 * @param eventTypes an array of event types
	 * @param onOff the boolean indicating the state of the event types.
	 * @throws ESLIFException if the interface failed
	 * 
	 * Note that trying to change the state of an event that was not pre-declared in the grammar is a no-op.
	 */
	public synchronized void eventOnOff(String symbol, ESLIFEventType[] eventTypes, boolean onOff) throws ESLIFException {
		jniEventOnOff(symbol, eventTypes, onOff);
	}

	/**
	 * A lexeme is a terminal in the legacy parsing terminology. The lexeme word mean that in the grammar it is associated to a sub-grammar.
	 * Pushing an alternative mean that the end-user is instructing the recognizer that, at this precise moment of lexing, there is a given
	 * lexeme associated to the name parameter, with a given opaque value.  
	 * Grammar length is usually one, i.e. one lexeme (which is a symbol in the grammar) correspond to one token.
	 * Nevertheless it is possible to say that an alternative span over more than one symbol.
	 * 
	 * @param name the name of the lexeme
	 * @param object the object that will represent the value of this lexeme at this parsing stage
	 * @param grammarLength the length in the grammar, must be greater or equal than one
	 * @return a boolean indicating if the call was successful or not
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean lexemeAlternative(String name, Object object, int grammarLength) throws ESLIFException {
		return jniLexemeAlternative(name, object, grammarLength);
	}
	
	/**
	 * Short-hand version of lexemeAlternative, where grammar length default to the recommended value of 1.
	 * 
	 * @param name the name of the lexeme
	 * @param object the object that will represent the value of this lexeme at this parsing stage
	 * @return a boolean indicating if the call was successful or not
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean lexemeAlternative(String name, Object object) throws ESLIFException {
		return jniLexemeAlternative(name, object, 1);
	}
	
	/**
	 * Say the recognizer that alternatives are complete at this precise moment of parsing, and that the recognizer must move
	 * forward by <code>length</code> bytes, which can be zero (end-user's responsibility).
	 * This method can generate events.
	 * 
	 * @param length the number of bytes consumed by the latest set of alternatives
	 * @return a boolean indicating if the call was successful or not
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean lexemeComplete(int length) throws ESLIFException {
		return jniLexemeComplete(length);
	}
	
	/**
	 * A short-hand version of lexemeAlternative() plus lexemeComplete() methods, whith the exact same meaning of all parameters.
	 * This method can generate events.
	 * 
	 * @param name the name of the lexeme
	 * @param object the object that will represent the value of this lexeme at this parsing stage
	 * @param length the number of bytes consumed by the latest set of alternatives
	 * @param grammarLength the length in the grammar, must be greater or equal than one
	 * @return a boolean indicating if the call was successful or not
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean lexemeRead(String name, Object object, int length, int grammarLength) throws ESLIFException {
		return jniLexemeRead(name, object, length, grammarLength);
	}
	
	/**
	 * A short-hand version of lexemeAlternative() followed by lexemeComplete() where grammarLength default to the recommended value of 1.
	 * This method can generate events.
	 * 
	 * @param name the name of the lexeme
	 * @param object the object that will represent the value of this lexeme at this parsing stage
	 * @param length the number of bytes consumed by the latest set of alternatives
	 * @return a boolean indicating if the call was successful or not
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean lexemeRead(String name, Object object, int length) throws ESLIFException {
		return jniLexemeRead(name, object, length, 1);
	}
	
	/**
	 * The end-user can ask the recognizer if a lexeme may match
	 * 
	 * @param name the name of the lexeme
	 * @return a boolean indicating if the lexeme is recognized
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean lexemeTry(String name) throws ESLIFException {
		return jniLexemeTry(name);
	}
	
	/**
	 * Ask the recognizer if :discard rule may match
	 * 
	 * @return a boolean indicating if :discard is recognized
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean discardTry() throws ESLIFException {
		return jniDiscardTry();
	}
	
	/**
	 * Ask the recognizer a list of expected lexemes
	 * 
	 * @return an array of lexeme names, may be null if there is none
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized String[] lexemeExpected() throws ESLIFException {
		return jniLexemeExpected();
	}
	
	/**
	 * Ask the recognizer the end-user data associated to last lexeme "pause after" event.
	 * A "pause after" event is the when the recognizer was responsible of lexeme recognition, after a call to scan() or resume() methods.
	 * This data will be an exact copy of the last bytes that matched for a given lexeme, where data is the internal representation of end-user data,
	 * meaning that it may be UTF-8 sequence of bytes in case of character stream, and always an UTF-8 seauence of bytes in case of end-user data
	 * (end-user data representation is always the toString() result converted to UTF8).
	 * 
	 * @param lexeme the lexeme name
	 * @return an array of bytes
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized byte[] lexemeLastPause(String lexeme) throws ESLIFException {
		return jniLexemeLastPause(lexeme);
	}

	/**
	 * Ask the recognizer the end-user data associated to last successful lexeme try.
	 * This data will be an exact copy of the last bytes that matched for a given lexeme, where data is the internal representation of end-user data,
	 * meaning that it may be UTF-8 sequence of bytes in case of character stream, and always an UTF-8 seauence of bytes in case of end-user data
	 * (end-user data representation is always the toString() result converted to UTF8).
	 * 
	 * @param lexeme the lexeme name
	 * @return an array of bytes
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized byte[] lexemeLastTry(String lexeme) throws ESLIFException {
		return jniLexemeLastTry(lexeme);
	}

	/**
	 * Ask the recognizer the end-user data associated to last successful discard try.
	 * This data will be an exact copy of the last bytes that matched for a given lexeme, where data is the internal representation of end-user data,
	 * meaning that it may be UTF-8 sequence of bytes in case of character stream, and always an UTF-8 seauence of bytes in case of end-user data
	 * (end-user data representation is always the toString() result converted to UTF8).
	 * 
	 * @return an array of bytes
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized byte[] discardLastTry() throws ESLIFException {
		return jniDiscardLastTry();
	}

	/**
	 * This method is similar to the isEof()'s recognizer interface. Except that this is asking the question directly to the recognizer's
	 * internal state, that maintains a copy of this flag.
	 * 
	 * @return a boolean indicating of end-of-user-data is reached
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean isEof() throws ESLIFException {
		return jniIsEof();
	}

	/**
	 * Forces the recognizer to read more data. Usually, the recognizer interface is called automatically whenever needed
	 * 
	 * @return a boolean value indicating success or not
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean read() throws ESLIFException {
		return jniRead();
	}

	/**
	 * Get a copy of the current internal recognizer buffer, starting at the exact byte where resume() would start.
	 * A null output does not mean there is an error, but that internal buffers are completely consumed. ESLIF will automatically require more data unless the EOF flag is set.
	 * Internal buffer is always UTF-8 encoded to every chunk of data that was declared to be a character stream.
	 * 
	 * @return an array of bytes, or null
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized byte[] input() throws ESLIFException {
		return jniInput();
	}

	/**
	 * Asks to get a logging representation of the current parse progress. The format is fixed by the underlying libraries.
	 * The <code>start</code> and <code>end</code> parameters follow the perl convention of indices, i.e. when they are negative,
	 * start that far from the end. For example, -1 mean the last indice, -2 mean one before the last indice, etc...
	 * 
	 * @param start start indice
	 * @param end end indice
	 * @param level logggin level
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized void progressLog(int start, int end, ESLIFLoggerLevel level) throws ESLIFException {
		jniProgressLog(start, end, level);
	}
	
	/**
	 * The recognizer is tentatively keeping an absolute offset every time a lexeme is complete. We say tentatively in the sense
	 * that no overflow checking is done, thus this number is not reliable in case the user data spanned over a very large number of bytes.
	 * In addition, the unit is in bytes.
	 * 
	 * @param name the last absolute offset where the symbol <code>name</code> was complete. <code>name</code> is any symbol in the grammar.
	 * @return the internal buffer absolute offset, in byte unit
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized long lastCompletedOffset(String name) throws ESLIFException {
		return jniLastCompletedOffset(name);
	}

	/**
	 * The recognizer is tentatively computing the length of every symbol completion. Since this value depend internally on the absolute previous offset,
	 * it is not guaranteed to be exact, in the sense that no overflow check is done.
	 * 
	 * @param name the last length of the symbol <code>name</code> completion. <code>name</code> is any symbol in the grammar.
	 * @return the internal buffer consumed length, in byte unit
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized long lastCompletedLength(String name) throws ESLIFException {
		return jniLastCompletedLength(name);
	}

	/**
	 * The recognizer is keeping a count of current line number if the interface is return a true value for isWithNewline() method.
	 * 
	 * @return the current line number, or 0 if line number count is switched off
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized long line() throws ESLIFException {
		return jniLine();
	}

	/**
	 * The recognizer is keeping a count of current column number if the interface is return a true value for isWithNewline() method.
	 * 
	 * @return the current column number, or 0 if column number count is switched off
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized long column() throws ESLIFException {
		return jniColumn();
	}

	/*
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
