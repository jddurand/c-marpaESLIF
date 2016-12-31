package org.parser.marpa;

import java.util.List;

public interface ESLIFValueInterface {
	public Object  ruleAction(String actionName, List<Object> objectList, boolean nullable);
	public Object  symbolAction(byte[] data);
	/* No free action because Java is able to free itself via its garbage collector */
	public boolean isWithHighRankOnly();
	public boolean isWithOrderByRank();
	public boolean isWithAmbiguous();
	public boolean isWithNull();
	public int     maxParses();
}
