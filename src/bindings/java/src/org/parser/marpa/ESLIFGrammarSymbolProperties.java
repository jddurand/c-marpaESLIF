package org.parser.marpa;

/**
 * ESLIFGrammarSymbolProperties is describing properties of a symbol within a grammar.
 * 
 * Calls to grammar's currentSymbolProperties() or symbolPropertiesByLevel() methods outputs an instance of this class.
 *
 * <pre>
 * ESLIF eslif = new ESLIF(...)
 * ESLIFGrammar eslifGrammar = new ESLIFGrammar(...);
 * ESLIFGrammarSymbolProperties eSLIFGrammarSymbolProperties = eslifGrammar{@code ->}currentSymbolProperties(0);
 * or
 * ESLIFGrammarSymbolProperties eSLIFGrammarSymbolProperties = eslifGrammar{@code ->}symbolPropertiesByLevel(0, 0);
 * ...
 * eslifGrammar.free();
 * eslif.free()
 * </pre>
 */
public class ESLIFGrammarSymbolProperties {
	  private ESLIFSymbolType type;
	  private boolean         start;
	  private boolean         discard;
	  private boolean         discardRhs;
	  private boolean         lhs;
	  private boolean         top;
	  private int             id;
	  private String          description;
	  private String          eventBefore;
	  private boolean         eventBeforeInitialState;
	  private String          eventAfter;
	  private boolean         eventAfterInitialState;
	  private String          eventPredicted;
	  private boolean         eventPredictedInitialState;
	  private String          eventNulled;
	  private boolean         eventNulledInitialState;
	  private String          eventCompleted;
	  private boolean         eventCompletedInitialState;
	  private String          discardEvent;
	  private boolean         discardEventInitialState;
	  private int             lookupResolvedLeveli;
	  private int             priority;
	  private String          nullableAction;
	  private int             propertyBitSet;

	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	/**
	 * Creation of an ESLIFGrammarSymbolProperties instance
	 * 
	 * @param type symbol type
	 * @param start true if this is the start symbol
	 * @param discard true if this is a discard symbol
	 * @param discardRhs true if this is on the right-hand side of a discard rule
	 * @param lhs true if this is on the left-hand side of a rule
	 * @param top true if this is the first symbol of the grammar
	 * @param id internal identifier
	 * @param description description
	 * @param eventBefore name of the "before" event, null if none
	 * @param eventBeforeInitialState initial state of the "before" event, meaningless if there is no "before" event
	 * @param eventAfter name of the "after" event, null if none
	 * @param eventAfterInitialState initial state of the "after" event, meaningless if there is no "after" event
	 * @param eventPredicted name of the "predicted" event, null if none
	 * @param eventPredictedInitialState initial state of "predicted" event, meaningless if there is no "predicted" event
	 * @param eventNulled name of the "nulled event, null if none
	 * @param eventNulledInitialState initial state of the "nulled" event, meaningless if there is no "nulled" event
	 * @param eventCompleted name of the "completed" event, null if none
	 * @param eventCompletedInitialState initial state of the "completed" event, meaningless if there is no "completed" event
	 * @param discardEvent name of the "discard" event, null if none
	 * @param discardEventInitialState initial state of the discard event, meaningless if there is no "discard" event
	 * @param lookupResolvedLeveli grammar level to which it is resolved, can be different to the grammar used when this is a lexeme
	 * @param priority symbol priority
	 * @param nullableAction nullable action, null if there is none
	 * @param propertyBitSet low-level properties
	 * 
	 */
	public ESLIFGrammarSymbolProperties(ESLIFSymbolType type, boolean start, boolean discard, boolean discardRhs, boolean lhs, boolean top, int id, String description, String eventBefore, boolean eventBeforeInitialState, String eventAfter, boolean eventAfterInitialState, String eventPredicted, boolean eventPredictedInitialState, String eventNulled, boolean eventNulledInitialState, String eventCompleted, boolean eventCompletedInitialState, String discardEvent, boolean discardEventInitialState, int lookupResolvedLeveli, int priority, String nullableAction, int propertyBitSet) {
		this.type                       = type;
		this.start                      = start;
		this.discard                    = discard;
		this.discardRhs                 = discardRhs;
		this.lhs                        = lhs;
		this.top                        = top;
		this.id                         = id;
		this.description                = description;
		this.eventBefore                = eventBefore;
		this.eventBeforeInitialState    = eventBeforeInitialState;
		this.eventAfter                 = eventAfter;
		this.eventAfterInitialState     = eventAfterInitialState;
		this.eventPredicted             = eventPredicted;
		this.eventPredictedInitialState = eventPredictedInitialState;
		this.eventNulled                = eventNulled;
		this.eventNulledInitialState    = eventNulledInitialState;
		this.eventCompleted             = eventCompleted;
		this.eventCompletedInitialState = eventCompletedInitialState;
		this.discardEvent               = discardEvent;
		this.discardEventInitialState   = discardEventInitialState;
		this.lookupResolvedLeveli       = lookupResolvedLeveli;
		this.priority                   = priority ;
		this.nullableAction             = nullableAction;
		this.propertyBitSet             = propertyBitSet;
	}
	
