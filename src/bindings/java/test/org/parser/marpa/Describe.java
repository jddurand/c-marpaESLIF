package org.parser.marpa;

import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Map;

/**
 * Generic Utility to be used as an alternative to toString method. This utility
 * can print the toString method of any object ( even the one's without a
 * suitable overriden toString() method )
 * 
 *
 * This also provides few options as below:
 * Ignore describing collections.
 * Ignore describing the arrays.
 * Ignore printing the type information of each field.
 *
 * 
 *
 * The default variant will print all the collections & arrays and all the type 
 * information. Also included a sample usage in the main method. 
 *
 * 
 * @author dprasad 
 * */ 
 public class Describe { 
    boolean ignoreCollections; 
	boolean ignoreArrays; 
	boolean ignoreType; 
	
	public Describe(boolean ignoreCollections, boolean ignoreArrays, boolean ignoreType) 
	{ 
	   this.ignoreArrays = ignoreArrays; 
	   this.ignoreCollections = ignoreCollections; 
	   this.ignoreType = ignoreType; 	   
	} 
	
	public Describe() { 
	   this.ignoreArrays = false; 
	   this.ignoreCollections = false; 
	   this.ignoreType = false; 
	} 
	
	@SuppressWarnings("rawtypes")
	String describeInner(Object o, StringBuilder sb) { 
	   if (o == null) { 
			sb.append("null"); 
	   } else if (o instanceof Collection) { 
	      if (!ignoreCollections) { 
	    	boolean haveContent = false;
		    sb.append(" Collection["); 
			for (Object i : (Collection) o) { 
			    describeInner(i, sb); 
				sb.append(", "); 
				haveContent = true;
			} 
			if (haveContent) {
				sb.replace(sb.length() - 2, sb.length(), "");
			}
			sb.append("] "); 
		} 
      } else if (o instanceof Map) { 
	     if (!ignoreCollections) { 
		    sb.append(" Map[");
		    Map map = (Map) o;
		    Object[] keys = map.keySet().toArray();
		    try {
		    	Arrays.sort(keys);
		    } catch (Exception e) {
		    	// ignored - this can fail if one of the key do not implement Comparable, and we do not really mind:
		    	// in this case, keys will not be sorted.
		    }
		    for (int i = 0; i < keys.length; i++) {
		    	Object key = keys[i];
		    	Object value = map.get(keys[i]);
		    	describeInner(key, sb); 
		    	sb.append(" : "); 
		    	describeInner(value, sb);
		    	sb.append(", "); 
			 }
		     if (keys.length> 0) {
		    	  sb.replace(sb.length() - 2, sb.length(), "");
		     }
			 sb.append("] "); 
		 } 
	 } else if (o.getClass().isArray()) { 
	    if (!ignoreArrays) {
	    	int length = Array.getLength(o);
	    	sb.append(" Array["); 
	    	for (int i = 0; i < length; i ++) {
	    		Object arrayElement = Array.get(o, i);
	    		describeInner(arrayElement, sb); 
	    		sb.append(", "); 
		   } 
	       if (length > 0) {
	    	   sb.replace(sb.length() - 2, sb.length(), "");
	       }
		   sb.append("] "); 
		} 
     } else if (o instanceof Byte) {
    	 /* We always want to have a number without exponent notation and without trailing zeroes */
    	 /* so that the same "values" have the same "output" regardless of the origin. */
    	 BigDecimal b = new BigDecimal((Byte)o);
    	 b = b.setScale(getNumberOfDecimalPlaces(b)).stripTrailingZeros();
	     sb.append(b.toPlainString()); 
     } else if (o instanceof Short) {
    	 /* We always want to have a number without exponent notation and without trailing zeroes */
    	 /* so that the same "values" have the same "output" regardless of the origin. */
    	 BigDecimal b = new BigDecimal((Short)o);
    	 b = b.setScale(getNumberOfDecimalPlaces(b)).stripTrailingZeros();
	     sb.append(b.toPlainString()); 
     } else if (o instanceof Integer) {
    	 /* We always want to have a number without exponent notation and without trailing zeroes */
    	 /* so that the same "values" have the same "output" regardless of the origin. */
    	 BigDecimal b = new BigDecimal((Integer)o);
    	 b = b.setScale(getNumberOfDecimalPlaces(b)).stripTrailingZeros();
	     sb.append(b.toPlainString()); 
     } else if (o instanceof Long) {
    	 /* We always want to have a number without exponent notation and without trailing zeroes */
    	 /* so that the same "values" have the same "output" regardless of the origin. */
    	 BigDecimal b = new BigDecimal((Long)o);
    	 b = b.setScale(getNumberOfDecimalPlaces(b)).stripTrailingZeros();
	     sb.append(b.toPlainString()); 
     } else if (o instanceof Float) {
    	 /* We always want to have a number without exponent notation and without trailing zeroes */
    	 /* so that the same "values" have the same "output" regardless of the origin. */
    	 /* Go through BigDecimal unless this is a special value */
    	 Float floatValue = (Float)o;
    	 if (floatValue.isInfinite() || floatValue.isNaN()) {
		     sb.append(floatValue.toString()); 
    	 } else {
	    	 BigDecimal b = new BigDecimal((Float)o);
	    	 b = b.setScale(getNumberOfDecimalPlaces(b)).stripTrailingZeros();
		     sb.append(b.toPlainString()); 
    	 }
     } else if (o instanceof Double) {
    	 /* We always want to have a number without exponent notation and without trailing zeroes */
    	 /* so that the same "values" have the same "output" regardless of the origin. */
    	 /* Go through BigDecimal unless this is a special value */
    	 Double doubleValue = (Double)o;
    	 if (doubleValue.isInfinite() || doubleValue.isNaN()) {
		     sb.append(doubleValue.toString()); 
    	 } else {
	    	 BigDecimal b = new BigDecimal((Double)o);
	    	 b = b.setScale(getNumberOfDecimalPlaces(b)).stripTrailingZeros();
		     sb.append(b.toPlainString()); 
    	 }
     } else if (o instanceof BigInteger) {
    	 /* We always want to have a number without exponent notation and without trailing zeroes */
    	 /* so that the same "values" have the same "output" regardless of the origin. */
    	 BigDecimal b = new BigDecimal((BigInteger)o);
    	 b = b.setScale(getNumberOfDecimalPlaces(b)).stripTrailingZeros();
	     sb.append(b.toPlainString()); 
     } else if (o instanceof BigDecimal) {
    	 /* We always want to have a number without exponent notation and without trailing zeroes */
    	 /* so that the same "values" have the same "output" regardless of the origin. */
    	 BigDecimal b = (BigDecimal) o;
    	 b = b.setScale(getNumberOfDecimalPlaces(b)).stripTrailingZeros();
	     sb.append(b.toPlainString()); 
     } else if (o instanceof Character || o instanceof Boolean || o instanceof String) { 
	    sb.append(o); 
	 } else if( o instanceof Enum){ 
	    sb.append(((Enum) o).name()); 
	 } else { 
	    sb.append(reflect(o)); 
	 } 
	 return null; 
 } 
	
  private int getNumberOfDecimalPlaces(BigDecimal bigDecimal) {
	  return Math.max(0, bigDecimal.stripTrailingZeros().scale());
  }
 
  @SuppressWarnings({ "rawtypes", "unchecked" })
  private List getAllFieldsRec(Class clazz, List list) { 
     Class superClazz = clazz.getSuperclass(); 
	 if(superClazz != null && !isFrameworkClass(superClazz)){ 
	    getAllFieldsRec(superClazz, list); 
	 } 
	 list.addAll(Arrays.asList(clazz.getDeclaredFields())); 
	 return list; 
  } 
  
  @SuppressWarnings("rawtypes")
  private boolean isFrameworkClass(Class clazz){ 
     if(clazz.getName().startsWith("java.")) 
	    return true; 
	 if(clazz.getName().startsWith("sun.")) 
	    return true; 
	 if(clazz.getName().startsWith("org.")) 
	    return true; 
	 return false; 
  } 
  
  @SuppressWarnings("unchecked")
  private String reflect(Object o) { 
     StringBuilder s = new StringBuilder(); 
	 if(isFrameworkClass(o.getClass())){ 
	    s.append(o); 
		return s.toString(); 
	 } 
	 
	 List<Field> fields = new ArrayList<Field>(); 
	 fields = getAllFieldsRec(o.getClass() ,fields ); 
	 s.append("Object{"); 
	 for (Field f : fields) { 
	    s.append(f.getName()); 
		s.append("=("); 
		try { 
		   f.setAccessible(true); 
		   if (!ignoreType) { 
		      String type = f.getGenericType().toString(); 
			  s.append(type); s.append(", "); 
		   } 
		   Object value = f.get(o); 
		   describeInner(value, s); 
		   s.append(")"); 
		} catch (IllegalArgumentException | IllegalAccessException e) { 
		   e.printStackTrace(); 
		} 
		s.append(", "); 
     } 
	 
	 s.replace(s.length() - 2, s.length(), ""); 
	 s.append("} "); 
	 return s.toString();  
	 
  } 
  
  public String describe(Object o) { 
     if (o != null) { 
	    StringBuilder sb = new StringBuilder(); 
		describeInner(o, sb); 
		return sb.toString(); 
	 } 
	 return null;  
  } 
}