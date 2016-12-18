package org.parser.marpa;

public class ESLIFApp  {

	public static void main(String[] args) throws Exception {
		ESLIFLogger eslifLogger = new ESLIFLogger();

		ESLIF eslif = new ESLIF(eslifLogger);
		
		eslifLogger.info("marpaESLIF version is " + eslif.version());

		final String grammar = 
				    ":default ::= action => do_double\n"
				  + ":discard ::= [\\s]\n"
				  + "Expression ::=\n"
				  + "    /[\\d]+/                          action => do_int\n"
				  + "    | '(' Expression ')'              assoc => group action => ::copy[1]\n"
				  + "   ||     Expression '**' Expression  assoc => right\n"
				  + "   ||     Expression  '*' Expression\n"
				  + "    |     Expression  '/' Expression\n"
				  + "   ||     Expression  '+' Expression\n"
				  + "    |     Expression  '-' Expression\n"
				  + "\n";

		ESLIFGrammar eslifGrammar = new ESLIFGrammar(eslif, grammar);
		eslifGrammar.free();

		eslif.free();
	}
}
