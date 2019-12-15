#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <genericLogger.h>
#include <ctype.h>
#include <marpaESLIF.h>

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp);
short        importb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static short dumpb(size_t indentl, short commab, marpaESLIFValueResult_t *marpaESLIFValueResultp);

typedef struct marpaESLIFTester_context {
  char   *inputs;
  size_t  inputl;
} marpaESLIFTester_context_t;

int main() {
  marpaESLIF_t                *marpaESLIFp            = NULL;
  genericLogger_t             *genericLoggerp         = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarJsonp = NULL;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFJSONDecodeOption_t marpaESLIFJSONDecodeOption;
  marpaESLIFValueOption_t      marpaESLIFValueOption;
  marpaESLIFTester_context_t   marpaESLIFTester_context;
  int                          ngrammari;
  int                          leveli;
  char                        *grammarshows;
  int                          i;
  int                          exiti;

  const static char           *inputs[] = {
    "{\"test\":[1,2,3]}",
    "{\"test\":\"1\", \"test2\":+Infinity}",
    "{\"test\":true,\"test2\":-Infinity}",
    "{\"test\":false,\"test2\":nan}",
    "{\"test\":null}",
    "{\"test\":null, \"test2\":\"hello world\"}",
    "{\"test\":\"1.25\"}",
    "{\"test\":\"1.25e4\"}",
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
    "}",
    "[\"\\uDd1ea\"]"
  };

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);
  if (genericLoggerp == NULL) {
    perror("GENERICLOGGER_NEW");
    goto err;
  }

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    perror("marpaESLIF_newp");
    goto err;
  }

  marpaESLIFGrammarJsonp = marpaESLIFJSON_newp(marpaESLIFp, 0 /* strictb */);
  if (marpaESLIFGrammarJsonp == NULL) {
    perror("marpaESLIFJSON_newp");
    goto err;
  }

  /* Dump grammar */
  if (marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarJsonp, &ngrammari)) {
    for (leveli = 0; leveli < ngrammari; leveli++) {
      if (marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammarJsonp, &grammarshows, leveli, NULL)) {
        GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "JSON grammar at level %d:", leveli);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n\n%s", grammarshows);
      }
    }
  }

  for (i = 0; i < (sizeof(inputs)/sizeof(inputs[0])); i++) {
    marpaESLIFJSONDecodeOption.bytep                   = (char *) inputs[i];
    marpaESLIFJSONDecodeOption.bytel                   = strlen(inputs[i]);
    marpaESLIFJSONDecodeOption.encodings               = NULL;
    marpaESLIFJSONDecodeOption.disallowDupkeysb        = 1;
    marpaESLIFJSONDecodeOption.maxDepthl               = 1;
    marpaESLIFJSONDecodeOption.numberActionp           = NULL;
    marpaESLIFJSONDecodeOption.positiveInfinityActionp = NULL;
    marpaESLIFJSONDecodeOption.negativeInfinityActionp = NULL;
    marpaESLIFJSONDecodeOption.nanActionp              = NULL;

    marpaESLIFTester_context.inputs = (char *) inputs[i];
    marpaESLIFTester_context.inputl = strlen(inputs[i]);;

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

    marpaESLIFValueOption.userDatavp            = NULL; /* User specific context */
    marpaESLIFValueOption.ruleActionResolverp   = NULL; /* Will return the function doing the wanted rule action */
    marpaESLIFValueOption.symbolActionResolverp = NULL; /* Will return the function doing the wanted symbol action */
    marpaESLIFValueOption.importerp             = importb;
    marpaESLIFValueOption.highRankOnlyb         = 1;    /* Default: 1 */
    marpaESLIFValueOption.orderByRankb          = 1;    /* Default: 1 */
    marpaESLIFValueOption.ambiguousb            = 0;    /* Default: 0 */
    marpaESLIFValueOption.nullb                 = 0;    /* Default: 0 */
    marpaESLIFValueOption.maxParsesi            = 0;    /* Default: 0 */

    GENERICLOGGER_INFO(genericLoggerp, "Scanning JSON");
    GENERICLOGGER_INFO(genericLoggerp, "-------------");
    GENERICLOGGER_INFOF(genericLoggerp, "%s", inputs[i]);
    GENERICLOGGER_INFO(genericLoggerp, "-------------");

    if (! marpaESLIFJSON_decode(marpaESLIFGrammarJsonp, &marpaESLIFJSONDecodeOption, &marpaESLIFRecognizerOption, &marpaESLIFValueOption)) {
      goto err;
    }

  }

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
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
short importb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  short rcb;

  switch (marpaESLIFValueResultp->type) {
  case MARPAESLIF_VALUE_TYPE_UNDEF:
    fprintf(stdout, "null [UNDEF]\n");
    break;
  case MARPAESLIF_VALUE_TYPE_CHAR:
    if (isprint(0xFF & marpaESLIFValueResultp->u.c)) {
      fprintf(stdout, "%c [CHAR]\n", marpaESLIFValueResultp->u.c);
    } else {
      fprintf(stdout, ".  [CHAR (not printable)]\n");
    }
    break;
  case MARPAESLIF_VALUE_TYPE_SHORT:
    fprintf(stdout, "%d [SHORT]\n", (int) marpaESLIFValueResultp->u.b);
    break;
  case MARPAESLIF_VALUE_TYPE_INT:
    fprintf(stdout, "%d [INT]\n", marpaESLIFValueResultp->u.i);
    break;
  case MARPAESLIF_VALUE_TYPE_LONG:
    fprintf(stdout, "%ld [LONG]\n", marpaESLIFValueResultp->u.l);
    break;
  case MARPAESLIF_VALUE_TYPE_FLOAT:
    fprintf(stdout, "%f [FLOAT]\n", (double) marpaESLIFValueResultp->u.f);
    break;
  case MARPAESLIF_VALUE_TYPE_DOUBLE:
    fprintf(stdout, "%f [DOUBLE]\n", (double) marpaESLIFValueResultp->u.d);
    break;
  case MARPAESLIF_VALUE_TYPE_PTR:
    fprintf(stdout, "%p [PTR]\n", marpaESLIFValueResultp->u.p);
    break;
  case MARPAESLIF_VALUE_TYPE_ARRAY:
    fprintf(stdout, "?? [ARRAY]\n");
    break;
  case MARPAESLIF_VALUE_TYPE_BOOL:
    fprintf(stdout, "%s [BOOL]\n", marpaESLIFValueResultp->u.y == MARPAESLIFVALUERESULTBOOL_FALSE ? "false" : "true");
    break;
  case MARPAESLIF_VALUE_TYPE_STRING:
    /* This is not correct, but accepted for our test case */
    fprintf(stdout, "\"%s\" [STRING]\n", marpaESLIFValueResultp->u.s.p);
    break;
  case MARPAESLIF_VALUE_TYPE_ROW:
    break;
  case MARPAESLIF_VALUE_TYPE_TABLE:
    break;
  case MARPAESLIF_VALUE_TYPE_LONG_DOUBLE:
    fprintf(stdout, "%Ld [LONG_DOUBLE]\n", marpaESLIFValueResultp->u.ld);
    break;
#ifdef MARPAESLIF_HAVE_LONG_LONG
  case MARPAESLIF_VALUE_TYPE_LONG_LONG:
    fprintf(stdout, "%lld [LONG_LONG]\n", marpaESLIFValueResultp->u.ll);
    break;
#endif
  default:
    fprintf(stdout, "Unexpected value type %d\n", marpaESLIFValueResultp->type);
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}
