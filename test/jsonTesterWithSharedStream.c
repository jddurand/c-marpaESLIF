#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static char context;

static short                         inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp);
static short                         doparseb(genericLogger_t *genericLoggerp, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, marpaESLIFGrammar_t *marpaESLIFGrammarObjectp, char *inputs, int recursionleveli);

typedef struct marpaESLIFTester_context {
  char            *inputs;
  size_t           inputl;
} marpaESLIFTester_context_t;

const static char *dsl = "\n"
":default ::= action => ::shift\n"
":start       ::= XXXXXX # Replaced on-the-fly by json or object\n"
"json         ::= object\n"
"               | array\n"
"# comma is provided as a char class here, to ensure that char classes\n"
"# as separators are in the test suite.\n"
"object       ::= LCURLY members RCURLY\n"
"               | OBJECT_FROM_INNER_GRAMMAR\n"
"members      ::= pair*                 action => do_array separator => ','\n"
"pair         ::= string ':' value      action => do_array\n"
"value        ::= string\n"
"               | object\n"
"               | number\n"
"               | array\n"
"               | 'true'                action => do_true\n"
"               | 'false'               action => do_true\n"
"               | 'null'                action => ::undef\n"
"array        ::= '[' ']'               action => do_empty_array\n"
"               | '[' elements ']'\n"
"# comma is provided as a char class here, to ensure that char classes\n"
"# as separators are in the test suite.\n"
"elements     ::= value+                action => do_array separator => ','\n"
"number         ~ int\n"
"               | int frac\n"
"               | int exp\n"
"               | int frac exp\n"
"int            ~ digits\n"
"               | '-' digits\n"
"digits         ~ [\\d]+\n"
"frac           ~ '.' digits\n"
"exp            ~ e digits\n"
"e              ~ 'e'\n"
"               | 'e+'\n"
"               | 'e-'\n"
"               | 'E'\n"
"               | 'E+'\n"
"               | 'E-'\n"
"string       ::= lstring\n"
":lexeme ::= lstring pause => after event => lstring$\n"
"lstring        ~ quote in_string quote\n"
"quote          ~ [\"]\n"
"in_string      ~ in_string_char*\n"
"in_string_char  ~ [^\"] | '\\\\\"'\n"
":discard       ::= whitespace\n"
"whitespace     ~ [\\s]+\n"
":lexeme ::= LCURLY pause => before event => ^LCURLY\n"
"LCURLY         ~ '{'\n"
":lexeme ::= RCURLY pause => before event => ^RCURLY\n"
"RCURLY         ~ '}'\n"
"OBJECT_FROM_INNER_GRAMMAR ~ [^\\s\\S]\n"
  ;

