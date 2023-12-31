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
	private native void    jniNew(String type, byte[] utf8, String modifiers, byte[] substitutionUtf8, String substitutionModifiers) throws ESLIFException;
	private native void    jniMetaNew(ESLIFGrammar eslifGrammar, String symbol) throws ESLIFException;
	private native void    jniFree() throws ESLIFException;
	private native byte[]  jniTest(byte[] bytes) throws ESLIFException;
	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	/**
	 * Creation of an ESLIFSymbol instance for string and regex literals
	 * 
	 * @param eslif an instance of ESLIF
	 * @param type is the symbol type, must be "regex" or "string"
	 * @param input is the symbol definition
	 * @param modifiers is the eventual symbol modifier, can be null
	 * @param substitutionInput is the symbol substitution definition - only for "regex" type
	 * @param substitutionModifiers is the eventual symbol substitution modifier, can be null - only for "regex" type
	 * @throws UnsupportedEncodingException if internal grammar cannot be converted to strict UTF-8
	 * @throws ESLIFException if grammar parse failed
	 */
	public ESLIFSymbol(ESLIF eslif, String type, String input, String modifiers, String substitutionInput, String substitutionModifiers) throws UnsupportedEncodingException, ESLIFException {
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
		jniNew(type, input.getBytes("UTF-8"), modifiers, substitutionInput != null ? substitutionInput.getBytes("UTF-8") : null, substitutionModifiers);
	}

	/**
	 * Creation of an ESLIFSymbol instance for lexemes
	 * 
	 * @param eslif an instance of ESLIF
	 * @param eslifGrammar an instance of ESLIFGrammar
	 * @param symbol the lexeme symbol
	 * @throws ESLIFException in case of creation failure
	 */
	public ESLIFSymbol(ESLIF eslif, ESLIFGrammar eslifGrammar, String symbol) throws ESLIFException {
		if (eslifGrammar == null) {
			throw new IllegalArgumentException("eslifGrammar must not be null");
		}
		if (symbol == null) {
			throw new IllegalArgumentException("symbol must not be null");
		}
		setEslif(eslif);
		jniMetaNew(eslifGrammar, symbol);
	}

	/**
	 * Creation of an ESLIFSymbol instance for string and regex literals
	 * 
	 * @param eslif an instance of ESLIF
	 * @param type is the symbol type, must be "regex" or "string"
	 * @param input is the symbol definition
	 * @param modifiers is the eventual symbol modifier, can be null
	 * @throws UnsupportedEncodingException if internal grammar cannot be converted to strict UTF-8
	 * @throws ESLIFException if grammar parse failed
	 */
	public ESLIFSymbol(ESLIF eslif, String type, String input, String modifiers) throws UnsupportedEncodingException, ESLIFException {
		this(eslif, type, input, modifiers, null, null);
	}

	/**
	 * Creation of an ESLIFSymbol instance for string and regex literals
	 * 
	 * @param eslif an instance of ESLIF
	 * @param type is the symbol type, must be "regex" or "string"
	 * @param input is the symbol definition
	 * @throws UnsupportedEncodingException if internal grammar cannot be converted to strict UTF-8
	 * @throws ESLIFException if grammar parse failed
	 */
	public ESLIFSymbol(ESLIF eslif, String type, String input) throws UnsupportedEncodingException, ESLIFException {
		this(eslif, type, input, null, null, null);
	}

	/* For subclasses that need only eslif parameter */
	/**
	 * Creation of an ESLIFSymbol instance for string and regex literals
	 * 
	 * @param eslif an instance of ESLIF
	 * @throws ESLIFException if grammar parse failed
	 * @throws IllegalArgumentException if eslif argument is null
	 */
	protected ESLIFSymbol(ESLIF eslif) throws ESLIFException, IllegalArgumentException {
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
	 * @param bytes the input bytes
	 * @throws ESLIFException if the interface failed
	 * 
	 * @return null if no match, else the matched bytes
	 */
	public synchronized byte[] test(byte[] bytes) throws ESLIFException {
		return jniTest(bytes);
	}
	/*
	 * ********************************************
	 * Private methods - used by the JNI
	 * ********************************************
	 */
	/**
	 * Get logger instance
	 * 
	 * @return the ESLIFLoggerInterface logger instance, which can be null
	 */
	protected ESLIFLoggerInterface getLoggerInterface() {
		return (eslif != null) ? eslif.getLoggerInterface() : null;
	}
	/**
	 * Get eslif
	 * 
	 * @return the ESLIF instance
	 */
	protected ESLIF getEslif() {
		return eslif;
	}
	/**
	 * Set eslif
	 * 
	 * @param eslif the ESLIF instance
	 */
	protected void setEslif(ESLIF eslif) {
		this.eslif = eslif;
	}
	/**
	 * Get ESLIF symbol internal pointer
	 * 
	 * @return the ESLIF symbol internal pointer
	 */
	protected ByteBuffer getMarpaESLIFSymbolp() {
		return marpaESLIFSymbolp;
	}
	/**
	 * Set ESLIF symbol internal pointer
	 * 
	 * @param marpaESLIFSymbolp the ESLIF symbol internal pointer
	 */
	protected void setMarpaESLIFSymbolp(ByteBuffer marpaESLIFSymbolp) {
		this.marpaESLIFSymbolp = marpaESLIFSymbolp;
	}
	/**
	 * Get ESLIF symbol internal context
	 * 
	 * @return the ESLIF symbol internal context
	 */
	private ByteBuffer getMarpaESLIFSymbolContextp() {
		return marpaESLIFSymbolContextp;
	}
	/**
	 * Set ESLIF symbol internal context
	 * 
	 * @param marpaESLIFSymbolContextp the ESLIF symbol internal context
	 */
	private void setMarpaESLIFSymbolContextp(ByteBuffer marpaESLIFSymbolContextp) {
		this.marpaESLIFSymbolContextp = marpaESLIFSymbolContextp;
	}
}
