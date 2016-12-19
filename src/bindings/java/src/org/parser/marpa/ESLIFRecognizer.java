package org.parser.marpa;

public class ESLIFRecognizer {
	private ESLIFReaderInterface readerInterface      = null;
	private boolean              withThresholdWarning = false;
	private boolean              withExhaustionEvent  = false;
	private boolean              withLineCount        = false;
	
	public ESLIFRecognizer(ESLIFReaderInterface readerInterface) {
		if (readerInterface == null) {
			throw new IllegalArgumentException("readerInterface must not be null");
		}
		setReaderInterface(readerInterface);
	}

	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	public ESLIFReaderInterface getReaderInterface() {
		return readerInterface;
	}
	public void setReaderInterface(ESLIFReaderInterface readerInterface) {
		this.readerInterface = readerInterface;
	}
	public boolean isWithThresholdWarning() {
		return withThresholdWarning;
	}
	public void setWithThresholdWarning(boolean withThresholdWarning) {
		this.withThresholdWarning = withThresholdWarning;
	}
	public boolean isWithExhaustionEvent() {
		return withExhaustionEvent;
	}
	public void setWithExhaustionEvent(boolean withExhaustionEvent) {
		this.withExhaustionEvent = withExhaustionEvent;
	}
	public boolean isWithLineCount() {
		return withLineCount;
	}
	public void setWithLineCount(boolean withLineCount) {
		this.withLineCount = withLineCount;
	}
}
