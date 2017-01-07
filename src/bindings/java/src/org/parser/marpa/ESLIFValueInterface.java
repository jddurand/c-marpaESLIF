package org.parser.marpa;

public interface ESLIFValueInterface {
	public boolean isWithHighRankOnly();
	public boolean isWithOrderByRank();
	public boolean isWithAmbiguous();
	public boolean isWithNull();
	public int     maxParses();
	public void    setResult(Object result);
	public Object  getResult();
}
