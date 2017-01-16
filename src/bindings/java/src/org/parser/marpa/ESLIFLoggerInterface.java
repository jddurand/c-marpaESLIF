package org.parser.marpa;

/**
 * ESLIFLoggerInterface is listing all required logging methods.
 * <p>
 * A logger interface may be associated to an {@link ESLIF} instance. When provided @{link ESLIF} can generate messages at different levels, that are:
 * <ul>
 * <li> <code>TRACE</code>
 * <li> <code>DEBUG</code>
 * <li> <code>INFO</code>
 * <li> <code>NOTIVE</code>
 * <li> <code>WARNING</code>
 * <li> <code>ERROR</code>
 * <li> <code>CRITICAL</code>
 * <li> <code>ALERT</code>
 * <li> <code>EMERGENCY</code>
 * </ul>
 *
 * This interface is explicitly reproducing the formalism of the <a href="http://github.com/jddurand/c-genericLogger" target="_top">genericLogger C library</a>.
 */
public interface ESLIFLoggerInterface {
	/**
	 * Log messages with level <code>TRACE</code>
	 * 
	 * @param message the string to log, never null
	 */
	public void trace(String message);

	/**
	 * Log messages with level <code>DEBUG</code>
	 * 
	 * @param message the string to log, never null
	 */
	public void debug(String message);

	/**
	 * Log messages with level <code>INFO</code>
	 * 
	 * @param message the string to log, never null
	 */
	public void info(String message);

	/**
	 * Log messages with level <code>NOTICE</code>
	 * 
	 * @param message the string to log, never null
	 */
	public void notice(String message);

	/**
	 * Log messages with level <code>WARNING</code>
	 * 
	 * @param message the string to log, never null
	 */
	public void warning(String message);

	/**
	 * Log messages with level <code>ERROR</code>
	 * 
	 * @param message the string to log, never null
	 */
	public void error(String message);

	/**
	 * Log messages with level <code>CRITICAL</code>
	 * 
	 * @param message the string to log, never null
	 */
	public void critical(String message);

	/**
	 * Log messages with level <code>ALERT</code>
	 * 
	 * @param message the string to log, never null
	 */
	public void alert(String message);

	/**
	 * Log messages with level <code>EMERGENCY</code>
	 * 
	 * @param message the string to log, never null
	 */
	public void emergency(String message);
}
