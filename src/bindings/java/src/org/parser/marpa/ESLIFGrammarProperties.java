package org.parser.marpa;

/**
 * ESLIFGrammarProperties is describing properties of a grammar.
 * 
 * Calls to grammar's currentProperties() or propertiesByLevel() methods outputs an instance of this class.
 *
 * <pre>
 * ESLIF eslif = new ESLIF(...)
 * ESLIFGrammar eslifGrammar = new ESLIFGrammar(...);
 * ESLIFGrammarProperties eslifGrammarProperties = eslifGrammar->currentProperties();
 * ESLIFGrammarProperties eslifGrammarProperties = eslifGrammar->propertiesByLevel(0);
 * ...
 * eslifGrammar.free();
 * eslif.free()
 * </pre>
 */
public class ESLIFGrammarProperties {
	private int     level;
    private String  description;
    private boolean latm;
    private String  defaultSymbolAction;
    private String  defaultRuleAction;
    private String  defaultFreeAction;
    private int     startId;
    private int     discardId;
    private int[]   symbolIds;
    private int[]   ruleIds;
    
	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	/**
	 * Creation of an ESLIFGrammarProperties instance
	 * 
	 * @param level Grammar level
	 * @param description Grammar description
	 * @param latm Grammar is in LATM (Longest Accepted Token Mode) ?
	 * @param defaultSymbolAction Grammar default symbol action
	 * @param defaultRuleAction Grammar default rule action
	 * @param defaultFreeAction Grammar default free action
	 * @param startId Start symbol Id
	 * @param discardId Discard symbol Id
	 * @param symbolIds Symbol Ids
	 * @param ruleIds Rule Ids
	 */
	public ESLIFGrammarProperties(int level, String description, boolean latm, String defaultSymbolAction, String defaultRuleAction, String defaultFreeAction, int startId, int discardId, int[] symbolIds, int[] ruleIds) {
		this.level               = level;
		this.description         = description;
		this.latm                = latm;
		this.defaultSymbolAction = defaultSymbolAction;
		this.defaultRuleAction   = defaultRuleAction;
		this.defaultFreeAction   = defaultFreeAction;
		this.startId             = startId;
		this.discardId           = discardId;
		this.symbolIds           = symbolIds;
		this.ruleIds             = ruleIds;
	}

	/**
	 * @return Grammar's level
	 */
	public int getLevel() {
		return level;
	}

	/**
	 * @return Grammar's description
	 */
	public String getDescription() {
		return description;
	}

	/**
	 * @return A boolean that indicates if this grammar is in the LATM (Longest Acceptable Token Mode) or not
	 */
	public boolean isLatm() {
		return latm;
	}

	/**
	 * Alias to isLatm().
	 * 
	 * @return A boolean that indicates if this grammar is in the LATM (Longest Acceptable Token Mode) or not
	 */
	public boolean getLatm() {
		return isLatm();
	}

	/**
	 * @return Grammar's default symbol action, never null
	 */
	public String getDefaultSymbolAction() {
		return defaultSymbolAction;
	}

	/**
	 * @return Grammar's default rule action, can be null
	 */
	public String getDefaultRuleAction() {
		return defaultRuleAction;
	}

	/**
	 * @return Grammar's default free action, never null and always ":defaultFreeActions" (hardcoded in the JNI interface)
	 */
	public String getDefaultFreeAction() {
		return defaultFreeAction;
	}

	/**
	 * @return Grammar's start symbol id, always >= 0
	 */
	public int getStartId() {
		return startId;
	}

	/**
	 * @return Grammar's discard symbol id, < 0 if none.
	 */
	public int getDiscardId() {
		return discardId;
	}

	/**
	 * @return List of symbol identifiers
	 */
	public int[] getSymbolIds() {
		return symbolIds;
	}

	/**
	 * @return List of rule identifiers
	 */
	public int[] getRuleIds() {
		return ruleIds;
	}
}
