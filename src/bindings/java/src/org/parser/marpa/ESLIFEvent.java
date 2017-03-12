package org.parser.marpa;

/**
 * ESLIFEvent is describing an event coming out from the grammar.
 * 
 * @see ESLIFEventType
 * @see ESLIFRecognizer#events()
 */
public class ESLIFEvent {
	private ESLIFEventType type;
	private String         symbol;
	private String         event;
	
	/**
	 * 
	 * @param type the event type
	 * @param symbol the symbol name
	 * @param event the event name
	 */
	ESLIFEvent(ESLIFEventType type, String symbol, String event) {
		this.type   = type;
		this.symbol = symbol;
		this.event  = event;
	}

	/**
	 * 
	 * @return the event type
	 */
	public ESLIFEventType getType() {
		return type;
	}

	/**
	 * 
	 * @return the symbol name, never null
	 */
	public String getSymbol() {
		return symbol;
	}

	/**
	 * 
	 * @return the event name, never null
	 */
	public String getEvent() {
		return event;
	}
	
}
