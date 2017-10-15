package org.parser.marpa;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;

/**
 * ESLIFGrammar is the second step after getting an ESLIF instance. As many grammars as wanted
 * can be created using the same ESLIF parent, though dispose of resources should follow the reverse order
 * of creation, i.e.:
 *
 * <pre>
 * ESLIF eslif = new ESLIF(...)
 * ESLIFGrammar eslifGrammar = new ESLIFGrammar(...);
 * ...
 * eslifGrammar.free();
 * eslif.free()
 * </pre>
 */
public class ESLIFGrammar {
	private ESLIF          eslif              = null;
	private ByteBuffer     marpaESLIFGrammarp = null;
	private native void    jniNew(byte[] utf8) throws ESLIFException;
	private native void    jniFree() throws ESLIFException;
	private native int     jniNgrammar() throws ESLIFException;
	private native int     jniCurrentLevel() throws ESLIFException;
	private native String  jniCurrentDescription() throws ESLIFException;
	private native String  jniDescriptionByLevel(int level) throws ESLIFException;
	private native int[]   jniCurrentRuleIds() throws ESLIFException;
	private native int[]   jniRuleIdsByLevel(int level) throws ESLIFException;
	private native int[]   jniCurrentSymbolIds() throws ESLIFException;
	private native int[]   jniSymbolIdsByLevel(int level) throws ESLIFException;
	private native String  jniRuleDisplay(int rule) throws ESLIFException;
	private native String  jniRuleShow(int rule) throws ESLIFException;
	private native String  jniRuleDisplayByLevel(int level, int rule) throws ESLIFException;
	private native String  jniRuleShowByLevel(int level, int rule) throws ESLIFException;
	private native String  jniSymbolDisplay(int symbol) throws ESLIFException;
	private native String  jniSymbolDisplayByLevel(int level, int symbol) throws ESLIFException;
	private native String  jniShow() throws ESLIFException;
	private native String  jniShowByLevel(int level) throws ESLIFException;
	private native boolean jniParse(ESLIFRecognizerInterface recognizerInterface, ESLIFValueInterface valueInterface) throws ESLIFException;
	private native ESLIFGrammarProperties jniProperties() throws ESLIFException;
	private native ESLIFGrammarProperties jniPropertiesByLevel(int level) throws ESLIFException;
	private native ESLIFGrammarRuleProperties jniRuleProperties(int rule) throws ESLIFException;
	private native ESLIFGrammarRuleProperties jniRulePropertiesByLevel(int level, int rule) throws ESLIFException;
	private native ESLIFGrammarSymbolProperties jniSymbolProperties(int symbol) throws ESLIFException;
	private native ESLIFGrammarSymbolProperties jniSymbolPropertiesByLevel(int level, int symbol) throws ESLIFException;
	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	/**
	 * Creation of an ESLIFGrammar instance
	 * 
	 * @param eslif an instance of ESLIF
	 * @param grammar the grammar to compile
	 * @throws UnsupportedEncodingException if internal grammar cannot be converted to strict UTF-8
	 * @throws ESLIFException if grammar parse failed
	 */
	public ESLIFGrammar(ESLIF eslif, String grammar) throws UnsupportedEncodingException, ESLIFException {
		if (eslif == null) {
			throw new IllegalArgumentException("eslif must not be null");
		}
		if (grammar == null) {
			throw new IllegalArgumentException("grammar must not be null");
		}
		setEslif(eslif);
		jniNew(grammar.getBytes("UTF-8"));
	}
	/**
	 * Dispose of an ESLIFGrammar resources.
	 * 
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized void free() throws ESLIFException {
		jniFree();
	}
	/**
	 * A grammar can have multiple "sub-grammars", identified by a level.
	 * Internally this is a "sparse" array of grammars, and it is legal that a level is not defined. 
	 * 
	 * @return the number of sub-grammars, always {@code >} 0.
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized int ngrammar() throws ESLIFException {
		return jniNgrammar();
	}
	/**
	 * @return the current level, which is always the first defined level of the grammar
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized int currentLevel() throws ESLIFException {
		return jniCurrentLevel();
	}
	/**
	 * 
	 * @return the description of the current level
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized String currentDescription() throws ESLIFException {
		return jniCurrentDescription();
	}
	/**
	 * 
	 * @param level the grammar level
	 * @return the description of the grammar at the specified <code>level</code>
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized String descriptionByLevel(int level) throws ESLIFException {
		return jniDescriptionByLevel(level);
	}
	/**
	 * Rule identifiers are integers that uniquely identify a rule
	 * 
	 * @return the list of rule identifiers of the current grammar
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized int[] currentRuleIds() throws ESLIFException {
		return jniCurrentRuleIds();
	}
	/**
	 * Rule identifiers are integers that uniquely identify a rule
	 * 
	 * @param level the grammar level
	 * @return the list of rule identifiers of the grammar at the specified <code>level</code>
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized int[] ruleIdsByLevel(int level) throws ESLIFException {
		return jniRuleIdsByLevel(level);
	}
	/**
	 * Symbol identifiers are integers that uniquely identify a symbol
	 * 
	 * @return the list of symbol identifiers of the current grammar
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized int[] currentSymbolIds() throws ESLIFException {
		return jniCurrentSymbolIds();
	}
	/**
	 * Symbol identifiers are integers that uniquely identify a symbol
	 * 
	 * @param level the grammar level
	 * @return the list of symbol identifiers of the grammar at the specified <code>level</code>
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized int[] symbolIdsByLevel(int level) throws ESLIFException {
		return jniSymbolIdsByLevel(level);
	}
	/**
	 * This method return the name of a rule
	 * This correspond to the <code>name</code> adverb, if present, else a default naming applies.
	 * 
	 * @param rule the rule ID
	 * @return the name of the rule
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized String ruleDisplay(int rule) throws ESLIFException {
		return jniRuleDisplay(rule);
	}
	/**
	 * This method returns a human-readable description of the rule
	 * 
	 * @param rule the rule ID
	 * @return the description of the rule
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized String ruleShow(int rule) throws ESLIFException {
		return jniRuleShow(rule);
	}
	/**
	 * This method return the display form of a symbol
	 * 
	 * @param symbol the symbol ID
	 * @return the display form of the symbol
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized String symbolDisplay(int symbol) throws ESLIFException {
		return jniSymbolDisplay(symbol);
	}
	/**
	 * This method return the name of a rule at a given level
	 * This correspond to the <code>name</code> adverb, if present, else a default naming applies.
	 * 
	 * @param level the grammar level
	 * @param rule the rule ID
	 * @return the name of the rule
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized String ruleDisplayByLevel(int level, int rule) throws ESLIFException {
		return jniRuleDisplayByLevel(level, rule);
	}
	/**
	 * This method return the display form of a symbol at a given level
	 * 
	 * @param level the grammar level
	 * @param symbol the symbol ID
	 * @return the display form of the symbol
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized String symbolDisplayByLevel(int level, int symbol) throws ESLIFException {
		return jniSymbolDisplayByLevel(level, symbol);
	}
	/**
	 * This method returns a human-readable description of the rule
	 * 
	 * @param level the grammar level
	 * @param rule the rule ID
	 * @return the description of the rule
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized String ruleShowByLevel(int level, int rule) throws ESLIFException {
		return jniRuleShowByLevel(level, rule);
	}
	/**
	 * 
	 * @return the description of the grammar
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized String show() throws ESLIFException {
		return jniShow();
	}
	/**
	 * 
	 * @param level the grammar level
	 * @return the description of the grammar at the specified <code>level</code>
	 * @throws ESLIFException if the interface failed
	 */
	public synchronized String showByLevel(int level) throws ESLIFException {
		return jniShowByLevel(level);
	}
	/**
	 * Short version of input validation and valuation, that will never give back control to the user until the end or a failure.
	 * No event is possible when using this method. If this method returns true, then it is guaranteed that the result is in
	 * valueInterface.getResult().
	 * 
	 * @param recognizerInterface the recognizer interface, must not be null
	 * @param valueInterface the value interface, must not be null
	 * @return a boolean indicating if the parse was successful or not
	 * @throws Exception if the interface failed
	 */
	public synchronized boolean parse(ESLIFRecognizerInterface recognizerInterface, ESLIFValueInterface valueInterface) throws Exception {
		if (recognizerInterface == null) {
			throw new Exception("recognizerInterface must not be null");
		}
		if (valueInterface == null) {
			throw new Exception("valueInterface must not be null");
		}
		return jniParse(recognizerInterface, valueInterface);
	}
	/**
	 * @return grammar properties
	 * @throws ESLIFException if the interface failed
	 */
	public ESLIFGrammarProperties properties() throws ESLIFException {
		/* Note that this method does not need to be synchronized */
		return jniProperties();
	}
	/**
	 * @param level the grammar level
	 * @return grammar properties
	 * @throws ESLIFException if the interface failed
	 */
	public ESLIFGrammarProperties propertiesByLevel(int level) throws ESLIFException {
		/* Note that this method does not need to be synchronized */
		return jniPropertiesByLevel(level);
	}
	/**
	 * @param rule the rule identifier
	 * @return ruleproperties
	 * @throws ESLIFException if the interface failed
	 */
	public ESLIFGrammarRuleProperties ruleProperties(int rule) throws ESLIFException {
		/* Note that this method does not need to be synchronized */
		return jniRuleProperties(rule);
	}
	/**
	 * @param level the grammar level
	 * @param rule the rule identifier
	 * @return ruleproperties
	 * @throws ESLIFException if the interface failed
	 */
	public ESLIFGrammarRuleProperties rulePropertiesByLevel(int level, int rule) throws ESLIFException {
		/* Note that this method does not need to be synchronized */
		return jniRulePropertiesByLevel(level, rule);
	}
	/**
	 * @param symbol the symbol identifier
	 * @return symbol properties
	 * @throws ESLIFException if the interface failed
	 */
	public ESLIFGrammarSymbolProperties symbolProperties(int symbol) throws ESLIFException {
		/* Note that this method does not need to be synchronized */
		return jniSymbolProperties(symbol);
	}
	/**
	 * @param level the grammar level
	 * @param symbol the symbol identifier
	 * @return symbol properties
	 * @throws ESLIFException if the interface failed
	 */
	public ESLIFGrammarSymbolProperties symbolPropertiesByLevel(int level, int symbol) throws ESLIFException {
		/* Note that this method does not need to be synchronized */
		return jniSymbolPropertiesByLevel(level, symbol);
	}
	/*
	 * ********************************************
	 * Private methods - used by the JNI
	 * ********************************************
	 */
	protected ESLIFLoggerInterface getLoggerInterface() {
		return (eslif != null) ? eslif.getLoggerInterface() : null;
	}
	private ESLIF getEslif() {
		return eslif;
	}
	private void setEslif(ESLIF eslif) {
		this.eslif = eslif;
	}
	private ByteBuffer getMarpaESLIFGrammarp() {
		return marpaESLIFGrammarp;
	}
	private void setMarpaESLIFGrammarp(ByteBuffer marpaESLIFGrammarp) {
		this.marpaESLIFGrammarp = marpaESLIFGrammarp;
	}

}
