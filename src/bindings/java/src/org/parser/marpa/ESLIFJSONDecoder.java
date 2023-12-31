package org.parser.marpa;

import java.nio.ByteBuffer;
import java.nio.charset.Charset;

/**
 * ESLIFJSONDecoder is a built-in ESLIFGrammar, targetting the JSON syntax, with an optional strict mode.
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
	private native Object  jniDecode(ESLIFJSONDecoderRecognizer eslifJSONDecoderRecognizer, ESLIFJSONDecoderOption eslifJSONDeoderOption) throws ESLIFException;
	private ESLIFRecognizerInterface eslifRecognizerInterface;

	/*
	 * ************************************************
	 * Internal ESLIFRecognizerInterface implementation
	 * ************************************************
	 */
	private class ESLIFJSONDecoderRecognizer implements ESLIFRecognizerInterface {
		private String s;
		private ESLIFRecognizer eslifRecognizer;
		
		public ESLIFJSONDecoderRecognizer(String s) {
			this.s = s;
		}
		
		@Override
		public boolean read() {
			return true;
		}

		@Override
		public boolean isEof() {
			return true;
		}

		@Override
		public boolean isCharacterStream() {
			return true;
		}

		@Override
		public String encoding() {
			if (s != null) {
				return Charset.defaultCharset().name();
			}
			return null;
		}

		@Override
		public byte[] data() {
			if (s != null) {
				return s.getBytes();
			}
			return null;
		}

		@Override
		public boolean isWithDisableThreshold() {
			return true;
		}

		@Override
		public boolean isWithExhaustion() {
			return false;
		}

		@Override
		public boolean isWithNewline() {
			return true;
		}

		@Override
		public boolean isWithTrack() {
			return false;
		}

		public void setEslifRecognizer(ESLIFRecognizer eslifRecognizer) {
			this.eslifRecognizer = eslifRecognizer;
		}

		public ESLIFRecognizer getEslifRecognizer() {
			return this.eslifRecognizer;
		}	

	}

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
	 * 
	 */
	public ESLIFJSONDecoder(ESLIF eslif, boolean strict) throws ESLIFException {
		super(eslif);
		jniNew(strict);
	}

	/**
	 * Creation of an ESLIFJSONDecoder instance
	 * 
	 * @param eslif an instance of ESLIF
	 * @throws ESLIFException if creation failed
	 * 
	 * Strict mode defaults to true.
	 */
	public ESLIFJSONDecoder(ESLIF eslif) throws ESLIFException {
		this(eslif, true);
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
	 * @return the decoded object
	 */
	public synchronized Object decode(String s) throws ESLIFException {
		return decode(s, new ESLIFJSONDecoderOption());
	}

	/**
	 * Decodes a string to a java object
	 *
	 * @param s the string to decode
	 * @param eslifJSONDecoderOption the options for decoding
	 * @throws ESLIFException if the interface failed
	 * @return the decoded object
	 */
	public synchronized Object decode(String s, ESLIFJSONDecoderOption eslifJSONDecoderOption) throws ESLIFException {
		ESLIFJSONDecoderRecognizer eslifJSONDecoderRecognizer = new ESLIFJSONDecoderRecognizer(s);
		return jniDecode(eslifJSONDecoderRecognizer, eslifJSONDecoderOption);
	}

}
