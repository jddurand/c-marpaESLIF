package org.parser.marpa;

public class AppEmptyRecognizer implements ESLIFRecognizerInterface {
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
}
