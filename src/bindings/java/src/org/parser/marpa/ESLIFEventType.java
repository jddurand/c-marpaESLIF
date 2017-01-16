package org.parser.marpa;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;

/**
 * ESLIFEventType is an enumeration of all ESLIF possible events.
 * 
 * @see ESLIFEvent
 */
public enum ESLIFEventType {

	/* Thanks to http://www.ajaxonomy.com/2007/java/making-the-most-of-java-50-enum-tricks */

	/**
	 * Should never happen -;
	 */
	NONE     (0x00),
	/**
	 * Symbol completion event
	 */
	COMPLETED(0x01), /* Grammar event */
	/**
	 * Symbol nulling event
	 */
	NULLED   (0x02), /* Grammar event */
	/**
	 * Symbol prediction event
	 */
	PREDICTED(0x04), /* Grammar event */
	/**
	 * Lexeme prediction event
	 */
	BEFORE   (0x08), /* Just before lexeme is commited */
	/**
	 * Lexeme consumption event
	 */
	AFTER    (0x10), /* Just after lexeme is commited */
	/**
	 * Exhaustion event
	 */
	EXHAUSTED(0x20), /* Exhaustion */
	/**
	 * Discard event
	 */
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

	/**
	 * Get the event value associated to an instance of ESLIFEventType.
	 * 
	 * @return the event value
	 */
	public int getCode() {
		return code;
	}

	/**
	 * Get an instance of ESLIFEventType from an event code value
	 * 
	 * @param code event code value
	 * @return the ESLIFEventType instance
	 */
	public static ESLIFEventType get(int code) { 
		return lookup.get(code); 
	}
	
}
