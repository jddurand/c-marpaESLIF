package org.parser.marpa;

import java.util.concurrent.CancellationException;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.FutureTask;

/**
 * ESLIF multiton
 *
 * ESLIF object is thread-safe, provided the logger is thread-safe. For one logger, there can be a single ESLIF.
 *
 * This module provides the necessary multiton implementation.
 */
public class ESLIFMultiton {

     private static final ConcurrentMap<ESLIFLoggerInterface, Future<ESLIF>> multitons = new ConcurrentHashMap<>();
     /* Because a null key is not possible with a ConcurrentHashMap */
     private static final ESLIFLoggerInterface nullLoggerInterface = new ESLIFMultitonNullLogger();

     /**
      * @return instance
      * @throws InterruptedException interrupted exception
      */
     public static ESLIF getInstance() throws InterruptedException  {
    	 return getInstance(nullLoggerInterface);
     }

     /**
      * @param loggerInterface logger interface
      * @return instance ESLIF multiton
      * @throws InterruptedException interrupted exception
      */
     public static ESLIF getInstance(ESLIFLoggerInterface loggerInterface) throws InterruptedException  {
         while (true) {
             Future<ESLIF> f = multitons.get(loggerInterface);
             if (f == null) {
                 FutureTask<ESLIF> ft = new FutureTask<ESLIF>(new ESLIFMultitonCallback(loggerInterface));
                 /*
                  * ESLIFMultitonCallback does not return a null value, so null can only mean
                  * that the key previously did not exist.
                  */
                 f = multitons.putIfAbsent(loggerInterface, ft);
                 if (f == null) {
                     f = ft;
                     ft.run();
                 }
             }
             try {
                 return f.get();
             } catch (CancellationException e) {
                 multitons.remove(loggerInterface, f);
             } catch (ExecutionException e) {
                 throw new RuntimeException(e.getCause());
             }
         }
     }
}
