package org.parser.marpa;

/**
 * ESLIFProgress is describing parsing progress from the recognizer.
 */
public class ESLIFProgress {
	private int earleySetId;
	private int earleySetOrigId;
	private int rule;
	private int position;
	
	/**
	 * 
	 * @param earleySetId the Earley Set Id
	 * @param earleySetOrigId the origin Earley Set Id
	 * @param rule the rule number
	 * @param position the position in the rule
	 */
	ESLIFProgress(int earleySetId, int earleySetOrigId, int rule, int position) {
		this.earleySetId     = earleySetId;
		this.earleySetOrigId = earleySetOrigId;
		this.rule            = rule;
		this.position        = position;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "ESLIFProgress [earleySetId=" + earleySetId + ", earleySetOrigId=" + earleySetOrigId + ", rule="
				+ rule + ", position=" + position + "]";
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#hashCode()
	 */
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + earleySetId;
		result = prime * result + earleySetOrigId;
		result = prime * result + position;
		result = prime * result + rule;
		return result;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(Object obj) {
		if (this == obj) {
			return true;
		}
		if (obj == null) {
			return false;
		}
		if (getClass() != obj.getClass()) {
			return false;
		}
		ESLIFProgress other = (ESLIFProgress) obj;
		if (earleySetId != other.earleySetId) {
			return false;
		}
		if (earleySetOrigId != other.earleySetOrigId) {
			return false;
		}
		if (position != other.position) {
			return false;
		}
		if (rule != other.rule) {
			return false;
		}
		return true;
	}

	/**
	 * @return the earleySetId
	 */
	public int getearleySetId() {
		return earleySetId;
	}

	/**
	 * @return the earleySetOrigId
	 */
	public int getearleySetOrigId() {
		return earleySetOrigId;
	}

	/**
	 * @return the rule
	 */
	public int getrule() {
		return rule;
	}

	/**
	 * @return the position
	 */
	public int getposition() {
		return position;
	}
}
