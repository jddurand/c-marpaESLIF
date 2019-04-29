package org.parser.marpa;

import java.util.HashMap;

public class AppHashMap implements AppLexeme {
	private HashMap h;

	public AppHashMap(HashMap h) {
		this.h = h;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((h == null) ? 0 : h.hashCode());
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
		AppHashMap other = (AppHashMap) obj;
		if (h == null) {
			if (other.h != null)
				return false;
		} else if (!h.equals(other.h))
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "AppHashMap [h=" + h + "]";
	}
	@Override
	public Object Value() {
		return this.h;
	}
	@Override
	public AppLexeme FromValue(Object o) throws Exception {
		if (! (o instanceof HashMap)) {
			throw new Exception("Bad type");
		}
		return new AppHashMap((HashMap) o);
	}

}
