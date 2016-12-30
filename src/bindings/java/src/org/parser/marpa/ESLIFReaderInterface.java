package org.parser.marpa;

public interface ESLIFReaderInterface {
	public short   read();
	public boolean isEof();
	public boolean isCharacterStream();
	public String  encoding();
	public byte[]  data();
}
