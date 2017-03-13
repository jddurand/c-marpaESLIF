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
 * This class and its derivatives are thread-safe. Although there can be many ESLIF instance, in practice a single instance is enough,
 * unless you want different logging interfaces.
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
	 * 
	 * @param loggerInterface instance of a {@link ESLIFLoggerInterface}, may be null
	 * @throws ESLIFException exception in case of JNI error or object creation failure
	 */
	public ESLIF(ESLIFLoggerInterface loggerInterface) throws ESLIFException {
		setLoggerInterface(loggerInterface);
		jniNew();
	}
	
	/**
	 * Equivalent to {@link #ESLIF(ESLIFLoggerInterface)}, giving a null parameter for the logger interface.
	 * 
	 * @throws ESLIFException exception in case of JNI error or object creation failure
	 */
	public ESLIF() throws ESLIFException {
		jniNew();
	}
	
	/**
	 * Dispose of ESLIF resources.
	 * <p>
	 * This call is <b>required</b> when you do not need anymore the ESLIF instance.
	 * 
	 * @throws ESLIFException exception in case of JNI error
	 */
	public synchronized void free() throws ESLIFException {
		jniFree();
	}
	
	/**
	 * Version of the ESLIF.
	 * 
	 * @return a string describing the ESLIF version, never null
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
