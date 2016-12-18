package org.parser.marpa;

public class ESLIFLogger implements ESLIFLoggerInterface {
	public void trace(String message) {
		System.out.println("<TRACE> " + message);	
	}

	public void debug(String message) {
		System.out.println("<TRACE> " + message);	
	}

	public void info(String message) {
		System.out.println("<TRACE> " + message);	
	}

	public void notice(String message) {
		System.out.println("<TRACE> " + message);	
	}

	public void warning(String message) {
		System.out.println("<TRACE> " + message);	
	}

	public void error(String message) {
		System.err.println("<TRACE> " + message);	
	}

	public void critical(String message) {
		System.err.println("<TRACE> " + message);	
	}

	public void alert(String message) {
		System.err.println("<TRACE> " + message);	
	}

	public void emergency(String message) {
		System.err.println("<TRACE> " + message);	
	}
}

