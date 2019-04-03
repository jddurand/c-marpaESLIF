package org.parser.marpa;

public class AppShort implements AppLexeme {
	private short s;

	public AppShort(short s) {
		this.s = s;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + s;
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
		AppShort other = (AppShort) obj;
		if (s != other.s)
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "AppShort [s=" + s + "]";
	}

	@Override
	public Object Value() {
		return this.s;
	}

	@Override
	public AppLexeme FromValue(Object o) throws Exception {
		if (! (o instanceof Short)) {
			throw new Exception("Bad type");
		}
		return new AppShort((Short) o);
	}
}
