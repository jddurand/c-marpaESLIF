/**
 * 
 */
package org.parser.marpa;

/**
 * @author Jean-Damien Durand
 *
 */
public class ESLIFOption {
	private ESLIFLoggerInterface loggerInterface = null;
	
	public ESLIFOption() {
	}

	public void setLoggerInterface(ESLIFLoggerInterface loggerInterface) {
		this.loggerInterface = loggerInterface;
	}

	public ESLIFLoggerInterface getLoggerInterface() {
		return loggerInterface;
	}
}
