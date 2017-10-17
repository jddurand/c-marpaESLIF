package org.parser.marpa;

/**
 * Null Logger implementation used in multiton
 */
public class ESLIFMultitonNullLogger implements ESLIFLoggerInterface {
	public void trace(String message) {}
	public void debug(String message) {}
	public void info(String message) {}
	public void notice(String message) {}
	public void warning(String message) {}
	public void error(String message) {}
	public void critical(String message) {}
	public void alert(String message) {}
	public void emergency(String message) {}
}
