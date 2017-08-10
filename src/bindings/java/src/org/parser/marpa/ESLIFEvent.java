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

	/* (non-Javadoc)
	 * @see java.lang.Object#hashCode()
	 */
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((event == null) ? 0 : event.hashCode());
		result = prime * result + ((symbol == null) ? 0 : symbol.hashCode());
		result = prime * result + ((type == null) ? 0 : type.hashCode());
		return result;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(Object obj) {
		if (this == obj) {
			return true;
		}
		if (obj == null) {
			return false;
		}
		if (!(obj instanceof ESLIFEvent)) {
			return false;
		}
		ESLIFEvent other = (ESLIFEvent) obj;
		if (event == null) {
			if (other.event != null) {
				return false;
			}
		} else if (!event.equals(other.event)) {
			return false;
		}
		if (symbol == null) {
			if (other.symbol != null) {
				return false;
			}
		} else if (!symbol.equals(other.symbol)) {
			return false;
		}
		if (type != other.type) {
			return false;
		}
		return true;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "ESLIFEvent [type=" + type + ", symbol=" + symbol + ", event=" + event + "]";
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
	 * @return the symbol name, null only when this the exhaustion event
	 */
	public String getSymbol() {
		return symbol;
	}

	/**
	 * 
	 * @return the event name, null only when this the exhaustion event
	 */
	public String getEvent() {
		return event;
	}
	
}
