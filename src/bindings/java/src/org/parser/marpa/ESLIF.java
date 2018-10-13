package org.parser.marpa;

import java.nio.ByteBuffer;
import java.util.Iterator;
import java.util.concurrent.Callable;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.CopyOnWriteArraySet;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.FutureTask;

/**
 * ESLIF is Extended ScanLess InterFace
 * <p>
 * ESLIF is an extension of perl's <a href="https://metacpan.org/pod/Marpa::R2" target="_top">Marpa::R2</a> BNF,
 * written as a stand-alone <a href="http://github.com/jddurand/c-marpaESLIF" target="_top">marpaESLIF</a> library.
 * <p>
 * Implementation is a multiton, resources being freed when the program exit.
 * <p>
 * Example:
 * <pre>
 * ESLIF eslif = null;
 * try &#123;
 *   eslif = new ESLIF();
 *   // Your work
 * &#125; catch (ESLIFException e) &#123;
 *  // ...
 * &#125; finally &#123;
 *   if (eslif != null) &#123;
 *     try &#123;
 *       eslif.free();
 *     &#125; catch (ESLIFException e) &#123;
 *       // ...
 *     &#125;
 *   &#125;
 * &#125;
 * </pre>
 * This class and its derivatives are thread-safe. Although there can be many ESLIF instance, in practice a single instance is enough,
 * unless you want different logging interfaces.
 */
public class ESLIF {
	private final static String  MARPAESLIFJAVA_LIBRARY_NAME = "marpaESLIFJava";
	private ESLIFLoggerInterface loggerInterface       = null;
	private ByteBuffer           marpaESLIFp           = null;
	private ByteBuffer           genericLoggerContextp = null;
	private ByteBuffer           genericLoggerp        = null;
	private native void          jniNew(int loggerInterfaceIndice) throws ESLIFException;
	// private native void          jniFree() throws ESLIFException;
	private native String        jniVersion();
	private native int           jniVersionMajor();
	private native int           jniVersionMinor();
	private native int           jniVersionPatch();
	private static native String jniJavaVersion();
	private static native int    jniJavaVersionMajor();
	private static native int    jniJavaVersionMinor();
	private static native int    jniJavaVersionPatch();
    private static final ConcurrentMap<ESLIFLoggerInterface, Future<ESLIF>> multitons = new ConcurrentHashMap<ESLIFLoggerInterface, Future<ESLIF>>();
    /* Because a null key is not possible with a ConcurrentHashMap */
    private static final ESLIFLoggerInterface nullLoggerInterface = new ESLIFMultitonNullLogger();
    /* Because genericLogger needs a "constant" */
    private static final CopyOnWriteArraySet<ESLIFLoggerInterface> loggerInterfaceArraySet = new CopyOnWriteArraySet<ESLIFLoggerInterface>();
	
	static {
		System.loadLibrary(getMarpaeslifjavaLibraryName());
	}
	
    /**
     * ESLIF multiton callback
     *
     * Technical layer so that a parameter can be given to a Callable.
     */
    private static final class ESLIFMultitonCallback implements Callable<ESLIF> {
   		private native ESLIF jniNew(ESLIFLoggerInterface loggerInterface) throws ESLIFException;
   		ESLIFLoggerInterface loggerInterface = null;

   		/**
   		 * @param loggerInterface logger interface
   		 */
   		public ESLIFMultitonCallback(ESLIFLoggerInterface loggerInterface) {
   			if (loggerInterface != nullLoggerInterface) {
   				this.loggerInterface = loggerInterface;
   			}
   	    }

   	    public ESLIF call() throws ESLIFException {
   	    	return new ESLIF(this.loggerInterface);
   	    }
   	}
    
    /**
     * @return Global instance of ESLIF without a logger
     * @throws InterruptedException interrupted exception
     */
    public static ESLIF getInstance() throws InterruptedException  {
   	 return getInstance(nullLoggerInterface);
    }

