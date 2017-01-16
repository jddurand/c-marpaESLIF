package org.parser.marpa;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;

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
	private native String  jniRuleDisplay(int rule) throws ESLIFException;
	private native String  jniRuleShow(int rule) throws ESLIFException;
	private native String  jniRuleDisplayByLevel(int level, int rule) throws ESLIFException;
	private native String  jniRuleShowByLevel(int level, int rule) throws ESLIFException;
	private native String  jniShow() throws ESLIFException;
	private native String  jniShowByLevel(int level) throws ESLIFException;
	private native boolean jniParse(ESLIFRecognizerInterface recognizerInterface, ESLIFValueInterface valueInterface) throws ESLIFException;
	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
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
	public synchronized void free() throws ESLIFException {
		jniFree();
	}
	public synchronized int ngrammar() throws ESLIFException {
		return jniNgrammar();
	}
	public synchronized int currentLevel() throws ESLIFException {
		return jniCurrentLevel();
	}
	public synchronized String currentDescription() throws ESLIFException {
		return jniCurrentDescription();
	}
	public synchronized String descriptionByLevel(int level) throws ESLIFException {
		return jniDescriptionByLevel(level);
	}
	public synchronized int[] currentRuleIds() throws ESLIFException {
		return jniCurrentRuleIds();
	}
	public synchronized int[] ruleIdsByLevel(int level) throws ESLIFException {
		return jniRuleIdsByLevel(level);
	}
	public synchronized String ruleDisplay(int rule) throws ESLIFException {
		return jniRuleDisplay(rule);
	}
	public synchronized String ruleShow(int rule) throws ESLIFException {
		return jniRuleShow(rule);
	}
	public synchronized String ruleDisplayByLevel(int level, int rule) throws ESLIFException {
		return jniRuleDisplayByLevel(level, rule);
	}
	public synchronized String ruleShowByLevel(int level, int rule) throws ESLIFException {
		return jniRuleShowByLevel(level, rule);
	}
	public synchronized String show() throws ESLIFException {
		return jniShow();
	}
	public synchronized String showByLevel(int level) throws ESLIFException {
		return jniShowByLevel(level);
	}
	public synchronized boolean parse(ESLIFRecognizerInterface recognizerInterface, ESLIFValueInterface valueInterface) throws ESLIFException {
		return jniParse(recognizerInterface, valueInterface);
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
