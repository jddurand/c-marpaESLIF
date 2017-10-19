package org.parser.marpa;

/**
 * Test Application Logger
 */
public class AppLogger implements ESLIFLoggerInterface {
	public void trace(String message) {
		// System.out.println("[" + Thread.currentThread() + "] <TRACE> " + message);
		// System.out.flush();
	}

	public void debug(String message) {
		System.out.println("[" + Thread.currentThread() + "] <DEBUG> " + message);	
		System.out.flush();
	}

	public void info(String message) {
		System.out.println("[" + Thread.currentThread() + "] <INFO> " + message);	
		System.out.flush();
	}

	public void notice(String message) {
		System.out.println("[" + Thread.currentThread() + "] <NOTICE> " + message);	
		System.out.flush();
	}

	public void warning(String message) {
		System.out.println("[" + Thread.currentThread() + "] <WARNING> " + message);	
		System.out.flush();
	}

	public void error(String message) {
		System.err.println("[" + Thread.currentThread() + "] <ERROR> " + message);	
		System.err.flush();
	}

	public void critical(String message) {
		System.err.println("[" + Thread.currentThread() + "] <CRITICAL> " + message);	
		System.err.flush();
	}

	public void alert(String message) {
		System.err.println("[" + Thread.currentThread() + "] <ALERT> " + message);	
		System.err.flush();
	}

	public void emergency(String message) {
		System.err.println("[" + Thread.currentThread() + "] <EMERGENCY> " + message);	
		System.err.flush();
	}
}
