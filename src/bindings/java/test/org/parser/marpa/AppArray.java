package org.parser.marpa;

import java.util.Arrays;

public class AppArray implements AppLexeme {
	private Object[] a;

	public AppArray(Object[] a) {
		this.a = a;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + Arrays.hashCode(a);
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
		AppArray other = (AppArray) obj;
		if (!Arrays.equals(a, other.a))
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "AppArray [a=" + Arrays.toString(a) + "]";
	}

	@Override
	public Object Value() {
		return this.a;
	}

	@Override
	public AppLexeme FromValue(Object o) throws Exception {
		if (! (o instanceof Object[])) {
			throw new Exception("Bad type");
		}
		return new AppArray((Object[]) o);
	}
}
