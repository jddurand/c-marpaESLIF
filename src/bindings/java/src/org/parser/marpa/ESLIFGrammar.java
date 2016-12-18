package org.parser.marpa;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;

public class ESLIFGrammar {
	private ESLIF       eslif         = null;
	private ByteBuffer  marpaGrammarp = null;
	private native void newp(byte[] utf8);
	private native void freev();
	private native int  ngrammari();
	
	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	public ESLIFGrammar(ESLIF eslif, String grammar) throws UnsupportedEncodingException {
		this.eslif = eslif;
		newp(grammar.getBytes("UTF-8"));
	}

	public ESLIFLoggerInterface getLoggerInterface() {
		ESLIFLoggerInterface loggerInterface = null;
		if (this.eslif != null) {
			loggerInterface = eslif.getLoggerInterface();
		}
		return loggerInterface;
	}
	
	public void free() {
		freev();
	}
	
	public int ngrammar() {
		return ngrammari();
	}
	
	/*
	 * ********************************************
	 * Private methods - used by the JNI
	 * ********************************************
	 */
	private ESLIF getEslif() {
		return eslif;
	}
	private ByteBuffer getMarpaGrammarp() {
		return marpaGrammarp;
	}
	private void setMarpaGrammarp(ByteBuffer marpaGrammarp) {
		this.marpaGrammarp = marpaGrammarp;
	}

}
