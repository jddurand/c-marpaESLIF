/* Module name */
%module marpaESLIFWrapper

 /* Standard includes and helpers */
%include "typemaps.i"
%{
#include "marpaESLIF.h"
%}

/* Language specific helpers */
#ifdef SWIGJAVA
%include "various.i"
%javaconst(1);
#endif

/* By default this will not follow the includes - we include ourself exactly what we want */
/* Generic logger is not needed in high-level languages */
/*
#define GENERICLOGGER_EXPORT
%include "genericLogger.h"
*/

/* The methods to wrap */
#define MARPAESLIF_EXPORT
%include "marpaESLIF.h"
