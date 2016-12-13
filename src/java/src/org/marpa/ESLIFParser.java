/**
 * 
 */
package org.marpa;

import java.nio.ByteBuffer;
import java.util.logging.Logger;

/**
 * @author Jean-Damien Durand
 *
 */
public class ESLIFParser {
	private ByteBuffer    marpaESLIFp;
	private Logger        logger = null;

	private native      ByteBuffer marpaESLIF_newp(Logger logger);
	private native void marpaESLIF_freev();
	
	static {
	   System.loadLibrary("marpaESLIF");
	}
	
	public ESLIFParser() {
		init();
	}
	public ESLIFParser(Logger logger) {
		setLogger(logger);
		init();
	}

	private void init() {
		setMarpaESLIFp(marpaESLIF_newp(getLogger()));
	}

	private ByteBuffer getMarpaESLIFp() {
		return marpaESLIFp;
	}

	private void setMarpaESLIFp(ByteBuffer marpaESLIFp) {
		this.marpaESLIFp = marpaESLIFp;
	}

	private Logger getLogger() {
		return logger;
	}

	private void setLogger(Logger logger) {
		this.logger = logger;
	}
}