	/* (non-Javadoc)
	 * @see java.lang.Object#hashCode()
	 */
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((description == null) ? 0 : description.hashCode());
		result = prime * result + (discard ? 1231 : 1237);
		result = prime * result + ((discardEvent == null) ? 0 : discardEvent.hashCode());
		result = prime * result + (discardEventInitialState ? 1231 : 1237);
		result = prime * result + (discardRhs ? 1231 : 1237);
		result = prime * result + ((eventAfter == null) ? 0 : eventAfter.hashCode());
		result = prime * result + (eventAfterInitialState ? 1231 : 1237);
		result = prime * result + ((eventBefore == null) ? 0 : eventBefore.hashCode());
		result = prime * result + (eventBeforeInitialState ? 1231 : 1237);
		result = prime * result + ((eventCompleted == null) ? 0 : eventCompleted.hashCode());
		result = prime * result + (eventCompletedInitialState ? 1231 : 1237);
		result = prime * result + ((eventNulled == null) ? 0 : eventNulled.hashCode());
		result = prime * result + (eventNulledInitialState ? 1231 : 1237);
		result = prime * result + ((eventPredicted == null) ? 0 : eventPredicted.hashCode());
		result = prime * result + (eventPredictedInitialState ? 1231 : 1237);
		result = prime * result + id;
		result = prime * result + (lhs ? 1231 : 1237);
		result = prime * result + lookupResolvedLeveli;
		result = prime * result + ((nullableAction == null) ? 0 : nullableAction.hashCode());
		result = prime * result + priority;
		result = prime * result + propertyBitSet;
		result = prime * result + (start ? 1231 : 1237);
		result = prime * result + (top ? 1231 : 1237);
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
		if (!(obj instanceof ESLIFGrammarSymbolProperties)) {
			return false;
		}
		ESLIFGrammarSymbolProperties other = (ESLIFGrammarSymbolProperties) obj;
		if (description == null) {
			if (other.description != null) {
				return false;
			}
		} else if (!description.equals(other.description)) {
			return false;
		}
		if (discard != other.discard) {
			return false;
		}
		if (discardEvent == null) {
			if (other.discardEvent != null) {
				return false;
			}
		} else if (!discardEvent.equals(other.discardEvent)) {
			return false;
		}
		if (discardEventInitialState != other.discardEventInitialState) {
			return false;
		}
		if (discardRhs != other.discardRhs) {
			return false;
		}
		if (eventAfter == null) {
			if (other.eventAfter != null) {
				return false;
			}
		} else if (!eventAfter.equals(other.eventAfter)) {
			return false;
		}
		if (eventAfterInitialState != other.eventAfterInitialState) {
			return false;
		}
		if (eventBefore == null) {
			if (other.eventBefore != null) {
				return false;
			}
		} else if (!eventBefore.equals(other.eventBefore)) {
			return false;
		}
		if (eventBeforeInitialState != other.eventBeforeInitialState) {
			return false;
		}
		if (eventCompleted == null) {
			if (other.eventCompleted != null) {
				return false;
			}
		} else if (!eventCompleted.equals(other.eventCompleted)) {
			return false;
		}
		if (eventCompletedInitialState != other.eventCompletedInitialState) {
			return false;
		}
		if (eventNulled == null) {
			if (other.eventNulled != null) {
				return false;
			}
		} else if (!eventNulled.equals(other.eventNulled)) {
			return false;
		}
		if (eventNulledInitialState != other.eventNulledInitialState) {
			return false;
		}
		if (eventPredicted == null) {
			if (other.eventPredicted != null) {
				return false;
			}
		} else if (!eventPredicted.equals(other.eventPredicted)) {
			return false;
		}
		if (eventPredictedInitialState != other.eventPredictedInitialState) {
			return false;
		}
		if (id != other.id) {
			return false;
		}
		if (lhs != other.lhs) {
			return false;
		}
		if (lookupResolvedLeveli != other.lookupResolvedLeveli) {
			return false;
		}
		if (nullableAction == null) {
			if (other.nullableAction != null) {
				return false;
			}
		} else if (!nullableAction.equals(other.nullableAction)) {
			return false;
		}
		if (priority != other.priority) {
			return false;
		}
		if (propertyBitSet != other.propertyBitSet) {
			return false;
		}
		if (start != other.start) {
			return false;
		}
		if (top != other.top) {
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
		return "ESLIFGrammarSymbolProperties [type=" + type + ", start=" + start + ", discard=" + discard
				+ ", discardRhs=" + discardRhs + ", lhs=" + lhs + ", top=" + top + ", id=" + id + ", description="
				+ description + ", eventBefore=" + eventBefore + ", eventBeforeInitialState=" + eventBeforeInitialState
				+ ", eventAfter=" + eventAfter + ", eventAfterInitialState=" + eventAfterInitialState
				+ ", eventPredicted=" + eventPredicted + ", eventPredictedInitialState=" + eventPredictedInitialState
				+ ", eventNulled=" + eventNulled + ", eventNulledInitialState=" + eventNulledInitialState
				+ ", eventCompleted=" + eventCompleted + ", eventCompletedInitialState=" + eventCompletedInitialState
				+ ", discardEvent=" + discardEvent + ", discardEventInitialState=" + discardEventInitialState
				+ ", lookupResolvedLeveli=" + lookupResolvedLeveli + ", priority=" + priority + ", nullableAction="
				+ nullableAction + ", propertyBitSet=" + propertyBitSet + "]";
	}

