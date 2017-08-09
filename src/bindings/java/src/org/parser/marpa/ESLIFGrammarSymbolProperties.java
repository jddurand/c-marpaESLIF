package org.parser.marpa;

/**
 * ESLIFGrammarSymbolProperties is describing properties of a symbol within a grammar.
 * 
 * Calls to grammar's currentSymbolProperties() or symbolPropertiesByLevel() methods outputs an instance of this class.
 *
 * <pre>
 * ESLIF eslif = new ESLIF(...)
 * ESLIFGrammar eslifGrammar = new ESLIFGrammar(...);
 * ESLIFGrammarSymbolProperties eSLIFGrammarSymbolProperties = eslifGrammar->currentSymbolProperties(0);
 * or
 * ESLIFGrammarSymbolProperties eSLIFGrammarSymbolProperties = eslifGrammar->symbolPropertiesByLevel(0, 0);
 * ...
 * eslifGrammar.free();
 * eslif.free()
 * </pre>
 */
public class ESLIFGrammarSymbolProperties {
	  private ESLIFGrammarSymbolType type;
	  private boolean                start;
	  private boolean                discard;
	  private boolean                discardRhs;
	  private boolean                lhs;
	  private boolean                top;
	  private int                    id;
	  private String                 description;
	  private String                 eventBefore;
	  private boolean                eventBeforeInitialState;
	  private String                 eventAfter;
	  private boolean                eventAfterInitialState;
	  private String                 eventPredicted;
	  private boolean                eventPredictedInitialState;
	  private String                 eventNulled;
	  private boolean                eventNulledInitialState;
	  private String                 eventCompleted;
	  private boolean                eventCompletedInitialState;
	  private String                 discardEvent;
	  private boolean                discardEventInitialState;
	  private int                    lookupResolvedLeveli;
	  private int                    priorityi;
	  private String                 nullableAction;
	  private int                    propertyBitSet;

	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	/**
	 * Creation of an ESLIFGrammarSymbolProperties instance
	 * 
	 */
	public ESLIFGrammarSymbolProperties(boolean start, boolean discard, boolean discardRhs, boolean lhs, boolean top, int id, String description, String eventBefore, boolean eventBeforeInitialState, String eventAfter, boolean eventAfterInitialState, String eventPredicted, boolean eventPredictedInitialState, String eventNulled, boolean eventNulledInitialState, String eventCompleted, boolean eventCompletedInitialState, String discardEvent, boolean discardEventInitialState, int lookupResolvedLeveli, int priorityi, String nullableAction, int propertyBitSet) {
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
		this.priorityi                  = priorityi;
		this.nullableAction             = nullableAction;
		this.propertyBitSet             = propertyBitSet;
	}

	/**
	 * @return the type
	 */
	public ESLIFGrammarSymbolType getType() {
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
	 * @return if this the start symbol
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
	public int getPriorityi() {
		return priorityi;
	}

	/**
	 * @return the nullable action, null if there is none
	 */
	public String getNullableAction() {
		return nullableAction;
	}

	/**
	 * @return the low-level properties (combination of ESLIFGrammarSymbolPropertyBitSet values)
	 */
	public int getPropertyBitSet() {
		return propertyBitSet;
	}

}
