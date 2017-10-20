package org.parser.marpa;

import java.util.ArrayList;
import java.util.concurrent.TimeUnit;

/**
 * Test Application
 */
public class AppParse  {
	
	/**
	 * @param args arguments, unused
	 */
	public static void main(String[] args)  {
		AppLogger eslifLogger1 = new AppLogger();
		AppLogger eslifLogger2 = new AppLogger();
		AppLogger eslifLogger = eslifLogger2;
		int       nbthread = 5;
		int       ithread;
		ArrayList<Thread> threadlist = new ArrayList<Thread>();
		int       nbalive;

		/*
		eslifLogger.info(" ATTACH ME");
		try {
			TimeUnit.SECONDS.sleep(10);
		} catch (InterruptedException e) {
			System.err.println(e);
		}
		*/
		
		eslifLogger.info("Running over " + nbthread + " threads, using two loggers");
		
		for (ithread = 0; ithread < nbthread; ithread++) {
			eslifLogger = (eslifLogger == eslifLogger2) ? eslifLogger1 : eslifLogger2;
			eslifLogger.info("Starting thread No " + ithread + " with logger " + eslifLogger);
			Thread t = new Thread(new AppThread(eslifLogger));
			t.start();
			threadlist.add(t);
		}
		
		nbalive = nbthread;
		while (nbalive > 0) {
			nbalive = 0;
			for (int i = 0; i < threadlist.size(); i++) {
				Thread t = threadlist.get(i);
				if (t != null) {
					if (t.isAlive()) {
						eslifLogger.info("Thread No " + i + " is still alive");
						nbalive++;
					} else {
						threadlist.set(i, null);
					}
				}
			}
			if (nbalive > 0) {
				eslifLogger.info(nbalive + " threads alive - sleeping one second");
				try {
					TimeUnit.SECONDS.sleep(1);
				} catch (InterruptedException e) {
					System.err.println(e);
				}
			} else {
				eslifLogger.info("All " + threadlist.size() + " threads finished");
			}
		}
	}
}
