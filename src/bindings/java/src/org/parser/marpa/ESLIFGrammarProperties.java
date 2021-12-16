package org.parser.marpa;

import java.util.Arrays;

/**
 * ESLIFGrammarProperties is describing properties of a grammar.
 * 
 * Calls to grammar's currentProperties() or propertiesByLevel() methods outputs an instance of this class.
 *
 * <pre>
 * ESLIF eslif = new ESLIF(...)
 * ESLIFGrammar eslifGrammar = new ESLIFGrammar(...);
 * ESLIFGrammarProperties eslifGrammarProperties = eslifGrammar{@code ->}currentProperties();
 * ESLIFGrammarProperties eslifGrammarProperties = eslifGrammar{@code ->}propertiesByLevel(0);
 * ...
 * eslifGrammar.free();
 * eslif.free()
 * </pre>
 */
public class ESLIFGrammarProperties {
	private int     level;
	private int     maxLevel;
    private String  description;
    private boolean latm;
    private boolean discardIsFallback;
    private String  defaultSymbolAction;
    private String  defaultRuleAction;
    private String  defaultEventAction;
    private String  defaultRegexAction;
    private int     startId;
    private int     discardId;
    private int[]   symbolIds;
    private int[]   ruleIds;
    private String  defaultEncoding;
    private String  fallbackEncoding;
    
	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	/**
	 * Creation of an ESLIFGrammarProperties instance
	 * 
	 * @param level Grammar level
	 * @param maxLevel Maximum grammar level
	 * @param description Grammar description
	 * @param latm Grammar is in LATM (Longest Accepted Token Mode) ?
	 * @param discardIsFallback Grammar's discard-is-fallback setting
	 * @param defaultSymbolAction Grammar default symbol action
	 * @param defaultRuleAction Grammar default rule action
	 * @param defaultEventAction Grammar default event action
	 * @param defaultRegexAction Grammar default regex action
	 * @param startId Start symbol Id
	 * @param discardId Discard symbol Id
	 * @param symbolIds Symbol Ids
	 * @param ruleIds Rule Ids
	 * @param defaultEncoding Grammar default encoding
	 * @param fallbackEncoding Grammar fallback encoding
	 */
	public ESLIFGrammarProperties(int level, int maxLevel, String description, boolean latm, boolean discardIsFallback, String defaultSymbolAction, String defaultRuleAction, String defaultEventAction, String defaultRegexAction, int startId, int discardId, int[] symbolIds, int[] ruleIds, String defaultEncoding, String fallbackEncoding) {
		this.level               = level;
		this.maxLevel            = maxLevel;
		this.description         = description;
		this.latm                = latm;
		this.discardIsFallback   = discardIsFallback;
		this.defaultSymbolAction = defaultSymbolAction;
		this.defaultRuleAction   = defaultRuleAction;
		this.defaultEventAction  = defaultEventAction;
		this.defaultRegexAction  = defaultRegexAction;
		this.startId             = startId;
		this.discardId           = discardId;
		this.symbolIds           = symbolIds;
		this.ruleIds             = ruleIds;
		this.defaultEncoding     = defaultEncoding;
		this.fallbackEncoding    = fallbackEncoding;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#hashCode()
	 */
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((defaultEncoding == null) ? 0 : defaultEncoding.hashCode());
		result = prime * result + ((defaultEventAction == null) ? 0 : defaultEventAction.hashCode());
		result = prime * result + ((defaultRegexAction == null) ? 0 : defaultRegexAction.hashCode());
		result = prime * result + ((defaultRuleAction == null) ? 0 : defaultRuleAction.hashCode());
		result = prime * result + ((defaultSymbolAction == null) ? 0 : defaultSymbolAction.hashCode());
		result = prime * result + ((description == null) ? 0 : description.hashCode());
		result = prime * result + discardId;
		result = prime * result + ((fallbackEncoding == null) ? 0 : fallbackEncoding.hashCode());
		result = prime * result + (latm ? 1231 : 1237);
		result = prime * result + (discardIsFallback ? 1231 : 1237);
		result = prime * result + level;
		result = prime * result + maxLevel;
		result = prime * result + Arrays.hashCode(ruleIds);
		result = prime * result + startId;
		result = prime * result + Arrays.hashCode(symbolIds);
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
		if (getClass() != obj.getClass()) {
			return false;
		}
		ESLIFGrammarProperties other = (ESLIFGrammarProperties) obj;
		if (defaultEncoding == null) {
			if (other.defaultEncoding != null) {
				return false;
			}
		} else if (!defaultEncoding.equals(other.defaultEncoding)) {
			return false;
		}
		if (defaultEventAction == null) {
			if (other.defaultEventAction != null) {
				return false;
			}
		} else if (!defaultEventAction.equals(other.defaultEventAction)) {
			return false;
		}
		if (defaultRegexAction == null) {
			if (other.defaultRegexAction != null) {
				return false;
			}
		} else if (!defaultRegexAction.equals(other.defaultRegexAction)) {
			return false;
		}
		if (defaultRuleAction == null) {
			if (other.defaultRuleAction != null) {
				return false;
			}
		} else if (!defaultRuleAction.equals(other.defaultRuleAction)) {
			return false;
		}
		if (defaultSymbolAction == null) {
			if (other.defaultSymbolAction != null) {
				return false;
			}
		} else if (!defaultSymbolAction.equals(other.defaultSymbolAction)) {
			return false;
		}
		if (description == null) {
			if (other.description != null) {
				return false;
			}
		} else if (!description.equals(other.description)) {
			return false;
		}
		if (discardId != other.discardId) {
			return false;
		}
		if (fallbackEncoding == null) {
			if (other.fallbackEncoding != null) {
				return false;
			}
		} else if (!fallbackEncoding.equals(other.fallbackEncoding)) {
			return false;
		}
		if (latm != other.latm) {
			return false;
		}
		if (discardIsFallback != other.discardIsFallback) {
			return false;
		}
		if (level != other.level) {
			return false;
		}
		if (maxLevel != other.maxLevel) {
			return false;
		}
		if (!Arrays.equals(ruleIds, other.ruleIds)) {
			return false;
		}
		if (startId != other.startId) {
			return false;
		}
		if (!Arrays.equals(symbolIds, other.symbolIds)) {
			return false;
		}
		return true;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "ESLIFGrammarProperties [level=" + level + ", maxLevel=" + maxLevel + ", description=" + description
				+ ", latm=" + latm + ", discardIsFallback=" + discardIsFallback + ", defaultSymbolAction=" + defaultSymbolAction + ", defaultRuleAction="
				+ defaultRuleAction + ", defaultEventAction=" + defaultEventAction + ", defaultRegexAction=" + defaultRegexAction + ", startId=" + startId
				+ ", discardId=" + discardId + ", symbolIds=" + Arrays.toString(symbolIds) + ", ruleIds="
				+ Arrays.toString(ruleIds) + ", defaultEncoding=" + defaultEncoding + ", fallbackEncoding="
				+ fallbackEncoding + "]";
	}

