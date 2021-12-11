package org.parser.marpa;

import java.util.HashMap;
import java.util.concurrent.TimeUnit;

/**
 * Import/export Application
 */
public class AppJSON implements Runnable {
	ESLIFLoggerInterface eslifLogger;
	
	/**
	 * @param eslifLogger logger interface
	 */
	public AppJSON(ESLIFLoggerInterface eslifLogger) {
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

		HashMap<String, Object> hmap = new HashMap<String, Object>();
		hmap.put("one", "one");
		hmap.put("two", "two");
		hmap.put("true", true);
		hmap.put("false", false);
		hmap.put("else", "again");
		hmap.put("undef", null);
		HashMap<String, Object> hmapinner = new HashMap<String, Object>();
		hmapinner.put("one", "one");
		hmapinner.put("two", "two");
		hmapinner.put("true", true);
		hmapinner.put("false", false);
		hmapinner.put("else", "again");
		hmapinner.put("undef", null);

		hmap.put("inner", hmapinner);
	      
		Object[] inputArray = {
				Character.MIN_VALUE,
				Character.MAX_VALUE,
				Short.MIN_VALUE,
				Short.MAX_VALUE,
				Integer.MIN_VALUE,
				Integer.MAX_VALUE,
				Long.MIN_VALUE,
				Long.MAX_VALUE,
				true,
				false,
				null,
				new byte[] { },
				new byte[] { '\0', '\1'},
				"",
				"test",
				new String[] {
						new String("String No 1"),
						new String("String No 2")
				},
				hmap,
				0.0f,
				Float.MAX_VALUE,
				Float.MIN_VALUE,
				Float.POSITIVE_INFINITY,
				Float.NEGATIVE_INFINITY,
				Float.NaN,
				Double.MAX_VALUE,
				Double.MIN_VALUE,
				Double.POSITIVE_INFINITY,
				Double.NEGATIVE_INFINITY,
				Double.NaN
		};  

		ESLIFJSON eslifJSON = null;
		try {
			eslifJSON = new ESLIFJSON(eslif, false);
			for (int i = 0; i < strings.length; i++) {
				String string = new String(strings[i]);
	
				eslifLogger.info("Testing decode() on " + string);
				try {
					Object decoded = eslifJSON.decode(string);
				    Describe d = new Describe();
				    String decodeDescribe = d.describe(decoded);
					eslifLogger.info("Decode result: " + decodeDescribe);

					String reencoded = eslifJSON.encode(decoded);
					eslifLogger.info("Re-encoding result: " + reencoded);
				} catch (Exception e) {
					System.err.println("Failed to parse " + string + ": " + e.getMessage());
				}
			}

			for (int i = 0; i < inputArray.length; i++) {
				String output = eslifJSON.encode(inputArray[i]);
			    Describe d = new Describe();
			    String inputDescribe = d.describe(inputArray[i]);
				eslifLogger.info("Encoding of " + inputDescribe + " : " + output);
			}
		} catch (Exception e) {
			e.printStackTrace();
			return;
		} finally {
			if (eslifJSON != null) {
				try {
					eslifJSON.free();
				} catch (ESLIFException e) {
					e.printStackTrace();
				}
			}
		}
	}
}
