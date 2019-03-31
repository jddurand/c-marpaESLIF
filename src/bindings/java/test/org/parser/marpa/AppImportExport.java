package org.parser.marpa;

import java.util.Arrays;
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

		if (true) {
			eslifLogger.info(" ATTACH ME");
			try {
				TimeUnit.SECONDS.sleep(10);
			} catch (InterruptedException e) {
				System.err.println(e);
			}
		}

		final String grammar = 
				    "event ^java_input = predicted java_input\n" + 
				    "\n" + 
				    "java_output ::= lua_proxy  action => java_proxy\n" + 
				    "lua_proxy   ::= java_input action => ::lua->lua_proxy\n" + 
				    "java_input  ::= JAVA_INPUT action => java_proxy\n" + 
				    "JAVA_INPUT    ~ [^\\s\\S]\n" + 
				    "\n" + 
				    "<luascript>\n" + 
				    "  function table_print (tt, indent, done)\n" + 
				    "    done = done or {}\n" + 
				    "    indent = indent or 0\n" + 
				    "    if type(tt) == \"table\" then\n" + 
				    "      for key, value in pairs (tt) do\n" + 
				    "        io.write(string.rep (\" \", indent)) -- indent it\n" + 
				    "        if type (value) == \"table\" and not done [value] then\n" + 
				    "          done [value] = true\n" + 
				    "          io.write(string.format(\"[%s] => table\\n\", tostring (key)));\n" + 
				    "          io.write(string.rep (\" \", indent+4)) -- indent it\n" + 
				    "          io.write(\"(\\n\");\n" + 
				    "          table_print (value, indent + 7, done)\n" + 
				    "          io.write(string.rep (\" \", indent+4)) -- indent it\n" + 
				    "          io.write(\")\\n\");\n" + 
				    "        else\n" + 
				    "          io.write(string.format(\"[%s] => %s\\n\",\n" + 
				    "              tostring (key), tostring(value)))\n" + 
				    "        end\n" + 
				    "      end\n" + 
				    "    else\n" + 
				    "      io.write(tostring(tt) .. \"\\n\")\n" + 
				    "    end\n" + 
				    "  end\n" + 
				    "  io.stdout:setvbuf('no')\n" + 
				    "\n" + 
				    "  function lua_proxy(value)\n" + 
				    "    print('lua_proxy received value of type: '..type(value))\n" + 
				    "    if type(value) == 'string' then\n" + 
				    "      print('lua_proxy value: '..tostring(value)..', encoding: '..tostring(value:encoding()))\n" + 
				    "    else\n" + 
				    "      print('lua_proxy value: '..tostring(value))\n" + 
				    "      if type(value) == 'table' then\n" + 
				    "        table_print(value)\n" + 
				    "      end\n" + 
				    "    end\n" + 
				    "    return value\n" + 
				    "  end\n" + 
				    "</luascript>\n"; 

		Object[] inputArray = {
				// Character.MIN_VALUE,
				// Character.MAX_VALUE,
				// Short.MIN_VALUE,
				// Short.MAX_VALUE,
				// Integer.MIN_VALUE,
				// Integer.MAX_VALUE,
				// Long.MIN_VALUE,
				// Long.MAX_VALUE,
				// true,
				// false,
				null,
				new byte[] { },
				""
				};  

		try {
			ESLIFGrammar eslifGrammar = new ESLIFGrammar(eslif, grammar);
			for (Object input : inputArray) {
				ESLIFRecognizerInterface eslifRecognizerInterface = new AppEmptyRecognizer();
			    ESLIFRecognizer eslifRecognizer = new ESLIFRecognizer(eslifGrammar, eslifRecognizerInterface);
			    eslifRecognizer.scan(true); // Initial events
			    eslifRecognizer.lexemeRead("JAVA_INPUT", input, 1, 1);
			    ESLIFValueInterface eslifValueInterface = new AppValue();
			    ESLIFValue eslifValue = new ESLIFValue(eslifRecognizer, eslifValueInterface);
			    eslifValue.value();
			    Object value = eslifValueInterface.getResult();
			    if (input == null) {
			    	// Generic object: then ESLIF guarantees it is the same that transit through all layers
				    if (value != null) {
				    	this.eslifLogger.error("KO for null");
				    	throw new Exception("null != null");
				    } else {
				    	this.eslifLogger.info("OK for null");
				    }
			    } else if (input.getClass().equals(Object.class)) {
			    	// Generic object: then ESLIF guarantees it is the same that transit through all layers
				    if (! input.equals(value)) {
				    	this.eslifLogger.error("KO for " + input + " (input class " + input.getClass().getName() + ", value class " + value.getClass().getName() + ")");
				    	throw new Exception(input + " != " + value);
				    } else {
				    	this.eslifLogger.info("OK for " + input + " (input class " + input.getClass().getName() + ", value class " + value.getClass().getName() + ")");
				    }
			    } else if (input.getClass().equals(value.getClass())) {
			    	// value is another instance of input but should be equal to input
			    	// comparing byte array is special
			    	if (value instanceof byte[]) {
			    		if (! Arrays.equals((byte[])input, (byte[])value)) {
					    	this.eslifLogger.error("KO for byte[]");
					    	throw new Exception(input + " != " + value);
			    		} else {
					    	this.eslifLogger.info("OK for byte[]");
			    		}
			    	} else {
					    if (! input.equals(value)) {
					    	this.eslifLogger.error("KO for " + input + " (input class " + input.getClass().getName() + ", value class " + value.getClass().getName() + ")");
					    	throw new Exception(input + " != " + value);
					    } else {
					    	this.eslifLogger.info("OK for " + input + " (input class " + input.getClass().getName() + ", value class " + value.getClass().getName() + ")");
					    }
			    	}
			    } else {
			    	this.eslifLogger.error("KO for " + input.toString() + " (input class " + input.getClass().getName() + ", value class " + value.getClass().getName() + ")");
			    	throw new Exception(input.toString() + " != " + value.toString());
			    }
			}
		} catch (Exception e) {
			e.printStackTrace();
			return;
		}
		
		String[] strings = {
				"(((3 * 4) + 2 * 7) / 2 - 1)/* This is a\n comment \n */** 3",
				"5 / (2 * 3)",
				"5 / 2 * 3",
				"(5 ** 2) ** 3",
				"5 * (2 * 3)",
				"5 ** (2 ** 3)",
				"5 ** (2 / 3)",
				"1 + ( 2 + ( 3 + ( 4 + 5) )",
				"1 + ( 2 + ( 3 + ( 4 + 50) ) )   /* comment after */",
				" 100",
				"not scannable at all",
				"100\nsecond line not scannable",
				"100 * /* incomplete */"
				};
		}
	}
