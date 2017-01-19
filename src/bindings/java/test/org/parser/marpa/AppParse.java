package org.parser.marpa;

import java.io.BufferedReader;
import java.io.StringReader;
import java.io.UnsupportedEncodingException;
import java.util.Arrays;

public class AppParse  {
	
	public void test() {
		
		 ESLIF eslif = null;
		 try {
		   eslif = new ESLIF();
		 } catch (ESLIFException e) {
		  // ...
		 } finally {
		   if (eslif != null) {
		     try {
		       eslif.free();
		     } catch (ESLIFException e) {
		       // ...
		     }
		   }
		 }
	}
	
	public static void main(String[] args) throws Exception {
		AppLogger eslifLogger = new AppLogger();

		ESLIF eslif = new ESLIF(eslifLogger);
		
		eslifLogger.info("marpaESLIF version is " + eslif.version());

		final String grammar = 
				    ":start   ::= Expression\n"
				  + ":default ::=             action => do_op\n"
				  + ":discard ::= whitespaces event  => discard_whitespaces$\n"
				  + ":discard ::= comment     event  => discard_comment$\n"
				  + "\n"
				  + "event ^Number = predicted Number\n"
				  + "event Number$ = completed Number\n"
				  + "Number   ::= NUMBER   action => ::shift\n"
				  + "\n"
				  + "event Expression$ = completed Expression\n"
				  + "event ^Expression = predicted Expression\n"
				  + "Expression ::=\n"
				  + "    Number                                           action => do_int\n"
				  + "    | '(' Expression ')'              assoc => group action => ::copy[1]\n"
				  + "   ||     Expression '**' Expression  assoc => right\n"
				  + "   ||     Expression  '*' Expression\n"
				  + "    |     Expression  '/' Expression\n"
				  + "   ||     Expression  '+' Expression\n"
				  + "    |     Expression  '-' Expression\n"
				  + "\n"
				  + ":lexeme ::= NUMBER pause => before event => ^NUMBER\n"
				  + ":lexeme ::= NUMBER pause => after  event => NUMBER$\n"
				  + "NUMBER     ~ /[\\d]+/\n"
				  + "whitespaces ::= WHITESPACES\n"
				  + "WHITESPACES ~ [\\s]+\n"
				  + "comment ::= /(?:(?:(?:\\/\\/)(?:[^\\n]*)(?:\\n|\\z))|(?:(?:\\/\\*)(?:(?:[^\\*]+|\\*(?!\\/))*)(?:\\*\\/)))/u\n"
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
				"(((3 * 4) + 2 * 7) / 2 - 1)/* This is a\n comment \n */** 3",
				"5 / (2 * 3)",
				"5 / 2 * 3",
				"(5 ** 2) ** 3",
				"5 * (2 * 3)",
				"5 ** (2 ** 3)",
				"5 ** (2 / 3)",
				"1 + ( 2 + ( 3 + ( 4 + 5) )",
				"1 + ( 2 + ( 3 + ( 4 + 50) ) )   /* comment after */",
				" 100"
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
			eslifLogger.info("Testing parse() on " + string);
			try {
				eslifGrammar.parse(eslifAppRecognizer, eslifAppValue);
				Object result = eslifAppValue.getResult();
				eslifLogger.info("Result: " + result);
			} catch (Exception e) {
				eslifLogger.error("Exception: " + e);
			}
		}
		/*
		 * ====================================================================
		 * Test the recognizer's scan()/resume and the value's result() methods
		 * ====================================================================
		 */
		for (int i = 0; i < strings.length; i++) {
			String string = new String(strings[i]);

			BufferedReader reader = new BufferedReader(new StringReader(string));
			AppRecognizer eslifAppRecognizer = new AppRecognizer(reader);
			ESLIFRecognizer eslifRecognizer = new ESLIFRecognizer(eslifGrammar, eslifAppRecognizer);
			eslifLogger.info("");
			eslifLogger.info("***********************************************************");
			eslifLogger.info("Testing scan()/resume() on " + string);
			eslifLogger.info("***********************************************************");
			eslifLogger.info("");
			try {
				doScan(eslifLogger, eslifRecognizer, true);

				if (! eslifRecognizer.isEof()) {
					eslifRecognizer.read();
					showRecognizerInput("after read", eslifLogger, eslifRecognizer);
				}
				if (i == 0) {
					eslifRecognizer.progressLog(-1, -1, ESLIFLoggerLevel.get(ESLIFLoggerLevel.NOTICE.getCode()));
				}
				int j = 0;
				while (eslifRecognizer.isCanContinue()) {
					doResume(eslifLogger, eslifRecognizer, 0);

					ESLIFEvent[] events = eslifRecognizer.events();
					if (events != null) {
						for (int k = 0; k < events.length; k++) {
							ESLIFEvent event = events[k];
						    if ("^NUMBER".equals(event.getEvent())) {
						    	//
						    	// Recognizer will wait forever if we do not feed the number
						    	//
								byte[] bytes = eslifRecognizer.lexemeLastPause("NUMBER");
								if (bytes == null) {
									throw new Exception("Pause before on NUMBER but no pause information!");
								}
								doLexemeRead(eslifLogger, eslifRecognizer, "NUMBER", j, bytes);
								doDiscardTry(eslifLogger, eslifRecognizer);
								doLexemeTry(eslifLogger, eslifRecognizer, "WHITESPACES");
								doLexemeTry(eslifLogger, eslifRecognizer, "whitespaces");
						    }
						}
					}
					if (j == 0) {
						changeEventState("Loop No " + j, eslifLogger, eslifRecognizer, "Expression", ESLIFEventType.PREDICTED, false);
						changeEventState("Loop No " + j, eslifLogger, eslifRecognizer, "whitespaces", ESLIFEventType.DISCARD, false);
						changeEventState("Loop No " + j, eslifLogger, eslifRecognizer, "NUMBER", ESLIFEventType.AFTER, false);
					}
					showLastCompletion("Loop No " + j, eslifLogger, eslifRecognizer, "Expression", string);
					showLastCompletion("Loop No " + j, eslifLogger, eslifRecognizer, "Number", string);
					j++;
				}
				// Thread.sleep(10000);
				AppValue eslifAppValue = new AppValue();
				eslifLogger.info("Testing value() on " + string);
				ESLIFValue value = new ESLIFValue(eslifRecognizer, eslifAppValue);
				while (value.value()) {
					Object result = eslifAppValue.getResult();
					eslifLogger.info("Result: " + result);
				}
				value.free();
			} catch (Exception e) {
				eslifLogger.error("Exception: " + e);
			}
			eslifRecognizer.free();
		}

