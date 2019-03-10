package org.parser.marpa;

import java.io.UnsupportedEncodingException;

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
				    "    -- print('lua_proxy received value of type: '..type(value))\n" + 
				    "    -- if type(value) == 'string' then\n" + 
				    "    --   print('lua_proxy value: '..tostring(value)..', encoding: '..tostring(value:encoding()))\n" + 
				    "    -- else\n" + 
				    "    --   print('lua_proxy value: '..tostring(value))\n" + 
				    "    --   if type(value) == 'table' then\n" + 
				    "    --     table_print(value)\n" + 
				    "    --   end\n" + 
				    "    -- end\n" + 
				    "    return value\n" + 
				    "  end\n" + 
				    "</luascript>\n"; 


		try {
			ESLIFGrammar eslifGrammar = new ESLIFGrammar(eslif, grammar);
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
			return;
		} catch (ESLIFException e) {
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
