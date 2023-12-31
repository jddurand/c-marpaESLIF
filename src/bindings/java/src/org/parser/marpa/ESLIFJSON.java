package org.parser.marpa;

/**
 * ESLIFJSON is a built-in ESLIFGrammar, targetting the JSON syntax, with an optional strict mode.
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
 * ESLIFJSON eslifJSON = new ESLIFJSON();
 * ...
 * eslifJSON.encode(someObject);
 * eslifJSON.decode("\"JSON string\"");
 * ...
 * eslifJSON.free();
 * eslif.free()
 * </pre>
 */
public class ESLIFJSON {
	private ESLIFJSONDecoder eslifJSONDecoder;
	private ESLIFJSONEncoder eslifJSONEncoder;

	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	/**
	 * Creation of an ESLIFJSON instance
	 * 
	 * @param eslif an instance of ESLIF
	 * @param strict the strict mode
	 * @throws ESLIFException if creation failed
	 * 
	 * default strict mode is true.
	 */
	public ESLIFJSON(ESLIF eslif, boolean strict) throws ESLIFException {
		this.eslifJSONDecoder = new ESLIFJSONDecoder(eslif, strict);
		this.eslifJSONEncoder = new ESLIFJSONEncoder(eslif, strict);
	}

	/**
	 * Creation of an ESLIFJSON instance
	 * 
	 * @param eslif an instance of ESLIF
	 * @throws ESLIFException if creation failed
	 * 
	 * Strict mode defaults to true.
	 */
	public ESLIFJSON(ESLIF eslif) throws ESLIFException {
		this(eslif, true);
	}

	/**
	 * Dispose of an ESLIFJSONDecoder resource.
	 * 
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized void free() throws ESLIFException {
		this.eslifJSONDecoder.free();
		this.eslifJSONEncoder.free();
	}

	/**
	 * Decodes a string to a java object
	 *
	 * @param s the string to decode
	 * @throws ESLIFException if the interface failed
	 * @return the decode object
	 */
	public synchronized Object decode(String s) throws ESLIFException {
		return this.eslifJSONDecoder.decode(s);
	}

	/**
	 * Decodes a string to a java object
	 *
	 * @param s the string to decode
	 * @param eslifJSONDecoderOption the options for decoding
	 * @throws ESLIFException if the interface failed
	 * @return the decode object
	 */
	public synchronized Object decode(String s, ESLIFJSONDecoderOption eslifJSONDecoderOption) throws ESLIFException {
		return this.eslifJSONDecoder.decode(s, eslifJSONDecoderOption);
	}

	/**
	 * Encodes a java object to a JSON string
	 *
	 * @param o the object to encode
	 * @throws ESLIFException if the interface failed
	 * @return the encoded object
	 */
	public synchronized String encode(Object o) throws ESLIFException {
		return this.eslifJSONEncoder.encode(o);
	}
}