		eslifGrammar.free();
		eslif.free();
	}
	
	private static void doScan(ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer, boolean initialEvents) throws Exception {
		String context;
		
		eslifLogger.debug(" =============> scan(initialEvents=" + initialEvents + ")");
		eslifRecognizer.scan(initialEvents);

		context = "after scan";
		showRecognizerInput(context, eslifLogger, eslifRecognizer);
		showEvents(context, eslifLogger, eslifRecognizer);
		showLexemeExpected(context, eslifLogger, eslifRecognizer);
	}

	private static void doResume(ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer, int deltaLength) throws Exception {
		String context;
		
		eslifLogger.debug(" =============> resume(deltaLength=" + deltaLength + ")");
		eslifRecognizer.resume(deltaLength);

		context = "after resume";
		showRecognizerInput(context, eslifLogger, eslifRecognizer);
		showEvents(context, eslifLogger, eslifRecognizer);
		showLexemeExpected(context, eslifLogger, eslifRecognizer);
	}

	private static void doDiscardTry(ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer) throws UnsupportedEncodingException {
		boolean test;
		try {
			test = eslifRecognizer.discardTry();
			eslifLogger.debug("... Testing discard at current position returns " + test);
			if (test) {
				byte[] bytes = eslifRecognizer.discardLastTry();
				String string = new String(bytes, "UTF-8");
				eslifLogger.debug("... Testing discard at current position gave \"" + string + "\"");
			}
		} catch (ESLIFException e) {
			// Because we test with a symbol that is not a lexeme, and that raises an exception
			eslifLogger.debug(e.getMessage());
		}
	}

	private static void doLexemeTry(ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer, String symbol) throws UnsupportedEncodingException {
		boolean test;
		try {
			test = eslifRecognizer.lexemeTry(symbol);
			eslifLogger.debug("... Testing " + symbol + " lexeme at current position returns " + test);
			if (test) {
				byte[] bytes = eslifRecognizer.lexemeLastTry(symbol);
				String string = new String(bytes, "UTF-8");
				eslifLogger.debug("... Testing " + symbol + " lexeme at current position gave \"" + string + "\"");
			}
		} catch (ESLIFException e) {
			// Because we test with a symbol that is not a lexeme, and that raises an exception
			eslifLogger.debug(e.getMessage());
		}
	}
	//
	// We replace current NUMBER by the Integer object representing value
	//
	private static void doLexemeRead(ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer, String symbol, int value, byte[] bytes) throws Exception {
		String context;
		String old = new String(bytes, "UTF-8");
		
		eslifLogger.debug("... Forcing Integer object for \"" + value + "\" spanned on " + bytes.length + " bytes" + " instead of \"" + old + "\"");
		eslifRecognizer.lexemeRead(symbol, new Integer(value), 1 /* grammarLength */, bytes.length);

		context = "after lexemeRead";
		showRecognizerInput(context, eslifLogger, eslifRecognizer);
		showEvents(context, eslifLogger, eslifRecognizer);
		showLexemeExpected(context, eslifLogger, eslifRecognizer);
	}

	private static void showRecognizerInput(String context, ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer) throws UnsupportedEncodingException, ESLIFException {
		byte[] bytes = eslifRecognizer.input();
		if (bytes != null) {
			String str = new String(bytes, "UTF-8"); // for UTF-8 encoding
			eslifLogger.debug("[" + context + "] Recognizer buffer:\n" + str);
		} else {
			eslifLogger.debug("[" + context + "] Recognizer buffer is null");
		}
	}
	
	private static void showEvents(String context, ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer) throws Exception {
		ESLIFEvent[] events = eslifRecognizer.events();
		if (events != null) {
			for (int j = 0; j < events.length; j++) {
				ESLIFEvent event     = events[j];
				ESLIFEventType type  = event.getEslifEventType();
				String         symbol = event.getSymbol();
				String         name   = event.getEvent();
			    eslifLogger.debug("[" + context + "]" + " Event: {Type, Symbol, Name}={" + type + ", " + symbol + ", " + name + "}");
			    if (ESLIFEventType.BEFORE.equals(type)) {
					byte[] bytes = eslifRecognizer.lexemeLastPause(symbol);
					if (bytes == null) {
						throw new Exception("Pause before on " + symbol + " but no pause information!");
					}
			    }
			}
		}
	}
	
	private static void showLexemeExpected(String context, ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer) throws ESLIFException {
		String[] lexemeExpected = eslifRecognizer.lexemeExpected();
		if (lexemeExpected != null) {
			for (int j = 0; j < lexemeExpected.length; j++) {
				eslifLogger.debug("[" + context + "] Expected lexeme: " + lexemeExpected[j]);
			}
		}
	}
	
	private static void changeEventState(String context, ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer, String symbol, ESLIFEventType type, boolean state) throws ESLIFException {
	    eslifLogger.debug("[" + context + "]" + " Changing " + type + " event state of symbol " + symbol + " to " + state);
		eslifRecognizer.eventOnOff(symbol, new ESLIFEventType[] { ESLIFEventType.get(type.getCode()) },	state);
	}
	
	private static void showLastCompletion(String context, ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer, String symbol, String origin) {
		try {
			int lastExpressionOffset = eslifRecognizer.lastCompletedOffset(symbol);
			int lastExpressionLength = eslifRecognizer.lastCompletedLength(symbol);
			byte[] string2byte = origin.getBytes();
			byte[] matchedbytes = Arrays.copyOfRange(string2byte, lastExpressionOffset, lastExpressionOffset + lastExpressionLength);
			String matchedString = new String(matchedbytes, "UTF-8");
			eslifLogger.debug("[" + context + "]  Last " + symbol + " completion is: " + matchedString);
		} catch (Exception e) {
			eslifLogger.warning("[" + context + "]  Last " + symbol + " completion raised an exception");
		}
	}
}
