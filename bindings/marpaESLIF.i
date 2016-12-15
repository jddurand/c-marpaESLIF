%module MarpaESLIF

%include "typemaps.i"

%{
#include "marpaESLIF.h"
%}

#ifdef SWIGJAVA
%rename(marpaESLIFEventType) MARPAESLIFEVENT_TYPE;
%include "various.i"

%javaconst(1);

#endif

/* By default this will not follow the includes - we include ourself exactly what we want */
/*
#define GENERICLOGGER_EXPORT
%include "genericLogger.h"
*/

#define MARPAESLIF_EXPORT
/* Everything about logging and strings is definitely handled by Java language -; */
%ignore marpaESLIFOption;
%ignore marpaESLIFString;
/* And all the C options are far too specific */
%ignore marpaESLIFGrammarOption;
%ignore marpaESLIFRecognizerOption;
%ignore marpaESLIFValueOption;

%include "marpaESLIF.h"
