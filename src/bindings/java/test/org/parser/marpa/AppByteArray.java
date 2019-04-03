package org.parser.marpa;

import java.util.Arrays;

public class AppByteArray implements AppLexeme {
	private byte[] a;

	public AppByteArray(byte[] a) {
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
		AppByteArray other = (AppByteArray) obj;
		if (!Arrays.equals(a, other.a))
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "AppByteArray [a=" + Arrays.toString(a) + "]";
	}

	@Override
	public Object Value() {
		return this.a;
	}

	@Override
	public AppLexeme FromValue(Object o) throws Exception {
		if (! (o instanceof byte[])) {
			throw new Exception("Bad type");
		}
		return new AppByteArray((byte[]) o);
	}
}
