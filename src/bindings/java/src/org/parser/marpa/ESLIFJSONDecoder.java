package org.parser.marpa;

import java.nio.ByteBuffer;

/**
 * ESLIFJSONDecoder is a built-in ESLIFGrammar, targetting the JSON syntax, with an optional strict mode.
 *
 * <pre>
 * ESLIF eslif = new ESLIF(...)
 * ESLIFJSONDecoder eslifJSONDecoder = new ESLIFJSONDecoder();
 * ...
 * eslifJSONDecoder.free();
 * eslif.free()
 * </pre>
 */
public class ESLIFJSONDecoder extends ESLIFGrammar {
	private ByteBuffer     marpaESLIFJSONDecoderp = null;
	private native void    jniNew(boolean strict) throws ESLIFException;
	private native void    jniFree() throws ESLIFException;
	private native Object  jniDecode(String s, ESLIFJSONDecoderOption eslifJSONDeoderOption) throws ESLIFException;
	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	/**
	 * Creation of an ESLIFJSONDecoder instance
	 * 
	 * @param eslif an instance of ESLIF
	 * @param strict optional strict mode
	 * @throws ESLIFException if creation failed
	 */
	public ESLIFJSONDecoder(ESLIF eslif, boolean strict) throws ESLIFException {
		super(eslif);
		jniNew(strict);
	}

	public ESLIFJSONDecoder(ESLIF eslif) throws ESLIFException {
		this(eslif, false);
	}

	/**
	 * Dispose of an ESLIFJSONDecoder resource.
	 * 
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized void free() throws ESLIFException {
		jniFree();
	}

	/**
	 * Decodes a string to a java object
	 *
	 * @param s the string to decode
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized Object decode(String s) throws ESLIFException {
		return decode(s);
	}

	/**
	 * Decodes a string to a java object
	 *
	 * @param s the string to decode
	 * @param eslifJSONDeoderOption the options for decoding
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized Object decode(String s, ESLIFJSONDecoderOption eslifJSONDeoderOption) throws ESLIFException {
		return jniDecode(s, eslifJSONDeoderOption);
	}

}
