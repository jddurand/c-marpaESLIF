package org.parser.marpa;

/**
 * ESLIFValueInterface is giving the required methods for valuation.
 */

public interface ESLIFValueInterface {
	/**
	 * When the interface returns true, only the choices with the highest rank are kept.
	 * 
	 * @return the high-rank only preference
	 */
	public boolean isWithHighRankOnly();
	/**
	 * 
	 * @return the rank order preference
	 */
	public boolean isWithOrderByRank();
	/**
	 * 
	 * @return the ambiguity acceptance
	 */
	public boolean isWithAmbiguous();
	/**
	 * 
	 * @return the null parse acceptance
	 */
	public boolean isWithNull();
	/**
	 * 
	 * @return the maximum number of wanted parses
	 */
	public int     maxParses();
	/**
	 * This method is used by the JNI proxy to marpaESLIF library to store the final result
	 * 
	 * @param result the final result
	 */
	public void    setResult(Object result);
	/**
	 * 
	 * @return the final result
	 */
	public Object  getResult();
}
