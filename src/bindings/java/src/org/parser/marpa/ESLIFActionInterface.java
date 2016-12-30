package org.parser.marpa;

import java.util.List;

public interface ESLIFActionInterface {
	public Object ruleAction(String actionName, List<Object> objectList, boolean nullable);
	public Object symbolAction(byte[] data);
	/* No free action because Java is able to free itself via its garbage collector */
}