static char *inputs[] = {
    "\xEF\xBB\xBF{\"test\":\"1\"}",
    "{\"test\":[1,2,3]}",
    "{\"test\":true}",
    "{\"test\":false}",
    "{\"test\":null}",
    "{\"test\":null, \"test2\":\"hello world\"}",
    "{\"test\":\"1.25\"}",
    "{\"test\":\"1.25e4\"}",
    "{\"empty string\":\"\"}",
    "[]",
    "[\n"
    "  {\n"
    "     \"precision\": \"zip\",\n"
    "     \"Latitude\":  37.7668,\n"
    "     \"Longitude\": -122.3959,\n"
    "     \"Address\":   \"\",\n"
    "     \"City\":      \"SAN FRANCISCO\",\n"
    "     \"State\":     \"CA\",\n"
    "     \"Zip\":       \"94107\",\n"
    "     \"Country\":   \"US\"\n"
    "  },\n"
    "  {\n"
    "     \"precision\": \"zip\",\n"
    "     \"Latitude\":  37.371991,\n"
    "     \"Longitude\": -122.026020,\n"
    "     \"Address\":   \"\",\n"
    "     \"City\":      \"SUNNYVALE\",\n"
    "     \"State\":     \"CA\",\n"
    "     \"Zip\":       \"94085\",\n"
    "     \"Country\":   \"US\"\n"
    "  }\n"
    "]",
    "{\n"
    "  \"Image\": {\n"
    "    \"Width\":  800,\n"
    "    \"Height\": 600,\n"
    "    \"Title\":  \"View from 15th Floor\",\n"
    "    \"Thumbnail\": {\n"
    "        \"Url\":    \"http://www.example.com/image/481989943\",\n"
    "        \"Height\": 125,\n"
    "        \"Width\":  \"100\"\n"
    "    },\n"
    "    \"IDs\": [116, 943, 234, 38793]\n"
    "  }\n"
    "}",
    "{\n"
    "  \"source\" : \"<a href=\\\"http://janetter.net/\\\" rel=\\\"nofollow\\\">Janetter</a>\",\n"
    "  \"entities\" : {\n"
    "      \"user_mentions\" : [ {\n"
    "              \"name\" : \"James Governor\",\n"
    "              \"screen_name\" : \"moankchips\",\n"
    "              \"indices\" : [ 0, 10 ],\n"
    "              \"id_str\" : \"61233\",\n"
    "              \"id\" : 61233\n"
    "          } ],\n"
    "      \"media\" : [ ],\n"
    "      \"hashtags\" : [ ],\n"
    "     \"urls\" : [ ]\n"
    "  },\n"
    "  \"in_reply_to_status_id_str\" : \"281400879465238529\",\n"
    "  \"geo\" : {\n"
    "  },\n"
    "  \"id_str\" : \"281405942321532929\",\n"
    "  \"in_reply_to_user_id\" : 61233,\n"
    "  \"text\" : \"@monkchips Ouch. Some regrets are harsher than others.\",\n"
    "  \"id\" : 281405942321532929,\n"
    "  \"in_reply_to_status_id\" : 281400879465238529,\n"
    "  \"created_at\" : \"Wed Dec 19 14:29:39 +0000 2012\",\n"
    "  \"in_reply_to_screen_name\" : \"monkchips\",\n"
    "  \"in_reply_to_user_id_str\" : \"61233\",\n"
    "  \"user\" : {\n"
    "      \"name\" : \"Sarah Bourne\",\n"
    "      \"screen_name\" : \"sarahebourne\",\n"
    "      \"protected\" : false,\n"
    "      \"id_str\" : \"16010789\",\n"
    "      \"profile_image_url_https\" : \"https://si0.twimg.com/profile_images/638441870/Snapshot-of-sb_normal.jpg\",\n"
    "      \"id\" : 16010789,\n"
    "     \"verified\" : false\n"
    "  }\n"
    "}"
};

enum {
  JSON = 0,
  OBJECT
};

