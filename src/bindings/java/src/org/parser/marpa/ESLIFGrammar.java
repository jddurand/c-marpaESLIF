package org.parser.marpa;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;

public class ESLIFGrammar {
	private ESLIF         eslif         = null;
	private ByteBuffer    marpaGrammarp = null;
	private native void   jniNew(byte[] utf8);
	private native void   jniFree();
	private native int    jniNgrammar();
	private native int    jniCurrentLevel();
	private native String jniCurrentDescription();
	private native String jniDescriptionByLevel(int level);
	private native int[]  jniCurrentRuleIds();
	private native int[]  jniRuleIdsByLevel(int level);
	private native String jniRuleDisplay(int rule);
	private native String jniRuleShow(int rule);
	private native String jniRuleDisplayByLevel(int level, int rule);
	private native String jniRuleShowByLevel(int level, int rule);
	private native String jniShow();
	private native String jniShowByLevel(int level);
	
	/*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
	public ESLIFGrammar(ESLIF eslif, String grammar) throws UnsupportedEncodingException  {
		if (eslif == null) {
			throw new IllegalArgumentException("eslif must not be null");
		}
		if (grammar == null) {
			throw new IllegalArgumentException("grammar must not be null");
		}
		setEslif(eslif);
		jniNew(grammar.getBytes("UTF-8"));
	}
	public void free() {
		jniFree();
	}
	public int ngrammar() {
		return jniNgrammar();
	}
	public int currentLevel() {
		return jniCurrentLevel();
	}
	public String currentDescription() {
		return jniCurrentDescription();
	}
	public String descriptionByLevel(int level) {
		return jniDescriptionByLevel(level);
	}
	public int[] currentRuleIds() {
		return jniCurrentRuleIds();
	}
	public int[] ruleIdsByLevel(int level) {
		return jniRuleIdsByLevel(level);
	}
	public String ruleDisplay(int rule) {
		return jniRuleDisplay(rule);
	}
	public String ruleShow(int rule) {
		return jniRuleShow(rule);
	}
	public String ruleDisplayByLevel(int level, int rule) {
		return jniRuleDisplayByLevel(level, rule);
	}
	public String ruleShowByLevel(int level, int rule) {
		return jniRuleShowByLevel(level, rule);
	}
	public String show() {
		return jniShow();
	}
	public String showByLevel(int level) {
		return jniShowByLevel(level);
	}
	/*
	 * ********************************************
	 * Private methods - used by the JNI
	 * ********************************************
	 */
	private ESLIFLoggerInterface getLoggerInterface() {
		return eslif != null ? eslif.getLoggerInterface() : null;
	}
	private ESLIF getEslif() {
		return eslif;
	}
	private void setEslif(ESLIF eslif) {
		this.eslif = eslif;
	}
	private ByteBuffer getMarpaGrammarp() {
		return marpaGrammarp;
	}
	private void setMarpaGrammarp(ByteBuffer marpaGrammarp) {
		this.marpaGrammarp = marpaGrammarp;
	}

}
