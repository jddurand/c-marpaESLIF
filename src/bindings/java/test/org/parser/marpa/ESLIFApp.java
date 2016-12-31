package org.parser.marpa;

import java.io.BufferedReader;
import java.io.StringReader;

public class ESLIFApp  {
	
	public static void main(String[] args) throws Exception {
		ESLIFLogger eslifLogger = new ESLIFLogger();

		ESLIF eslif = new ESLIF(eslifLogger);
		
		eslifLogger.info("marpaESLIF version is " + eslif.version());

		final String grammar = 
				    ":default ::= action => do_double\n"
				  + ":discard ::= whitespace\n"
				  + "Expression ::=\n"
				  + "    /[\\d]+/                          action => do_int\n"
				  + "    | '(' Expression ')'              assoc => group action => ::copy[1]\n"
				  + "   ||     Expression '**' Expression  assoc => right\n"
				  + "   ||     Expression  '*' Expression\n"
				  + "    |     Expression  '/' Expression\n"
				  + "   ||     Expression  '+' Expression\n"
				  + "    |     Expression  '-' Expression\n"
				  + "whitespace :[1]:= [\\s]\n"
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
		
		BufferedReader input = new BufferedReader(new StringReader("(3 * 4) + 2 * 7"));
		ESLIFAppRecognizer eslifAppRecognizer = new ESLIFAppRecognizer(input);
		ESLIFAppValue eslifAppValue = new ESLIFAppValue();
		eslifGrammar.parse(eslifAppRecognizer, eslifAppValue);
		eslifGrammar.free();

		eslif.free();
	}
}