	/**
	 * @return Grammar's level
	 */
	public int getLevel() {
		return level;
	}

	/**
	 * @return Maximum grammar level
	 */
	public int getMaxLevel() {
		return maxLevel;
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
	 * @return A boolean that returns the grammar's discard-is-fallback setting
	 */
	public boolean isDiscardIsFallback() {
		return discardIsFallback;
	}

	/**
	 * Alias to isDiscardIsFallback().
	 * 
	 * @return A boolean that returns the grammar's discard-is-fallback setting
	 */
	public boolean getDiscardIsFallback() {
		return isDiscardIsFallback();
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
	 * @return Grammar's default event action, can be null
	 */
	public String getDefaultEventAction() {
		return defaultEventAction;
	}

	/**
	 * @return Grammar's default regex action, can be null
	 */
	public String getDefaultRegexAction() {
		return defaultRegexAction;
	}

	/**
	 * @return Grammar's start symbol id, always {@code >=} 0
	 */
	public int getStartId() {
		return startId;
	}

	/**
	 * @return Grammar's discard symbol id, {@code <} 0 if none.
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

	/**
	 * @return the default encoding, can be null
	 */
	public String getDefaultEncoding() {
		return defaultEncoding;
	}

	/**
	 * @return the fallback encoding, can be null
	 */
	public String getFallbackEncoding() {
		return fallbackEncoding;
	}
}
