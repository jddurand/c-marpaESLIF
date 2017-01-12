package org.parser.marpa;

import java.io.BufferedReader;
import java.io.StringReader;
import java.util.Arrays;

public class AppParse  {
	
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
				"1 + ( 2 + ( 3 + ( 4 + 5) ) )   /* comment after */",
				" 1"
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
			boolean eslifRecognizerFree = true;
			eslifLogger.info("Testing scan()/resume() on " + string);
			try {
				{
					byte[] bytes = eslifRecognizer.input();
					if (bytes != null) {
						String str = new String(bytes, "UTF-8"); // for UTF-8 encoding
						eslifLogger.debug("Recognizer buffer before scan: " + str);
					} else {
						eslifLogger.debug("Recognizer buffer before scan is null");
					}
				}
				eslifRecognizer.scan(true);
				{
					byte[] bytes = eslifRecognizer.input();
					if (bytes != null) {
						String str = new String(bytes, "UTF-8"); // for UTF-8 encoding
						eslifLogger.debug("Recognizer buffer after first scan: " + str);
					} else {
						eslifLogger.debug("Recognizer buffer after first scan is null");
					}
				}
				ESLIFEvent[] events = eslifRecognizer.events();
				if (events != null) {
					for (int j = 0; j < events.length; j++) {
						ESLIFEvent event = events[j];
					    eslifLogger.debug("  Event: Type=" + event.getEslifEventType() + ", Symbol=" + event.getSymbol() + ", Event=" + event.getEvent());
					}
				}
				String[] lexemeExpected = eslifRecognizer.lexemeExpected();
				if (lexemeExpected != null) {
					for (int j = 0; j < lexemeExpected.length; j++) {
						eslifLogger.debug("  Expected: " + lexemeExpected[j]);
					}
				}
				if (! eslifRecognizer.isEof()) {
					byte[] bytes = eslifRecognizer.read();
					String str = new String(bytes, "UTF-8"); // for UTF-8 encoding
					eslifLogger.debug("Current recognizer buffer after scan then read: " + str);
				}
				if (i == 0) {
					eslifRecognizer.progressLog(-1, -1, ESLIFLoggerLevel.get(ESLIFLoggerLevel.NOTICE.getCode()));
				}
				while (eslifRecognizer.isCanContinue()) {
					eslifRecognizer.resume();
					{
						byte[] bytes = eslifRecognizer.input();
						if (bytes != null) {
							String str = new String(bytes, "UTF-8"); // for UTF-8 encoding
							eslifLogger.debug("Recognizer buffer after resume: " + str);
						} else {
							eslifLogger.debug("Recognizer buffer after resume is null");
						}
					}
					events = eslifRecognizer.events();
					if (events != null) {
						for (int j = 0; j < events.length; j++) {
							ESLIFEvent event = events[j];
						    eslifLogger.debug("  Event: Type=" + event.getEslifEventType() + ", Symbol=" + event.getSymbol() + ", Event=" + event.getEvent());
						    if ("^NUMBER".equals(event.getEvent())) {
						    	//
						    	// Recognizer will wait forever if we do not feed the number
						    	//
								byte[] bytes = eslifRecognizer.pause();
								if (bytes == null) {
									throw new Exception("Pause before on NUMBER but no pause information!");
								}
								eslifRecognizer.lexemeRead("NUMBER", bytes.length);
						    }
						}
					}
					eslifRecognizer.eventOnOff(
							"Expression",
							new ESLIFEventType[] {
									ESLIFEventType.get(ESLIFEventType.PREDICTED.getCode())
							},
							false);
					eslifRecognizer.eventOnOff(
							"whitespaces",
							new ESLIFEventType[] {
									ESLIFEventType.get(ESLIFEventType.DISCARD.getCode())
							},
							false);
					eslifRecognizer.eventOnOff(
							"NUMBER",
							new ESLIFEventType[] {
									ESLIFEventType.get(ESLIFEventType.AFTER.getCode())
							},
							false);
					{
						//
						// Take great care: returned information is in terms of offsets and length meaningful to the
						// RECOGNIZER - not to us. It is documented that in the case of stream characters, marpaESLIF
						// is always thinking in terms of UTF-8. So this is an UTF-8 byte-oriented information in our
						// case
						//
						try {
							int lastExpressionOffset = eslifRecognizer.lastCompletedOffset("Expression");
							int lastExpressionLength = eslifRecognizer.lastCompletedLength("Expression");
							byte[] string2byte = string.getBytes();
							byte[] matchedbytes = Arrays.copyOfRange(string2byte, lastExpressionOffset, lastExpressionOffset + lastExpressionLength);
							String matchedString = new String(matchedbytes, "UTF-8");
							eslifLogger.debug("  Last Expression completed is: " + matchedString);
						} catch (ESLIFException e) {
							eslifLogger.error("Expression: " + e.getMessage());
						}
						try {
							int lastNumberOffset =  eslifRecognizer.lastCompletedOffset("Number");
							int lastNumberLength = eslifRecognizer.lastCompletedLength("Number");
							byte[] string2byte = string.getBytes();
							byte[] matchedbytes = Arrays.copyOfRange(string2byte, lastNumberOffset, lastNumberOffset + lastNumberLength);
							String matchedString = new String(matchedbytes, "UTF-8");
							eslifLogger.debug("  Last Number completed is: " + matchedString);
						} catch (ESLIFException e) {
							eslifLogger.error("Number: " + e.getMessage());
						}
					}
				}
			} catch (Exception e) {
				eslifLogger.error("Exception: " + e);
			} finally {
				if (eslifRecognizerFree) {
					eslifRecognizer.free();
				}
			}
		}

		eslifGrammar.free();
		eslif.free();
	}
}
