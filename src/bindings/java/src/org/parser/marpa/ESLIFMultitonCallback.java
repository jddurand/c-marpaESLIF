package org.parser.marpa;

import java.util.concurrent.Callable;

/**
 * ESLIF multiton callback
 *
 * Technical layer so that a parameter can be given to a Callable.
 */
public class ESLIFMultitonCallback implements Callable<ESLIF> {
	private native ESLIF jniNew(ESLIFLoggerInterface loggerInterface) throws ESLIFException;
	ESLIFLoggerInterface loggerInterface = null;

	/**
	 * @param loggerInterface logger interface
	 */
	public ESLIFMultitonCallback(ESLIFLoggerInterface loggerInterface) {
        this.loggerInterface = loggerInterface;
    }

    public ESLIF call() throws ESLIFException {
    	return new ESLIF(this.loggerInterface);
    }
}