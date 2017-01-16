package org.parser.marpa;

import java.nio.ByteBuffer;

/**
 * ESLIF is Extended ScanLess InterFace
 * <p>
 * ESLIF is an extension of perl's <a href="https://metacpan.org/pod/Marpa::R2" target="_top">Marpa::R2</a> BNF,
 * written as a stand-alone <a href="http://github.com/jddurand/c-marpaESLIF" target="_top">marpaESLIF</a> library.
 * <p>
 * Please note that the call to the {@link #free()} method is required to dispose the resources allocated by constructors.
 * <p>
 * Example:
 * <pre>
 * ESLIF eslif = null;
 * try &#123;
 *   eslif = new ESLIF();
 *   // Your work
 * &#125; catch (ESLIFException e) &#123;
 *  // ...
 * &#125; finally &#123;
 *   if (eslif != null) &#123;
 *     try &#123;
 *       eslif.free();
 *     &#125; catch (ESLIFException e) &#123;
 *       // ...
 *     &#125;
 *   &#125;
 * &#125;
 * </pre>
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
	 * @throws ESLIFException when the C library returns {@code NULL}
	 * 
	 * <p>
	 * See {@link <a href="https://github.com/jddurand/c-marpaESLIF/tree/master/doc/API#marpaESLIF_newp" "target="_top">marpaESLIF_newp</a>}.
	 */
	public ESLIF() throws ESLIFException {
		jniNew();
	}
	
	/**
	 * Dispose of ESLIF resources
	 * <p>
	 * This call is <b>required</b> when you do not need anymore the ESLIF instance.
	 * <p>
	 * See {@link <a href="https://github.com/jddurand/c-marpaESLIF/tree/master/doc/API#marpaESLIF_freev" "target="_top">marpaESLIF_freev</a>}.
	 */
	public synchronized void free() throws ESLIFException {
		jniFree();
	}
	
	/**
	 * Version of the ESLIF
	 * 
	 * @return a string describing the ESLIF version, never null
	 * 
	 * <p>
	 * See {@link <a href="https://github.com/jddurand/c-marpaESLIF/tree/master/doc/API#marpaESLIF_versions" "target="_top">marpaESLIF_versions</a>}.
	 */
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
