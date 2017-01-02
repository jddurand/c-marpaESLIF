package org.parser.marpa;

public interface ESLIFValueInterface {
	public boolean ruleAction(String actionName, int arg0i, int argni, int resulti, boolean nullable);
	public boolean symbolAction(byte[] data, int resulti);
	/* No free action because Java is able to free itself via its garbage collector */
	public boolean isWithHighRankOnly();
	public boolean isWithOrderByRank();
	public boolean isWithAmbiguous();
	public boolean isWithNull();
	public int     maxParses();
}
