package org.parser.marpa;

import java.util.Arrays;

public class AppEmptyRecognizer implements ESLIFRecognizerInterface {
	private ESLIFLoggerInterface eslifLogger;
	private ESLIFRecognizer eslifRecognizer;
	
	public AppEmptyRecognizer(ESLIFLoggerInterface eslifLogger) {
		this.eslifLogger = eslifLogger;
	}

	public boolean read() {
		return true;
	}

	public boolean isEof() {
		return true;
	}

	public boolean isCharacterStream() {
		return false;
	}

	public String encoding() {
		return null;
	}

	public byte[] data() {
		return new byte[1]; // One dummy byte
	}

	public boolean isWithDisableThreshold() {
		return false;
	}

	public boolean isWithExhaustion() {
		return false;
	}

	public boolean isWithNewline() {
		return true;
	}
	
	public boolean isWithTrack() {
		return true;
	}

	public boolean java_eventAction(ESLIFEvent[] eventArray) {
		this.eslifLogger.debug("event_action(" + Arrays.toString(eventArray) + ")");
		return true;
	}

	public void setEslifRecognizer(ESLIFRecognizer eslifRecognizer) {
		this.eslifRecognizer = eslifRecognizer;
	}

	public ESLIFRecognizer getEslifRecognizer() {
		return this.eslifRecognizer;
	}	
}
