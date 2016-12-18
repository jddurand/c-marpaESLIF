package org.parser.marpa;

public class ESLIFLogger implements ESLIFLoggerInterface {
	public void trace(String message) {
		System.out.println("<TRACE> " + message);	
	}

	public void debug(String message) {
		System.out.println("<DEBUG> " + message);	
	}

	public void info(String message) {
		System.out.println("<INFO> " + message);	
	}

	public void notice(String message) {
		System.out.println("<NOTICE> " + message);	
	}

	public void warning(String message) {
		System.out.println("<WARNING> " + message);	
	}

	public void error(String message) {
		System.err.println("<ERROR> " + message);	
	}

	public void critical(String message) {
		System.err.println("<CRITICAL> " + message);	
	}

	public void alert(String message) {
		System.err.println("<ALERT> " + message);	
	}

	public void emergency(String message) {
		System.err.println("<EMERGENCY> " + message);	
	}
}

