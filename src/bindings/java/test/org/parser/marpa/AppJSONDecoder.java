package org.parser.marpa;

import java.util.concurrent.TimeUnit;

/**
 * Import/export Application
 */
public class AppJSONDecoder implements Runnable {
	ESLIFLoggerInterface eslifLogger;
	
	/**
	 * @param eslifLogger logger interface
	 */
	public AppJSONDecoder(ESLIFLoggerInterface eslifLogger) {
		this.eslifLogger = eslifLogger;
	}

	public void run() {
		ESLIF eslif;
		try {
			eslif = ESLIF.getInstance(eslifLogger);
		} catch (InterruptedException e) {
			e.printStackTrace();
			return;
		}

		if (false) {
			eslifLogger.info(" ATTACH ME");
			try {
				TimeUnit.SECONDS.sleep(10);
			} catch (InterruptedException e) {
				System.err.println(e);
			}
		}

		String[] strings = {
						"     [ " +
						"       { " +
						"          \"precision\": \"zip\", " +
						"          \"Latitude\":  37.7668, " +
						"          \"Longitude\": -122.3959, " +
						"          \"Address\":   \"\", " +
						"          \"City\":      \"SAN FRANCISCO\", " +
						"          \"State\":     \"CA\", " +
						"          \"Zip\":       \"94107\", " +
						"          \"Country\":   \"US\" " +
						"       }, " +
						"       { " +
						"          \"precision\": \"zip\", " +
						"          \"Latitude\":  37.371991, " +
						"          \"Longitude\": -122.026020, " +
						"          \"Address\":   \"\", " +
						"          \"City\":      \"SUNNYVALE\", " +
						"          \"State\":     \"CA\", " +
						"          \"Zip\":       \"94085\", " +
						"          \"Country\":   \"US\" " +
						"       }, " +
						"       { " +
						"          \"+Infinity\": +Infinity, " +
						"          \"-Infinity\": -Infinity, " +
						"          \"NaN\":  NaN " +
						"       } " +
						"     ] " +
						" "
						};  

		ESLIFJSONDecoder eslifJSONDecoder = null;
		try {
			eslifJSONDecoder = new ESLIFJSONDecoder(eslif, false);
			for (int i = 0; i < strings.length; i++) {
				String string = new String(strings[i]);
	
				eslifLogger.info("Testing decode() on " + string);
				try {
					Object result = eslifJSONDecoder.decode(string);
				    Describe d = new Describe();
				    String resultDescribe = d.describe(result);
					eslifLogger.info("Result: " + resultDescribe);
				} catch (Exception e) {
					System.err.println("Failed to parse " + string + ": " + e.getMessage());
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
			return;
		} finally {
			if (eslifJSONDecoder != null) {
				try {
					eslifJSONDecoder.free();
				} catch (ESLIFException e) {
					e.printStackTrace();
				}
			}
		}
	}
}
