package org.parser.marpa;

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
				    ":default ::= symbol-action => java_symbolAction\n" +
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
	      
		AppLexeme[] inputArray = {
				new AppCharacter(Character.MIN_VALUE),
				new AppCharacter(Character.MAX_VALUE),
				new AppShort(Short.MIN_VALUE),
				new AppShort(Short.MAX_VALUE),
				new AppInteger(Integer.MIN_VALUE),
				new AppInteger(Integer.MAX_VALUE),
				new AppLong(Long.MIN_VALUE),
				new AppLong(Long.MAX_VALUE),
				new AppBoolean(true),
				new AppBoolean(false),
				new AppNull(),
				new AppByteArray(new byte[] { }),
				new AppByteArray(new byte[] { '\0', '\1'}),
				new AppString(""),
				new AppString("test"),
				new AppArray(
						new String[] {
								new String("String No 1"),
								new String("String No 2")
						}
						),
				new AppHashMap(hmap)
		};  

		ESLIFGrammar eslifGrammar = null;
	    ESLIFValue eslifValue = null;
		try {
			eslifGrammar = new ESLIFGrammar(eslif, grammar);
			for (AppLexeme input : inputArray) {
				ESLIFRecognizerInterface eslifRecognizerInterface = new AppEmptyRecognizer();
			    ESLIFRecognizer eslifRecognizer = new ESLIFRecognizer(eslifGrammar, eslifRecognizerInterface);
			    eslifRecognizer.scan(true); // Initial events
			    eslifRecognizer.lexemeRead("JAVA_INPUT", input.Value(), 1, 1);
			    ESLIFValueInterface eslifValueInterface = new AppValue();
			    eslifValue = new ESLIFValue(eslifRecognizer, eslifValueInterface);
			    eslifValue.value();
			    Object value = eslifValueInterface.getResult();
			    Object fromValue = input.FromValue(value);
			    
			    if (!input.equals(fromValue)) {
			    	this.eslifLogger.error("KO for " + input + " (value: " + fromValue + ")");
			    	throw new Exception(input + " != " + value);
			    } else {
			    	this.eslifLogger.info("OK for " + input + " (value: " + fromValue + ")");
			    }
			    eslifValue.free();
			    eslifValue = null;
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
