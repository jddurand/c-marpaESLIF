package org.parser.marpa;

public class ESLIFEvent {
	private ESLIFEventType eslifEventType;
	private String         symbol;
	private String         event;
	
	ESLIFEvent(ESLIFEventType eslifEventType, String symbol, String event) {
		this.eslifEventType = eslifEventType;
		this.symbol         = symbol;
		this.event          = event;
	}

	public ESLIFEventType getEslifEventType() {
		return eslifEventType;
	}

	public String getSymbol() {
		return symbol;
	}

	public String getEvent() {
		return event;
	}

}
