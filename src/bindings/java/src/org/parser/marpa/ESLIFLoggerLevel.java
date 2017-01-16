package org.parser.marpa;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;

/**
 * ESLIFLoggerLevel is a enumeration of all ESLIF logging levels.
 * <p>
 * This class is explicitly reproducing the formalism of the <a href="http://github.com/jddurand/c-genericLogger" target="_top">genericLogger C library</a>.
 * 
 * @see ESLIFLoggerInterface
 */
public enum ESLIFLoggerLevel {

	/* Thanks to http://www.ajaxonomy.com/2007/java/making-the-most-of-java-50-enum-tricks */

	/**
	 * TRACE log level, value is {@code 0}
	 */
	TRACE    (0),
	/**
	 * DEBUG log level, value is {@code 1}
	 */
	DEBUG    (1),
	/**
	 * INFO log level, value is {@code 2}
	 */
	INFO     (2),
	/**
	 * NOTICE log level, value is {@code 3}
	 */
	NOTICE   (3),
	/**
	 * WARNING log level, value is {@code 4}
	 */
	WARNING  (4),
	/**
	 * ERROR log level, value is {@code 5}
	 */
	ERROR    (5),
	/**
	 * CRITICAL log level, value is {@code 6}
	 */
	CRITICAL (6),
	/**
	 * ALERT log level, value is {@code 7}
	 */
	ALERT    (7),
	/**
	 * EMERGENCY log level, value is {@code 8}
	 */
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
	 * Get the value associated to a log level.
	 * 
	 * @return the log level value
	 */
	public int getCode() {
		return code;
	}

	public static ESLIFLoggerLevel get(int code) { 
		return lookup.get(code); 
	}
	
}
