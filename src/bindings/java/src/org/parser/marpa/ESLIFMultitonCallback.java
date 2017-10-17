package org.parser.marpa;

import java.util.concurrent.Callable;

public class ESLIFMultitonCallback implements Callable<ESLIF> {
	private native ESLIF jniNew(ESLIFLoggerInterface loggerInterface) throws ESLIFException;
	ESLIFLoggerInterface loggerInterface = null;

	public ESLIFMultitonCallback(ESLIFLoggerInterface loggerInterface) {
        this.loggerInterface = loggerInterface;
    }

    public ESLIF call() throws ESLIFException {
    	return new ESLIF(this.loggerInterface);
    }
}