package org.parser.marpa;

public class ESLIFValuator {
	private ESLIFActionInterface actionInterface  = null;
	
	public ESLIFValuator(ESLIFActionInterface actionInterface) {
		if (actionInterface == null) {
			throw new IllegalArgumentException("actionInterface must not be null");
		}
		setActionInterface(actionInterface);
	}
	public ESLIFActionInterface getActionInterface() {
		return actionInterface;
	}
	public void setActionInterface(ESLIFActionInterface actionInterface) {
		this.actionInterface = actionInterface;
	}
	public boolean isHighRank() {
		return highRank;
	}
	public void setHighRank(boolean highRank) {
		this.highRank = highRank;
	}
	public boolean isOrderByRank() {
		return orderByRank;
	}
	public void setOrderByRank(boolean orderByRank) {
		this.orderByRank = orderByRank;
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
	public int getMaxNumberOfParse() {
		return maxNumberOfParse;
	}
	public void setMaxNumberOfParse(int maxNumberOfParse) {
		this.maxNumberOfParse = maxNumberOfParse;
	}
	private boolean              highRank         = true;
	private boolean              orderByRank      = true;
	private boolean              withAmbiguity    = false;
	private boolean              withNullParse    = false;
	private int                  maxNumberOfParse = 0;
}
