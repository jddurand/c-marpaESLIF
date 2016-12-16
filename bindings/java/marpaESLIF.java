package org.parser;

import org.parser.marpaESLIFWrapperJava.*;

public class marpaESLIF {
    private SWIGTYPE_p_marpaESLIF marpaESLIFp = null;

    static {
        System.loadLibrary("marpaESLIFWrapperJava");
    }

    marpaESLIF() {
    	setMarpaESLIFp(org.parser.marpaESLIFWrapperJava.marpaESLIFWrapper.marpaESLIF_newp(null));
    }

    public SWIGTYPE_p_marpaESLIF getMarpaESLIFp() {
        return marpaESLIFp;
    }

    public void setMarpaESLIFp(SWIGTYPE_p_marpaESLIF marpaESLIFp) {
        this.marpaESLIFp = marpaESLIFp;
    }
}
