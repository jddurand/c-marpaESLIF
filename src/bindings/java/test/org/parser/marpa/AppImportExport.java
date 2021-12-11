package org.parser.marpa;

import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.concurrent.TimeUnit;

/**
 * Import/export Application
 */
public class AppImportExport implements Runnable {
	ESLIFLoggerInterface eslifLogger;
	
	/**
	 * @param eslifLogger logger interface
	 */
	public AppImportExport(ESLIFLoggerInterface eslifLogger) {
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

		final String grammar = 
				    ":default ::= symbol-action => java_symbolAction event-action => java_eventAction\n" +
				    "event ^java_input = predicted java_input\n" + 
				    "\n" + 
				    "java_output ::= lua_proxy  action => java_proxy\n" + 
				    "lua_proxy   ::= java_input action => ::lua->lua_proxy\n" + 
				    "java_input  ::= JAVA_INPUT action => java_proxy\n" + 
				    "JAVA_INPUT    ~ [^\\s\\S]\n" + 
				    "\n" + 
				    "<luascript>\n" + 
				    "function table_print (tt, indent, done)\n" +
				    "  done = done or {}\n" +
				    "  indent = indent or 0\n" +
				    "  if type(tt) == 'table' then\n" +
				    "    for key, value in pairs (tt) do\n" +
				    "      io.write(string.rep (' ', indent)) -- indent it\n" +
				    "      if type (value) == 'table' and not done [value] then\n" +
				    "        done [value] = true\n" +
				    "        io.write(string.format(\"  [%s] => table\\n\", tostring(key)));\n" +
				    "        io.write(string.rep (' ', indent+4)) -- indent it\n" +
				    "        io.write(\"(\\n\");\n" +
				    "        table_print (value, indent + 7, done)\n" +
				    "        io.write(string.rep (\" \", indent+4)) -- indent it\n" +
				    "        io.write(\")\\n\");\n" +
				    "      else\n" +
				    "        if type(value) == 'string' then\n" +
				    "          io.write(string.format(\"  [%s] => %s (type: %s, encoding: %s, length: %d bytes)\\n\", tostring (key), tostring(value), type(value), tostring(value:encoding()), string.len(value)))\n" +
				    "        else\n" +
				    "          io.write(string.format(\"  [%s] => %s (type: %s)\\n\", tostring (key), tostring(value), type(value)))\n" +
				    "        end\n" +
				    "      end\n" +
				    "    end\n" +
				    "  else\n" +
				    "    io.write(tostring(tt) .. \"\\n\")\n" +
				    "  end\n" +
				    "end\n" +
				    "io.stdout:setvbuf('no')\n" +
				    "\n" +
				    "function lua_proxy(value)\n" +
				    "  print('  lua_proxy received value of type: '..type(value))\n" +
				    "  if type(value) == 'string' then\n" +
				    "    print('  lua_proxy value: '..tostring(value)..', encoding: '..tostring(value:encoding())..', length: '..string.len(value)..' bytes')\n" +
				    "  else\n" +
				    "    print('  lua_proxy value: '..tostring(value))\n" +
				    "    if type(value) == 'table' then\n" +
				    "      table_print(value)\n" +
				    "    end\n" +
				    "  end\n" +
				    "  return value\n" +
				    "end\n" +
				    "</luascript>\n"; 

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

		ESLIFGrammar eslifGrammar = null;
	    ESLIFValue eslifValue = null;
		try {
			eslifGrammar = new ESLIFGrammar(eslif, grammar);
			for (Object input : inputArray) {
				ESLIFRecognizerInterface eslifRecognizerInterface = new AppEmptyRecognizer(this.eslifLogger);
			    ESLIFRecognizer eslifRecognizer = new ESLIFRecognizer(eslifGrammar, eslifRecognizerInterface);
			    eslifRecognizer.scan(true); // Initial events
			    eslifRecognizer.alternativeRead("JAVA_INPUT", input, 1, 1);
			    ESLIFValueInterface eslifValueInterface = new AppValueInterface();
			    eslifValue = new ESLIFValue(eslifRecognizer, eslifValueInterface);
			    eslifValue.value();
			    Object result = eslifValueInterface.getResult();
			    eslifValue.free();
			    eslifValue = null;

			    if (input == null) {
			    	if (result == null) {
				    	eslifLogger.info("OK: null");
			    	} else {
					    Describe d = new Describe();
					    String resultDescribe = d.describe(result);
				    	Class resultClass = result.getClass();
						throw new Exception("KO: input=null, result=" + resultDescribe + " (class: " + resultClass + ")");
			    	}
			    } else {
				    Describe d = new Describe();
				    String inputDescribe = d.describe(input);
				    String resultDescribe = d.describe(result);
	
				    if (inputDescribe.equals(resultDescribe)) {
				    	eslifLogger.info("OK: " + inputDescribe);
				    } else {
				    	/* The only case when we accept discripancy is when result is of class */
				    	/* BigDecimal or BigInteger and input is not */
				    	if (  ((result instanceof BigDecimal) || (result instanceof BigInteger)) &&
				    		! ((input  instanceof BigDecimal) || (input  instanceof BigInteger))) {
					    	Class resultClass = result.getClass();
							eslifLogger.warning("OK: input=" + inputDescribe + ", result=" + resultDescribe + " (class: " + resultClass + ")");
				    	} else {
					    	Class resultClass = result.getClass();
							throw new Exception("KO: input=" + inputDescribe + ", result=" + resultDescribe + " (class: " + resultClass + ")");
				    	}
			    	}
			    }
			}
		} catch (Exception e) {
			e.printStackTrace();
			return;
		} finally {
			if (eslifGrammar != null) {
				try {
					eslifGrammar.free();
				} catch (ESLIFException e) {
					e.printStackTrace();
				}
			}
			if (eslifValue != null) {
				try {
					eslifValue.free();
				} catch (ESLIFException e) {
					e.printStackTrace();
				}
			}
		}
	}
}
