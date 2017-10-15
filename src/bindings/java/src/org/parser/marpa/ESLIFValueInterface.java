package org.parser.marpa;

/**
 * ESLIFValueInterface is giving the required methods for valuation
 */

public interface ESLIFValueInterface {
	/**
	 * When the interface returns true, only the choices with the highest rank are kept.
	 * This method is used at valuation instance creation step only.
	 * 
	 * @return the high-rank only preference
	 */
	public boolean isWithHighRankOnly();
	/**
	 * Orders the parse tree values by their rank value. 
	 * This method is used at valuation instance creation step only.
	 * 
	 * @return the rank order preference
	 */
	public boolean isWithOrderByRank();
	/**
	 * Accept ambiguous parse tree.
	 * This method is used at valuation instance creation step only.
	 * 
	 * @return the ambiguity acceptance
	 */
	public boolean isWithAmbiguous();
	/**
	 * Accept a null parse tree value (e.g. when the start rule is not complete)
	 * This method is used at valuation instance creation step only.
	 * 
	 * @return the null parse acceptance
	 */
	public boolean isWithNull();
	/**
	 * A very ambiguous parsing can provide a lot of parse tree values, it is possible to reduce such number, at the cost of not having all the valuation possibilities.
	 * This method is used at valuation instance creation step only.
	 * 
	 * @return the maximum number of wanted parses, 0 for all parse trees.
	 */
	public int maxParses();
	/**
	 * This method is used by the JNI proxy to marpaESLIF library to store the final result
	 * 
	 * @param result the final result
	 */
	public void setResult(Object result);
	/**
	 * Returns the object stored using <code>setResult()</code>.
	 * 
	 * @return the final result
	 */
	public Object  getResult();
	/**
	 * Sets current symbol name, when value interface is called on the context of a symbol.
	 * 
	 * @param symbolName symbol name 
	 */
	public void  setSymbolName(String symbolName);
	/**
	 * Sets current symbol number, when value interface is called on the context of a symbol.
	 * 
	 * @param symbolNumber symbol identifier 
	 */
	public void  setSymbolNumber(int symbolNumber);
	/**
	 * Sets current rule name, when value interface is called on the context of a rule.
	 * 
	 * @param ruleName rule name
	 */
	public void  setRuleName(String ruleName);
	/**
	 * Sets current rule number, when value interface is called on the context of a rule.
	 * 
	 * @param ruleNumber rule identifier
	 */
	public void  setRuleNumber(int ruleNumber);
	/**
	 * Sets current rule grammar at every value interface call.
	 * 
	 * @param grammar grammar object 
	 */
	public void  setGrammar(ESLIFGrammar grammar);
}
