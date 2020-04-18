package org.parser.marpa;

/**
 * Test Application Logger
 */
public class AppLogger implements ESLIFLoggerInterface {
	public void trace(String message) {
		flushAll();
		System.out.println("[" + Thread.currentThread() + "] <TRACE> " + message);
		flushAll();
	}

	public void debug(String message) {
		flushAll();
		System.out.println("[" + Thread.currentThread() + "] <DEBUG> " + message);	
		flushAll();
	}

	public void info(String message) {
		flushAll();
		System.out.println("[" + Thread.currentThread() + "] <INFO> " + message);	
		flushAll();
	}

	public void notice(String message) {
		flushAll();
		System.out.println("[" + Thread.currentThread() + "] <NOTICE> " + message);	
		flushAll();
	}

	public void warning(String message) {
		flushAll();
		System.out.println("[" + Thread.currentThread() + "] <WARNING> " + message);	
		flushAll();
	}

	public void error(String message) {
		flushAll();
		System.err.println("[" + Thread.currentThread() + "] <ERROR> " + message);	
		flushAll();
	}

	public void critical(String message) {
		flushAll();
		System.err.println("[" + Thread.currentThread() + "] <CRITICAL> " + message);	
		flushAll();
	}

	public void alert(String message) {
		flushAll();
		System.err.println("[" + Thread.currentThread() + "] <ALERT> " + message);	
		flushAll();
	}

	public void emergency(String message) {
		flushAll();
		System.err.println("[" + Thread.currentThread() + "] <EMERGENCY> " + message);	
		flushAll();
	}
	
	private void flushAll() {
		System.err.flush();
		System.out.flush();
	}
}
