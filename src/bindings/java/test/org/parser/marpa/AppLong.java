package org.parser.marpa;

public class AppLong implements AppLexeme {
	private long l;

	public AppLong(long l) {
		this.l = l;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + (int) (l ^ (l >>> 32));
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
		AppLong other = (AppLong) obj;
		if (l != other.l)
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "AppLong [l=" + l + "]";
	}

	@Override
	public Object Value() {
		return this.l;
	}

	@Override
	public AppLexeme FromValue(Object o) throws Exception {
		if (! (o instanceof Long)) {
			throw new Exception("Bad type");
		}
		return new AppLong((Long) o);
	}
}
