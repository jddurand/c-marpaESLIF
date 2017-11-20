package org.parser.marpa;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;

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
	private byte[] getActiveArray(ByteBuffer buffer)
	{
	  byte[] ret = new byte[buffer.remaining()];
	  if (buffer.hasArray())
	  {
	      byte[] array = buffer.array();
	      System.arraycopy(array, buffer.arrayOffset() + buffer.position(), ret, 0, ret.length);
	  }
	  else
	  {
	      buffer.slice().get(ret);
	  }
	  return ret;
	}
	
	private String byteBufferToString(ByteBuffer byteBuffer) throws UnsupportedEncodingException {
		return new String(getActiveArray(byteBuffer), "UTF-8");
	}
		
	private Double toDouble(Object object) {
		return (object instanceof Integer) ? new Double(((Integer) object).doubleValue()) : (Double) object;
	}

	private Integer toInteger(Object object) {
		return (Integer) object;
	}
	
	/**
	 * @param list object list
	 * @return resulting object
	 * @throws UnsupportedEncodingException unsupported encoding exception
	 */
	public Object do_int(Object[] list) throws UnsupportedEncodingException {
		Object result;
		
		if (list[0] instanceof ByteBuffer) {
			String input = byteBufferToString((ByteBuffer) list[0]); 
			result = new Integer(input);
		} else {
			result = list[0];
			return list[0];
		}

		return result;
	}

	/**
	 * @param list list of objects
	 * @return result object
	 * @throws UnsupportedEncodingException unsupported encoding exception
	 */
	public Object do_op(Object[] list) throws UnsupportedEncodingException {
		Object left   = list[0];
		String op     = byteBufferToString((ByteBuffer) list[1]); 
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
		if (result instanceof ByteBuffer) {
			try {
				this.result = byteBufferToString((ByteBuffer) result);
			} catch (UnsupportedEncodingException e) {
				this.result = null;
			}
		} else {
			this.result = result;
		}
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

}

