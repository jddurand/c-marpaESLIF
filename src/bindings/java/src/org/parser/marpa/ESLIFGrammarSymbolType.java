package org.parser.marpa;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;

/**
 * ESLIFGrammarSymbolType is an enumeration to disambiguate symbol types, that can be either a terminal, either a meta-symbol.
 */
public enum ESLIFGrammarSymbolType {

	/* Thanks to http://www.ajaxonomy.com/2007/java/making-the-most-of-java-50-enum-tricks */

	MARPAESLIF_SYMBOLTYPE_TERMINAL(0),
	MARPAESLIF_SYMBOLTYPE_META(1);

	private int code;
	private static final Map<Integer,ESLIFGrammarSymbolType> lookup = new HashMap<Integer,ESLIFGrammarSymbolType>();

	static {
		for(ESLIFGrammarSymbolType s : EnumSet.allOf(ESLIFGrammarSymbolType.class)) {
			lookup.put(s.getCode(), s);
		}
	}

	private ESLIFGrammarSymbolType(int code) {
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
	public static ESLIFGrammarSymbolType get(int code) { 
		return lookup.get(code); 
	}
	
}
