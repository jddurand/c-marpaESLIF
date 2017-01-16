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
public enum ESLIFLoggerLevel {

	/* Thanks to http://www.ajaxonomy.com/2007/java/making-the-most-of-java-50-enum-tricks */

	TRACE    (0),
	DEBUG    (1),
	INFO     (2),
	NOTICE   (3),
	WARNING  (4),
	ERROR    (5),
	CRITICAL (6),
	ALERT    (7),
	EMERGENCY(8);

	private int code;
	private static final Map<Integer,ESLIFLoggerLevel> lookup = new HashMap<Integer,ESLIFLoggerLevel>();

	static {
		for(ESLIFLoggerLevel s : EnumSet.allOf(ESLIFLoggerLevel.class)) {
			lookup.put(s.getCode(), s);
		}
	}

	private ESLIFLoggerLevel(int code) {
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
	public static ESLIFLoggerLevel get(int code) { 
		return lookup.get(code); 
	}
	
}
