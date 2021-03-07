package org.parser.marpa;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;

/**
 * ESLIFSymbol is a symbol external to grammars that can be used on any raw input or running recognizer.
 * There are two types of external symbol: regex and string. Both have to follow ESLIF grammar semantic.
 *
 * <pre>
 * ESLIF eslif = new ESLIF(...)
 * ESLIFSymbol eslifStringSymbol = new ESLIFGrammar("string", "'('");
 * ESLIFSymbol eslifStringRegex = new ESLIFGrammar("regex", "[\\d]+");
 * ...
 * eslifSymbol.free();
 * eslif.free()
 * </pre>
 */
public class ESLIFSymbol {
	private ESLIF          eslif             = null;
	private ByteBuffer     marpaESLIFSymbolp = null;
	private ByteBuffer     marpaESLIFSymbolContextp = null;
	private native void    jniNew(String type, byte[] utf8, String modifiers) throws ESLIFException;
	private native void    jniFree() throws ESLIFException;
	private native Object  jniTest(byte[] bytes) throws ESLIFException;
	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	/**
	 * Creation of an ESLIFSymbol instance
	 * 
	 * @param eslif an instance of ESLIF
	 * @param type is the symbol type, must be "regex" or "string"
	 * @param modifiers is the eventual symbol modifier, can be null
	 * @throws UnsupportedEncodingException if internal grammar cannot be converted to strict UTF-8
	 * @throws ESLIFException if grammar parse failed
	 */
	public ESLIFSymbol(ESLIF eslif, String type, String input, String modifiers) throws UnsupportedEncodingException, ESLIFException {
		if (eslif == null) {
			throw new IllegalArgumentException("eslif must not be null");
		}
		if (type == null) {
			throw new IllegalArgumentException("type must not be null");
		}
		if (! type.equals("regex") && ! type.equals("string")) {
			throw new IllegalArgumentException("type must be \"regex\" or \"string\"");
		}
		if (input == null) {
			throw new IllegalArgumentException("input must not be null");
		}
		setEslif(eslif);
		jniNew(type, input.getBytes("UTF-8"), modifiers);
	}

	/**
	 * Creation of an ESLIFSymbol instance
	 * 
	 * @param eslif an instance of ESLIF
	 * @param type is the symbol type, must be "regex" or "string"
	 * @param modifiers is the eventual symbol modifier, can be null
	 * @throws UnsupportedEncodingException if internal grammar cannot be converted to strict UTF-8
	 * @throws ESLIFException if grammar parse failed
	 */
	public ESLIFSymbol(ESLIF eslif, String type, String input) throws UnsupportedEncodingException, ESLIFException {
		this(eslif, type, input, null);
	}

		/* For subclasses that need only eslif parameter */
	protected ESLIFSymbol(ESLIF eslif) throws ESLIFException {
		if (eslif == null) {
			throw new IllegalArgumentException("eslif must not be null");
		}
		setEslif(eslif);
	}
	/**
	 * Dispose of an ESLIFSymbol resource.
	 * 
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized void free() throws ESLIFException {
		jniFree();
	}
	/**
	 * Try to match the symbol on any input. 
	 * 
	 * @param the input bytes
	 * @throws ESLIFException if the interface failed
	 * 
	 * @return null if no match, else the matched bytes
	 */
	public synchronized Object test(byte[] bytes) throws ESLIFException {
		return jniTest(bytes);
	}
	/*
	 * ********************************************
	 * Private methods - used by the JNI
	 * ********************************************
	 */
	protected ESLIFLoggerInterface getLoggerInterface() {
		return (eslif != null) ? eslif.getLoggerInterface() : null;
	}
	protected ESLIF getEslif() {
		return eslif;
	}
	protected void setEslif(ESLIF eslif) {
		this.eslif = eslif;
	}
	protected ByteBuffer getMarpaESLIFSymbolp() {
		return marpaESLIFSymbolp;
	}
	protected void setMarpaESLIFSymbolp(ByteBuffer marpaESLIFSymbolp) {
		this.marpaESLIFSymbolp = marpaESLIFSymbolp;
	}
	private ByteBuffer getMarpaESLIFSymbolContextp() {
		return marpaESLIFSymbolContextp;
	}
	private void setMarpaESLIFSymbolContextp(ByteBuffer marpaESLIFSymbolContextp) {
		this.marpaESLIFSymbolContextp = marpaESLIFSymbolContextp;
	}
}
