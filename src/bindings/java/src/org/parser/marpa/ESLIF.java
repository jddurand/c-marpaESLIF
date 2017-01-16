/**
 * 
 */
package org.parser.marpa;

import java.nio.ByteBuffer;

/**
 * ESLIF is Extended ScanLess InterFace
 * <p>
 * ESLIF is an extension of perl's <a href="https://metacpan.org/pod/Marpa::R2" target="_top">Marpa::R2</a> BNF,
 * written as a stand-alone <a href="http://github.com/jddurand/c-marpaESLIF" target="_top">C library</a>.
 *
 * This class and its derivatives is thread-safe.
 */
public class ESLIF {
	private final static String  MARPAESLIFJAVA_LIBRARY_NAME = "marpaESLIFJava";
	private ESLIFLoggerInterface loggerInterface       = null;
	private ByteBuffer           marpaESLIFp           = null;
	private ByteBuffer           genericLoggerContextp = null;
	private ByteBuffer           genericLoggerp        = null;
	private native void          jniNew() throws ESLIFException;
	private native void          jniFree() throws ESLIFException;
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
	/**
	 * Constructor of the ESLIF
	 * 
	 * @param loggerInterface instance of a {@link ESLIFLoggerInterface}, may be null
	 * 
	 * @return an ESLIF instance
	 * 
	 * @throws ESLIFException when the C library returns {@code NULL}
	 * 
	 * <p>
	 * See {@link <a href="https://github.com/jddurand/c-marpaESLIF/tree/master/doc/API#marpaESLIF_newp" "target="_top">marpaESLIF_newp</a>}.
	 */
	public ESLIF(ESLIFLoggerInterface loggerInterface) throws ESLIFException {
		setLoggerInterface(loggerInterface);
		jniNew();
	}
	
	/**
	 * Constructor of the ESLIF
	 * <p>
	 * Equivalent to {@link #ESLIF(ESLIFLoggerInterface)}, giving a null parameter for the logger interface.
	 * 
	 * @return an ESLIF instance
	 * 
	 * @throws ESLIFException when the C library returns {@code NULL}
	 * 
	 * <p>
	 * See {@link <a href="https://github.com/jddurand/c-marpaESLIF/tree/master/doc/API#marpaESLIF_newp" "target="_top">marpaESLIF_newp</a>}.
	 */
	public ESLIF() throws ESLIFException {
		jniNew();
	}
	
	public synchronized void free() throws ESLIFException {
		jniFree();
	}
	
	/* This is accessing static inside the library */ 
	public String version() {
		return jniVersion();
	}
	/*
	 * *******************************************
	 * Private/protected methods - used by the JNI
	 * *******************************************
	 */
	private static String getMarpaeslifjavaLibraryName() {
		return MARPAESLIFJAVA_LIBRARY_NAME;
	}
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
