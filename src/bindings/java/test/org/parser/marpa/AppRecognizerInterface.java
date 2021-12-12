package org.parser.marpa;

import java.io.BufferedReader;
import java.io.UnsupportedEncodingException;
import java.nio.charset.Charset;
import java.util.Arrays;

/**
 * Test Application Recognizer
 */
public class AppRecognizerInterface implements ESLIFRecognizerInterface {
		private String               line                 = null;
		private BufferedReader       bufferedReader       = null;
		private ESLIFLoggerInterface eslifLoggerInterface = null;
		private ESLIFRecognizer      eslifRecognizer      = null;

		/**
		 * @param bufferedReader buffered reader
		 * @throws Exception exception
		 */
		public AppRecognizerInterface(BufferedReader bufferedReader, ESLIFLoggerInterface eslifLoggerInterface) throws Exception {
			if (bufferedReader == null) {
				throw new Exception("bufferedReader is null");
			}
			this.bufferedReader       = bufferedReader;
			if (eslifLoggerInterface == null) {
				throw new Exception("eslifLoggerInterface is null");
			}
			this.eslifLoggerInterface = eslifLoggerInterface;
		}

		public boolean read() {
			boolean rcb;
			try {
				line = bufferedReader.readLine();
				rcb = true;
			} catch (Exception e) {
				rcb = false;
			}
			return rcb;
		}

		public boolean isEof() {
			return (line == null);
		}

		public boolean isCharacterStream() {
			return true;
		}

		public String encoding() {
			return Charset.defaultCharset().displayName();
		}

		public byte[] data() {
			return (line != null) ? line.getBytes() : null;
		}

		public boolean isWithDisableThreshold() {
			return false;
		}

		public boolean isWithExhaustion() {
			return false;
		}

		public boolean isWithNewline() {
			return true;
		}
		
		public boolean isWithTrack() {
			return true;
		}
		
		public void setEslifRecognizer(ESLIFRecognizer eslifRecognizer) {
			eslifLoggerInterface.debug("... setEslifRecognizer is called: " + eslifRecognizer);
			this.eslifRecognizer = eslifRecognizer;
		}

		public ESLIFRecognizer getEslifRecognizer() {
			return this.eslifRecognizer;
		}	

		public boolean if_number(byte[] byteArray) throws UnsupportedEncodingException {
			String string = new String(byteArray, "UTF-8");
			eslifLoggerInterface.debug("... if_number(" + string + ")");
			return true;
		}	

		public boolean event_action(ESLIFEvent[] eventArray) throws ESLIFException, UnsupportedEncodingException {
			eslifLoggerInterface.debug("... event_action(" + Arrays.toString(eventArray) + ")");
			eslifLoggerInterface.debug("... event_action current input length: " + eslifRecognizer.inputLength());
			byte[] bytes = eslifRecognizer.input(0);
			if (bytes != null) {
				String input = new String(bytes, "UTF-8");
				eslifLoggerInterface.debug("... regex_action current input is: " + input);
			} else {
				eslifLoggerInterface.debug("... regex_action current input is null");
			}
			return true;
		}	

		public int regex_action(ESLIFRegexCallout eslifRegexCallout) throws ESLIFException, UnsupportedEncodingException {
			eslifLoggerInterface.debug("regex_action(" + eslifRegexCallout + ")");
			int grammarLevel = eslifRegexCallout.getGrammarLevel();
			int symbolId = eslifRegexCallout.getSymbolId();
			ESLIFGrammar eslifGrammar  = this.getEslifRecognizer().getEslifGrammar();
			ESLIFGrammarSymbolProperties eslifSymbolProperties = eslifGrammar.symbolPropertiesByLevel(grammarLevel, symbolId);
			eslifLoggerInterface.debug("... regex_action was for symbol " + symbolId + " of grammar level " + grammarLevel + " :" + eslifSymbolProperties);
			eslifLoggerInterface.debug("... regex_action current input length: " + eslifRecognizer.inputLength());
			byte[] bytes = eslifRecognizer.input(0);
			if (bytes != null) {
				String input = new String(bytes, "UTF-8");
				eslifLoggerInterface.debug("... regex_action current input is: " + input);
			} else {
				eslifLoggerInterface.debug("... regex_action current input is null");
			}
			return 0;
		}	

		public boolean if_action_comma(byte[] byteArray) throws ESLIFException, UnsupportedEncodingException {
			String string = new String(byteArray, "UTF-8");
			eslifLoggerInterface.debug("... if_action_comma(" + string + ")");
			eslifLoggerInterface.debug("... if_action_comma current input length: " + eslifRecognizer.inputLength());
			byte[] bytes = eslifRecognizer.input(0);
			if (bytes != null) {
				String input = new String(bytes, "UTF-8");
				eslifLoggerInterface.debug("... if_action_comma current input is: " + input);
			} else {
				eslifLoggerInterface.debug("... if_action_comma current input is null");
			}
			return true;
		}	

		public String rhs(Object...list) throws UnsupportedEncodingException {
			eslifLoggerInterface.debug("... rhs called with " + list.length + " objects");
			for (int i = 0; i < list.length; i++) {
				if (list[i] == null) {
					eslifLoggerInterface.debug("... Object[" + i + "] is null");
				} else {
				    Describe d = new Describe();
				    String resultDescribe = d.describe(list[i]);
			    	Class resultClass = list[i].getClass();
					eslifLoggerInterface.debug("... Object[" + i + "] is a " + resultClass + " and description: " + resultDescribe);
				}
			}
			
			Integer parameter = (Integer) list[0];
			String explanation = (list[2] instanceof byte[]) ? new String((byte[]) list[2], "UTF-8") : (String) list[2];

			eslifLoggerInterface.debug("... parameter=" + parameter + ", explanation=\"" + explanation + "\"");

		    String output;
		    if (parameter > 4) {
		        output = "start ::= '" + parameter + "'\n";
		    } else {
		        output = "start ::= . => rhs->(5, { [String('x')] = String('Value of x'), [String('y')] = String('Value of y') }, String('Input should be \"5\"'))\n";
		    }
			eslifLoggerInterface.debug("  ==> " + output);

		    return output;
		}	
}
