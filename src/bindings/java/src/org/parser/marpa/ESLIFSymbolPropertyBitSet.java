package org.parser.marpa;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;

/**
 * ESLIFSymbolPropertyBitSet is an enumeration of all low-level Rule properties, directly coming from libmarpa.
 */
public enum ESLIFSymbolPropertyBitSet {

	/* Thanks to http://www.ajaxonomy.com/2007/java/making-the-most-of-java-50-enum-tricks */

	MARPAESLIF_SYMBOL_IS_ACCESSIBLE(0x01),
	MARPAESLIF_SYMBOL_IS_NULLABLE(0x02),
	MARPAESLIF_SYMBOL_IS_NULLING(0x04),
	MARPAESLIF_SYMBOL_IS_PRODUCTIVE(0x08),
	MARPAESLIF_SYMBOL_IS_START(0x10),
	MARPAESLIF_SYMBOL_IS_TERMINAL(0x10);

	private int code;
	private static final Map<Integer,ESLIFSymbolPropertyBitSet> lookup = new HashMap<Integer,ESLIFSymbolPropertyBitSet>();

	static {
		for(ESLIFSymbolPropertyBitSet s : EnumSet.allOf(ESLIFSymbolPropertyBitSet.class)) {
			lookup.put(s.getCode(), s);
		}
	}

	private ESLIFSymbolPropertyBitSet(int code) {
		this.code = code;
	}

	/**
	 * Get the log level value associated to an instance of ESLIFLoggerLevel
	 * 
	 * @return the log level value
	 */
	public int getCode() {
		return code;
	}

	/**
	 * Get an instance of ESLIFLoggerLevel from a log level value
	 * 
	 * @param code the log level value
	 * @return the ESLIFLoggerLevel instance
	 */
	public static ESLIFSymbolPropertyBitSet get(int code) { 
		return lookup.get(code); 
	}
	
}
