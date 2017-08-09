package org.parser.marpa;

/**
 * ESLIFGrammarSymbolProperties is describing properties of a symbol within a grammar.
 * 
 * Calls to grammar's currentSymbolProperties() or symbolPropertiesByLevel() methods outputs an instance of this class.
 *
 * <pre>
 * ESLIF eslif = new ESLIF(...)
 * ESLIFGrammar eslifGrammar = new ESLIFGrammar(...);
 * ESLIFGrammarSymbolProperties eSLIFGrammarSymbolProperties = eslifGrammar->currentRuleProperties(0);
 * or
 * ESLIFGrammarSymbolProperties eSLIFGrammarSymbolProperties = eslifGrammar->rulePropertiesByLevel(0, 0);
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
	 * @return the start
	 */
	public boolean isStart() {
		return start;
	}

	/**
	 * @return the discard
	 */
	public boolean isDiscard() {
		return discard;
	}

	/**
	 * @return the discardRhs
	 */
	public boolean isDiscardRhs() {
		return discardRhs;
	}

	/**
	 * @return the lhs
	 */
	public boolean isLhs() {
		return lhs;
	}

	/**
	 * @return the top
	 */
	public boolean isTop() {
		return top;
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
	 * @return the eventBefore
	 */
	public String getEventBefore() {
		return eventBefore;
	}

	/**
	 * @return the eventBeforeInitialState
	 */
	public boolean isEventBeforeInitialState() {
		return eventBeforeInitialState;
	}

	/**
	 * @return the eventAfter
	 */
	public String getEventAfter() {
		return eventAfter;
	}

	/**
	 * @return the eventAfterInitialState
	 */
	public boolean isEventAfterInitialState() {
		return eventAfterInitialState;
	}

	/**
	 * @return the eventPredicted
	 */
	public String getEventPredicted() {
		return eventPredicted;
	}

	/**
	 * @return the eventPredictedInitialState
	 */
	public boolean isEventPredictedInitialState() {
		return eventPredictedInitialState;
	}

	/**
	 * @return the eventNulled
	 */
	public String getEventNulled() {
		return eventNulled;
	}

	/**
	 * @return the eventNulledInitialState
	 */
	public boolean isEventNulledInitialState() {
		return eventNulledInitialState;
	}

	/**
	 * @return the eventCompleted
	 */
	public String getEventCompleted() {
		return eventCompleted;
	}

	/**
	 * @return the eventCompletedInitialState
	 */
	public boolean isEventCompletedInitialState() {
		return eventCompletedInitialState;
	}

	/**
	 * @return the discardEvent
	 */
	public String getDiscardEvent() {
		return discardEvent;
	}

	/**
	 * @return the discardEventInitialState
	 */
	public boolean isDiscardEventInitialState() {
		return discardEventInitialState;
	}

	/**
	 * @return the lookupResolvedLeveli
	 */
	public int getLookupResolvedLeveli() {
		return lookupResolvedLeveli;
	}

	/**
	 * @return the priorityi
	 */
	public int getPriorityi() {
		return priorityi;
	}

	/**
	 * @return the nullableAction
	 */
	public String getNullableAction() {
		return nullableAction;
	}

	/**
	 * @return the propertyBitSet
	 */
	public int getPropertyBitSet() {
		return propertyBitSet;
	}

}
