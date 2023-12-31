package org.parser.marpa;

/**
 * ESLIFJSONEncoder is a built-in ESLIFGrammar, targetting the JSON syntax, with an optional strict mode.
 *
 * Non-strict mode supports:
 * - Unlimited commas
 * - Trailing separator
 * - Perl style comment
 * - C++ style comment
 * - InfinityUnlimited commas
 * - NaN
 * - Unicode's control characters (range [\x00-\x1F]).
 *
 * <pre>
 * ESLIF eslif = new ESLIF(...)
 * ESLIFJSONEncoder eslifJSONEncoder = new ESLIFJSONEncoder();
 * ...
 * String stringInUTF8Encoding = eslifJSONEncoder.encode(anything);
 * ...
 * eslifJSONEncoder.free();
 * eslif.free()
 * </pre>
 */
public class ESLIFJSONEncoder extends ESLIFGrammar {
	private native void    jniNew(boolean strict) throws ESLIFException;
	private native void    jniFree() throws ESLIFException;
	private native String  jniEncode(Object o) throws ESLIFException;
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
	 * 
	 */
	public ESLIFJSONEncoder(ESLIF eslif, boolean strict) throws ESLIFException {
		super(eslif);
		jniNew(strict);
	}

	/**
	 * Creation of an ESLIFJSONEncoder instance
	 * 
	 * @param eslif an instance of ESLIF
	 * @throws ESLIFException if creation failed
	 * 
	 * Strict mode defaults to true.
	 */
	public ESLIFJSONEncoder(ESLIF eslif) throws ESLIFException {
		this(eslif, true);
	}

	/**
	 * Dispose of an ESLIFJSONEncoder resource.
	 * 
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized void free() throws ESLIFException {
		jniFree();
	}

	/**
	 * Encodes java data to a JSON UTF-8 string
	 * 
	 * @param o the object to encode
	 * @return the encoded string
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized String encode(Object o) throws ESLIFException {
		return jniEncode(o);
	}
}
