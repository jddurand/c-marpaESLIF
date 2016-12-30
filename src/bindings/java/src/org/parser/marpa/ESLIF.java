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
	private final static String  MARPAESLIFJAVA_LIBRARY_NAME = "marpaESLIFJava";
	private ESLIFLoggerInterface loggerInterface       = null;
	private ByteBuffer           marpaESLIFp           = null;
	private ByteBuffer           genericLoggerContextp = null;
	private ByteBuffer           genericLoggerp        = null;
	private native void          jniNew(ESLIFLoggerInterface loggerInterface);
	private native void          jniFree();
	private native String        jniVersion();
	
	static {
		try {
			System.loadLibrary(getMarpaeslifjavaLibraryName());
		} catch (Exception e) {
			System.err.println("Failed to load " + getMarpaeslifjavaLibraryName() + ", " + e);
			System.exit(1);
		}
	}
	
	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	public static String getMarpaeslifjavaLibraryName() {
		return MARPAESLIFJAVA_LIBRARY_NAME;
	}
	public ESLIF(ESLIFLoggerInterface loggerInterface) {
		setLoggerInterface(loggerInterface);
		jniNew(loggerInterface);
	}
	
	public ESLIF() throws Exception {
		jniNew(null);
	}
	
	public void free() {
		jniFree();
	}
	
	public String version() {
		return jniVersion();
	}
	/*
	 * *******************************************
	 * Private/protected methods - used by the JNI
	 * *******************************************
	 */
	protected ESLIFLoggerInterface getLoggerInterface() {
		return loggerInterface;
	}
	private void setLoggerInterface(ESLIFLoggerInterface loggerInterface) {
		this.loggerInterface = loggerInterface;
	}
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
