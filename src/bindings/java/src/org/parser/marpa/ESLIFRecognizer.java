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
	private ESLIFRecognizer          eslifRecognizerShared        = null;
	private ESLIFRecognizer          eslifRecognizerPeeked        = null;
	private ByteBuffer               marpaESLIFRecognizerp        = null;
	private ByteBuffer               marpaESLIFRecognizerContextp = null;
	private native void              jniNew(ESLIFGrammar eslifGrammar, boolean shallow) throws ESLIFException;
	private native void              jniFree() throws ESLIFException;
	private native boolean			 jniScan(boolean initialEvents) throws ESLIFException;
	private native boolean 			 jniResume(int deltaLength) throws ESLIFException;
	private native ESLIFEvent[]      jniEvent() throws ESLIFException;
	private native ESLIFProgress[]   jniProgress(int start, int end) throws ESLIFException;
	private native void              jniEventOnOff(String symbol, ESLIFEventType[] eventTypes, boolean onOff) throws ESLIFException;
	private native void              jniHookDiscard(boolean onOff) throws ESLIFException;
	private native void              jniHookDiscardSwitch() throws ESLIFException;
	private native boolean       	 jniAlternative(String name, Object object, int grammarLength) throws ESLIFException;
	private native boolean		 jniAlternativeComplete(int length) throws ESLIFException;
	private native boolean           jniAlternativeRead(String name, Object object, int grammarLength, int length) throws ESLIFException;
	private native boolean           jniNameTry(String name) throws ESLIFException;
	private native long              jniDiscard() throws ESLIFException;
	private native boolean           jniDiscardTry() throws ESLIFException;
	private native String[]          jniNameExpected() throws ESLIFException;
	private native byte[]            jniNameLastPause(String name) throws ESLIFException;
	private native byte[]            jniNameLastTry(String name) throws ESLIFException;
	private native byte[]            jniDiscardLastTry() throws ESLIFException;
	private native byte[]            jniDiscardLast() throws ESLIFException;
	private native boolean           jniIsEof() throws ESLIFException;
	private native boolean           jniIsStartComplete() throws ESLIFException;
	private native boolean           jniIsExhausted() throws ESLIFException;
	private native void              jniSetExhaustedFlag(boolean flag) throws ESLIFException;
	private native boolean           jniIsCanContinue() throws ESLIFException;
	private native boolean           jniRead() throws ESLIFException;
	private native byte[]            jniInput(int offset, int length) throws ESLIFException;
	private native long              jniInputLength() throws ESLIFException;
	private native void              jniError() throws ESLIFException;
	private native void              jniProgressLog(int start, int end, ESLIFLoggerLevel level) throws ESLIFException;
	private native long              jniLastCompletedOffset(String name) throws ESLIFException;
	private native long              jniLastCompletedLength(String name) throws ESLIFException;
	private native long              jniLine() throws ESLIFException;
	private native long              jniColumn() throws ESLIFException;
	private native void              jniShare(ESLIFRecognizer eslifRecognizerShared) throws ESLIFException;
	private native void              jniUnshare() throws ESLIFException;
	private native void              jniPeek(ESLIFRecognizer eslifRecognizerPeeked) throws ESLIFException;
	private native void              jniUnpeek() throws ESLIFException;
	private native byte[]            jniSymbolTry(ESLIFSymbol eslifSymbol) throws ESLIFException;

	/**
	 * Creates an ESLIFRecognizer instance
	 * 
	 * @param eslifGrammar the ESLIFGrammar instance
	 * @param eslifRecognizerInterface the recognizer interface
	 * @throws ESLIFException if the interface failed
	 */
	public ESLIFRecognizer(ESLIFGrammar eslifGrammar, ESLIFRecognizerInterface eslifRecognizerInterface) throws ESLIFException {
		this(eslifGrammar, eslifRecognizerInterface, false);
	}
	
	/**
	 * Creates a shallow ESLIFRecognizer instance from an ESLIFRecognizerInterface.
	 * 
	 * This constructor should not be used by the end-user. The JNI uses it to create a shallow instance injected in ESLIFRecognizerInterface.
	 * 
	 * @param eslifRecognizerInterface the recognizer interface
	 * @throws ESLIFException if the interface failed
	 */
	public ESLIFRecognizer(ESLIFRecognizerInterface eslifRecognizerInterface) throws ESLIFException {
		this(null, eslifRecognizerInterface, true);
	}
	
	private ESLIFRecognizer(ESLIFGrammar eslifGrammar, ESLIFRecognizerInterface eslifRecognizerInterface, boolean shallow) throws ESLIFException {
		if (eslifGrammar == null) {
			// This is fatal unless shallow is set
			if (! shallow) {
				throw new IllegalArgumentException("eslifGrammar must not be null");
			}
		}
		if (eslifRecognizerInterface == null) {
			throw new IllegalArgumentException("eslifRecognizerInterface must not be null");
		}
		setEslifGrammar(eslifGrammar);
		setEslifRecognizerInterface(eslifRecognizerInterface);
		jniNew(eslifGrammar, shallow);
	}
	
	/**
	 * 
	 * @param eslifRecognizerShared shared recognizer
	 * @throws ESLIFException if the interface failed
	 * 
	 * eslifRecognizerShared and current recognizer will share this stream.
	 * The sharing will stop if eslifRecognizerShared is set to a null value.
	 */
	public void share(ESLIFRecognizer eslifRecognizerShared) throws ESLIFException {
		jniShare(eslifRecognizerShared);
		setEslifRecognizerShared(eslifRecognizerShared);
	}

	/**
	 * 
	 * @throws ESLIFException if the interface failed
	 * 
	 * Stops sharing the stream. Equivalent to a call to share with a null shared recognizer.
	 */
	public void unshare() throws ESLIFException {
		jniUnshare();
		setEslifRecognizerShared(null);
	}

	/**
	 * 
	 * @param eslifRecognizerPeeked peeked recognizer
	 * @throws ESLIFException if the interface failed
	 * 
	 * eslifRecognizerPeeked and current recognizer will share this stream, though internal buffers will grow until a call to unpeek.
	 * The peeking will stop if eslifRecognizerPeeked is set to a null value.
	 */
	public void peek(ESLIFRecognizer eslifRecognizerPeeked) throws ESLIFException {
		jniPeek(eslifRecognizerPeeked);
		setEslifRecognizerPeeked(eslifRecognizerPeeked);
	}

	/**
	 * 
	 * @throws ESLIFException if the interface failed
	 * 
	 * Stops peeking the stream. Equivalent to a call to peek with a null peeked recognizer.
	 */
	public void unpeek() throws ESLIFException {
		jniUnpeek();
		setEslifRecognizerPeeked(null);
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
		return jniIsCanContinue();
	}

	/**
	 * 
	 * @return a boolean indicating if parsing is exhausted, always set even without exhaustion event
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean isExhausted() throws ESLIFException {
		return jniIsExhausted();
	}
	
	/**
	 *
	 * @param flag a flag indicating if exhaustion is supported
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized void setExhaustedFlag(boolean flag) throws ESLIFException {
		jniSetExhaustedFlag(flag);
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
	 * Asks to get the current parse progress.
	 * The <code>start</code> and <code>end</code> parameters follow the perl convention of indices, i.e. when they are negative,
	 * start that far from the end. For example, -1 mean the last indice, -2 mean one before the last indice, etc...
	 * 
	 * @param start start indice
	 * @param end end indice
	 * 
	 * @return the array of progress, eventually empty if there is none
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized ESLIFProgress[] progress(int start, int end) throws ESLIFException {
		return jniProgress(start, end);
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
	 * Discard can be switched on or off. This is a grammar hook, somewhat dangerous and a permanent setting.
	 *
	 * @param onOff the boolean indicating the state of the event types.
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized void hookDiscard(boolean onOff) throws ESLIFException {
		jniHookDiscard(onOff);
	}

	/**
	 * Discard can be switched on or off, it if was off or on, respectively. This is a grammar hook, somewhat dangerous and a permanent setting.
	 *
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized void hookDiscardSwitch() throws ESLIFException {
		jniHookDiscardSwitch();
	}

	/**
	 * Pushing an alternative mean that the end-user is instructing the recognizer that, at this precise moment of lexing, there is a given
	 * symbol associated to the name parameter, with a given opaque value.  
	 * Grammar length is usually one, i.e. one token.
	 * Nevertheless it is possible to say that an alternative span over more than one symbol.
	 * 
	 * @param name the name of the symbol
	 * @param object the object that will represent the value of this symbol at this parsing stage
	 * @param grammarLength the length in the grammar, must be greater or equal than one
	 * @return a boolean indicating if the call was successful or not
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean alternative(String name, Object object, int grammarLength) throws ESLIFException {
		return jniAlternative(name, object, grammarLength);
	}
	
	/**
	 * Short-hand version of alternative, where grammar length default to the recommended value of 1.
	 * 
	 * @param name the name of the symbol
	 * @param object the object that will represent the value of this symbol at this parsing stage
	 * @return a boolean indicating if the call was successful or not
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean alternative(String name, Object object) throws ESLIFException {
		return alternative(name, object, 1);
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
	public synchronized boolean alternativeComplete(int length) throws ESLIFException {
		return jniAlternativeComplete(length);
	}
	
	/**
	 * A short-hand version of alternative() plus complete() methods, whith the exact same meaning of all parameters.
	 * This method can generate events.
	 * 
	 * @param name the name of the symbol
	 * @param object the object that will represent the value of this symbol at this parsing stage
	 * @param length the number of bytes consumed by the latest set of alternatives
	 * @param grammarLength the length in the grammar, must be greater or equal than one
	 * @return a boolean indicating if the call was successful or not
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean alternativeRead(String name, Object object, int length, int grammarLength) throws ESLIFException {
		return jniAlternativeRead(name, object, length, grammarLength);
	}
	
	/**
	 * A short-hand version of alternativeRead() where grammarLength default to the recommended value of 1.
	 * This method can generate events.
	 * 
	 * @param name the name of the symbol
	 * @param object the object that will represent the value of this symbol at this parsing stage
	 * @param length the number of bytes consumed by the latest set of alternatives
	 * @return a boolean indicating if the call was successful or not
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean alternativeRead(String name, Object object, int length) throws ESLIFException {
		return alternativeRead(name, object, length, 1);
	}
	
	/**
	 * The end-user can ask the recognizer if a symbol name may match
	 * 
	 * @param name the name of the symbol
	 * @return a boolean indicating if the symbol is recognized
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean nameTry(String name) throws ESLIFException {
		return jniNameTry(name);
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
	 * Ask the recognizer to discard data
	 * 
	 * @return the number of bytes discarded
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized long discard() throws ESLIFException {
		return jniDiscard();
	}
	
	/**
	 * Ask the recognizer a list of expected symbol names
	 * 
	 * @return an array of symbol names, may be null if there is none
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized String[] nameExpected() throws ESLIFException {
		return jniNameExpected();
	}
	
	/**
	 * Ask the recognizer the end-user data associated to a last symbol "pause" event.
	 * A "pause" event is the when the recognizer was responsible of symbol recognition, after a call to scan() or resume() methods.
	 * This data will be an exact copy of the last bytes that matched for a given symbol, where data is the internal representation of end-user data,
	 * meaning that it may be UTF-8 sequence of bytes in case of character stream, and always an UTF-8 seauence of bytes in case of end-user data
	 * (end-user data representation is always the toString() result converted to UTF8).
	 * 
	 * @param name the symbol name
	 * @return an array of bytes
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized byte[] nameLastPause(String name) throws ESLIFException {
		return jniNameLastPause(name);
	}

	/**
	 * Ask the recognizer the end-user data associated to last successful symbol name try.
	 * This data will be an exact copy of the last bytes that matched for a given symbol, where data is the internal representation of end-user data,
	 * meaning that it may be UTF-8 sequence of bytes in case of character stream, and always an UTF-8 sequence of bytes in case of end-user data
	 * (end-user data representation is always the toString() result converted to UTF8).
	 * 
	 * @param name the symbol name
	 * @return an array of bytes
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized byte[] nameLastTry(String name) throws ESLIFException {
		return jniNameLastTry(name);
	}

	/**
	 * Ask the recognizer the bytes associated to last successful discard try.
	 * This data will be an exact copy of the last bytes that matched for the latest :discard try,
	 * meaning that it may be UTF-8 sequence of bytes in case of character stream.
	 * 
	 * @return an array of bytes
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized byte[] discardLastTry() throws ESLIFException {
		return jniDiscardLastTry();
	}

	/**
	 * Ask the recognizer the bytes associated to last successful discard.
	 * This data will be an exact copy of the last bytes that matched for the latest :discard,
	 * meaning that it may be UTF-8 sequence of bytes in case of character stream.
	 * 
	 * For performance reasons, latest discard data is available only of the recognizer interface returned true for the isWithTrack method,
	 * and if there an event associated to the :discard rule that matched.
	 * 
	 * @return an array of bytes
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized byte[] discardLast() throws ESLIFException {
		return jniDiscardLast();
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
	 * Indicates if the start symbol is complete. Note that this does mean that the grammar is exhausted.
	 * 
	 * @return a boolean indicating start symbol completion
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized boolean isStartComplete() throws ESLIFException {
		return jniIsStartComplete();
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
	 * Get a copy of the current internal recognizer buffer, where offset is in byte unit and with the same semantics as perl's substr function.
	 * An undefined output does not necessarily mean there is an error, but that the internal buffer is completely consumed.
	 * It is recommended to set length parameter to a reasonable value, to prevent an internal copy of a potentially big number of bytes.
	 * 
	 * @param offset offset in byte unit
	 * @return an array of bytes, or null
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized byte[] input(int offset) throws ESLIFException {
		return this.input(offset, 0);
	}

	/**
	 * Get a copy of the current internal recognizer buffer, where offset and length are in byte unit and with the same semantics as
	 * perl's substr function for the offset parameter, same semantics as for the length parameter when it is not 0 (the zero value is ignored).
	 * An undefined output does not necessarily mean there is an error, but that the internal buffer is completely consumed.
	 * It is recommended to set length parameter to a reasonable value, to prevent an internal copy of a potentially big number of bytes.
	 * 
	 * @param offset offset in byte unit
	 * @param length length in byte unit
	 * @return an array of bytes, or null
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized byte[] input(int offset, int length) throws ESLIFException {
		return jniInput(offset, length);
	}

	/**
	 * Get the length of the current internal recognizer buffer.
	 * 
	 * @return length in bytes
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized long inputLength() throws ESLIFException {
		return jniInputLength();
	}

	/**
	 * Asks to simulate an error report.
	 * 
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized void error() throws ESLIFException {
		jniError();
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
	 * The recognizer is tentatively keeping an absolute offset every time a symbol is complete. We say tentatively in the sense
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

	/**
	 * Try to match a symbol. 
	 * 
	 * @param eslifSymbol the symbol
	 * @throws ESLIFException if the interface failed
	 * 
	 * @return null if no match, else the matched bytes
	 */
	public synchronized byte[] symbolTry(ESLIFSymbol eslifSymbol) throws ESLIFException {
		if (eslifSymbol == null) {
			throw new IllegalArgumentException("eslifSymbol must not be null");
		}
		return jniSymbolTry(eslifSymbol);
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
	private ESLIFRecognizer getEslifRecognizerShared() {
		return eslifRecognizerShared;
	}
	private void setEslifRecognizerShared(ESLIFRecognizer eslifRecognizerShared) {
		this.eslifRecognizerShared = eslifRecognizerShared;
	}
	private ESLIFRecognizer getEslifRecognizerPeeked() {
		return eslifRecognizerPeeked;
	}
	private void setEslifRecognizerPeeked(ESLIFRecognizer eslifRecognizerPeeked) {
		this.eslifRecognizerPeeked = eslifRecognizerPeeked;
	}
	/**
	 * Get logger instance
	 * 
	 * @return an ESLIFLoggerInterface instance, which can be null
	 */
	protected ESLIFLoggerInterface getLoggerInterface() {
		return (eslifGrammar != null) ? eslifGrammar.getLoggerInterface() : null;
	}
	/**
	 * Get grammar instance
	 * 
	 * @return an ESLIFGrammar instance
	 */
	protected ESLIFGrammar getEslifGrammar() {
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
