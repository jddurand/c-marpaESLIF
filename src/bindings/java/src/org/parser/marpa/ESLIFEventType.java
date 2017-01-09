package org.parser.marpa;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;

public enum ESLIFEventType {

	/* Thanks to http://www.ajaxonomy.com/2007/java/making-the-most-of-java-50-enum-tricks */

	NONE     (0x00),
	COMPLETED(0x01), /* Grammar event */
	NULLED   (0x02), /* Grammar event */
	PREDICTED(0x04), /* Grammar event */
	BEFORE   (0x08), /* Just before lexeme is commited */
	AFTER    (0x10), /* Just after lexeme is commited */
	EXHAUSTED(0x20), /* Exhaustion */
	DISCARD  (0x40); /* Discard */

	private int code;
	private static final Map<Integer,ESLIFEventType> lookup = new HashMap<Integer,ESLIFEventType>();

	static {
		for(ESLIFEventType s : EnumSet.allOf(ESLIFEventType.class)) {
			lookup.put(s.getCode(), s);
		}
	}

	private ESLIFEventType(int code) {
		this.code = code;
	}

	public int getCode() {
		return code;
	}

	public static ESLIFEventType get(int code) { 
		return lookup.get(code); 
	}
	
}
