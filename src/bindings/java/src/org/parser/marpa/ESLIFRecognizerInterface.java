package org.parser.marpa;

/**
 * ESLIF Recognizer Interface
 */
public interface ESLIFRecognizerInterface {
	/**
	 * Callback executed whenever the recognizer need more data.
	 * 
	 * It is the end-user responsibility to provide coherent chunks of data in case there is a mix of
	 * character and binary streams. Every execution of this method should impact the other methods:
	 * <ul>
	 *   <li> <code>isEof()</code>
	 *   <li> <code>isCharacterStream()</code>
	 *   <li> <code>encoding()</code>
	 *   <li> <code>data()</code>
	 * </ul>
	 * 
	 * @return a boolean value indicating read success or failure
	 */
	public boolean read();
	/**
	 * End of data flag. 
	 * 
	 * @return a boolean value indicating the end of the stream
	 */
	public boolean isEof();
	/**
	 * Character stream flag.
	 * 
	 * @return a boolean value indicating if current chunk is a character stream or not
	 */
	public boolean isCharacterStream();
	/**
	 * Encoding of latest chunk of data, when the later is a character chunk.
	 * 
	 * If current chunk of data is a character stream, and this method returns <code>null</code>, then marpaESLIF will either:
	 * <ul>
	 *   <li>guess the encoding if there previous chunk of data was not a character stream,
	 *   <li>continue with previous encoding if previous chunk of data was a character stream
	 * </ul>
	 * 
	 * @return a string giving current character encoding, may be null if unknown
	 */
	public String  encoding();
	/**
	 * Chunk content.
	 * 
	 * @return byte content of current chunk, may be of zero size
	 */
	public byte[]  data();
	/**
	 * Disable threshold warning.
	 * 
	 * When the parsing becomes very ambiguous, the native library will automatically emit warnings.
	 * This setting is used at recognizer creation step only.
	 * 
	 * @return a boolean indicating if threshold warnings should be turned off
	 */
	public boolean isWithDisableThreshold();
	/**
	 * Enable exhaustion event.
	 * 
	 * When the parse is exhausted, the normal behavior is to exit with an error if the eof flag is not set.
	 * This method is saying that an exhaustion event should be raised instead, and is used at recognizer creation step only.
	 * 
	 * @see ESLIFEventType
	 * @return a boolean indicating if exhaustion should trigger an exhaustion event
	 */
	public boolean isWithExhaustion();
	/**
	 * User-friendly error reporting.
	 * 
	 * Error reporting can be accurate up to line and column numbers when this is happening on a character stream enabled chunk of data.
	 * This is handy, but has an extra cost on parsing performance.
	 * This method is used at recognizer creation step only.
	 * 
	 * @return a boolean indicating if line/number accounting is on
	 */
	public boolean isWithNewline();
	/**
	 * Absolute position tracking.
	 * 
	 * Absolute position tracking is telling the recognizer to keep track of absolute position at every new alternative.
	 * This function not only has a cost, but is not fully reliable because there is no tentative to check for internal turnaround
	 * of the associated implied internal variables.
	 * You should turn this option to true <i>only</i> if you plan to use the lastCompletedOffset() or lastCompletedLEngth() methods.
	 * 
	 * @return a boolean indicating if absolute position tracking is on
	 */
	public boolean isWithTrack();
}
