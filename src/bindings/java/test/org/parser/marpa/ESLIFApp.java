package org.parser.marpa;

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
				  + "   |x|     Expression  '*' Expression\n"
				  + "    |     Expression  '/' Expression\n"
				  + "   ||     Expression  '+' Expression\n"
				  + "    |     Expression  '-' Expression\n"
				  + "whitespace :[1]:= [\\s]\n"
				  + "\n";


		Thread.sleep(15000);
		ESLIFGrammar eslifGrammar = new ESLIFGrammar(eslif, grammar);
		eslifLogger.info("number of grammars: " + eslifGrammar.ngrammar());
		eslifGrammar.free();

		eslif.free();
	}
}
