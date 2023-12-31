package org.parser.marpa;

/**
 * ESLIFJSONDecoderOption controls the behaviour of decoding a JSON string to an object.
 *
 * Options affect:
 * - Duplicated keys
 * - Maximum depth
 * - Replacement character
 *
 */
public class ESLIFJSONDecoderOption {
	private boolean disallowDupkeys = false;
	private long maxDepth = 0;
	private boolean noReplacementCharacter = false;

	
	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */

	/**
	 * @return the disallowDupkeys option
	 */
	public boolean isDisallowDupkeys() {
		return disallowDupkeys;
	}
	/**
	 * @param disallowDupkeys the disallowDupkeys option to set
	 */
	public void setDisallowDupkeys(boolean disallowDupkeys) {
		this.disallowDupkeys = disallowDupkeys;
	}
	/**
	 * @return the maxDepth option
	 */
	public long getMaxDepth() {
		return maxDepth;
	}
	/**
	 * @param maxDepth the maxDepth option to set
	 */
	public void setMaxDepth(long maxDepth) {
		this.maxDepth = maxDepth;
	}
	/**
	 * @return the noReplacementCharacter option
	 */
	public boolean isNoReplacementCharacter() {
		return noReplacementCharacter;
	}
	/**
	 * @param noReplacementCharacter the noReplacementCharacter option to set
	 */
	public void setNoReplacementCharacter(boolean noReplacementCharacter) {
		this.noReplacementCharacter = noReplacementCharacter;
	}
}