int main() {
  marpaESLIF_t                *marpaESLIFp        = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarArrayp[2] = { NULL, NULL };
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  int                          exiti;
  genericLogger_t             *genericLoggerp;
  char                        *grammars = NULL;
  char                        *p;
  int                          i;
  marpaESLIFTester_context_t   marpaESLIFTester_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerJsonp = NULL;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_INFO);
  if (genericLoggerp == NULL) {
    perror("GENERICLOGGER_NEW");
    goto err;
  }

  grammars = strdup(dsl);
  if (grammars == NULL) {
    perror("strdup");
    goto err;
  }
  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  marpaESLIFGrammarOption.bytep               = (void *) grammars;
  marpaESLIFGrammarOption.bytel               = strlen(grammars);
  marpaESLIFGrammarOption.encodings           = NULL;
  marpaESLIFGrammarOption.encodingl           = 0;

  p = strstr(grammars, "XXXXXX");

  GENERICLOGGER_INFO(genericLoggerp, "Creating JSON grammar");
  strncpy(p, "json   ", 6);
  marpaESLIFGrammarArrayp[JSON] = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarArrayp[JSON] == NULL) {
    goto err;
  }

  GENERICLOGGER_INFO(genericLoggerp, "Creating object grammar");
  strncpy(p, "object", 6);
  marpaESLIFGrammarArrayp[OBJECT] = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarArrayp[OBJECT] == NULL) {
    goto err;
  }

  for (i = 0; i < (sizeof(inputs)/sizeof(inputs[0])); i++) {
    marpaESLIFTester_context.inputs = inputs[i];
    marpaESLIFTester_context.inputl = strlen(inputs[i]);

    marpaESLIFRecognizerOption.userDatavp        = &marpaESLIFTester_context;
    marpaESLIFRecognizerOption.readerCallbackp   = inputReaderb;
    marpaESLIFRecognizerOption.disableThresholdb = 0;
    marpaESLIFRecognizerOption.exhaustedb        = 0;
    marpaESLIFRecognizerOption.newlineb          = 1;
    marpaESLIFRecognizerOption.trackb            = 0;
    marpaESLIFRecognizerOption.bufsizl           = 0;
    marpaESLIFRecognizerOption.buftriggerperci   = 50;
    marpaESLIFRecognizerOption.bufaddperci       = 50;
    marpaESLIFRecognizerOption.ifActionResolverp = NULL;

    marpaESLIFRecognizerJsonp = marpaESLIFRecognizer_newp(marpaESLIFGrammarArrayp[JSON], &marpaESLIFRecognizerOption);
    if (marpaESLIFRecognizerJsonp == NULL) {
      goto err;
    }

    if (! doparseb(genericLoggerp, marpaESLIFRecognizerJsonp, marpaESLIFGrammarArrayp[OBJECT], inputs[i], 0 /* recursionleveli */)) {
      goto err;
    }

    marpaESLIFRecognizer_freev(marpaESLIFRecognizerJsonp);
    marpaESLIFRecognizerJsonp = NULL;
  }

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerJsonp);
  marpaESLIFGrammar_freev(marpaESLIFGrammarArrayp[JSON]);
  marpaESLIFGrammar_freev(marpaESLIFGrammarArrayp[OBJECT]);
  if (grammars != NULL) {
    free(grammars);
  }
  marpaESLIF_freev(marpaESLIFp);

  GENERICLOGGER_FREE(genericLoggerp);
  exit(exiti);
}

/*****************************************************************************/
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp)
/*****************************************************************************/
{
  marpaESLIFTester_context_t *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;

  *inputsp              = marpaESLIFTester_contextp->inputs;
  *inputlp              = marpaESLIFTester_contextp->inputl;
  *eofbp                = 1;
  *characterStreambp    = 1; /* We say this is a stream of characters */
  *encodingsp           = NULL;
  *encodinglp           = 0;

  return 1;
}

