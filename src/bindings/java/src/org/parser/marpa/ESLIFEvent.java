package org.parser.marpa;

/**
 * ESLIFEvent is describing an event coming out from the grammar.
 * 
 * @see ESLIFEventType
 * @see ESLIFRecognizer#events()
 */
public class ESLIFEvent {
	private ESLIFEventType eslifEventType;
	private String         symbol;
	private String         event;
	
	/**
	 * 
	 * @param eslifEventType the event type
	 * @param symbol the symbol name
	 * @param event the event name
	 */
	ESLIFEvent(ESLIFEventType eslifEventType, String symbol, String event) {
		this.eslifEventType = eslifEventType;
		this.symbol         = symbol;
		this.event          = event;
	}

	/**
	 * 
	 * @return the event type
	 */
	public ESLIFEventType getEslifEventType() {
		return eslifEventType;
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
