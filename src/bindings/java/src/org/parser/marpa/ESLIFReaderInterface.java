package org.parser.marpa;

public interface ESLIFReaderInterface {
	public byte[] read();
	public boolean isEof();
	public boolean isCharacterStream();
	public String  encoding();
}
