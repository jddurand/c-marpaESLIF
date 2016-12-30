package org.parser.marpa;

import java.nio.ByteBuffer;
import java.util.List;

public class ESLIFValue implements ESLIFValueInterface {
	private ESLIFRecognizer      eslifRecognizer  = null;
	private ByteBuffer           marpaValuep      = null;
	private ESLIFActionInterface actionInterface  = null;
	private boolean              withHighRankOnly = true;
	private boolean              withOrderByRank  = true;
	private boolean              withAmbiguous    = false;
	private boolean              withNull         = false;
	private int                  maxParses        = 0;
	private native void          jniNew(ESLIFRecognizer recognizer);
	private native void          jniFree();
	
	public ESLIFValue(ESLIFRecognizer recognizer, ESLIFActionInterface actionInterface) {
		if (recognizer == null) {
			throw new IllegalAccessError("recognizer must not be null");
		}
		if (actionInterface == null) {
			throw new IllegalAccessError("actionInterface must not be null");
		}
		setEslifRecognizer(recognizer);
		setActionInterface(actionInterface);
		jniNew(recognizer);
	}

	public void free() {
		jniFree();
	}
	public Object ruleAction(String actionName, List<Object> objectList, boolean nullable) {
		return getActionInterface().ruleAction(actionName, objectList, nullable);
	}
	public Object symbolAction(byte[] data) {
		return getActionInterface().symbolAction(data);
	}
	private ESLIFActionInterface getActionInterface() {
		return actionInterface;
	}
	private void setActionInterface(ESLIFActionInterface actionInterface) {
		this.actionInterface = actionInterface;
	}
	public boolean isWithHighRankOnly() {
		return withHighRankOnly;
	}
	protected void setWithHighRankOnly(boolean withHighRankOnly) {
		this.withHighRankOnly = withHighRankOnly;
	}
	public boolean isWithOrderByRank() {
		return withOrderByRank;
	}
	protected void setWithOrderByRank(boolean withOrderByRank) {
		this.withOrderByRank = withOrderByRank;
	}
	public boolean isWithAmbiguous() {
		return withAmbiguous;
	}
	protected void setWithAmbiguous(boolean withAmbiguous) {
		this.withAmbiguous = withAmbiguous;
	}
	public boolean isWithNull() {
		return withNull;
	}
	protected void setWithNull(boolean withNull) {
		this.withNull = withNull;
	}
	public int getMaxParses() {
		return maxParses;
	}
	protected void setMaxParses(int maxParses) {
		this.maxParses = maxParses;
	}
	public int maxParses() {
		return getMaxParses();
	}
	/*
	 * ********************************************
	 * Private methods - used by the JNI
	 * ********************************************
	 */
	private ESLIFRecognizer getEslifRecognizer() {
		return eslifRecognizer;
	}
	private void setEslifRecognizer(ESLIFRecognizer eslifRecognizer) {
		this.eslifRecognizer = eslifRecognizer;
	}
	private ByteBuffer getMarpaValuep() {
		return marpaValuep;
	}
	private void setMarpaValuep(ByteBuffer marpaValuep) {
		this.marpaValuep = marpaValuep;
	}
}
