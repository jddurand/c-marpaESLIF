package org.parser.marpa;

public class ESLIFRecognizer {
	private ESLIFReaderInterface readerInterface       = null;
	private boolean              disableThreshold      = false;
	private boolean              withExhaustion        = false;
	private boolean              withLineCount         = false;
	private int                  bufferSizel           = 0;
	private int                  bufferReduceTrigger   = 50;
	private int                  bufferIncreaseTrigger = 50;

	public ESLIFRecognizer(ESLIFReaderInterface readerInterface) {
		if (readerInterface == null) {
			throw new IllegalArgumentException("readerInterface must not be null");
		}
		setReaderInterface(readerInterface);
	}
	public ESLIFReaderInterface getReaderInterface() {
		return readerInterface;
	}
	public void setReaderInterface(ESLIFReaderInterface readerInterface) {
		this.readerInterface = readerInterface;
	}
	public boolean isDisableThreshold() {
		return disableThreshold;
	}
	public void setDisableThreshold(boolean disableThreshold) {
		this.disableThreshold = disableThreshold;
	}
	public boolean isWithExhaustion() {
		return withExhaustion;
	}
	public void setWithExhaustion(boolean withExhaustion) {
		this.withExhaustion = withExhaustion;
	}
	public boolean isWithLineCount() {
		return withLineCount;
	}
	public void setWithLineCount(boolean withLineCount) {
		this.withLineCount = withLineCount;
	}
	public int getBufferSizel() {
		return bufferSizel;
	}
	public void setBufferSizel(int bufferSizel) {
		this.bufferSizel = bufferSizel;
	}
	public int getBufferReduceTrigger() {
		return bufferReduceTrigger;
	}
	public void setBufferReduceTrigger(int bufferReduceTrigger) {
		this.bufferReduceTrigger = bufferReduceTrigger;
	}
	public int getBufferIncreaseTrigger() {
		return bufferIncreaseTrigger;
	}
	public void setBufferIncreaseTrigger(int bufferIncreaseTrigger) {
		this.bufferIncreaseTrigger = bufferIncreaseTrigger;
	}
}
