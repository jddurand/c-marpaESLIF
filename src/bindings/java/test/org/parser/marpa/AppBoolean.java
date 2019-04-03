package org.parser.marpa;

public class AppBoolean implements AppLexeme {
	private boolean b;

	public AppBoolean(boolean b) {
		this.b = b;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + (b ? 1231 : 1237);
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
		AppBoolean other = (AppBoolean) obj;
		if (b != other.b)
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "AppBoolean [b=" + b + "]";
	}

	@Override
	public Object Value() {
		return this.b;
	}

	@Override
	public AppLexeme FromValue(Object o) throws Exception {
		if (! (o instanceof Boolean)) {
			throw new Exception("Bad type");
		}
		return new AppBoolean((boolean) o);
	}
}
