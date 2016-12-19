package org.parser.marpa;

import java.util.List;

public class ESLIFValue {
	private ESLIFActionInterface actionInterface = null;
	private boolean              withHighRank    = true;
	private boolean              withOrderByRank = true;
	private boolean              withAmbiguity   = false;
	private boolean              withNullParse   = false;
	private int                  maxParse        = 0;
	private List<Object>         stack           = null;
	
	public ESLIFValue(ESLIFActionInterface actionInterface) {
		if (actionInterface == null) {
			throw new IllegalArgumentException("actionInterface must not be null");
		}
		setActionInterface(actionInterface);
	}

	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	public boolean isWithHighRank() {
		return withHighRank;
	}
	public void setWithHighRank(boolean withHighRank) {
		this.withHighRank = withHighRank;
	}
	public boolean isWithOrderByRank() {
		return withOrderByRank;
	}
	public void setWithOrderByRank(boolean withOrderByRank) {
		this.withOrderByRank = withOrderByRank;
	}
	public boolean isWithAmbiguity() {
		return withAmbiguity;
	}
	public void setWithAmbiguity(boolean withAmbiguity) {
		this.withAmbiguity = withAmbiguity;
	}
	public boolean isWithNullParse() {
		return withNullParse;
	}
	public void setWithNullParse(boolean withNullParse) {
		this.withNullParse = withNullParse;
	}
	public int getMaxParse() {
		return maxParse;
	}
	public void setMaxParse(int maxParse) {
		this.maxParse = maxParse;
	}
	/*
	 * ********************************************
	 * Private methods - used by the JNI
	 * ********************************************
	 */
	private ESLIFActionInterface getActionInterface() {
		return actionInterface;
	}

	private void setActionInterface(ESLIFActionInterface actionInterface) {
		this.actionInterface = actionInterface;
	}
	private List<Object> getStack() {
		return stack;
	}
	private void setStack(List<Object> stack) {
		this.stack = stack;
	}
	private Object getObject(List<Object> stack, int i) {
		return stack.get(i);
	}
	private void setObject(List<Object> stack, int i, Object object) {
		stack.set(i, object);
	}
	private void freeObject(List<Object> stack, int i) {
		setObject(stack, i, null);
	}
}
