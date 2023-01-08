package org.parser.marpa;

import java.io.BufferedReader;
import java.io.StringReader;
import java.io.UnsupportedEncodingException;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

/**
 * Test Application thread
 */
public class AppThread implements Runnable {
	ESLIFLoggerInterface eslifLogger;
	
	/**
	 * @param eslifLogger logger interface
	 */
	public AppThread(ESLIFLoggerInterface eslifLogger) {
		this.eslifLogger = eslifLogger;
	}

	public void run() {
		try {
			ESLIF eslif = ESLIF.getInstance(eslifLogger);

			eslifLogger.info("marpaESLIFJava version is " + eslif.version());
			eslifLogger.info("marpaESLIFJava major version is " + eslif.versionMajor());
			eslifLogger.info("marpaESLIFJava minor version is " + eslif.versionMinor());
			eslifLogger.info("marpaESLIFJava patch version is " + eslif.versionPatch());
			eslifLogger.info("ESLIF object is " + eslif);
			eslifLogger.info("marpaESLIF version is " + eslif.version());
			eslifLogger.info("marpaESLIF major version is " + eslif.versionMajor());
			eslifLogger.info("marpaESLIF minor version is " + eslif.versionMinor());
			eslifLogger.info("marpaESLIF patch version is " + eslif.versionPatch());
	
			final String grammar = 
					    ":start   ::= Object2\n"
					  + "Object2  ::= Object action => ::convert[UTF-32]\n"
					  + "Object   ::= Expression action => ::concat\n"
					  + ":default ::=             action => do_op event-action => event_action\n"
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
					  + ":symbol ::= NUMBER pause => before event => ^NUMBER\n"
					  + ":symbol ::= NUMBER pause => after  event => NUMBER$\n"
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
			eslifLogger.info("current grammar properties: " + eslifGrammar.properties());
			eslifLogger.info("current grammar methods:\n" + eslifGrammar.show());
			{
				int[] rules = eslifGrammar.currentRuleIds();
				for (int i = 0; i < rules.length; i++) {
					eslifLogger.info("- current rule No " + i + ": " + rules[i]); 
					eslifLogger.info("    properties: " + eslifGrammar.ruleProperties(i)); 
					eslifLogger.info("    display form: " + eslifGrammar.ruleDisplay(i)); 
					eslifLogger.info("    show    form: " + eslifGrammar.ruleShow(i)); 
				}
				int[] symbols = eslifGrammar.currentSymbolIds();
				for (int i = 0; i < symbols.length; i++) {
					eslifLogger.info("- current symbol No " + i + ": " + symbols[i]); 
					eslifLogger.info("    properties  : " + eslifGrammar.symbolProperties(i)); 
					eslifLogger.info("    display form: " + eslifGrammar.symbolDisplay(i)); 
				}
			}
			for (int level = 0; level < eslifGrammar.ngrammar(); level++) {
				eslifLogger.info("grammar at level: " + level);
				eslifLogger.info("- grammar properties : " + eslifGrammar.propertiesByLevel(level));
				eslifLogger.info("- grammar show at level: " + level + "\n:" + eslifGrammar.showByLevel(level));
				eslifLogger.info("- rule array at level " + level);
				int[] rules = eslifGrammar.ruleIdsByLevel(level);
				for (int i = 0; i < rules.length; i++) {
					eslifLogger.info("  + rule No " + i + ": " + rules[i]); 
					eslifLogger.info("    properties  : " + eslifGrammar.rulePropertiesByLevel(level, i)); 
					eslifLogger.info("    display form: " + eslifGrammar.ruleDisplayByLevel(level, i)); 
					eslifLogger.info("    show    form: " + eslifGrammar.ruleShowByLevel(level, i)); 
				}
				eslifLogger.info("- symbol array at level " + level);
				int[] symbols = eslifGrammar.symbolIdsByLevel(level);
				for (int i = 0; i < symbols.length; i++) {
					eslifLogger.info("  + symbol No " + i + ": " + symbols[i]); 
					eslifLogger.info("    properties  : " + eslifGrammar.symbolPropertiesByLevel(level, i)); 
					eslifLogger.info("    display form: " + eslifGrammar.symbolDisplayByLevel(level, i)); 
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
					" 100",
					"not scannable at all",
					"100\nsecond line not scannable",
					"100 * /* incomplete */"
					};
	
			/*
			 * =========================================
			 * Test the gramma's parse() shortcut method
			 * =========================================
			 */
			ESLIFSymbol eslifSymbolRegex = new ESLIFSymbol(eslif, "regex", "[\\d]+");
			ESLIFSymbol eslifSymbolString = new ESLIFSymbol(eslif, "string", "'('");
			ESLIFSymbol eslifSymbolSubstitutionRegex = new ESLIFSymbol(eslif, "regex", "([\\d]+)", null, "'$1 via substitution'", null);
			ESLIFGrammar eslifGrammarForMeta = new ESLIFGrammar(eslif, "<anything up to newline> ::= <ANYTHING UP TO NEWLINE>\n<ANYTHING UP TO NEWLINE> ~ /[^\\n]*/");
			ESLIFSymbol eslifSymbolMeta = new ESLIFSymbol(eslif, eslifGrammarForMeta, "ANYTHING UP TO NEWLINE");
			for (int i = 0; i < strings.length; i++) {
				String string = new String(strings[i]);
	
				BufferedReader reader = new BufferedReader(new StringReader(string));
				AppRecognizerInterface eslifAppRecognizerInterface = new AppRecognizerInterface(reader, eslifLogger);
				AppValueInterface eslifAppValue = new AppValueInterface();
				eslifLogger.info("Testing parse() on " + string);
				try {
					byte[] eslifSymbolMatch;
					eslifSymbolMatch = eslifSymbolRegex.test(string.getBytes(StandardCharsets.UTF_8));
					if (eslifSymbolMatch != null) {
						Describe d = new Describe();
						eslifLogger.info("Direct Regex \"[\\d]+\": match: " + d.describe(new String(eslifSymbolMatch, StandardCharsets.UTF_8)));
					} else {
						eslifLogger.info("Direct Regex \"[\\d]+\": no match");
					}

					eslifSymbolMatch = eslifSymbolString.test(string.getBytes(StandardCharsets.UTF_8));
					if (eslifSymbolMatch != null) {
						Describe d = new Describe();
						eslifLogger.info("Direct String \"'('\": match: " + d.describe(new String(eslifSymbolMatch, StandardCharsets.UTF_8)));
					} else {
						eslifLogger.info("Direct String \"'('\": no match");
					}

					eslifSymbolMatch = eslifSymbolMeta.test(string.getBytes(StandardCharsets.UTF_8));
					if (eslifSymbolMatch != null) {
						Describe d = new Describe();
						eslifLogger.info("Direct Meta \"'('\": match: " + d.describe(new String(eslifSymbolMatch, StandardCharsets.UTF_8)));
					} else {
						eslifLogger.info("Direct Meta \"'('\": no match");
					}

					eslifSymbolMatch = eslifSymbolSubstitutionRegex.test(string.getBytes(StandardCharsets.UTF_8));
					if (eslifSymbolMatch != null) {
						Describe d = new Describe();
						eslifLogger.info("Substitution Regex \"[\\d]+\": match: " + d.describe(new String(eslifSymbolMatch, StandardCharsets.UTF_8)));
					} else {
						eslifLogger.info("Substitution Regex \"[\\d]+\": no match");
					}

					if (eslifGrammar.parse(eslifAppRecognizerInterface, eslifAppValue)) {
						eslifLogger.info("Result: " + eslifAppValue.getResult());
					}
				} catch (Exception e) {
					System.err.println("Failed to parse " + string + ": " + e.getMessage());
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
				AppRecognizerInterface eslifAppRecognizerInterface = new AppRecognizerInterface(reader, eslifLogger);
				ESLIFRecognizer eslifRecognizerShared = new ESLIFRecognizer(eslifGrammar, eslifAppRecognizerInterface);
				ESLIFRecognizer eslifRecognizer = new ESLIFRecognizer(eslifGrammar, eslifAppRecognizerInterface);
				eslifRecognizer.share(eslifRecognizerShared);
				eslifLogger.info("");
				eslifLogger.info("***********************************************************");
				eslifLogger.info("Testing scan()/resume() on " + string);
				eslifLogger.info("***********************************************************");
				eslifLogger.info("");
				Object eslifSymbolMatch;
				eslifSymbolMatch = eslifRecognizer.symbolTry(eslifSymbolRegex);
				if (eslifSymbolMatch != null) {
					Describe d = new Describe();
					eslifLogger.info("Indirect Regex \"[\\d]+\": match: " + d.describe(eslifSymbolMatch));
				} else {
					eslifLogger.info("Indirect Regex \"[\\d]+\": no match");
				}
				eslifSymbolMatch = eslifRecognizer.symbolTry(eslifSymbolString);
				if (eslifSymbolMatch != null) {
					Describe d = new Describe();
					eslifLogger.info("Indirect String \"'('\": match: " + d.describe(eslifSymbolMatch));
				} else {
					eslifLogger.info("Indirect String \"'('\": no match");
				}
				eslifSymbolMatch = eslifRecognizer.symbolTry(eslifSymbolSubstitutionRegex);
				if (eslifSymbolMatch != null) {
					Describe d = new Describe();
					eslifLogger.info("Indirect Substitution Regex \"[\\d]+\": match: " + d.describe(eslifSymbolMatch));
				} else {
					eslifLogger.info("Indirect Substitution Regex \"[\\d]+\": no match");
				}
				if (doScan(eslifLogger, eslifRecognizer, true)) {
					showLocation("After doScan", eslifLogger, eslifRecognizer);
					showInputLength("After doScan", eslifLogger, eslifRecognizer);
					showInput("After doScan", eslifLogger, eslifRecognizer);
					showError("After doScan", eslifLogger, eslifRecognizer);
					eslifLogger.info("isStartComplete: " + eslifRecognizer.isStartComplete());
					if (! eslifRecognizer.isEof()) {
						if (! eslifRecognizer.read()) {
							break;
						}
						showRecognizerInput("after read", eslifLogger, eslifRecognizer);
					}
					int j = 0;
					while (eslifRecognizer.isCanContinue()) {
					        eslifLogger.info("isStartComplete: " + eslifRecognizer.isStartComplete());
						eslifRecognizer.progressLog(-1, -1, ESLIFLoggerLevel.get(ESLIFLoggerLevel.NOTICE.getCode()));
						ESLIFProgress[] progress = eslifRecognizer.progress(-1, -1);
						for (int k = 0; k < progress.length; k++) {
							eslifLogger.notice("Progress[" + k + "]: " + progress[k]);
						}
						if (! doResume(eslifLogger, eslifRecognizer, 0)) {
							break;
						}
						showLocation("After doResume", eslifLogger, eslifRecognizer);
						ESLIFEvent[] events = eslifRecognizer.events();
						for (int k = 0; k < events.length; k++) {
							ESLIFEvent event = events[k];
						    if ("^NUMBER".equals(event.getEvent())) {
						    	//
						    	// Recognizer will wait forever if we do not feed the number
						    	//
								byte[] bytes = eslifRecognizer.nameLastPause("NUMBER");
								if (bytes == null) {
									throw new Exception("Pause before on NUMBER but no pause information!");
								}
								if (! doAlternativeRead(eslifLogger, eslifRecognizer, "NUMBER", j+1, bytes)) {
									throw new Exception("NUMBER expected but reading such name fails!");
								}
								doDiscardTry(eslifLogger, eslifRecognizer);
								doNameTry(eslifLogger, eslifRecognizer, "WHITESPACES");
								doNameTry(eslifLogger, eslifRecognizer, "whitespaces");
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
					byte[] discardLast = eslifRecognizer.discardLast();
					if (discardLast != null) {
						String discardLastString = new String(discardLast, "UTF-8");
						eslifLogger.debug("... Latest discarded string is \"" + discardLastString + "\"");
					}
					try {
						AppValueInterface eslifAppValue = new AppValueInterface();
						eslifLogger.info("Testing value() on " + string);
						ESLIFValue value = null;
						try {
							value = new ESLIFValue(eslifRecognizer, eslifAppValue);
							while (value.value()) {
								Object result = eslifAppValue.getResult();
								eslifLogger.info("Result: " + result);
							}
						} catch (Exception e) {
							eslifLogger.error("Failed to parse " + string + ": " + e.getMessage());
						} finally {
							if (value != null) {
								value.free();
							}
						}
					} catch (Exception e){
						eslifLogger.error("Cannot value the input: " + e);
					}
				}
				eslifRecognizer.unshare();
				eslifRecognizer.free();
			}
	
			eslifGrammar.free();
		} catch (Exception e) {
		    Thread t = Thread.currentThread();
		    t.getUncaughtExceptionHandler().uncaughtException(t, e);
		}
	}
	
	private static boolean doScan(ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer, boolean initialEvents) throws Exception {
		String context;
		
		eslifLogger.debug(" =============> scan(initialEvents=" + initialEvents + ")");
		if (! eslifRecognizer.scan(initialEvents)) {
			return false;
		}

		context = "after scan";
		showRecognizerInput(context, eslifLogger, eslifRecognizer);
		showEvents(context, eslifLogger, eslifRecognizer);
		showNameExpected(context, eslifLogger, eslifRecognizer);
		
		return true;
	}

	private static boolean doResume(ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer, int deltaLength) throws Exception {
		String context;
		
		eslifLogger.debug(" =============> resume(deltaLength=" + deltaLength + ")");
		if (! eslifRecognizer.resume(deltaLength)) {
			return false;
		}

		context = "after resume";
		showRecognizerInput(context, eslifLogger, eslifRecognizer);
		showEvents(context, eslifLogger, eslifRecognizer);
		showNameExpected(context, eslifLogger, eslifRecognizer);
		
		return true;
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
				long discardedbytes = eslifRecognizer.discard(); 
				eslifLogger.debug("... Applying discard at current position removed " + discardedbytes + " bytes, " + bytes.length + " were predicted");
			}
		} catch (ESLIFException e) {
			// Because we test with a symbol that is not a vali name, and that raises an exception
			eslifLogger.debug(e.getMessage());
		}
	}

	private static void doNameTry(ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer, String symbol) throws UnsupportedEncodingException {
		boolean test;
		try {
			test = eslifRecognizer.nameTry(symbol);
			eslifLogger.debug("... Testing " + symbol + " name at current position returns " + test);
			if (test) {
				byte[] bytes = eslifRecognizer.nameLastTry(symbol);
				String string = new String(bytes, "UTF-8");
				eslifLogger.debug("... Testing " + symbol + " name at current position gave \"" + string + "\"");
			}
		} catch (ESLIFException e) {
			// Because we test with a symbol that is not a valid name, and that raises an exception
			eslifLogger.debug(e.getMessage());
		}
	}
	//
	// We replace current NUMBER by the Integer object representing value
	//
	@SuppressWarnings("deprecation")
	private static boolean doAlternativeRead(ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer, String symbol, int value, byte[] bytes) throws Exception {
		String context;
		String old = new String(bytes, "UTF-8");
		
		eslifLogger.debug("... Forcing Integer object for \"" + value + "\" spanned on " + bytes.length + " bytes" + " instead of \"" + old + "\"");
		if (! eslifRecognizer.alternativeRead(symbol, Integer.valueOf(value), bytes.length, 1 /* grammarLength */)) {
			return false;
		}

		context = "after nameRead";
		showRecognizerInput(context, eslifLogger, eslifRecognizer);
		showEvents(context, eslifLogger, eslifRecognizer);
		showNameExpected(context, eslifLogger, eslifRecognizer);
		
		return true;
	}

	private static void showRecognizerInput(String context, ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer) throws UnsupportedEncodingException, ESLIFException {
		byte[] bytes = eslifRecognizer.input(0);
		if (bytes != null) {
			String str = new String(bytes, "UTF-8"); // for UTF-8 encoding
			eslifLogger.debug("[" + context + "] Recognizer buffer:\n" + str);
		} else {
			eslifLogger.debug("[" + context + "] Recognizer buffer is null");
		}
	}
	
	private static void showEvents(String context, ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer) throws Exception {
		ESLIFEvent[] events = eslifRecognizer.events();
		for (int j = 0; j < events.length; j++) {
			ESLIFEvent event     = events[j];
			ESLIFEventType type  = event.getType();
			String         symbol = event.getSymbol();
			String         name   = event.getEvent();
		    eslifLogger.debug("[" + context + "]" + " Event: {Type, Symbol, Name}={" + type + ", " + symbol + ", " + name + "}");
		    if (ESLIFEventType.BEFORE.equals(type)) {
				byte[] bytes = eslifRecognizer.nameLastPause(symbol);
				if (bytes == null) {
					throw new Exception("Pause before on " + symbol + " but no pause information!");
				}
		    }
		}
	}
	
	private static void showNameExpected(String context, ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer) throws ESLIFException {
		String[] nameExpected = eslifRecognizer.nameExpected();
		if (nameExpected != null) {
			for (int j = 0; j < nameExpected.length; j++) {
				eslifLogger.debug("[" + context + "] Expected name: " + nameExpected[j]);
			}
		}
	}
	
	private static void changeEventState(String context, ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer, String symbol, ESLIFEventType type, boolean state) throws ESLIFException {
	    eslifLogger.debug("[" + context + "]" + " Changing event state  " + type + " of symbol " + symbol + " to " + state);
		eslifRecognizer.eventOnOff(symbol, new ESLIFEventType[] { ESLIFEventType.get(type.getCode()) },	state);
	}
	
	private static void showLastCompletion(String context, ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer, String symbol, String origin) {
		try {
			long lastExpressionOffset = eslifRecognizer.lastCompletedOffset(symbol);
			long lastExpressionLength = eslifRecognizer.lastCompletedLength(symbol);
			byte[] string2byte = origin.getBytes();
			byte[] matchedbytes = Arrays.copyOfRange(string2byte, (int) lastExpressionOffset, (int) (lastExpressionOffset + lastExpressionLength));
			String matchedString = new String(matchedbytes, "UTF-8");
			eslifLogger.debug("[" + context + "]  Last " + symbol + " completion is: " + matchedString);
		} catch (Exception e) {
			eslifLogger.warning("[" + context + "]  Last " + symbol + " completion raised an exception: " + e);
		}
	}

	private static void showLocation(String context, ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer) {
		try {
			long line   = eslifRecognizer.line();
			long column = eslifRecognizer.column();
			eslifLogger.debug("[" + context + "]  Location is [" + line + "," + column + "]");
		} catch (Exception e) {
			eslifLogger.warning("[" + context + "]  line() or column() raised an exception: " + e);
		}
	}

	private static void showInput(String context, ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer) {
		try {
			byte[] bytes = eslifRecognizer.input(0);
			if (bytes != null) {
				String input = new String(bytes, "UTF-8");
				eslifLogger.debug("[" + context + "]  Input is: " + input);
			} else {
				eslifLogger.debug("[" + context + "]  Input returned null");
			}
			for (int offset = -10; offset <= 10; offset++) {
				bytes = eslifRecognizer.input(offset);
				if (bytes != null) {
					String input = new String(bytes, "UTF-8");
					eslifLogger.debug("[" + context + "]  ... input(" + offset + ") returns: " + input);
				} else {
					eslifLogger.debug("[" + context + "]  ... input(" + offset + ") returned null");
				}
				for (int length = -10; length <= 10; length++) {
					bytes = eslifRecognizer.input(offset, length);
					if (bytes != null) {
						String input = new String(bytes, "UTF-8");
						eslifLogger.debug("[" + context + "]  ... input(" + offset + "," + length + ") returns: " + input);
					} else {
						eslifLogger.debug("[" + context + "]  ... input(" + offset + "," + length + ") returned null");
					}
				}
			}
		} catch (Exception e) {
			eslifLogger.warning("[" + context + "]  input() raised an exception: " + e);
		}
	}

	private static void showInputLength(String context, ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer) {
		try {
			long inputLength = eslifRecognizer.inputLength();
			eslifLogger.debug("[" + context + "]  Input length is " + inputLength);
		} catch (Exception e) {
			eslifLogger.warning("[" + context + "]  inputLength() raised an exception: " + e);
		}
	}

	private static void showError(String context, ESLIFLoggerInterface eslifLogger, ESLIFRecognizer eslifRecognizer) {
		try {
			eslifLogger.debug("[" + context + "] Simulating error report:");
			eslifRecognizer.error();
		} catch (Exception e) {
			eslifLogger.warning("[" + context + "]  error() raised an exception: " + e);
		}
	}
}