/*****************************************************************************/
static short doparseb(genericLogger_t *genericLoggerp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFGrammar_t *marpaESLIFGrammarObjectp, char *inputs, int recursionleveli)
/*****************************************************************************/
{
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerObjectp = NULL;
  short                        continueb;
  short                        exhaustedb;
  char                        *pauses;
  size_t                       pausel;
  size_t                       linel;
  size_t                       columnl;
  size_t                       eventArrayl;
  marpaESLIFEvent_t           *eventArrayp;
  char                        *events;
  size_t                       l;
  short                        rcb;
  marpaESLIFAlternative_t      marpaESLIFAlternative;
  size_t                       inputl;
  short                        eofb;

  if (inputs != NULL) {
    GENERICLOGGER_INFOF(genericLoggerp, "[%d] Scanning JSON", recursionleveli);
    GENERICLOGGER_INFO(genericLoggerp, "-------------");
    GENERICLOGGER_INFOF(genericLoggerp, "%s", inputs);
    GENERICLOGGER_INFO(genericLoggerp, "-------------");
  } else {
    GENERICLOGGER_INFOF(genericLoggerp, "[%d] Scanning JSON's object", recursionleveli);
  }

  /* Scan the input */
  if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, 1 /* initialEventsb */, &continueb, &exhaustedb)) {
    goto err;
  }

  while (continueb) {
    if (! marpaESLIFRecognizer_eventb(marpaESLIFRecognizerp, &eventArrayl, &eventArrayp)) {
      goto err;
    }
    for (l = 0; l < eventArrayl; l++) {
      events = eventArrayp[l].events;
      if (events == NULL) {
        continue;
      }
      GENERICLOGGER_DEBUGF(genericLoggerp, "Event %s", events);
      if (strcmp(events, "lstring$") == 0) {

        if (! marpaESLIFRecognizer_lexeme_last_pauseb(marpaESLIFRecognizerp, "lstring", &pauses, &pausel)) {
          goto err;
        }
        if (! marpaESLIFRecognizer_locationb(marpaESLIFRecognizerp, &linel, &columnl)) {
          goto err;
        }
        GENERICLOGGER_INFOF(genericLoggerp, "Got lstring: %s; length=%ld, current position is {line, column} = {%ld, %ld}", pauses, (unsigned long) pausel, (unsigned long) linel, (unsigned long) columnl);

      } else if (strcmp(events, "^LCURLY") == 0) {

        marpaESLIFRecognizerObjectp = marpaESLIFRecognizer_newFromp(marpaESLIFGrammarObjectp, marpaESLIFRecognizerp);
        if (marpaESLIFRecognizerObjectp == NULL) {
          goto err;
        }

        /* Set exhausted flag since this grammar is very likely to exit when data remains */
        if (! marpaESLIFRecognizer_set_exhausted_flagb(marpaESLIFRecognizerObjectp, 1)) {
          goto err;
        }
  
        /* Force read of the LCURLY lexeme */
        marpaESLIFAlternative.lexemes               = "LCURLY";
        marpaESLIFAlternative.value.contextp        =  &context; /* Must be != NULL */
        marpaESLIFAlternative.value.representationp = NULL; /* Default representation is ok */
        marpaESLIFAlternative.value.type            = MARPAESLIF_VALUE_TYPE_CHAR;
        marpaESLIFAlternative.value.u.c             = '{';
        marpaESLIFAlternative.grammarLengthl        = 1;
        if (! marpaESLIFRecognizer_lexeme_readb(marpaESLIFRecognizerObjectp, &marpaESLIFAlternative, 1 /* Length in the real input */)) {
          goto err;
        }

        if (! doparseb(genericLoggerp, marpaESLIFRecognizerObjectp, marpaESLIFGrammarObjectp, NULL, recursionleveli + 1)) {
          goto err;
        }
        marpaESLIFRecognizer_freev(marpaESLIFRecognizerObjectp);
        marpaESLIFRecognizerObjectp = NULL;

        /* Read a fake object - no need to advance the stream, because we shared it with marpaESLIFRecognizerObjectp */
        marpaESLIFAlternative.lexemes               = "OBJECT_FROM_INNER_GRAMMAR";
        marpaESLIFAlternative.value.type            = MARPAESLIF_VALUE_TYPE_UNDEF;
        marpaESLIFAlternative.grammarLengthl        = 1;
        if (! marpaESLIFRecognizer_lexeme_readb(marpaESLIFRecognizerp, &marpaESLIFAlternative, 0 /* Length in the real input */)) {
          goto err;
        }
      } else if (strcmp(events, "^RCURLY") == 0) {
        /* Force read of the RCURLY lexeme */
        marpaESLIFAlternative.lexemes               = "RCURLY";
        marpaESLIFAlternative.value.type            = MARPAESLIF_VALUE_TYPE_CHAR;
        marpaESLIFAlternative.value.contextp        =  &context; /* Must be != NULL */
        marpaESLIFAlternative.value.representationp = NULL; /* Default representation is ok */
        marpaESLIFAlternative.value.u.c             = '{';
        marpaESLIFAlternative.grammarLengthl        = 1;
        if (! marpaESLIFRecognizer_lexeme_readb(marpaESLIFRecognizerp, &marpaESLIFAlternative, 1 /* Length in the real input */)) {
          goto err;
        }

        rcb = 1;
        goto done;
      } else {

        GENERICLOGGER_ERRORF(genericLoggerp, "Unmanaged event %s", events);
        goto err;

      }
    }
    /* Check if there is something else to read */
    if (! marpaESLIFRecognizer_isEofb(marpaESLIFRecognizerp, &eofb)) {
      goto err;
    }
    if (! marpaESLIFRecognizer_inputb(marpaESLIFRecognizerp, NULL /* inputsp */, &inputl)) {
      goto err;
    }
    if ((inputl <= 0) && eofb) {
      rcb = 1;
      goto done;
    }
    /* Resume */
    if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 0, &continueb, &exhaustedb)) {
      goto err;
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerObjectp);

  return rcb;
}

