package org.parser.marpa;

public class ESLIFApp  {

	public static void main(String[] args) throws Exception {
		ESLIFOption eslifOption = new ESLIFOption();
		eslifOption.setLoggerInterface(new ESLIFLogger());

		ESLIF eslif = new ESLIF(eslifOption);
		eslif.free();
	}
}
