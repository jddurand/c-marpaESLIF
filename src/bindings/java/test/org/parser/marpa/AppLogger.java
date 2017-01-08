package org.parser.marpa;

public class AppLogger implements ESLIFLoggerInterface {
	public void trace(String message) {
		System.out.println("<TRACE> " + message);
		System.out.flush();
	}

	public void debug(String message) {
		System.out.println("<DEBUG> " + message);	
		System.out.flush();
	}

	public void info(String message) {
		System.out.println("<INFO> " + message);	
		System.out.flush();
	}

	public void notice(String message) {
		System.out.println("<NOTICE> " + message);	
		System.out.flush();
	}

	public void warning(String message) {
		System.out.println("<WARNING> " + message);	
		System.out.flush();
	}

	public void error(String message) {
		System.err.println("<ERROR> " + message);	
		System.err.flush();
	}

	public void critical(String message) {
		System.err.println("<CRITICAL> " + message);	
		System.err.flush();
	}

	public void alert(String message) {
		System.err.println("<ALERT> " + message);	
		System.err.flush();
	}

	public void emergency(String message) {
		System.err.println("<EMERGENCY> " + message);	
		System.err.flush();
	}
}
