package org.parser.marpa;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;

/**
 * ESLIFRulePropertyBitSet is an enumeration of all low-level Rule properties, directly coming from libmarpa.
 */
public enum ESLIFRulePropertyBitSet {

	/* Thanks to http://www.ajaxonomy.com/2007/java/making-the-most-of-java-50-enum-tricks */

	/**
	 * Set if the rule is accessible
	 */
	MARPAESLIF_RULE_IS_ACCESSIBLE(0x01),
	/**
	 * Set if the rule is nullable
	 */
	MARPAESLIF_RULE_IS_NULLABLE(0x02),
	/**
	 * Set if the rule is nulling
	 */
	MARPAESLIF_RULE_IS_NULLING(0x04),
	/**
	 * Set if this is a loop rule
	 */
	MARPAESLIF_RULE_IS_LOOP(0x08),
	/**
	 * Set if the rule is productive
	 */
	MARPAESLIF_RULE_IS_PRODUCTIVE(0x10);

	private int code;
	private static final Map<Integer,ESLIFRulePropertyBitSet> lookup = new HashMap<Integer,ESLIFRulePropertyBitSet>();

	static {
		for(ESLIFRulePropertyBitSet s : EnumSet.allOf(ESLIFRulePropertyBitSet.class)) {
			lookup.put(s.getCode(), s);
		}
	}

	private ESLIFRulePropertyBitSet(int code) {
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
	public static ESLIFRulePropertyBitSet get(int code) { 
		return lookup.get(code); 
	}
	
}
