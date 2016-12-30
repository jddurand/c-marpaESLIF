package org.parser.marpa;

public interface ESLIFValueInterface extends ESLIFActionInterface {
	public boolean    isWithHighRankOnly();
	public boolean    isWithOrderByRank();
	public boolean    isWithAmbiguous();
	public boolean    isWithNull();
	public int        maxParses();
}
