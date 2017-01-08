package org.parser.marpa;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;

/*
 * In our example we have NOT specified a symbol action, therefore
 * lexemes that come directly from the grammar are in the form of
 * a ByteBuffer.
 */
public class ESLIFAppValue implements ESLIFValueInterface {
	private Object result = null;
	
	/*
	 * https://java.net/jira/browse/TYRUS-216
	 */
	private byte[] getActiveArray(ByteBuffer buffer)
	{
	  byte[] ret = new byte[buffer.remaining()];
	  if (buffer.hasArray())
	  {
	      byte[] array = buffer.array();
	      System.arraycopy(array, buffer.arrayOffset() + buffer.position(), ret, 0, ret.length);
	  }
	  else
	  {
	      buffer.slice().get(ret);
	  }
	  return ret;
	}
	
	private String byteBufferToString(ByteBuffer byteBuffer) throws UnsupportedEncodingException {
		return new String(getActiveArray(byteBuffer), "UTF-8");
	}
		
	private Double toDouble(Object object) {
		return (object instanceof Integer)  ? new Double(((Integer) object).doubleValue())  : (Double) object;
	}

	private Integer toInteger(Object object) {
		return (Integer) object;
	}
	
	public Object do_int(Object[] list) throws UnsupportedEncodingException {
		Object result;
		String input = byteBufferToString((ByteBuffer) list[0]); 

		result = new Integer(input);
		return result;
	}

	public Object do_op(Object[] list) throws UnsupportedEncodingException {
		Object left   = list[0];
		String op     = byteBufferToString((ByteBuffer) list[1]); 
		Object right  = list[2];
		Object result = null;

		boolean leftIsInteger  = (left  instanceof Integer);
		boolean rightIsInteger = (right instanceof Integer);
		
		if ("**".equals(op)) {
			result = new Double(Math.pow(toDouble(left), toDouble(right)));
		} else if ("*".equals(op)) {
			if (leftIsInteger && rightIsInteger) {
				result = toInteger(left) * toInteger(right);
			} else {
				result = toDouble(left) * toDouble(right);
			}
		} else if ("/".equals(op)) {
			result = toDouble(left) / toDouble(right);
		} else if ("+".equals(op)) {
			if (leftIsInteger && rightIsInteger) {
				result = toInteger(left) + toInteger(right);
			} else {
				result = toDouble(left) + toDouble(right);
			}
		} else if ("-".equals(op)) {
			if (leftIsInteger && rightIsInteger) {
				result = toInteger(left) - toInteger(right);
			} else {
				result = toDouble(left) - toDouble(right);
			}
		}
		return result;
	}

	public boolean isWithHighRankOnly() {
		return true;
	}

	public boolean isWithOrderByRank() {
		return true;
	}

	public boolean isWithAmbiguous() {
		return false;
	}

	public boolean isWithNull() {
		return false;
	}

	public int maxParses() {
		return 0;
	}

	public Object getResult() {
		return result;
	}

	public void setResult(Object result) {
		this.result = result;
	}

}

