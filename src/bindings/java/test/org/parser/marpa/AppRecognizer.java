package org.parser.marpa;

import java.io.BufferedReader;
import java.nio.charset.Charset;

public class AppRecognizer implements ESLIFRecognizerInterface {
		private String         line           = null;
		private BufferedReader bufferedReader = null;

		public AppRecognizer(BufferedReader bufferedReader) throws Exception {
			if (bufferedReader == null) {
				throw new Exception("bufferedReader is null");
			}
			this.bufferedReader = bufferedReader;
		}

		public void read() throws Exception {
			line = bufferedReader.readLine();
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
		
	}

