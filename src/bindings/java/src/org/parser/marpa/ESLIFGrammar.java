package org.parser.marpa;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;

public class ESLIFGrammar {
	private ESLIF       eslif         = null;
	private ByteBuffer  marpaGrammarp = null;
	private native void jniNew(byte[] utf8);
	private native void jniFree();
	private native int  jniNgrammar();
	
	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	public ESLIFGrammar(ESLIF eslif, String grammar) throws UnsupportedEncodingException {
		this.eslif = eslif;
		jniNew(grammar.getBytes("UTF-8"));
	}

	public void free() {
		jniFree();
	}
	
	public int ngrammar() {
		return jniNgrammar();
	}
	
	/*
	 * ********************************************
	 * Private methods - used by the JNI
	 * ********************************************
	 */
	protected ESLIFLoggerInterface getLoggerInterface() {
		return eslif != null ? eslif.getLoggerInterface() : null;
	}
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
