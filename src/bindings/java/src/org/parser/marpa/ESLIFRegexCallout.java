package org.parser.marpa;

import java.util.Arrays;

/**
 * ESLIFRegexCallout is describing the callback properties of a regular expression.
 * 
 * If the grammar at level X has a :default setting like e.g. regex-action =&gt; RegexAction
 * then it is required that the ESLIFRecognizerInterface instance implements a method
 * with this name, returning an integer and accepting a single argument of type ESLIFRegexCallout, i.e.:
 * 
 * int RegexAction(ESLIFRegexCallout eslifRegexCallout) {
 *   // Anything
 *   return continueValue;
 * }
 * 
 * where continueValue is an integer that follows PCRE2 callout return value specification, c.f.
 * https://www.pcre.org/current/doc/html/pcre2callout.html
 * 
 * - If the value is zero, matching proceeds as normal
 * - If the value is greater than zero, matching fails at the current point,
 *   but the testing of other matching possibilities goes ahead, just as if
 *   a lookahead assertion had failed.
 * - If the value is less than zero, the match is abandoned, and the matching function returns the negative value. 
 *
 * ESLIF prevents negative values to be lower than the most negative meaningful value. Exhaustive current list is
 * as of PCRE2 version 10.33, i.e.:
 * 
#define PCRE2_ERROR_NOMATCH          (-1)
#define PCRE2_ERROR_PARTIAL          (-2)
#define PCRE2_ERROR_UTF8_ERR1        (-3)
#define PCRE2_ERROR_UTF8_ERR2        (-4)
#define PCRE2_ERROR_UTF8_ERR3        (-5)
#define PCRE2_ERROR_UTF8_ERR4        (-6)
#define PCRE2_ERROR_UTF8_ERR5        (-7)
#define PCRE2_ERROR_UTF8_ERR6        (-8)
#define PCRE2_ERROR_UTF8_ERR7        (-9)
#define PCRE2_ERROR_UTF8_ERR8       (-10)
#define PCRE2_ERROR_UTF8_ERR9       (-11)
#define PCRE2_ERROR_UTF8_ERR10      (-12)
#define PCRE2_ERROR_UTF8_ERR11      (-13)
#define PCRE2_ERROR_UTF8_ERR12      (-14)
#define PCRE2_ERROR_UTF8_ERR13      (-15)
#define PCRE2_ERROR_UTF8_ERR14      (-16)
#define PCRE2_ERROR_UTF8_ERR15      (-17)
#define PCRE2_ERROR_UTF8_ERR16      (-18)
#define PCRE2_ERROR_UTF8_ERR17      (-19)
#define PCRE2_ERROR_UTF8_ERR18      (-20)
#define PCRE2_ERROR_UTF8_ERR19      (-21)
#define PCRE2_ERROR_UTF8_ERR20      (-22)
#define PCRE2_ERROR_UTF8_ERR21      (-23)
#define PCRE2_ERROR_UTF16_ERR1      (-24)
#define PCRE2_ERROR_UTF16_ERR2      (-25)
#define PCRE2_ERROR_UTF16_ERR3      (-26)
#define PCRE2_ERROR_UTF32_ERR1      (-27)
#define PCRE2_ERROR_UTF32_ERR2      (-28)
#define PCRE2_ERROR_BADDATA           (-29)
#define PCRE2_ERROR_MIXEDTABLES       (-30)
#define PCRE2_ERROR_BADMAGIC          (-31)
#define PCRE2_ERROR_BADMODE           (-32)
#define PCRE2_ERROR_BADOFFSET         (-33)
#define PCRE2_ERROR_BADOPTION         (-34)
#define PCRE2_ERROR_BADREPLACEMENT    (-35)
#define PCRE2_ERROR_BADUTFOFFSET      (-36)
#define PCRE2_ERROR_CALLOUT           (-37)  // Never used by PCRE2 itself
#define PCRE2_ERROR_DFA_BADRESTART    (-38)
#define PCRE2_ERROR_DFA_RECURSE       (-39)
#define PCRE2_ERROR_DFA_UCOND         (-40)
#define PCRE2_ERROR_DFA_UFUNC         (-41)
#define PCRE2_ERROR_DFA_UITEM         (-42)
#define PCRE2_ERROR_DFA_WSSIZE        (-43)
#define PCRE2_ERROR_INTERNAL          (-44)
#define PCRE2_ERROR_JIT_BADOPTION     (-45)
#define PCRE2_ERROR_JIT_STACKLIMIT    (-46)
#define PCRE2_ERROR_MATCHLIMIT        (-47)
#define PCRE2_ERROR_NOMEMORY          (-48)
#define PCRE2_ERROR_NOSUBSTRING       (-49)
#define PCRE2_ERROR_NOUNIQUESUBSTRING (-50)
#define PCRE2_ERROR_NULL              (-51)
#define PCRE2_ERROR_RECURSELOOP       (-52)
#define PCRE2_ERROR_DEPTHLIMIT        (-53)
#define PCRE2_ERROR_RECURSIONLIMIT    (-53)
#define PCRE2_ERROR_UNAVAILABLE       (-54)
#define PCRE2_ERROR_UNSET             (-55)
#define PCRE2_ERROR_BADOFFSETLIMIT    (-56)
#define PCRE2_ERROR_BADREPESCAPE      (-57)
#define PCRE2_ERROR_REPMISSINGBRACE   (-58)
#define PCRE2_ERROR_BADSUBSTITUTION   (-59)
#define PCRE2_ERROR_BADSUBSPATTERN    (-60)
#define PCRE2_ERROR_TOOMANYREPLACE    (-61)
#define PCRE2_ERROR_BADSERIALIZEDDATA (-62)
#define PCRE2_ERROR_HEAPLIMIT         (-63)
#define PCRE2_ERROR_CONVERT_SYNTAX    (-64)
#define PCRE2_ERROR_INTERNAL_DUPMATCH (-65)

 * Any value lower than PCRE2_ERROR_INTERNAL_DUPMATCH will emit a warning by ESLIF, the later changing
 * it to PCRE2_ERROR_CALLOUT
 *
 * By default subject and pattern are undefined for performance reasons. The user can always retrieve
 * current subject by using recognizer's input(), and current symbol pattern using grammar's symbolPropertiesByLevel().
 */

