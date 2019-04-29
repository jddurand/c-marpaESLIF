package org.parser.marpa;

import java.io.UnsupportedEncodingException;

/**
 * Test Application Value
 * 
 * In our example we have NOT specified a symbol action, therefore
 * lexemes that come directly from the grammar are in the form of
 * a ByteBuffer.
 */
public class AppValue implements ESLIFValueInterface {
	private Object result = null;
	private String symbolName;
	private int symbolNumber;
	private String ruleName;
	private int ruleNumber;
	private ESLIFGrammar grammar;
	
	/*
	 * https://java.net/jira/browse/TYRUS-216
	 */
	@SuppressWarnings("deprecation")
	private Double toDouble(Object object) {
		return (object instanceof Integer) ? new Double(((Integer) object).doubleValue()) : (Double) object;
	}

	private Integer toInteger(Object object) {
		return (Integer) object;
	}
	
	private String toString(Object object) throws UnsupportedEncodingException {
		return (object instanceof byte[]) ? new String((byte[]) object, "UTF-8") : null;
	}

	/**
	 * @param list object list
	 * @return resulting object
	 * @throws UnsupportedEncodingException unsupported encoding exception
	 */
	@SuppressWarnings("deprecation")
	public Object do_int(Object[] list) throws UnsupportedEncodingException {
		Object result;
		
		if (list[0] instanceof byte[]) {
			String input = toString(list[0]);
			result = new Integer(input);
		} else {
			result = list[0];
		}

		System.err.println("... " + result);
		return result;
	}

	/**
	 * @param list list of objects
	 * @return result object
	 * @throws UnsupportedEncodingException unsupported encoding exception
	 */
	@SuppressWarnings("deprecation")
	public Object do_op(Object[] list) throws UnsupportedEncodingException {
		Object left   = list[0];
		String op     = toString(list[1]); 
		Object right  = list[2];
		Object result = null;
		
		boolean leftIsInteger  = (left  instanceof Integer);
		boolean rightIsInteger = (right instanceof Integer);
		
		if ("**".equals(op)) {
			result = new Double(Math.pow(toDouble(left), toDouble(right)));
		} else if ("*".equals(op)) {
			if (leftIsInteger && rightIsInteger) {
				result = toInteger(left) * toInteger(right);
			} else {
				result = toDouble(left) * toDouble(right);
			}
		} else if ("/".equals(op)) {
			result = toDouble(left) / toDouble(right);
		} else if ("+".equals(op)) {
			if (leftIsInteger && rightIsInteger) {
				result = toInteger(left) + toInteger(right);
			} else {
				result = toDouble(left) + toDouble(right);
			}
		} else if ("-".equals(op)) {
			if (leftIsInteger && rightIsInteger) {
				result = toInteger(left) - toInteger(right);
			} else {
				result = toDouble(left) - toDouble(right);
			}
		}

		System.err.println("... " + left + " " + op + " " + right + " => " + result);

		return result;
	}

	public boolean isWithHighRankOnly() {
		return true;
	}

	public boolean isWithOrderByRank() {
		return true;
	}

	public boolean isWithAmbiguous() {
		return false;
	}

	public boolean isWithNull() {
		return false;
	}

	public int maxParses() {
		return 0;
	}

	public Object getResult() {
		return result;
	}

	public void setResult(Object result) {
		this.result = result;
	}

	public void setSymbolName(String symbolName) {
		this.symbolName = symbolName;	
		System.err.println("... Symbol name: " + symbolName);
	}

	public void setSymbolNumber(int symbolNumber) {
		this.symbolNumber = symbolNumber;
		System.err.println("... Symbol number: " + symbolNumber);
	}

	public void setRuleName(String ruleName) {
		this.ruleName = ruleName;	
		System.err.println("... Rule name: " + ruleName);
	}

	public void setRuleNumber(int ruleNumber) {
		this.ruleNumber = ruleNumber;	
		System.err.println("... Rule number: " + ruleNumber);
	}

	public void setGrammar(ESLIFGrammar grammar) {
		this.grammar = grammar;
	}

	/**
	 * @return the symbolName
	 */
	public String getSymbolName() {
		return symbolName;
	}

	/**
	 * @return the symbolNumber
	 */
	public int getSymbolNumber() {
		return symbolNumber;
	}

	/**
	 * @return the ruleName
	 */
	public String getRuleName() {
		return ruleName;
	}

	/**
	 * @return the ruleNumber
	 */
	public int getRuleNumber() {
		return ruleNumber;
	}

	/**
	 * @return the grammar
	 */
	public ESLIFGrammar getGrammar() {
		return grammar;
	}

	/**
	 * @param list list of objects
	 * @param object input object
	 * @return result object
	 */
	public Object java_proxy(Object...list) {
		return list[0];
	}

	/**
	 * @param list list of objects
	 * @param object input object
	 * @return result object
	 */
	public Object java_symbolAction(Object obj) {
		return obj;
	}
}

