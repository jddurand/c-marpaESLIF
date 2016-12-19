package org.parser.marpa;

public interface ESLIFActionInterface {
	public Object ruleAction(int arg0i, int argni, int resulti, boolean nullable);
	public Object symbolAction(byte[] data, int resulti);
}
