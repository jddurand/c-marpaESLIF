package org.parser.marpa;

import java.io.BufferedReader;
import java.io.IOException;
import java.nio.charset.Charset;

public class AppRecognizer implements ESLIFRecognizerInterface {
		private String         line           = null;
		private boolean        eof            = false; 
		private BufferedReader bufferedReader = null;

		public AppRecognizer(BufferedReader bufferedReader) throws Exception {
			if (bufferedReader == null) {
				throw new Exception("bufferedReader is null");
			}
			this.bufferedReader = bufferedReader;
		}

		public short read() {
			short rc;

			try {
				line = bufferedReader.readLine();
				eof = (line == null);
				rc = 1;
			} catch (IOException e) {
				e.printStackTrace();
				rc = 0;
			}
			
			return rc;
		}

		public boolean isEof() {
			return this.eof;
		}

		public boolean isCharacterStream() {
			return true;
		}

		public String encoding() {
			return Charset.defaultCharset().displayName();
		}

		public byte[] data() {
			byte[] rc = (line != null) ? line.getBytes() : null;
			
			/*
			if (rc != null) {
				System.err.println("Returning " + rc.length + " bytes");
			} else {
				System.err.println("Returning null");
			}
			*/
			return rc;
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