public class ESLIFRegexCallout {
	private Integer calloutNumber;
	private String calloutString;
	private byte[] subject;
	private String pattern;
	private long captureTop;
	private long captureLast;
	private long[] offsetVector;
	private Long mark;
	private long startMatch;
	private long currentPosition;
	private String nextItem;
	private int grammarLevel;
	private int symbolId;

	/**
	 * @param calloutNumber callout number
	 * @param calloutString callout string
	 * @param subject current subject 
	 * @param pattern regular expression pattern
	 * @param captureTop maximum recent capture
	 * @param captureLast most recently closed capture
	 * @param offsetVector offset vector
	 * @param mark current mark offset
	 * @param startMatch current mark start attempt offset
	 * @param currentPosition current subject offset
	 * @param nextItem next item in the pattern
	 * @param grammarLevel current grammar level
	 * @param symbolId current symbol id
	 */
	public ESLIFRegexCallout(Integer calloutNumber, String calloutString, byte[] subject, String pattern, long captureTop, long captureLast, long[] offsetVector, Long mark, long startMatch, long currentPosition, String nextItem, int grammarLevel, int symbolId) {
		this.calloutNumber = calloutNumber;
		this.calloutString = calloutString;
		this.subject = subject;
		this.pattern = pattern;
		this.captureTop = captureTop;
		this.captureLast = captureLast;
		this.offsetVector = offsetVector;
		this.mark = mark;
		this.startMatch = startMatch;
		this.currentPosition = currentPosition;
		this.nextItem = nextItem;
		this.grammarLevel = grammarLevel;
		this.symbolId = symbolId;
	}

	/**
	 * @return the callout number
	 */
	public Integer getCalloutNumber() {
		return calloutNumber;
	}

	/**
	 * @return the callout string
	 */
	public String getCalloutString() {
		return calloutString;
	}

	/**
	 * @return the current subject
	 * 
	 * null by default unless ESLIF library is compiled in trace mode.
	 */
	public byte[] getSubject() {
		return subject;
	}

