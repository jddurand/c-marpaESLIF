package org.parser.marpa;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;

/**
 * ESLIFSymbolEventBitSet is an enumeration of all low-level symbol event, directly coming from libmarpa.
 */
public enum ESLIFSymbolEventBitSet {

	/* Thanks to http://www.ajaxonomy.com/2007/java/making-the-most-of-java-50-enum-tricks */

	/**
	 * Set if symbol has the completion event
	 */
	MARPAESLIF_SYMBOL_EVENT_COMPLETION(0x01),
	/**
	 * Set if symbol is has the nulled event
	 */
	MARPAESLIF_SYMBOL_EVENT_NULLED(0x02),
	/**
	 * Set if symbol has the prediction event
	 */
	MARPAESLIF_SYMBOL_EVENT_PREDICTION(0x04);

	private int code;
	private static final Map<Integer,ESLIFSymbolEventBitSet> lookup = new HashMap<Integer,ESLIFSymbolEventBitSet>();

	static {
		for(ESLIFSymbolEventBitSet s : EnumSet.allOf(ESLIFSymbolEventBitSet.class)) {
			lookup.put(s.getCode(), s);
		}
	}

	private ESLIFSymbolEventBitSet(int code) {
		this.code = code;
	}

	/**
	 * Get the code value associated to an ESLIFSymbolEventBitSet
	 * 
	 * @return the code value
	 */
	public int getCode() {
		return code;
	}

	/**
	 * Get an ESLIFSymbolEventBitSet from a code value
	 * 
	 * @param code the code value
	 * @return the ESLIFSymbolEventBitSet
	 */
	public static ESLIFSymbolEventBitSet get(int code) { 
		return lookup.get(code); 
	}
	
}
