package org.parser.marpa;

import java.util.ArrayList;
import java.util.List;

public class ESLIFAppValue implements ESLIFValueInterface {
	private List<Object> stack = new ArrayList<Object>();
	
	public Object result() {
		return stack.get(0);
	}

	private void setInStack(int indice, Object object) {
		/* Fill eventual holes */
		if (indice >= stack.size()) {
			for (int i = stack.size(); i < indice - 1; i++) {
				stack.set(i, null);
			}
			stack.add(object);
		} else {
			stack.set(indice, object);
		}
	}

	private Double toDouble(Object object) {
		return (object instanceof Integer)  ? new Double(((Integer) object).doubleValue())  : (Double) object;
	}

	private Integer toInteger(Object object) {
		return (Integer) object;
	}

	public boolean ruleAction(String actionName, int arg0i, int argni, int resulti, boolean nullable) {
		Object result = null;
		
		/* The stack contains either an Integer, either a Double, either a String */
		if ("do_int".equals(actionName)) {
			String input = (String) stack.get(arg0i);
			result = new Integer(input);
		} else if ("do_group".equals(actionName)) {
			result = stack.get(arg0i+1);
		} else if ("do_op".equals(actionName)) {
			Object left  =          stack.get(arg0i);
			String op    = (String) stack.get(arg0i+1); 
			Object right =          stack.get(arg0i+2);

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

		} else {
			return false;
		}

		setInStack(resulti, result);
		return true;
	}

	public boolean symbolAction(byte[] data, int resulti) throws Exception {
		String string = new String(data, "UTF-8");
		
		setInStack(resulti, string);
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

