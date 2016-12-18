/**
 * 
 */
package org.parser.marpa;

import java.nio.ByteBuffer;

/**
 * @author Jean-Damien DUrand
 *
 */
public class ESLIF {
	final static String MARPAESLIFJAVA_LIBRARY_NAME = "marpaESLIFJava";

	private ESLIFLoggerInterface loggerInterface       = null;
	private ByteBuffer           marpaESLIFp           = null;
	private ByteBuffer           genericLoggerContextp = null;
	private ByteBuffer           genericLoggerp        = null;
	private native void          newp();
	private native void          freev();
	private native static String versions();
	
	static {
		try {
			System.loadLibrary(MARPAESLIFJAVA_LIBRARY_NAME);
		} catch (Exception e) {
			System.err.println("Failed to load " + MARPAESLIFJAVA_LIBRARY_NAME + ", " + e);
			System.exit(1);
		}
	}
	
	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	public ESLIF(ESLIFLoggerInterface loggerInterface) throws Exception {
		this.loggerInterface = loggerInterface;
		newp();
	}
	
	public ESLIF() throws Exception {
		newp();
	}
	
	public void free() {
		freev();
	}
	
	final public static String version() {
		return versions();
	}

	public ESLIFLoggerInterface getLoggerInterface() {
		return loggerInterface;
	}

	/*
	 * ********************************************
	 * Private methods - used by the JNI
	 * ********************************************
	 */
	private ByteBuffer getMarpaESLIFp() {
		return marpaESLIFp;
	}
	private void setMarpaESLIFp(ByteBuffer marpaESLIFp) {
		this.marpaESLIFp = marpaESLIFp;
	}
	private ByteBuffer getGenericLoggerContextp() {
		return genericLoggerContextp;
	}
	private void setGenericLoggerContextp(ByteBuffer genericLoggerContextp) {
		this.genericLoggerContextp = genericLoggerContextp;
	}
	private ByteBuffer getGenericLoggerp() {
		return genericLoggerp;
	}
	private void setGenericLoggerp(ByteBuffer genericLoggerp) {
		this.genericLoggerp = genericLoggerp;
	}
}
