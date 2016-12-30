package org.parser.marpa;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;

public class ESLIFGrammar {
	private ESLIF          eslif              = null;
	private ByteBuffer     marpaESLIFGrammarp = null;
	private native void    jniNew(byte[] utf8);
	private native void    jniFree();
	private native int     jniNgrammar();
	private native int     jniCurrentLevel();
	private native String  jniCurrentDescription();
	private native String  jniDescriptionByLevel(int level);
	private native int[]   jniCurrentRuleIds();
	private native int[]   jniRuleIdsByLevel(int level);
	private native String  jniRuleDisplay(int rule);
	private native String  jniRuleShow(int rule);
	private native String  jniRuleDisplayByLevel(int level, int rule);
	private native String  jniRuleShowByLevel(int level, int rule);
	private native String  jniShow();
	private native String  jniShowByLevel(int level);
	private native boolean jniParse(ESLIFRecognizer recognizer, ESLIFValue value);
	
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
	public boolean parse(ESLIFRecognizerInterface recognizerInterface, ESLIFValueInterface valueInterface) throws Exception {
		boolean   rc = false;
		Exception exception = null;
		/*
		 * The JNI will work with ESLIFRecognizer and ESLIFValue instances.
		 * So we create temporary instances from the interfaces.
		 * It is not a hazard that ESLIFRecognizer implements only ESLIFRecognizerInterface, ditto for ESLIFValue -;
		 */
		ESLIFRecognizer recognizer = new ESLIFRecognizer(this, recognizerInterface);
		ESLIFValue      value      = new ESLIFValue(recognizer, valueInterface);

		recognizer.setWithDisableThreshold(recognizerInterface.isWithDisableThreshold());
		recognizer.setWithExhaustion(recognizerInterface.isWithExhaustion());
		recognizer.setWithNewline(recognizerInterface.isWithNewline());

		value.setWithHighRankOnly(valueInterface.isWithHighRankOnly());
		value.setWithOrderByRank(valueInterface.isWithOrderByRank());
		value.setWithAmbiguous(valueInterface.isWithAmbiguous());
		value.setWithNull(valueInterface.isWithNull());
		value.setMaxParses(valueInterface.maxParses());

		try {
			rc = jniParse(recognizer, value);
		} catch (Exception e) {
			exception = e;
		} finally {
			value.free();
			recognizer.free();
		}
		
		if (exception != null) {
			throw new Exception(exception);
		}
		return rc;
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
