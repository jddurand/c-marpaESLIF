package org.parser.marpa;

public class ESLIFValueOption {
	private Object  object = null;
	private boolean highRank = true;
	private boolean orderByRank = true;
	private boolean ambiguous = false;
	private boolean nullParse = false;
	private int     maxParse = 0;
	private boolean exhausted = false;

	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	public ESLIFValueOption(Object object) throws Exception {
		if (object == null) {
			throw new Exception("Action object must not be null");
		}
	}
	public Object getObject() {
		return object;
	}
	public void setObject(Object object) {
		this.object = object;
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
	public boolean isAmbiguous() {
		return ambiguous;
	}
	public void setAmbiguous(boolean ambiguous) {
		this.ambiguous = ambiguous;
	}
	public boolean isNullParse() {
		return nullParse;
	}
	public void setNullParse(boolean nullParse) {
		this.nullParse = nullParse;
	}
	public int getMaxParse() {
		return maxParse;
	}
	public void setMaxParse(int maxParse) {
		this.maxParse = maxParse;
	}
	public boolean isExhausted() {
		return exhausted;
	}
	/*
	 * ********************************************
	 * Private methods - used by the JNI
	 * ********************************************
	 */
	private void setExhausted(boolean exhausted) {
		this.exhausted = exhausted;
	}
}
