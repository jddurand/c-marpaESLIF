package org.parser.marpa;

public class ESLIFBuffer {
	private boolean eof       = false;
	private boolean character = false;
	private String  encoding  = null;
	private byte[]  buffer; 

	public boolean isEof() {
		return eof;
	}
	public void setEof(boolean eof) {
		this.eof = eof;
	}
	public boolean isCharacter() {
		return character;
	}
	public void setCharacter(boolean character) {
		this.character = character;
	}
	public String getEncoding() {
		return encoding;
	}
	public void setEncoding(String encoding) {
		this.encoding = encoding;
	}
	public byte[] getBuffer() {
		return buffer;
	}
	public void setBuffer(byte[] buffer) {
		this.buffer = buffer;
	}
}