	/**
	 * @return the pattern
	 * 
	 * null by default unless ESLIF library is compiled in trace mode.
	 */
	public String getPattern() {
		return pattern;
	}

	/**
	 * @return the max recent capture
	 */
	public long getCaptureTop() {
		return captureTop;
	}

	/**
	 * @return the most recently closed capture
	 */
	public long getCaptureLast() {
		return captureLast;
	}

	/**
	 * @return the offset vector
	 */
	public long[] getOffsetVector() {
		return offsetVector;
	}

	/**
	 * @return the current mark offset
	 */
	public Long getMark() {
		return mark;
	}

	/**
	 * @return the current mark start attempt offset
	 */
	public long getStartMatch() {
		return startMatch;
	}

	/**
	 * @return the current subject offset
	 */
	public long getCurrentPosition() {
		return currentPosition;
	}

	/**
	 * @return the next item in the pattern
	 */
	public String getNextItem() {
		return nextItem;
	}

	/**
	 * @return the current grammar level
	 */
	public int getGrammarLevel() {
		return grammarLevel;
	}

	/**
	 * @return the current symbol id
	 */
	public int getSymbolId() {
		return symbolId;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "ESLIFRegexCallout [calloutNumber=" + calloutNumber + ", calloutString=" + calloutString + ", subject="
				+ Arrays.toString(subject) + ", pattern=" + pattern + ", captureTop=" + captureTop + ", captureLast="
				+ captureLast + ", offsetVector=" + Arrays.toString(offsetVector) + ", mark=" + mark + ", startMatch="
				+ startMatch + ", currentPosition=" + currentPosition + ", nextItem=" + nextItem + ", grammarLevel="
				+ grammarLevel + ", symbolId=" + symbolId + "]";
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#hashCode()
	 */
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((calloutNumber == null) ? 0 : calloutNumber.hashCode());
		result = prime * result + ((calloutString == null) ? 0 : calloutString.hashCode());
		result = prime * result + (int) (captureLast ^ (captureLast >>> 32));
		result = prime * result + (int) (captureTop ^ (captureTop >>> 32));
		result = prime * result + (int) (currentPosition ^ (currentPosition >>> 32));
		result = prime * result + grammarLevel;
		result = prime * result + ((mark == null) ? 0 : mark.hashCode());
		result = prime * result + ((nextItem == null) ? 0 : nextItem.hashCode());
		result = prime * result + Arrays.hashCode(offsetVector);
		result = prime * result + ((pattern == null) ? 0 : pattern.hashCode());
		result = prime * result + (int) (startMatch ^ (startMatch >>> 32));
		result = prime * result + Arrays.hashCode(subject);
		result = prime * result + symbolId;
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
		ESLIFRegexCallout other = (ESLIFRegexCallout) obj;
		if (calloutNumber == null) {
			if (other.calloutNumber != null) {
				return false;
			}
		} else if (!calloutNumber.equals(other.calloutNumber)) {
			return false;
		}
		if (calloutString == null) {
			if (other.calloutString != null) {
				return false;
			}
		} else if (!calloutString.equals(other.calloutString)) {
			return false;
		}
		if (captureLast != other.captureLast) {
			return false;
		}
		if (captureTop != other.captureTop) {
			return false;
		}
		if (currentPosition != other.currentPosition) {
			return false;
		}
		if (grammarLevel != other.grammarLevel) {
			return false;
		}
		if (mark == null) {
			if (other.mark != null) {
				return false;
			}
		} else if (!mark.equals(other.mark)) {
			return false;
		}
		if (nextItem == null) {
			if (other.nextItem != null) {
				return false;
			}
		} else if (!nextItem.equals(other.nextItem)) {
			return false;
		}
		if (!Arrays.equals(offsetVector, other.offsetVector)) {
			return false;
		}
		if (pattern == null) {
			if (other.pattern != null) {
				return false;
			}
		} else if (!pattern.equals(other.pattern)) {
			return false;
		}
		if (startMatch != other.startMatch) {
			return false;
		}
		if (!Arrays.equals(subject, other.subject)) {
			return false;
		}
		if (symbolId != other.symbolId) {
			return false;
		}
		return true;
	}
}
