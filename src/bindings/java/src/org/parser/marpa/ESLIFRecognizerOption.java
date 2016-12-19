package org.parser.marpa;

import java.io.Reader;

public class ESLIFRecognizerOption {
	private Reader  reader          = null;
	private boolean threshold       = false;
	private boolean newline         = false;
	private int     bufsizl         = 0;
	private int     buftriggerperci = 50;
	private int     bufaddperci     = 50;

	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	public ESLIFRecognizerOption(Reader reader) throws Exception {
		if (reader == null) {
			throw new Exception("Reader must not be null");
		}
	}
	public Reader getReader() {
		return reader;
	}
	public void setReader(Reader reader) {
		this.reader = reader;
	}
	public boolean isThreshold() {
		return threshold;
	}
	public void setThreshold(boolean threshold) {
		this.threshold = threshold;
	}
	public boolean isNewline() {
		return newline;
	}
	public void setNewline(boolean newline) {
		this.newline = newline;
	}
	public int getBufsizl() {
		return bufsizl;
	}
	public void setBufsizl(int bufsizl) {
		this.bufsizl = bufsizl;
	}
	public int getBuftriggerperci() {
		return buftriggerperci;
	}
	public void setBuftriggerperci(int buftriggerperci) {
		this.buftriggerperci = buftriggerperci;
	}
	public int getBufaddperci() {
		return bufaddperci;
	}
	public void setBufaddperci(int bufaddperci) {
		this.bufaddperci = bufaddperci;
	} 
}
