package org.parser.marpa;

public interface AppLexeme {
	public Object Value();
	public AppLexeme FromValue(Object o) throws Exception;
}
