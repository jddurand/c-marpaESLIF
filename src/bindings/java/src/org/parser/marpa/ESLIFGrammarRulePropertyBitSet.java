package org.parser.marpa;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;

/**
 * ESLIFLoggerLevel is an enumeration of all ESLIF logging levels.
 * <p>
 * This class is explicitly reproducing the formalism of the <a href="http://github.com/jddurand/c-genericLogger" target="_top">genericLogger</a> C library.
 * 
 * @see ESLIFLoggerInterface
 */
public enum ESLIFGrammarRulePropertyBitSet {

	/* Thanks to http://www.ajaxonomy.com/2007/java/making-the-most-of-java-50-enum-tricks */

	MARPAESLIF_RULE_IS_ACCESSIBLE(0x01),
	MARPAESLIF_RULE_IS_NULLABLE(0x02),
	MARPAESLIF_RULE_IS_NULLING(0x04),
	MARPAESLIF_RULE_IS_LOOP(0x08),
	MARPAESLIF_RULE_IS_PRODUCTIVE(0x10);

	private int code;
	private static final Map<Integer,ESLIFGrammarRulePropertyBitSet> lookup = new HashMap<Integer,ESLIFGrammarRulePropertyBitSet>();

	static {
		for(ESLIFGrammarRulePropertyBitSet s : EnumSet.allOf(ESLIFGrammarRulePropertyBitSet.class)) {
			lookup.put(s.getCode(), s);
		}
	}

	private ESLIFGrammarRulePropertyBitSet(int code) {
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
	public static ESLIFGrammarRulePropertyBitSet get(int code) { 
		return lookup.get(code); 
	}
	
}
