package org.parser.marpa;

import java.nio.ByteBuffer;

/**
 * ESLIFJSONEncoder is a built-in ESLIFGrammar, targetting the JSON syntax, with an optional strict mode.
 *
 * <pre>
 * ESLIF eslif = new ESLIF(...)
 * ESLIFJSONEncoder eslifJSONEncoder = new ESLIFJSONEncoder();
 * ...
 * eslifJSONEncoder.free();
 * eslif.free()
 * </pre>
 */
public class ESLIFJSONEncoder extends ESLIFGrammar {
	private ByteBuffer     marpaESLIFJSONEncoderp = null;
	private native void    jniNew(boolean strict) throws ESLIFException;
	private native void    jniFree() throws ESLIFException;
	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	/**
	 * Creation of an ESLIFJSONEncoder instance
	 * 
	 * @param eslif an instance of ESLIF
	 * @param strict optional strict mode
	 * @throws ESLIFException if creation failed
	 */
	public ESLIFJSONEncoder(ESLIF eslif, boolean strict) throws ESLIFException {
		super(eslif);
		jniNew(strict);
	}

	public ESLIFJSONEncoder(ESLIF eslif) throws ESLIFException {
		this(eslif, false);
	}

	/**
	 * Dispose of an ESLIFJSONEncoder resource.
	 * 
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized void free() throws ESLIFException {
		jniFree();
	}
}