    /**
     * @param loggerInterface logger interface
     * @return Global instance of ESLIF with a logger
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

    /* Voluntarily left commented */
    /*
    public static void removeInstance(ESLIFLoggerInterface loggerInterface)  {
        Future<ESLIF> f = multitons.get(loggerInterface);
        if (f == null) {
        	return;
        }
        multitons.remove(loggerInterface, f);
    }
    */
    /*
	 * ********************************************
	 * Public methods
	 * ********************************************
	 */
    /* Voluntarily left commented */
    /*
	public synchronized void free() throws ESLIFException {
		jniFree();
	}
	*/
	
	/**
	 * Version of the Java wrapper.
	 * 
	 * @return a string describing the Java wrapper version, never null
	 */
    public static String Version() {
		return jniJavaVersion();
    }

	/**
	 * Major version of the Java wrapper.
	 * 
	 * @return an int describing the Java wrapper major version
	 */
	public static int VersionMajor() {
		return jniJavaVersionMajor();
	}

	/**
	 * Minor version of the Java wrapper.
	 * 
	 * @return an int describing the Java wrapper minor version
	 */
	public static int VersionMinor() {
		return jniJavaVersionMinor();
	}

	/**
	 * Patch version of the Java wrapper.
	 * 
	 * @return an int describing the Java wrapper patch version
	 */
	public static int VersionPatch() {
		return jniJavaVersionPatch();
	}

    /**
	 * Version of the ESLIF.
	 * 
	 * @return a string describing the ESLIF version, never null
	 */
	public String version() {
		return jniVersion();
	}

	/**
	 * Major version of the ESLIF.
	 * 
	 * @return an int describing the ESLIF major version
	 */
	public int versionMajor() {
		return jniVersionMajor();
	}

	/**
	 * Minor version of the ESLIF.
	 * 
	 * @return an int describing the ESLIF minor version
	 */
	public int versionMinor() {
		return jniVersionMinor();
	}

	/**
	 * Patch version of the ESLIF.
	 * 
	 * @return an int describing the ESLIF patch version
	 */
	public int versionPatch() {
		return jniVersionPatch();
	}

	/*
	 * *******************************************
	 * Private/protected methods - used by the JNI
	 * *******************************************
	 */
	private static int getLoggerInterfaceIndice(ESLIFLoggerInterface loggerInterface) {
		int indice = 0;
		Iterator<ESLIFLoggerInterface> iterator = loggerInterfaceArraySet.iterator();

		/* The iterator returns elements in the same order in which they were added */
		while (iterator.hasNext()) {
			/* null is possible */
			if (iterator.next() == loggerInterface) {
				return indice;
			}
			++indice;
		}

		return -1;
	}
	private static ESLIFLoggerInterface getLoggerInterfaceByIndice(int indice) {
		if (indice >= 0) {
			int i = 0;
			Iterator<ESLIFLoggerInterface> iterator = loggerInterfaceArraySet.iterator();

			/* The iterator returns elements in the same order in which they were added */
			while (iterator.hasNext()) {
				if (i == indice) {
					return iterator.next();
				}
				++i;
			}
		}

		return null;
	}
	private ESLIF(ESLIFLoggerInterface loggerInterface) throws ESLIFException {
		setLoggerInterface(loggerInterface);
		loggerInterfaceArraySet.add(loggerInterface); /* No-op if already in the set */
		jniNew(getLoggerInterfaceIndice(loggerInterface));
	}
	private static String getMarpaeslifjavaLibraryName() {
		return MARPAESLIFJAVA_LIBRARY_NAME;
	}
	protected ESLIFLoggerInterface getLoggerInterface() {
		return loggerInterface;
	}
	private void setLoggerInterface(ESLIFLoggerInterface loggerInterface) {
		this.loggerInterface = loggerInterface;
	}
	private ByteBuffer getMarpaESLIFp() {
		return marpaESLIFp;
	}
	private void setMarpaESLIFp(ByteBuffer marpaESLIFp) {
		this.marpaESLIFp = marpaESLIFp;
	}
	private ByteBuffer getGenericLoggerContextp() {
		return genericLoggerContextp;
	}
	private void setGenericLoggerContextp(ByteBuffer genericLoggerContextp) {
		this.genericLoggerContextp = genericLoggerContextp;
	}
	private ByteBuffer getGenericLoggerp() {
		return genericLoggerp;
	}
	private void setGenericLoggerp(ByteBuffer genericLoggerp) {
		this.genericLoggerp = genericLoggerp;
	}
}
