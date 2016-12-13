/**
 * 
 */
package org.marpa;

import java.nio.ByteBuffer;

/**
 * @author Jean-Damien Durand
 *
 */
public class ESLIF {
	private ByteBuffer  _marpaESLIFp;
	private ESLIFOption _eSLIFOption;

	private native      ByteBuffer marpaESLIF_newp(ESLIFOption eSLIFOption);
	private native void marpaESLIF_freev();
	
	static {
	   System.loadLibrary("marpaESLIF");
	}
	
	public ESLIF() {
		init();
	}
	public ESLIF(ESLIFOption eSLIFOption) {
		seteSLIFOption(eSLIFOption);
		init();
	}

	private void init() {
		_eSLIFOption = geteSLIFOption();
		ByteBuffer marpaESLIFp;
		
		marpaESLIFp = marpaESLIF_newp(_eSLIFOption); 
		setMarpaESLIFp(marpaESLIFp);
	}

}
