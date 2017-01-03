package org.parser.marpa;

import java.util.ArrayList;
import java.util.List;

public class ESLIFAppValue implements ESLIFValueInterface {
	private List<Object> stack = new ArrayList<Object>();

	public boolean ruleAction(String actionName, int arg0i, int argni, int resulti, boolean nullable) {
		
		/* Fill eventual holes */
		if (resulti > stack.lastIndexOf(stack)) {
			for (int i = 0; i < resulti; i++) {
				stack.set(i, null);
			}
		}

		/* The stack contains either an Integer, either a Double, either a String */
		if ("do_int".equals(actionName)) {
			String input = (String) stack.get(arg0i);
			stack.set(resulti, new Integer(input));
			return true;
		} else if ("do_double".equals(actionName)) {
			Object left  =  stack.get(arg0i);
			String op    = (String) stack.get(arg0i+1); 
			Object right = stack.get(arg0i+2);

			/* For simplification of this example, we do all maths with Double */
			Double dleft  = (left instanceof Integer) ? new Double((Integer) left) : (Double) left; 
			Double dright = (left instanceof Integer) ? new Double((Integer) right) : (Double) right;
			Double dresult = null;
			if ("**".equals(op)) {
				dresult = Math.pow(dleft, dright);
			} else if ("*".equals(op)) {
				dresult = dleft * dright;
			} else if ("/".equals(op)) {
				dresult = dleft / dright;
			} else if ("+".equals(op)) {
				dresult = dleft + dright;
			} else if ("-".equals(op)) {
				dresult = dleft - dright;
			}
			stack.set(resulti, dresult);
			return true;
		} else {
			return false;
		}
	}

	public boolean symbolAction(byte[] data, int resulti) throws Exception {
		String string = new String(data, "UTF-8");

		/* Fill eventual holes */
		if (resulti > stack.lastIndexOf(stack)) {
			for (int i = 0; i < resulti; i++) {
				stack.set(i, null);
			}
		}

		stack.set(resulti, string);
		return true;
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