	/**
	 * @return the type
	 */
	public ESLIFSymbolType getType() {
		return type;
	}

	/**
	 * @return if this the start symbol
	 */
	public boolean isStart() {
		return start;
	}

	/**
	 * Alias to isStart()
	 * 
	 * @return if this is the start symbol
	 */
	public boolean getStart() {
		return isStart();
	}

	/**
	 * @return if this is the discard symbol
	 */
	public boolean isDiscard() {
		return discard;
	}

	/**
	 * Alias to isDiscard()
	 * 
	 * @return if this is the discard symbol
	 */
	public boolean getDiscard() {
		return isDiscard();
	}

	/**
	 * @return if this is a RHS of a discard rule
	 */
	public boolean isDiscardRhs() {
		return discardRhs;
	}

	/**
	 * Alias to isDiscardRhs()
	 * 
	 * @return if this is a RHS of a discard rule
	 */
	public boolean getDiscardRhs() {
		return isDiscardRhs();
	}

	/**
	 * @return if this is an LHS
	 */
	public boolean isLhs() {
		return lhs;
	}

	/**
	 * Alias to isLhs()
	 * 
	 * @return if this is an LHS
	 */
	public boolean getLhs() {
		return isLhs();
	}

	/**
	 * @return if this is the first symbol of the grammar
	 */
	public boolean isTop() {
		return top;
	}

	/**
	 * Alias to isTop()
	 * 
	 * @return if this is the first symbol of the grammar
	 */
	public boolean getTop() {
		return isTop();
	}

