package org.parser.marpa;

public class AppInteger implements AppLexeme {
	private int i;

	public AppInteger(int i) {
		this.i = i;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + i;
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
		AppInteger other = (AppInteger) obj;
		if (i != other.i)
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "AppInteger [i=" + i + "]";
	}

	@Override
	public Object Value() {
		return this.i;
	}

	@Override
	public AppLexeme FromValue(Object o) throws Exception {
		if (! (o instanceof Integer)) {
			throw new Exception("Bad type");
		}
		return new AppInteger((Integer) o);
	}
}
