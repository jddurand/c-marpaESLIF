package org.parser.marpa;

import java.util.List;

public class ESLIFAppValue implements ESLIFValueInterface {
	private List<Object> stack = null;

	public boolean ruleAction(String actionName, int arg0i, int argni, int resulti, boolean nullable) {
		
		if ("do_int".equals(actionName)) {
			String input = (String) stack.get(arg0i);
			stack.set(resulti, new Integer(input));
			return true;
		} else {
			return false;
		}
	}

	public boolean symbolAction(byte[] data, int resulti) {
		return false;
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

}

