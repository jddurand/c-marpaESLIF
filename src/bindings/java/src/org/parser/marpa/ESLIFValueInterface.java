package org.parser.marpa;

import java.util.List;

public interface ESLIFValueInterface {
	public void    stackSet(int indice, Object object);
	public void    stackFree(int indice);
	public Object  stackGet(int indice);
	public Object  action(String actionName, List<Object> list);
	public boolean isWithHighRankOnly();
	public boolean isWithOrderByRank();
	public boolean isWithAmbiguous();
	public boolean isWithNull();
	public int     maxParses();
}
