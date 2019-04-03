package org.parser.marpa;

public class AppCharacter implements AppLexeme {
	private char c;

	public AppCharacter(char c) {
		this.c = c;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + c;
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		AppCharacter other = (AppCharacter) obj;
		if (c != other.c)
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "AppCharacter [c=" + c + "]";
	}

	@Override
	public Object Value() {
		return this.c;
	}

	@Override
	public AppLexeme FromValue(Object o) throws Exception {
		if (o instanceof String) {
			// Okay only if the length is 1
			String s = (String) o;
			if (s.length() != 1) {
				throw new Exception("String length must be 1");
			}
			return new AppCharacter(s.charAt(0));
		}
		if (! (o instanceof Character)) {
			throw new Exception("Bad type");
		}
		return new AppCharacter((Character) o);
	}
}
