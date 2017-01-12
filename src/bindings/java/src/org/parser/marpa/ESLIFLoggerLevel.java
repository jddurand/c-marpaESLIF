package org.parser.marpa;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;

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

	public int getCode() {
		return code;
	}

	public static ESLIFLoggerLevel get(int code) { 
		return lookup.get(code); 
	}
	
}