	/**
	 * @return the id
	 */
	public int getId() {
		return id;
	}

	/**
	 * @return the description
	 */
	public String getDescription() {
		return description;
	}

	/**
	 * @return the event before name, null if there is none
	 */
	public String getEventBefore() {
		return eventBefore;
	}

	/**
	 * @return if the event before initial state is on, meaningless if there is no event before
	 */
	public boolean isEventBeforeInitialState() {
		return eventBeforeInitialState;
	}

	/**
	 * Alias to isEventBeforeInitialState()
	 * 
	 * @return if the eventBefore initial state is on, meaningless if there is no event before
	 */
	public boolean getEventBeforeInitialState() {
		return isEventBeforeInitialState();
	}

	/**
	 * @return the event after name, null if there is none
	 */
	public String getEventAfter() {
		return eventAfter;
	}

	/**
	 * @return if the event after initial state is on, meaningless if there is no event after
	 */
	public boolean isEventAfterInitialState() {
		return eventAfterInitialState;
	}

	/**
	 * Alias to isEventAfterInitialState()
	 * 
	 * @return if the event after initial state is on, meaningless if there is no event after
	 */
	public boolean getEventAfterInitialState() {
		return isEventAfterInitialState();
	}

	/**
	 * @return the event predicted name, null if there is none
	 */
	public String getEventPredicted() {
		return eventPredicted;
	}

	/**
	 * @return if the event predicted initial state is on, meaningless if there is no prediction event
	 */
	public boolean isEventPredictedInitialState() {
		return eventPredictedInitialState;
	}

	/**
	 * Alias to isEventPredictedInitialState()
	 * 
	 * @return if the event predicted initial state is on, meaningless if there is no prediction event
	 */
	public boolean getEventPredictedInitialState() {
		return isEventPredictedInitialState();
	}

	/**
	 * @return the null event name, null if there is none - used internally for ":discard[on]" and ":discard[off]" in particular
	 */
	public String getEventNulled() {
		return eventNulled;
	}

	/**
	 * @return the nulled event initial state, meaningless if there is nulled event
	 */
	public boolean isEventNulledInitialState() {
		return eventNulledInitialState;
	}

	/**
	 * Alias isEventNulledInitialState()
	 * 
	 * @return the nulled event initial state, meaningless if there is nulled event
	 */
	public boolean getEventNulledInitialState() {
		return isEventNulledInitialState();
	}

	/**
	 * @return the completion event name, null if there is none
	 */
	public String getEventCompleted() {
		return eventCompleted;
	}

	/**
	 * @return the completion event initial state, meaningless if there is no completion event
	 */
	public boolean isEventCompletedInitialState() {
		return eventCompletedInitialState;
	}

	/**
	 * Alias to isEventCompletedInitialState()
	 * 
	 * @return the completion event initial state, meaningless if there is no completion event
	 */
	public boolean getEventCompletedInitialState() {
		return isEventCompletedInitialState();
	}

	/**
	 * @return the discard event, null if there is none
	 */
	public String getDiscardEvent() {
		return discardEvent;
	}

	/**
	 * @return the discard event initial state, meaningless if there is no discard event
	 */
	public boolean isDiscardEventInitialState() {
		return discardEventInitialState;
	}

	/**
	 * Alias to isDiscardEventInitialState()
	 * 
	 * @return the discard event initial state, meaningless if there is no discard event
	 */
	public boolean getDiscardEventInitialState() {
		return isDiscardEventInitialState();
	}

	/**
	 * @return the grammar level to which it is resolved, can be different to the grammar used when this is a lexeme
	 */
	public int getLookupResolvedLeveli() {
		return lookupResolvedLeveli;
	}

	/**
	 * @return the symbol priority
	 */
	public int getPriority() {
		return priority;
	}

	/**
	 * @return the nullable action, null if there is none
	 */
	public String getNullableAction() {
		return nullableAction;
	}

	/**
	 * @return the low-level properties (combination of ESLIFSymbolPropertyBitSet values)
	 */
	public int getPropertyBitSet() {
		return propertyBitSet;
	}

}
