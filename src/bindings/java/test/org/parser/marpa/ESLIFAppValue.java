package org.parser.marpa;

import java.util.ArrayList;
import java.util.List;

public class ESLIFAppValue implements ESLIFValueInterface {
	private List<Object> stack = new ArrayList<Object>();
	
	public void stackSet(int indice, Object object) {
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

	public void stackFree(int indice) {
		stackSet(indice, null);
	}

	public Object stackGet(int indice) {
		return stack.get(indice);
	}

	private Double toDouble(Object object) {
		return (object instanceof Integer)  ? new Double(((Integer) object).doubleValue())  : (Double) object;
	}

	private Integer toInteger(Object object) {
		return (Integer) object;
	}

	public Object action(String actionName, List<Object> list) {
		Object result = null;
		
		/* The stack contains either an Integer, either a Double, either a String */
		if ("do_int".equals(actionName)) {
			String input = (String) list.get(0);
			result = new Integer(input);
		} else if ("do_group".equals(actionName)) {
			result = list.get(1);
		} else if ("do_op".equals(actionName)) {
			Object left  =          list.get(0);
			String op    = (String) list.get(1); 
			Object right =          list.get(2);

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
		}
		
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

}

