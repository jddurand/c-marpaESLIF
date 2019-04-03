package org.parser.marpa;

public class AppString implements AppLexeme {
	private String s;

	public AppString(String s) {
		this.s = s;
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
		AppString other = (AppString) obj;
		if (s == null) {
			if (other.s != null)
				return false;
		} else if (!s.equals(other.s))
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "AppString [s=" + s + "]";
	}

	@Override
	public Object Value() {
		return this.s;
	}

	@Override
	public AppLexeme FromValue(Object o) throws Exception {
		if (! (o instanceof String)) {
			throw new Exception("Bad type");
		}
		return new AppString((String) o);
	}
}
