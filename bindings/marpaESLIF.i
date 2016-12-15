%module marpaESLIF

%include "typemaps.i"

%{
#include "marpaESLIF.h"
%}

#ifdef SWIGJAVA
%include "various.i"
%javaconst(1);
#endif

/* By default this will not follow the includes - we include ourself exactly what we want */
#define GENERICLOGGER_EXPORT
%include "genericLogger.h"

#define MARPAESLIF_EXPORT
%include "marpaESLIF.h"
