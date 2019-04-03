package org.parser.marpa;

public class AppNull implements AppLexeme {
	String s;

	public AppNull() {
		this.s = null;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((s == null) ? 0 : s.hashCode());
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
		AppNull other = (AppNull) obj;
		if (s == null) {
			if (other.s != null)
				return false;
		} else if (!s.equals(other.s))
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "AppNull [s=" + s + "]";
	}

	@Override
	public Object Value() {
		return null;
	}

	@Override
	public AppLexeme FromValue(Object o) throws Exception {
		if (o != null) {
			throw new Exception("Bad type");
		}
		return new AppNull();
	}
}
