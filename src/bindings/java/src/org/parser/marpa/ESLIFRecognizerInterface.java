package org.parser.marpa;

public interface ESLIFRecognizerInterface {
	public short   read();
	public boolean isEof();
	public boolean isCharacterStream();
	public String  encoding();
	public byte[]  data();
	public boolean isWithDisableThreshold();
	public boolean isWithExhaustion();
	public boolean isWithNewline();
}
