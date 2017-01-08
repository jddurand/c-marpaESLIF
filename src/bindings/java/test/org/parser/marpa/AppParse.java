package org.parser.marpa;

import java.io.BufferedReader;
import java.io.StringReader;

public class AppParse  {
	
	public static void main(String[] args) throws Exception {
		AppLogger eslifLogger = new AppLogger();

		ESLIF eslif = new ESLIF(eslifLogger);
		
		eslifLogger.info("marpaESLIF version is " + eslif.version());

		final String grammar = 
				    ":default ::= action => do_op\n"
				  + ":discard ::= whitespace\n"
				  + ":discard ::= comment\n"
				  + "Expression ::=\n"
				  + "    /[\\d]+/                                         action => do_int\n"
				  + "    | '(' Expression ')'              assoc => group action => ::copy[1]\n"
				  + "   ||     Expression '**' Expression  assoc => right\n"
				  + "   ||     Expression  '*' Expression\n"
				  + "    |     Expression  /\\/(?!\\*)/ Expression\n"
				  + "   ||     Expression  '+' Expression\n"
				  + "    |     Expression  '-' Expression\n"
				  + "whitespace :[1]:= [\\s]\n"
				  + "comment ~ /(?:(?:(?:\\/\\/)(?:[^\\n]*)(?:\\n|\\z))|(?:(?:\\/\\*)(?:(?:[^\\*]+|\\*(?!\\/))*)(?:\\*\\/)))/u\n"
				  + "\n";


		ESLIFGrammar eslifGrammar = new ESLIFGrammar(eslif, grammar);
		eslifLogger.info("number of grammars         : " + eslifGrammar.ngrammar());
		eslifLogger.info("current grammar level      : " + eslifGrammar.currentLevel());
		eslifLogger.info("current grammar description: " + eslifGrammar.currentDescription());
		for (int level = 0; level < eslifGrammar.ngrammar(); level++) {
			eslifLogger.info("- grammar description at level " + level + ": " + eslifGrammar.descriptionByLevel(level));;
		}
		eslifLogger.info("current grammar show:\n" + eslifGrammar.show());
		{
			int[] rules = eslifGrammar.currentRuleIds();
			for (int i = 0; i < rules.length; i++) {
				eslifLogger.info("- current rule No " + i + ": " + rules[i]); 
				eslifLogger.info("    display form: " + eslifGrammar.ruleDisplay(i)); 
				eslifLogger.info("    show    form: " + eslifGrammar.ruleShow(i)); 
			}
		}
		for (int level = 0; level < eslifGrammar.ngrammar(); level++) {
			eslifLogger.info("- grammar show at level: " + level + "\n:" + eslifGrammar.showByLevel(level));
			eslifLogger.info("- rule array at level " + level);
			int[] rules = eslifGrammar.ruleIdsByLevel(level);
			for (int i = 0; i < rules.length; i++) {
				eslifLogger.info("  + rule No " + i + ": " + rules[i]); 
				eslifLogger.info("    display form: " + eslifGrammar.ruleDisplayByLevel(level, i)); 
				eslifLogger.info("    show    form: " + eslifGrammar.ruleShowByLevel(level, i)); 
			}
		}
		
		String[] strings = {
				"(((3 * 4) + 2 * 7) / 2 - 1) \n" + "/* This is a comment */\n ** 3",
				"5 / (2 * 3)",
				"5 / 2 * 3",
				"(5 ** 2) ** 3",
				"5 * (2 * 3)",
				"5 ** (2 ** 3)",
				"5 ** (2 / 3)",
				"1 + ( 2 + ( 3 + ( 4 + 5) )",
				"1 + ( 2 + ( 3 + ( 4 + 5) ) )"
				};

		/*
		 * =========================================
		 * Test the gramma's parse() shortcut method
		 * =========================================
		 */
		for (int i = 0; i < strings.length; i++) {
			String string = new String(strings[i]);

			BufferedReader reader = new BufferedReader(new StringReader(string));
			AppRecognizer eslifAppRecognizer = new AppRecognizer(reader);
			AppValue eslifAppValue = new AppValue();
			try {
				eslifGrammar.parse(eslifAppRecognizer, eslifAppValue);
				Object result = eslifAppValue.getResult();
				eslifLogger.trace(string + " = " + result);
			} catch (Exception e) {
				eslifLogger.error(string + ": " + e);
			}
		}
		/*
		 * ====================================================================
		 * Test the recognizer's scan()/resume and the value's result() methods
		 * ====================================================================
		 */

		eslifGrammar.free();
		eslif.free();
	}
}
