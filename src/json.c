#include <locale.h>
#include <stdlib.h>
#include "json_encode_extended_grammar.c"
#include "json_decode_extended_grammar.c"
#include "json_encode_strict_grammar.c"
#include "json_decode_strict_grammar.c"

#undef  FILENAMES
#define FILENAMES "json.c" /* For logging */

typedef struct marpaESLIFJSONDecodeDepositCallbackContext marpaESLIFJSONDecodeDepositCallbackContext_t;
typedef struct marpaESLIFJSONDecodeDeposit                marpaESLIFJSONDecodeDeposit_t;
typedef struct marpaESLIFJSONDecodeContext                marpaESLIFJSONDecodeContext_t;
typedef short (*marpaESLIFJSONDecodeDepositCallback_t)(marpaESLIFJSONDecodeDepositCallbackContext_t *contextp, marpaESLIFValueResult_t *dstp, marpaESLIFValueResult_t *srcp);
typedef struct marpaESLIFJSONEncodeContext                marpaESLIFJSONEncodeContext_t;

#define MARPAESLIFJSON_ARRAYL_IN_STRUCTURE 4096
struct marpaESLIFJSONDecodeContext {
  marpaESLIF_t                      *marpaESLIFp;
  marpaESLIFRecognizer_t            *marpaESLIFRecognizerp;
  marpaESLIFJSONDecodeOption_t      *marpaESLIFJSONDecodeOptionp;
  marpaESLIFRecognizerOption_t      *marpaESLIFRecognizerOptionp;
  marpaESLIFValueOption_t           *marpaESLIFValueOptionp;
  marpaESLIFReaderDispose_t          readerDisposep;
  marpaESLIFRepresentationDispose_t  representationDisposep;
  char                              *numbers;
  marpaESLIF_uint32_t               *uint32p;
  genericStack_t                   _depositStack;
  genericStack_t                   *depositStackp;
  size_t                             currentDepthl;
  size_t                             numberallocl; /* Used when we analyse a number */
  size_t                             stringallocl; /* Used when we iterate within DQUOTE_START ... DQUOTE_END */
  size_t                             uint32allocl; /* Used when we iterate within DQUOTE_START ... DQUOTE_END and compute series of \\uXXXX */
  char                              _numbers[MARPAESLIFJSON_ARRAYL_IN_STRUCTURE + 1]; /* To avoid allocation for the vast majority of cases in my opinion -; */
  marpaESLIF_uint32_t               _uint32p[MARPAESLIFJSON_ARRAYL_IN_STRUCTURE + 1]; /* Ditto */
};

struct marpaESLIFJSONEncodeContext {
  marpaESLIFValueOption_t           *marpaESLIFValueOptionp;
  marpaESLIFRepresentationDispose_t  representationDisposep;
};

struct marpaESLIFJSONDecodeDepositCallbackContext {
  marpaESLIF_t                  *marpaESLIFp;
  marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp;
  short                          keyb; /* MUST BE INITIALIZED to 1 (case when destination is a table) */
  size_t                         allocl;
};
  
struct marpaESLIFJSONDecodeDeposit {
  marpaESLIFJSONDecodeDepositCallbackContext_t *contextp;
  marpaESLIFJSONDecodeDepositCallback_t         actionp;
  marpaESLIFValueResult_t                      *dstp;
};

static const char MARPAESLIFJSON_DQUOTE          = '\'';
static const char MARPAESLIFJSON_BACKSLASH       = '\\';
static const char MARPAESLIFJSON_SLASH           = '/';
static const char MARPAESLIFJSON_BACKSPACE       = '\x08';
static const char MARPAESLIFJSON_FORMFEED        = '\x0C';
static const char MARPAESLIFJSON_LINEFEED        = '\x0A';
static const char MARPAESLIFJSON_CARRIAGE_RETURN = '\x0D';
static const char MARPAESLIFJSON_HORIZONTAL_TAB  = '\x09';

/* For the \uXXXX case in string */
#define MARPAESLIFJSON_DST_OR_VALCHAR(marpaESLIFJSONDecodeContextp, dst, valchar) do { \
    unsigned char _valchar = (unsigned char) (valchar);                 \
    switch (_valchar) {                                                 \
    case '0':                                                           \
      dst |= 0x00;                                                      \
      break;                                                            \
    case '1':                                                           \
      dst |= 0x01;                                                      \
      break;                                                            \
    case '2':                                                           \
      dst |= 0x02;                                                      \
      break;                                                            \
    case '3':                                                           \
      dst |= 0x03;                                                      \
      break;                                                            \
    case '4':                                                           \
      dst |= 0x04;                                                      \
      break;                                                            \
    case '5':                                                           \
      dst |= 0x05;                                                      \
      break;                                                            \
    case '6':                                                           \
      dst |= 0x06;                                                      \
      break;                                                            \
    case '7':                                                           \
      dst |= 0x07;                                                      \
      break;                                                            \
    case '8':                                                           \
      dst |= 0x08;                                                      \
      break;                                                            \
    case '9':                                                           \
      dst |= 0x09;                                                      \
      break;                                                            \
    case 'a':                                                           \
    case 'A':                                                           \
      dst |= 0x0A;                                                      \
      break;                                                            \
    case 'b':                                                           \
    case 'B':                                                           \
      dst |= 0x0B;                                                      \
      break;                                                            \
    case 'c':                                                           \
    case 'C':                                                           \
      dst |= 0x0C;                                                      \
      break;                                                            \
    case 'd':                                                           \
    case 'D':                                                           \
      dst |= 0x0D;                                                      \
      break;                                                            \
    case 'e':                                                           \
    case 'E':                                                           \
      dst |= 0x0E;                                                      \
      break;                                                            \
    case 'f':                                                           \
    case 'F':                                                           \
      dst |= 0x0F;                                                      \
      break;                                                            \
    default:                                                            \
      MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "Unsupported hexadecimal character '%c' (0x%lx)", _valchar, (unsigned long) _valchar); \
      goto err;                                                         \
    }                                                                   \
  } while (0)

static void                                 _marpaESLIFJSONReaderDisposev(void *userDatavp, char *inputcp, size_t inputl, short eofb, short characterStreamb, char *encodings, size_t encodingl);
static short                                _marpaESLIFJSONDecodeReaderb(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);

/* Decoder specific actions */
static marpaESLIFRecognizerEventCallback_t  _marpaESLIFJSONDecodeEventActionResolverp(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions);
static short                                _marpaESLIFJSONDecodeEventCallbackb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFEvent_t *eventArrayp, size_t eventArrayl, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static inline short                         _marpaESLIFJSONDecodeIncb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static inline short                         _marpaESLIFJSONDecodeDecb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static short                                _marpaESLIFJSONDecodeImportb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static inline short                         _marpaESLIFJSONDecodeSetPositiveInfinityb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, char *inputs, size_t inputl);
static inline short                         _marpaESLIFJSONDecodeSetNegativeInfinityb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, char *inputs, size_t inputl);
static inline short                         _marpaESLIFJSONDecodeSetPositiveNanb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, char *inputs, size_t inputl);
static inline short                         _marpaESLIFJSONDecodeSetNegativeNanb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, char *inputs, size_t inputl);
static inline short                         _marpaESLIFJSONDecodeSetNumberb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, char *inputs, size_t inputl);
static inline short                         _marpaESLIFJSONDecodeExtendStringContainerb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, size_t incl);
static inline short                         _marpaESLIFJSONDecodeAppendCharb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, char *inputs, size_t inputl);
static inline short                         _marpaESLIFJSONDecodeProposalb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFJSONProposalAction_t proposalp, char *inputs, size_t inputl, marpaESLIFValueResult_t *currentValuep, short confidenceb);
static inline short                         _marpaESLIFJSONDecodeSetConstantb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, char *inputs, size_t inputl);
static inline short                         _marpaESLIFJSONDecodeDepositStackPushb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFJSONDecodeDeposit_t *marpaESLIFJSONDecodeDepositp);
static inline short                         _marpaESLIFJSONDecodeDepositStackGetLastb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFJSONDecodeDeposit_t *marpaESLIFJSONDecodeDepositp);
static inline short                         _marpaESLIFJSONDecodeDepositStackGetb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFJSONDecodeDeposit_t *marpaESLIFJSONDecodeDepositp, int indicei);
static inline void                          _marpaESLIFJSONDecodeDepositStackPopb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp);
static short                                _marpaESLIFJSONDecodeSetValueCallbackv(marpaESLIFJSONDecodeDepositCallbackContext_t *marpaESLIFJSONDecodeDepositCallbackContextp, marpaESLIFValueResult_t *dstp, marpaESLIFValueResult_t *srcp);
static short                                _marpaESLIFJSONDecodePushArrayCallbackv(marpaESLIFJSONDecodeDepositCallbackContext_t *marpaESLIFJSONDecodeDepositCallbackContextp, marpaESLIFValueResult_t *dstp, marpaESLIFValueResult_t *srcp);
static short                                _marpaESLIFJSONDecodeSetHashCallbackv(marpaESLIFJSONDecodeDepositCallbackContext_t *marpaESLIFJSONDecodeDepositCallbackContextp, marpaESLIFValueResult_t *dstp, marpaESLIFValueResult_t *srcp);
static inline short                         _marpaESLIFJSONDecodePropagateValueb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep);
static inline short                         _marpaESLIFJSONDecodeManageEventsb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep);
static short                                _marpaESLIFJSONDecodeValueResultImportb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static short                                _marpaESLIFJSONDecodeRepresentationb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp, char **encodingasciisp, marpaESLIFRepresentationDispose_t *disposeCallbackpp, short *stringbp);
static void                                 _marpaESLIFJSONDecodeRepresentationDisposev(void *userDatavp, char *inputcp, size_t inputl, char *encodingasciis);
static short                                _marpaESLIFJSONEncodeValueResultImportb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static short                                _marpaESLIFJSONEncodeRepresentationb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp, char **encodingasciisp, marpaESLIFRepresentationDispose_t *disposeCallbackpp, short *stringbp);
static void                                 _marpaESLIFJSONEncodeRepresentationDisposev(void *userDatavp, char *inputcp, size_t inputl, char *encodingasciis);

/*****************************************************************************/
static inline marpaESLIFGrammar_t *_marpaESLIFJSON_decode_newp(marpaESLIF_t *marpaESLIFp, short strictb)
/*****************************************************************************/
{
  marpaESLIFGrammar_t       *marpaESLIFJSONp             = NULL;
  marpaESLIFGrammarOption_t  marpaESLIFGrammarOption;

  if (MARPAESLIF_UNLIKELY(marpaESLIFp == NULL)) {
    errno = EINVAL;
    goto err;
  }

  marpaESLIFGrammarOption.bytep     = strictb ? (char *) marpaESLIFJSON_decode_strict_grammars : (char *) marpaESLIFJSON_decode_extended_grammars;
  marpaESLIFGrammarOption.bytel     = strlen((const char *) marpaESLIFGrammarOption.bytep);
  marpaESLIFGrammarOption.encodings = "ASCII";
  marpaESLIFGrammarOption.encodingl = 5; /* strlen("ASCII") */

  marpaESLIFJSONp = _marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption, 0 /* startGrammarIsLexemeb */);
  if (MARPAESLIF_UNLIKELY(marpaESLIFJSONp == NULL)) {
    goto err;
  }

  goto done;

 err:
  marpaESLIFGrammar_freev(marpaESLIFJSONp);
  marpaESLIFJSONp = NULL;

 done:
  return marpaESLIFJSONp;
}

/*****************************************************************************/
static inline marpaESLIFGrammar_t *_marpaESLIFJSON_encode_newp(marpaESLIF_t *marpaESLIFp, short strictb)
/*****************************************************************************/
{
  marpaESLIFGrammar_t       *marpaESLIFJSONp             = NULL;
  marpaESLIFGrammarOption_t  marpaESLIFGrammarOption;

  if (MARPAESLIF_UNLIKELY(marpaESLIFp == NULL)) {
    errno = EINVAL;
    goto err;
  }

  marpaESLIFGrammarOption.bytep     = strictb ? (char *) marpaESLIFJSON_encode_strict_grammars : (char *) marpaESLIFJSON_encode_extended_grammars;
  marpaESLIFGrammarOption.bytel     = strlen((const char *) marpaESLIFGrammarOption.bytep);
  marpaESLIFGrammarOption.encodings = "ASCII";
  marpaESLIFGrammarOption.encodingl = 5; /* strlen("ASCII") */

  marpaESLIFJSONp = _marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption, 0 /* startGrammarIsLexemeb */);
  if (MARPAESLIF_UNLIKELY(marpaESLIFJSONp == NULL)) {
    goto err;
  }

  goto done;

 err:
  marpaESLIFGrammar_freev(marpaESLIFJSONp);
  marpaESLIFJSONp = NULL;

 done:
  return marpaESLIFJSONp;
}

/*****************************************************************************/
short marpaESLIFJSON_encodeb(marpaESLIFGrammar_t *marpaESLIFGrammarJSONp, marpaESLIFValueResult_t *marpaESLIFValueResultp, marpaESLIFValueOption_t *marpaESLIFValueOptionp)
/*****************************************************************************/
/* Strict or not, encode grammar is always at level 10                       */
/*****************************************************************************/
{
  marpaESLIFRecognizer_t        *marpaESLIFRecognizerp = NULL;
  marpaESLIFValue_t             *marpaESLIFValuep      = NULL;
  short                          rcb;
  marpaESLIFAlternative_t        marpaESLIFAlternative;
  marpaESLIFValueOption_t        marpaESLIFValueOption;
  marpaESLIFJSONEncodeContext_t  marpaESLIFJSONEncodeContext;

  marpaESLIFJSONEncodeContext.marpaESLIFValueOptionp = marpaESLIFValueOptionp;
  marpaESLIFJSONEncodeContext.representationDisposep = NULL;

  if (MARPAESLIF_UNLIKELY((marpaESLIFGrammarJSONp == NULL) || (marpaESLIFValueResultp == NULL) || (marpaESLIFValueOptionp == NULL))) {
    errno = EINVAL;
    goto err;
  }

  marpaESLIFValueOption                           = *marpaESLIFValueOptionp;
  marpaESLIFValueOption.userDatavp                = &marpaESLIFJSONEncodeContext;
  marpaESLIFValueOption.ruleActionResolverp       = NULL; /* Not used */
  marpaESLIFValueOption.symbolActionResolverp     = NULL; /* We use the native ::transfer action */
  marpaESLIFValueOption.importerp                 = ((marpaESLIFValueOptionp != NULL) && (marpaESLIFValueOptionp->importerp != NULL)) ? _marpaESLIFJSONEncodeValueResultImportb : NULL;
  marpaESLIFValueOption.highRankOnlyb             = 1; /* Fixed */
  marpaESLIFValueOption.orderByRankb              = 1; /* Fixed */
  marpaESLIFValueOption.ambiguousb                = 0; /* Fixed */
  marpaESLIFValueOption.nullb                     = 0; /* Fixed */
  marpaESLIFValueOption.maxParsesi                = 0; /* Fixed */

  marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarJSONp, NULL);
  if (MARPAESLIF_UNLIKELY(marpaESLIFRecognizerp == NULL)) {
    goto err;
  }

  /* Insert a lexeme with length 0 in the input, though length 1 in the grammar */
  marpaESLIFAlternative.lexemes        = "INPUT";
  marpaESLIFAlternative.value          = *marpaESLIFValueResultp;
  marpaESLIFAlternative.grammarLengthl = 1;
  if (MARPAESLIF_UNLIKELY(! marpaESLIFRecognizer_lexeme_readb(marpaESLIFRecognizerp, &marpaESLIFAlternative, 0 /* lengthl */))) {
    goto err;
  }
  marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);

  /* Set-up proxy representation */
  marpaESLIFValuep->proxyRepresentationp = _marpaESLIFJSONEncodeRepresentationb;
  if (MARPAESLIF_UNLIKELY(marpaESLIFValuep == NULL)) {
    goto err;
  }

  if (MARPAESLIF_UNLIKELY(! marpaESLIFValue_valueb(marpaESLIFValuep))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (marpaESLIFValuep != NULL) {
    marpaESLIFValue_freev(marpaESLIFValuep);
  }
  if (marpaESLIFRecognizerp != NULL) {
    marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  }
  return rcb;
}

/*****************************************************************************/
short marpaESLIFJSON_decodeb(marpaESLIFGrammar_t *marpaESLIFGrammarJSONp, marpaESLIFJSONDecodeOption_t *marpaESLIFJSONDecodeOptionp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp)
/*****************************************************************************/
{
  static const char                            *funcs                 = "marpaESLIFJSON_decodeb";
  marpaESLIFValueResult_t                       output                = marpaESLIFValueResultUndef;
  marpaESLIFValue_t                            *marpaESLIFValuep      = NULL;
  marpaESLIFRecognizerOption_t                  marpaESLIFRecognizerOption;
  marpaESLIFValueOption_t                       marpaESLIFValueOption;
  marpaESLIFJSONDecodeContext_t                 marpaESLIFJSONDecodeContext;
  marpaESLIFJSONDecodeDeposit_t                 marpaESLIFJSONDecodeDeposit;
  marpaESLIFJSONDecodeDepositCallbackContext_t  marpaESLIFJSONDecodeDepositCallbackContext;
  short                                         continueb;
  short                                         isExhaustedb;
  short                                         isEofb;
  size_t                                        inputl;
  marpaESLIFValueResult_t                       currentValue;
  short                                         rcb;
  short                                         parseokb;
  int                                           i;

  /* Whatever happens, we take entire control on the callbacks so that we have our own context on top of it */
  marpaESLIFJSONDecodeContext.marpaESLIFp                 = marpaESLIFGrammarJSONp->marpaESLIFp;
  marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp       = NULL;
  marpaESLIFJSONDecodeContext.marpaESLIFJSONDecodeOptionp = marpaESLIFJSONDecodeOptionp;
  marpaESLIFJSONDecodeContext.marpaESLIFRecognizerOptionp = marpaESLIFRecognizerOptionp;
  marpaESLIFJSONDecodeContext.marpaESLIFValueOptionp      = marpaESLIFValueOptionp;
  marpaESLIFJSONDecodeContext.readerDisposep              = NULL;
  marpaESLIFJSONDecodeContext.representationDisposep      = NULL;
  marpaESLIFJSONDecodeContext.numbers                     = NULL;
  marpaESLIFJSONDecodeContext.uint32p                     = NULL;
  marpaESLIFJSONDecodeContext.depositStackp               = &(marpaESLIFJSONDecodeContext._depositStack);
  marpaESLIFJSONDecodeContext.currentDepthl               = 0;
  marpaESLIFJSONDecodeContext.numberallocl                = 0;
  marpaESLIFJSONDecodeContext.stringallocl                = 0;
  marpaESLIFJSONDecodeContext.uint32allocl                = 0;

  GENERICSTACK_INIT(marpaESLIFJSONDecodeContext.depositStackp);
  if (MARPAESLIF_UNLIKELY(GENERICSTACK_ERROR(marpaESLIFJSONDecodeContext.depositStackp))) {
    MARPAESLIF_ERRORF(marpaESLIFGrammarJSONp->marpaESLIFp, "depositStackp initialization failure, %s", strerror(errno));
    marpaESLIFJSONDecodeContext.depositStackp = NULL;
    goto err;
  }

  if (MARPAESLIF_UNLIKELY((marpaESLIFGrammarJSONp                       == NULL) ||
                          (marpaESLIFJSONDecodeOptionp                  == NULL) ||
                          (marpaESLIFRecognizerOptionp                  == NULL) ||
                          (marpaESLIFRecognizerOptionp->readerCallbackp == NULL) ||
                          (marpaESLIFValueOptionp                       == NULL))) {
    errno = EINVAL;
    goto err;
  }

  marpaESLIFRecognizerOption                      = *marpaESLIFRecognizerOptionp;
  marpaESLIFRecognizerOption.userDatavp           = &marpaESLIFJSONDecodeContext;
  marpaESLIFRecognizerOption.readerCallbackp      = _marpaESLIFJSONDecodeReaderb;
  marpaESLIFRecognizerOption.ifActionResolverp    = NULL; /* Our grammar has no if-action anyway */
  marpaESLIFRecognizerOption.eventActionResolverp = _marpaESLIFJSONDecodeEventActionResolverp;
  marpaESLIFRecognizerOption.regexActionResolverp = NULL;
  marpaESLIFRecognizerOption.importerp            = NULL;

  marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarJSONp, &marpaESLIFRecognizerOption);
  if (MARPAESLIF_UNLIKELY(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp == NULL)) {
    goto err;
  }

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, funcs, "start");

  marpaESLIFJSONDecodeDepositCallbackContext.marpaESLIFp                   = marpaESLIFGrammarJSONp->marpaESLIFp;
  marpaESLIFJSONDecodeDepositCallbackContext.marpaESLIFJSONDecodeContextp  = &marpaESLIFJSONDecodeContext;
  marpaESLIFJSONDecodeDepositCallbackContext.keyb                          = 1;
  marpaESLIFJSONDecodeDepositCallbackContext.allocl                        = 0;

  marpaESLIFJSONDecodeDeposit.dstp     = &output;
  marpaESLIFJSONDecodeDeposit.contextp = &marpaESLIFJSONDecodeDepositCallbackContext;
  marpaESLIFJSONDecodeDeposit.actionp  = _marpaESLIFJSONDecodeSetValueCallbackv;

  if (! _marpaESLIFJSONDecodeDepositStackPushb(&marpaESLIFJSONDecodeContext, &marpaESLIFJSONDecodeDeposit)) {
    goto err;
  }

  if (MARPAESLIF_UNLIKELY(! marpaESLIFRecognizer_scanb(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, 1 /* initialEventsb */, &continueb, NULL /* exhaustedbp */))) {
    goto err;
  }
  if (! _marpaESLIFJSONDecodeManageEventsb(&marpaESLIFJSONDecodeContext, &currentValue)) {
    goto err;
  }
  while (continueb) {
    if (MARPAESLIF_UNLIKELY(! marpaESLIFRecognizer_resumeb(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, 0 /* deltaLengthl */, &continueb, NULL /* exhaustedbp */))) {
      break;
    }
    if (! _marpaESLIFJSONDecodeManageEventsb(&marpaESLIFJSONDecodeContext, &currentValue)) {
      goto err;
    }
  }

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, funcs, "Number of items in depositStackp: %d", GENERICSTACK_USED(marpaESLIFJSONDecodeContext.depositStackp));

  if (! _marpaESLIFRecognizer_isExhaustedb(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, &isExhaustedb)) {
    goto err;
  }
  if (! _marpaESLIFRecognizer_isEofb(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, &isEofb)) {
    goto err;
  }
  if (! _marpaESLIFRecognizer_inputb(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, NULL, &inputl)) {
    goto err;
  }

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, funcs, "isExhaustedb = %d", (int) isExhaustedb);
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, funcs, "isEofb       = %d", (int) isEofb);
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, funcs, "inputl       = %ld", (unsigned long) inputl);

  if ((! isExhaustedb) || (! isEofb) || (inputl > 0)) {
    goto err;
  }

  /* Verify valuation */
  marpaESLIFValueOption                           = *marpaESLIFValueOptionp;
  marpaESLIFValueOption.userDatavp                = &marpaESLIFJSONDecodeContext;
  marpaESLIFValueOption.ruleActionResolverp       = NULL;
  marpaESLIFValueOption.symbolActionResolverp     = NULL;
  marpaESLIFValueOption.importerp                 = ((marpaESLIFValueOptionp != NULL) && (marpaESLIFValueOptionp->importerp != NULL)) ? _marpaESLIFJSONDecodeValueResultImportb : NULL;
  marpaESLIFValueOption.highRankOnlyb             = 1; /* Fixed */
  marpaESLIFValueOption.orderByRankb              = 1; /* Fixed */
  marpaESLIFValueOption.ambiguousb                = 0; /* Fixed */
  marpaESLIFValueOption.nullb                     = 0; /* Fixed */
  marpaESLIFValueOption.maxParsesi                = 0; /* Fixed */

  marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, &marpaESLIFValueOption);
  if (MARPAESLIF_UNLIKELY(marpaESLIFValuep == NULL)) {
    goto err;
  }

  /* Set-up proxy representation */
  marpaESLIFValuep->proxyRepresentationp = _marpaESLIFJSONDecodeRepresentationb;

  /* Call for import (no-op if end-user has set no importer */
  if (! _marpaESLIFValue_importb(marpaESLIFValuep, &output)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (marpaESLIFValuep != NULL) {
    marpaESLIFValue_freev(marpaESLIFValuep);
  }
  if (marpaESLIFJSONDecodeContext.depositStackp != NULL) {
    /* It is in theory impossible that marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp is NULL if marpaESLIFJSONDecodeContext.depositStackp is not NULL */
    if (marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp != NULL) {
      /* All entries contain abused marpaESLIFValueResult's that are allocated except the one on the top */
      for (i = 0; i < GENERICSTACK_USED(marpaESLIFJSONDecodeContext.depositStackp); i++) {
        if (_marpaESLIFJSONDecodeDepositStackGetb(&marpaESLIFJSONDecodeContext, &marpaESLIFJSONDecodeDeposit, i)) {
          if (marpaESLIFJSONDecodeDeposit.dstp != NULL) {
            MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, funcs, "Calling for a deep deallocation inside dstp %p", marpaESLIFJSONDecodeDeposit.dstp);
            _marpaESLIFRecognizer_marpaESLIFValueResult_freeb(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, marpaESLIFJSONDecodeDeposit.dstp, 1 /* deepb */);
            if (i > 0) {
              /* They are always allocated per def */
              if (marpaESLIFJSONDecodeDeposit.contextp != NULL) {
                MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, funcs, "Freeing callback context %p", marpaESLIFJSONDecodeDeposit.contextp);
                free(marpaESLIFJSONDecodeDeposit.contextp);
              }
              MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, funcs, "Freeing dstp %p", marpaESLIFJSONDecodeDeposit.dstp);
              free(marpaESLIFJSONDecodeDeposit.dstp);
            }
          }
        }
      }
    }
    GENERICSTACK_RESET(marpaESLIFJSONDecodeContext.depositStackp);
  }
  if (marpaESLIFJSONDecodeContext.numbers != NULL) {
    free(marpaESLIFJSONDecodeContext.numbers);
  }
  if (marpaESLIFJSONDecodeContext.uint32p != NULL) {
    free(marpaESLIFJSONDecodeContext.uint32p);
  }
  if (marpaESLIFJSONDecodeContext.numbers != NULL) {
    free(marpaESLIFJSONDecodeContext.numbers);
  }
  if (marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp != NULL) {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
    MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp);
    marpaESLIFRecognizer_freev(marpaESLIFJSONDecodeContext.marpaESLIFRecognizerp);
  }

  return rcb;
}

/*****************************************************************************/
static void _marpaESLIFJSONReaderDisposev(void *userDatavp, char *inputcp, size_t inputl, short eofb, short characterStreamb, char *encodings, size_t encodingl)
/*****************************************************************************/
{
  static const char             *funcs                        = "_marpaESLIFJSONReaderDisposev";
  marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp = (marpaESLIFJSONDecodeContext_t *) userDatavp;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  /* Proxy to caller's read disposer */
  if (marpaESLIFJSONDecodeContextp->readerDisposep != NULL) {
    marpaESLIFJSONDecodeContextp->readerDisposep(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerOptionp->userDatavp,
                                                 inputcp,
                                                 inputl,
                                                 eofb,
                                                 characterStreamb,
                                                 encodings,
                                                 encodingl);
  }

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return");
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
}

/*****************************************************************************/
static short _marpaESLIFJSONDecodeReaderb(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp)
/*****************************************************************************/
{
  static const char             *funcs                        = "_marpaESLIFJSONDecodeReaderb";
  marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp = (marpaESLIFJSONDecodeContext_t *) userDatavp;
  short                          rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  /* Proxy to caller's recognizer */
  rcb = marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerOptionp->readerCallbackp(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerOptionp->userDatavp,
                                                                                   inputcpp,
                                                                                   inputlp,
                                                                                   eofbp,
                                                                                   characterStreambp,
                                                                                   encodingsp,
                                                                                   encodinglp,
                                                                                   &(marpaESLIFJSONDecodeContextp->readerDisposep));

  *disposeCallbackpp = _marpaESLIFJSONReaderDisposev;

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);

  return rcb;
}

/*****************************************************************************/
static marpaESLIFRecognizerEventCallback_t _marpaESLIFJSONDecodeEventActionResolverp(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions)
/*****************************************************************************/
{
  static const char                   *funcs = "_marpaESLIFJSONDecodeEventActionResolverp";
  marpaESLIFRecognizerEventCallback_t  rcp;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  rcp = _marpaESLIFJSONDecodeEventCallbackb;

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %p", rcp);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFRecognizerp);

  return rcp;
}

/*****************************************************************************/
static short _marpaESLIFJSONDecodeEventCallbackb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFEvent_t *eventArrayp, size_t eventArrayl, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp)
/*****************************************************************************/
{
  static const char             *funcs = "_marpaESLIFJSONDecodeEventCallbackb";
  marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp = (marpaESLIFJSONDecodeContext_t *) userDatavp;
  size_t                         i;
  short                          rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "start");

  for (i = 0; i < eventArrayl; i++) {
    if (eventArrayp[i].events != NULL) {
      switch (eventArrayp[i].events[0]) {
      case '0':
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Processing %s event", eventArrayp[i].events);
        if (MARPAESLIF_UNLIKELY(! _marpaESLIFJSONDecodeIncb(marpaESLIFJSONDecodeContextp, marpaESLIFValueResultBoolp))) {
          goto err;
        }
        break;
      case '1':
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Processing %s event", eventArrayp[i].events);
        if (MARPAESLIF_UNLIKELY(! _marpaESLIFJSONDecodeDecb(marpaESLIFJSONDecodeContextp, marpaESLIFValueResultBoolp))) {
          goto err;
        }
        break;
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Skipping %s event", eventArrayp[i].events);
        *marpaESLIFValueResultBoolp = MARPAESLIFVALUERESULTBOOL_TRUE;
        break;
      default:
        MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp->marpaESLIFp, "Unrecognized event \"%s\"", eventArrayp[i].events);
        goto err;
        break;
      }
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeIncb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIFJSONDecodeIncb";
  size_t             currentDepthl;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  if (marpaESLIFJSONDecodeContextp->marpaESLIFJSONDecodeOptionp->maxDepthl > 0) {
    currentDepthl = marpaESLIFJSONDecodeContextp->currentDepthl;
    if (++currentDepthl < marpaESLIFJSONDecodeContextp->currentDepthl) {
      MARPAESLIF_ERROR(marpaESLIFJSONDecodeContextp->marpaESLIFp, "size_t turnaround when computing currentDepthl");
      *marpaESLIFValueResultBoolp = MARPAESLIFVALUERESULTBOOL_FALSE;
    } else {
      marpaESLIFJSONDecodeContextp->currentDepthl = currentDepthl;
      if (currentDepthl > marpaESLIFJSONDecodeContextp->marpaESLIFJSONDecodeOptionp->maxDepthl) {
        MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "Maximum depth %ld reached", (unsigned long) marpaESLIFJSONDecodeContextp->marpaESLIFJSONDecodeOptionp->maxDepthl);
        errno = EINVAL;
        *marpaESLIFValueResultBoolp = MARPAESLIFVALUERESULTBOOL_FALSE;
      } else {
        *marpaESLIFValueResultBoolp = MARPAESLIFVALUERESULTBOOL_TRUE;
      }
    }
  } else {
    *marpaESLIFValueResultBoolp = MARPAESLIFVALUERESULTBOOL_TRUE;
  }

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return 1");
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return 1;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeDecb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIFJSONDecodeDecb";
  size_t             currentDepthl;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  if (marpaESLIFJSONDecodeContextp->marpaESLIFJSONDecodeOptionp->maxDepthl > 0) {
    /* No need to check size_t turnaround: currentDepthl can only be decrease if this was successfuly increased previously */
    --marpaESLIFJSONDecodeContextp->currentDepthl;
  }

  *marpaESLIFValueResultBoolp = MARPAESLIFVALUERESULTBOOL_TRUE;

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return 1");
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return 1;
}

/*****************************************************************************/
static short _marpaESLIFJSONDecodeImportb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  static const char             *funcs = "_marpaESLIFJSONDecodeImportb";
  marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp = (marpaESLIFJSONDecodeContext_t *) userDatavp;
  short                          rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  /* Proxy to user-defined importb */
  rcb = marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerOptionp->importerp(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp,
                                                                             marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerOptionp->userDatavp,
                                                                             marpaESLIFValueResultp);

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeSetPositiveInfinityb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, char *inputs, size_t inputl)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFJSONDecodeSetPositiveInfinityb";
  short              confidenceb;
  short              rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

#ifdef MARPAESLIF_INFINITY
  currentValuep->contextp        = NULL;
  currentValuep->representationp = NULL;
  currentValuep->type            = MARPAESLIF_VALUE_TYPE_FLOAT;
  currentValuep->u.f             = marpaESLIFJSONDecodeContextp->marpaESLIFp->positiveinfinityf;
  confidenceb                             = 1;
#else
  currentValuep->contextp        = NULL;
  currentValuep->representationp = NULL;
  currentValuep->type            = MARPAESLIF_VALUE_TYPE_UNDEF;
  confidenceb                             = 0;
#endif

  rcb = _marpaESLIFJSONDecodeProposalb(marpaESLIFJSONDecodeContextp,
                                       marpaESLIFJSONDecodeContextp->marpaESLIFJSONDecodeOptionp->positiveInfinityActionp,
                                       inputs,
                                       inputl,
                                       currentValuep,
                                       confidenceb);
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeSetNegativeInfinityb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, char *inputs, size_t inputl)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFJSONDecodeSetNegativeInfinityb";
  short              confidenceb;
  short              rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

#ifdef MARPAESLIF_INFINITY
  currentValuep->contextp        = NULL;
  currentValuep->representationp = NULL;
  currentValuep->type            = MARPAESLIF_VALUE_TYPE_FLOAT;
  currentValuep->u.f             = marpaESLIFJSONDecodeContextp->marpaESLIFp->negativeinfinityf;
  confidenceb                             = 1;
#else
  currentValuep->contextp        = NULL;
  currentValuep->representationp = NULL;
  currentValuep->type            = MARPAESLIF_VALUE_TYPE_UNDEF;
  confidenceb                             = 0;
#endif

  rcb = _marpaESLIFJSONDecodeProposalb(marpaESLIFJSONDecodeContextp,
                                       marpaESLIFJSONDecodeContextp->marpaESLIFJSONDecodeOptionp->negativeInfinityActionp,
                                       inputs,
                                       inputl,
                                       currentValuep,
                                       confidenceb);
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeSetPositiveNanb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, char *inputs, size_t inputl)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFJSONDecodeSetPositiveNanb";
  short              confidenceb;
  short              rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

#ifdef MARPAESLIF_INFINITY
  currentValuep->contextp        = NULL;
  currentValuep->representationp = NULL;
  currentValuep->type            = MARPAESLIF_VALUE_TYPE_FLOAT;
  currentValuep->u.f             = marpaESLIFJSONDecodeContextp->marpaESLIFp->positivenanf;
  confidenceb                             = marpaESLIFJSONDecodeContextp->marpaESLIFp->nanconfidenceb;
#else
  currentValuep->contextp        = NULL;
  currentValuep->representationp = NULL;
  currentValuep->type            = MARPAESLIF_VALUE_TYPE_UNDEF;
  confidenceb                             = 0;
#endif

  rcb = _marpaESLIFJSONDecodeProposalb(marpaESLIFJSONDecodeContextp,
                                       marpaESLIFJSONDecodeContextp->marpaESLIFJSONDecodeOptionp->positiveNanActionp,
                                       inputs,
                                       inputl,
                                       currentValuep,
                                       confidenceb);
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeSetNegativeNanb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, char *inputs, size_t inputl)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFJSONDecodeSetNegativeNanb";
  short              confidenceb;
  short              rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

#ifdef MARPAESLIF_INFINITY
  currentValuep->contextp        = NULL;
  currentValuep->representationp = NULL;
  currentValuep->type            = MARPAESLIF_VALUE_TYPE_FLOAT;
  currentValuep->u.f             = marpaESLIFJSONDecodeContextp->marpaESLIFp->negativenanf;
  confidenceb                             = marpaESLIFJSONDecodeContextp->marpaESLIFp->nanconfidenceb;
#else
  currentValuep->contextp        = NULL;
  currentValuep->representationp = NULL;
  currentValuep->type            = MARPAESLIF_VALUE_TYPE_UNDEF;
  confidenceb                             = 0;
#endif

  rcb = _marpaESLIFJSONDecodeProposalb(marpaESLIFJSONDecodeContextp,
                                       marpaESLIFJSONDecodeContextp->marpaESLIFJSONDecodeOptionp->negativeNanActionp,
                                       inputs,
                                       inputl,
                                       currentValuep,
                                       confidenceb);
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeSetNumberb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, char *inputs, size_t inputl)
/*****************************************************************************/
/* NUMBER ~ /(?:[+-]?(?:[0-9]+)(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?)|(?:[+-]?[Ii][Nn][Ff](?:[Ii][Nn][Ii][Tt][Yy])?)|(?:[+-]?[Nn][Aa][Nn])/ */
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFJSONDecodeSetNumberb";
  short              confidenceb = 1; /* Set to 0 only when we got through the double case */
  char              *numbers;
  char              *tmps;
  size_t             heapl;
  short              rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  /* Infinity or NaN case ? */
  switch (inputs[0]) {
  case 'I':
  case 'i':
    return _marpaESLIFJSONDecodeSetPositiveInfinityb(marpaESLIFJSONDecodeContextp, currentValuep, inputs, inputl);
  case 'N':
  case 'n':
    return _marpaESLIFJSONDecodeSetPositiveNanb(marpaESLIFJSONDecodeContextp, currentValuep, inputs, inputl);
  case '+':
    if (inputl >= 2) {
      switch (inputs[1]) {
      case 'I':
      case 'i':
        return _marpaESLIFJSONDecodeSetPositiveInfinityb(marpaESLIFJSONDecodeContextp, currentValuep, inputs, inputl);
      case 'N':
      case 'n':
        return _marpaESLIFJSONDecodeSetPositiveNanb(marpaESLIFJSONDecodeContextp, currentValuep, inputs, inputl);
      default:
        break;
      }
    }
    break;
  case '-':
    if (inputl >= 2) {
      switch (inputs[1]) {
      case 'I':
      case 'i':
        return _marpaESLIFJSONDecodeSetNegativeInfinityb(marpaESLIFJSONDecodeContextp, currentValuep, inputs, inputl);
      case 'N':
      case 'n':
        return _marpaESLIFJSONDecodeSetNegativeNanb(marpaESLIFJSONDecodeContextp, currentValuep, inputs, inputl);
      default:
        break;
      }
    }
    break;
  default:
    break;
  }
  
  if (inputl <= MARPAESLIFJSON_ARRAYL_IN_STRUCTURE) {
    numbers = marpaESLIFJSONDecodeContextp->_numbers;
  } else {
    heapl = _marpaESLIF_next_power_of_twob(marpaESLIFJSONDecodeContextp->marpaESLIFp, inputl);
    if (heapl <= 0) {
      goto err;
    }
    if (marpaESLIFJSONDecodeContextp->numbers == NULL) {
      marpaESLIFJSONDecodeContextp->numbers = (char *) malloc(heapl + 1); /* + 1 for the NUL byte */
      if (marpaESLIFJSONDecodeContextp->numbers == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Allocated marpaESLIFJSONDecodeContextp->numbers to %p", marpaESLIFJSONDecodeContextp->numbers);
      numbers = marpaESLIFJSONDecodeContextp->numbers;
      marpaESLIFJSONDecodeContextp->numberallocl = heapl;
    } else if (marpaESLIFJSONDecodeContextp->numberallocl < heapl) {
      tmps = (char *) realloc(marpaESLIFJSONDecodeContextp->numbers, heapl + 1); /* + 1 for the NUL byte */
      if (tmps == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "realloc failure, %s", strerror(errno));
        goto err;
      }
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Reallocated marpaESLIFJSONDecodeContextp->numbers from %p to %p", marpaESLIFJSONDecodeContextp->numbers, tmps);
      numbers = marpaESLIFJSONDecodeContextp->numbers = tmps;
      marpaESLIFJSONDecodeContextp->numberallocl = heapl;
    } else {
      numbers = marpaESLIFJSONDecodeContextp->numbers;
    }
  }

  memcpy(numbers, inputs, inputl);
  numbers[inputl] = '\0';

  /* Note that the grammar made sure that the number respect the strict mode or not, therefore parsing */
  /* the string use the non-strict mode used by _marpaESLIF_numberb() will work regardless of the      */
  /* strict mode.                                                                                      */
  if (! _marpaESLIF_numberb(marpaESLIFJSONDecodeContextp->marpaESLIFp,
                            numbers,
                            currentValuep,
                            &confidenceb)) {
    goto err;
  }

  rcb = _marpaESLIFJSONDecodeProposalb(marpaESLIFJSONDecodeContextp,
                                       marpaESLIFJSONDecodeContextp->marpaESLIFJSONDecodeOptionp->numberActionp,
                                       inputs,
                                       inputl,
                                       currentValuep,
                                       confidenceb);
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeExtendStringContainerb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, size_t incl)
/*****************************************************************************/
/* /(?:[^"\\\x00-\x1F]+)|(?:\\["\\\/bfnrt])|(?:(?:\\u[[:xdigit:]]{4})+)/     */
/* Cases are:                                                                */
/* - First character is     '\': this is an escaped character                */
/* - First character is not '\': the whole match is ASCII chars              */
/* Caller is responsible to have set stringallocl correctly.                 */
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIFJSONDecodeExtendStringContainerb";
  size_t             wantedl;
  size_t             heapl;
  char              *tmps;
  short              rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  wantedl = currentValuep->u.s.sizel + incl;
  if (wantedl < currentValuep->u.s.sizel) {
    MARPAESLIF_ERROR(marpaESLIFJSONDecodeContextp->marpaESLIFp, "size_t turnaround when computing string size");
    goto err;
  }

  if (wantedl > marpaESLIFJSONDecodeContextp->stringallocl) {
    heapl = _marpaESLIF_next_power_of_twob(marpaESLIFJSONDecodeContextp->marpaESLIFp, wantedl);
    if (heapl <= 0) {
      goto err;
    }

    if (marpaESLIFJSONDecodeContextp->stringallocl <= 0) {
      currentValuep->u.s.p = (char *) malloc(heapl + 1); /* +1 for NUL byte */
      if (currentValuep->u.s.p == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Allocated current value %p->u.s.p to %p", currentValuep, currentValuep->u.s.p);
    } else {
      tmps = (char *) realloc(currentValuep->u.s.p, heapl + 1); /* +1 for NUL byte */
      if (tmps == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "realloc failure, %s", strerror(errno));
        goto err;
      }
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Reallocated current value %p->u.s.p from %p to %p", currentValuep, currentValuep->u.s.p, tmps);
      currentValuep->u.s.p = tmps;
    }

    if (marpaESLIFJSONDecodeContextp->stringallocl <= 0) {
      /* First time */
      currentValuep->contextp           = NULL;
      currentValuep->representationp    = NULL;
      currentValuep->type               = MARPAESLIF_VALUE_TYPE_STRING;
      currentValuep->u.s.shallowb       = 0;
      currentValuep->u.s.freeUserDatavp = marpaESLIFJSONDecodeContextp->marpaESLIFp;
      currentValuep->u.s.freeCallbackp  = _marpaESLIF_generic_freeCallbackv;
      currentValuep->u.s.encodingasciis = (char *) MARPAESLIF_UTF8_STRING;
    }

    currentValuep->u.s.sizel          = wantedl;
    marpaESLIFJSONDecodeContextp->stringallocl = heapl;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeAppendCharb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, char *inputs, size_t inputl)
/*****************************************************************************/
/* /(?:[^"\\\x00-\x1F]+)|(?:\\["\\\/bfnrt])|(?:(?:\\u[[:xdigit:]]{4})+)/     */
/* Cases are:                                                                */
/* - First character is     '\': this is an escaped character                */
/* - First character is not '\': the whole match is ASCII chars              */
/* Caller is responsible to have set stringallocl correctly.                 */
/*****************************************************************************/
{
  static const char       *funcs = "_marpaESLIFJSONDecodeAppendCharb";
  marpaESLIF_uint32_t     *uint32p;
  marpaESLIF_uint32_t     *tmpp;
  size_t                   uint32l;
  size_t                   previousStringSizel;
  size_t                   heapl;
  size_t                   dstl;
  marpaESLIF_uint32_t      c;
  char                    *p;
  unsigned char           *q;
  size_t                   i;
  size_t                   j;
  short                    rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  /* We re-process input only when this is an escaped character */
  if (inputs[0] != '\\') {
    previousStringSizel = currentValuep->u.s.sizel;
    if (! _marpaESLIFJSONDecodeExtendStringContainerb(marpaESLIFJSONDecodeContextp, currentValuep, inputl /* incl */)) {
      goto err;
    }
    memcpy(&(currentValuep->u.s.p[previousStringSizel]), inputs, inputl);
  } else {
    /* By definition there is something else after */
    switch (inputs[1]) {
    case '"':
      if (! _marpaESLIFJSONDecodeExtendStringContainerb(marpaESLIFJSONDecodeContextp, currentValuep, 1 /* incl */)) {
        goto err;
      }
      currentValuep->u.s.p[currentValuep->u.s.sizel-1] = MARPAESLIFJSON_DQUOTE;
      break;
    case '\\':
      if (! _marpaESLIFJSONDecodeExtendStringContainerb(marpaESLIFJSONDecodeContextp, currentValuep, 1 /* incl */)) {
        goto err;
      }
      currentValuep->u.s.p[currentValuep->u.s.sizel-1] = MARPAESLIFJSON_BACKSLASH;
      break;
    case '/':
      if (! _marpaESLIFJSONDecodeExtendStringContainerb(marpaESLIFJSONDecodeContextp, currentValuep, 1 /* incl */)) {
        goto err;
      }
      currentValuep->u.s.p[currentValuep->u.s.sizel-1] = MARPAESLIFJSON_SLASH;
      break;
    case 'b':
      if (! _marpaESLIFJSONDecodeExtendStringContainerb(marpaESLIFJSONDecodeContextp, currentValuep, 1 /* incl */)) {
        goto err;
      }
      currentValuep->u.s.p[currentValuep->u.s.sizel-1] = MARPAESLIFJSON_BACKSPACE;
      break;
    case 'f':
      if (! _marpaESLIFJSONDecodeExtendStringContainerb(marpaESLIFJSONDecodeContextp, currentValuep, 1 /* incl */)) {
        goto err;
      }
      currentValuep->u.s.p[currentValuep->u.s.sizel-1] = MARPAESLIFJSON_FORMFEED;
      break;
    case 'r':
      if (! _marpaESLIFJSONDecodeExtendStringContainerb(marpaESLIFJSONDecodeContextp, currentValuep, 1 /* incl */)) {
        goto err;
      }
      currentValuep->u.s.p[currentValuep->u.s.sizel-1] = MARPAESLIFJSON_LINEFEED;
      break;
    case 'n':
      if (! _marpaESLIFJSONDecodeExtendStringContainerb(marpaESLIFJSONDecodeContextp, currentValuep, 1 /* incl */)) {
        goto err;
      }
      currentValuep->u.s.p[currentValuep->u.s.sizel-1] = MARPAESLIFJSON_CARRIAGE_RETURN;
      break;
    case 't':
      if (! _marpaESLIFJSONDecodeExtendStringContainerb(marpaESLIFJSONDecodeContextp, currentValuep, 1 /* incl */)) {
        goto err;
      }
      currentValuep->u.s.p[currentValuep->u.s.sizel-1] = MARPAESLIFJSON_HORIZONTAL_TAB;
      break;
      break;
    default:
      /* It is a sequence of '\uXXXX' by definition, i.e. 6 bytes - so there are (currentValuep->u.a.sizel / 6) hex digits */
      uint32l = inputl / 6;

      if (uint32l <= MARPAESLIFJSON_ARRAYL_IN_STRUCTURE) {
        uint32p = marpaESLIFJSONDecodeContextp->_uint32p;
      } else {
        heapl = _marpaESLIF_next_power_of_twob(marpaESLIFJSONDecodeContextp->marpaESLIFp, uint32l);
        if (heapl <= 0) {
          goto err;
        }
        if (marpaESLIFJSONDecodeContextp->uint32p == NULL) {
          marpaESLIFJSONDecodeContextp->uint32p = (marpaESLIF_uint32_t *) malloc(heapl * sizeof(marpaESLIF_uint32_t));
          if (marpaESLIFJSONDecodeContextp->uint32p == NULL) {
            MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "malloc failure, %s", strerror(errno));
            goto err;
          }
          MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Allocated marpaESLIFJSONDecodeContextp->uint32p to %p", marpaESLIFJSONDecodeContextp->uint32p);
          uint32p = marpaESLIFJSONDecodeContextp->uint32p;
          marpaESLIFJSONDecodeContextp->uint32allocl = heapl;
        } else if (marpaESLIFJSONDecodeContextp->uint32allocl < heapl) {
          tmpp = (marpaESLIF_uint32_t *) realloc(marpaESLIFJSONDecodeContextp->uint32p, heapl * sizeof(marpaESLIF_uint32_t));
          if (tmpp == NULL) {
            MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "realloc failure, %s", strerror(errno));
            goto err;
          }
          MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Reallocated marpaESLIFJSONDecodeContextp->uint32p from %p to %p", marpaESLIFJSONDecodeContextp->uint32p, tmpp);
          uint32p = marpaESLIFJSONDecodeContextp->uint32p = tmpp;
          marpaESLIFJSONDecodeContextp->uint32allocl = heapl;
        } else {
          uint32p = marpaESLIFJSONDecodeContextp->uint32p;
        }
      }

      for (i = 0, p = inputs + 2; i < uint32l; i++, p += 2) {
        c = 0;

        MARPAESLIFJSON_DST_OR_VALCHAR(marpaESLIFJSONDecodeContextp, c, *p++);
        c <<= 4;
        MARPAESLIFJSON_DST_OR_VALCHAR(marpaESLIFJSONDecodeContextp, c, *p++);
        c <<= 4;
        MARPAESLIFJSON_DST_OR_VALCHAR(marpaESLIFJSONDecodeContextp, c, *p++);
        c <<= 4;
        MARPAESLIFJSON_DST_OR_VALCHAR(marpaESLIFJSONDecodeContextp, c, *p++);

        uint32p[i] = c;
      }

      /* Worst case is four UTF-8 bytes */
      previousStringSizel = currentValuep->u.s.sizel;
      dstl = uint32l * 4;
      if (! _marpaESLIFJSONDecodeExtendStringContainerb(marpaESLIFJSONDecodeContextp, currentValuep, dstl /* incl */)) {
        goto err;
      }

      /* Based on efi_utf16_to_utf8 from Linux kernel */
      q = &(currentValuep->u.s.p[previousStringSizel]);
      for (i = 0, j = 1; i < uint32l; i++, j++) {
        c = uint32p[i];

        if ((j < uint32l) && (c >= 0xD800) && (c <= 0xDBFF) && (uint32p[j] >= 0xDC00) && (uint32p[j] <= 0xDFFF)) {
          /* Surrogate UTF-16 pair */
          c = 0x10000 + ((c & 0x3FF) << 10) + (uint32p[j] & 0x3FF);
          ++i;
          ++j;
        }

        if ((c >= 0xD800) && (c <= 0xDFFFF)) {
          if (marpaESLIFJSONDecodeContextp->marpaESLIFJSONDecodeOptionp->noReplacementCharacterb) {
            MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp,
                              "Invalid UTF-16 character \\%c%c%c%c%c",
                              inputs[(i * 6) + 1],
                              inputs[(i * 6) + 2],
                              inputs[(i * 6) + 3],
                              inputs[(i * 6) + 4],
                              inputs[(i * 6) + 5]);
            goto err;
          } else {
            MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp,
                                        funcs,
                                        "Invalid UTF-16 character \\%c%c%c%c%c replaced by 0xFFFD",
                                        inputs[(i * 6) + 1],
                                        inputs[(i * 6) + 2],
                                        inputs[(i * 6) + 3],
                                        inputs[(i * 6) + 4],
                                        inputs[(i * 6) + 5]);
            c = 0xFFFD; /* Replacement character */
          }
        }

        if (c < 0x80) {
          *q++ = c;
          continue;
        }

        if (c < 0x800) {
          *q++ = 0xC0 + (c >> 6);
          goto t1;
        }

        if (c < 0x10000) {
          *q++ = 0xE0 + (c >> 12);
          goto t2;
        }

        *q++ = 0xF0 + (c >> 18);
        *q++ = 0x80 + ((c >> 12) & 0x3F);
      t2:
        *q++ = 0x80 + ((c >> 6) & 0x3F);
      t1:
        *q++ = 0x80 + (c & 0x3F);
      }

      break;
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeProposalb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFJSONProposalAction_t proposalp, char *inputs, size_t inputl, marpaESLIFValueResult_t *currentValuep, short confidenceb)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIFJSONDecodeProposalb";
  short rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  if (proposalp != NULL) {
    if (MARPAESLIF_UNLIKELY(! proposalp(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerOptionp->userDatavp,
                                        inputs,
                                        inputl,
                                        currentValuep,
                                        confidenceb))) {
      MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "%s: Callback failure", inputs);
      goto err;
    }
  } else if (currentValuep->type == MARPAESLIF_VALUE_TYPE_UNDEF) {
    MARPAESLIF_WARNF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "%s: Parsing failure, using undefined value", inputs);
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeSetConstantb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep, char *inputs, size_t inputl)
/*****************************************************************************/
{
  static const char *funcs = "__marpaESLIFJSONDecodeSetConstantb";
  short              rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  switch (inputs[0]) {
  case 't':
  case 'T':
    *currentValuep = marpaESLIFJSONDecodeContextp->marpaESLIFp->marpaESLIFValueResultTrue;
    break;
  case 'f':
  case 'F':
    *currentValuep = marpaESLIFJSONDecodeContextp->marpaESLIFp->marpaESLIFValueResultFalse;
    break;
  case 'n':
  case 'N':
    *currentValuep = marpaESLIFValueResultUndef;
    break;
  default:
    MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp,
                      "Invalid constant first character '%c'",
                      inputs[0]);
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeDepositStackPushb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFJSONDecodeDeposit_t *marpaESLIFJSONDecodeDepositp)
/*****************************************************************************/
/* We abuse marpaESLIFValueResult_t:                                         */
/*                                                                           */
/*          marpaESLIFValueResult_t             marpaESLIFJSONDeposit_t      */
/*            representationp                     dstp                       */
/*            contextp                            contextp                   */
/*            u.p.p                               actionp                    */
/*****************************************************************************/
{
  static const char       *funcs = "_marpaESLIFJSONDecodeDepositStackPushb";
  marpaESLIFValueResult_t  marpaESLIFValueResult;
  short                    rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  marpaESLIFValueResult.representationp  = (marpaESLIFRepresentation_t) marpaESLIFJSONDecodeDepositp->dstp;
  marpaESLIFValueResult.contextp         = marpaESLIFJSONDecodeDepositp->contextp;
  marpaESLIFValueResult.u.p.p            = marpaESLIFJSONDecodeDepositp->actionp;

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Set depositStackp[%d]->dstp              = %p", GENERICSTACK_USED(marpaESLIFJSONDecodeContextp->depositStackp), marpaESLIFJSONDecodeDepositp->dstp);
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Set depositStackp[%d]->contextp          = %p", GENERICSTACK_USED(marpaESLIFJSONDecodeContextp->depositStackp), marpaESLIFJSONDecodeDepositp->contextp);
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Set depositStackp[%d]->contextp->keyb    = %d", GENERICSTACK_USED(marpaESLIFJSONDecodeContextp->depositStackp), (int) marpaESLIFJSONDecodeDepositp->contextp->keyb);
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Set depositStackp[%d]->contextp->alllocl = %ld", GENERICSTACK_USED(marpaESLIFJSONDecodeContextp->depositStackp), (unsigned long) marpaESLIFJSONDecodeDepositp->contextp->allocl);
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Set depositStackp[%d]->actionp           = %p", GENERICSTACK_USED(marpaESLIFJSONDecodeContextp->depositStackp), marpaESLIFJSONDecodeDepositp->actionp);

  GENERICSTACK_PUSH_CUSTOM(marpaESLIFJSONDecodeContextp->depositStackp, marpaESLIFValueResult);
  if (GENERICSTACK_ERROR(marpaESLIFJSONDecodeContextp->depositStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "stackp push failure, %s", strerror(errno));
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeDepositStackGetLastb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFJSONDecodeDeposit_t *marpaESLIFJSONDecodeDepositp)
/*****************************************************************************/
/* We abuse marpaESLIFValueResult_t:                                         */
/*                                                                           */
/*          marpaESLIFValueResult_t             marpaESLIFJSONDeposit_t      */
/*            representationp                     dstp                       */
/*            contextp                            contextp                   */
/*            u.p.p                               actionp                    */
/*****************************************************************************/
{
  return _marpaESLIFJSONDecodeDepositStackGetb(marpaESLIFJSONDecodeContextp, marpaESLIFJSONDecodeDepositp, GENERICSTACK_USED(marpaESLIFJSONDecodeContextp->depositStackp) - 1);
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeDepositStackGetb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFJSONDecodeDeposit_t *marpaESLIFJSONDecodeDepositp, int indicei)
/*****************************************************************************/
/* We abuse marpaESLIFValueResult_t:                                         */
/*                                                                           */
/*          marpaESLIFValueResult_t             marpaESLIFJSONDeposit_t      */
/*            representationp                     dstp                       */
/*            contextp                            contextp                   */
/*            u.p.p                               actionp                    */
/*****************************************************************************/
{
  static const char       *funcs   = "_marpaESLIFJSONDecodeDepositStackGetb";
  marpaESLIFValueResult_t *marpaESLIFValueResultp;
  short                    rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start indice=%d", indicei);

  marpaESLIFValueResultp = GENERICSTACK_GET_CUSTOMP(marpaESLIFJSONDecodeContextp->depositStackp, indicei);
  if (GENERICSTACK_ERROR(marpaESLIFJSONDecodeContextp->depositStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "depositStackp get failure, %s", strerror(errno));
    goto err;
  }

  marpaESLIFJSONDecodeDepositp->dstp     = (marpaESLIFValueResult_t *) marpaESLIFValueResultp->representationp;
  marpaESLIFJSONDecodeDepositp->contextp = marpaESLIFValueResultp->contextp;
  marpaESLIFJSONDecodeDepositp->actionp  = (marpaESLIFJSONDecodeDepositCallback_t) marpaESLIFValueResultp->u.p.p;

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Got depositStackp[%d]->dstp             = %p", indicei, marpaESLIFJSONDecodeDepositp->dstp);
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Got depositStackp[%d]->contextp         = %p", indicei, marpaESLIFJSONDecodeDepositp->contextp);
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Got depositStackp[%d]->contextp->keyb   = %d", indicei, (int) marpaESLIFJSONDecodeDepositp->contextp->keyb);
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Got depositStackp[%d]->contextp->allocl = %ld", indicei, (unsigned long) marpaESLIFJSONDecodeDepositp->contextp->allocl);
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Got depositStackp[%d]->actionp          = %p", indicei, marpaESLIFJSONDecodeDepositp->actionp);

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline void _marpaESLIFJSONDecodeDepositStackPopb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIFJSONDecodeDepositStackPopb";

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Popping depositStackp from %d to %d elements", GENERICSTACK_USED(marpaESLIFJSONDecodeContextp->depositStackp), GENERICSTACK_USED(marpaESLIFJSONDecodeContextp->depositStackp) - 1);

  /* The stack contains only shallow elements: it is legal to play directly on the number of used elements */
  GENERICSTACK_USED(marpaESLIFJSONDecodeContextp->depositStackp)--;

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return");
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
}

/*****************************************************************************/
static short _marpaESLIFJSONDecodeSetValueCallbackv(marpaESLIFJSONDecodeDepositCallbackContext_t *marpaESLIFJSONDecodeDepositCallbackContextp, marpaESLIFValueResult_t *dstp, marpaESLIFValueResult_t *srcp)
/*****************************************************************************/
{
  static const char *funcs = "_marpaESLIFJSONDecodeSetValueCallbackv";

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Setting value of type %s", _marpaESLIF_value_types(srcp->type));
  *dstp = *srcp;

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return 1");
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return 1;
}

/*****************************************************************************/
static short _marpaESLIFJSONDecodePushArrayCallbackv(marpaESLIFJSONDecodeDepositCallbackContext_t *marpaESLIFJSONDecodeDepositCallbackContextp, marpaESLIFValueResult_t *dstp, marpaESLIFValueResult_t *srcp)
/*****************************************************************************/
{
  static const char       *funcs = "_marpaESLIFJSONDecodePushArrayCallbackv";
  size_t                   nextSizel;
  size_t                   nextAllocl;
  marpaESLIFValueResult_t *marpaESLIFValueResultTmpp;
  size_t                   indicel;
  short                    rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  if (marpaESLIFJSONDecodeDepositCallbackContextp->allocl <= 0) {
    /* First time */
    dstp->u.r.p = (marpaESLIFValueResult_t *) malloc(sizeof(marpaESLIFValueResult_t));
    if (dstp->u.r.p == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Allocated destination %p->u.r.p to %p", dstp, dstp->u.r.p);

    dstp->contextp           = NULL;
    dstp->representationp    = NULL;
    dstp->type               = MARPAESLIF_VALUE_TYPE_ROW;
    dstp->u.r.shallowb       = 0;
    dstp->u.r.freeUserDatavp = marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp;
    dstp->u.r.freeCallbackp  = _marpaESLIF_generic_freeCallbackv;
    dstp->u.r.sizel          = 1;

    marpaESLIFJSONDecodeDepositCallbackContextp->allocl = 1;
  } else {
    nextSizel = dstp->u.r.sizel + 1;
    /* Paranoid mode */
    if (nextSizel < dstp->u.r.sizel) {
      MARPAESLIF_ERROR(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp, "size_t turnaround when computing nextSizel");
      goto err;
    }

    if (nextSizel > marpaESLIFJSONDecodeDepositCallbackContextp->allocl) {
      nextAllocl = marpaESLIFJSONDecodeDepositCallbackContextp->allocl * 2;
      /* Paranoid mode */
      if (nextAllocl < marpaESLIFJSONDecodeDepositCallbackContextp->allocl * 2) {
        MARPAESLIF_ERROR(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp, "size_t turnaround when computing nextAllocl");
        goto err;
      }
      marpaESLIFValueResultTmpp = (marpaESLIFValueResult_t *) realloc(dstp->u.r.p, nextAllocl * sizeof(marpaESLIFValueResult_t));
      if (marpaESLIFValueResultTmpp == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp, "realloc failure, %s", strerror(errno));
        goto err;
      }

      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Reallocated destination %p->u.r.p from %p to %p", dstp, dstp->u.r.p, marpaESLIFValueResultTmpp);
      dstp->u.r.p = marpaESLIFValueResultTmpp;

      /* It is not a hasard that MARPAESLIF_VALUE_TYPE_UNDEF is 0... */
      if (marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp->NULLisZeroBytesb && marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp->ZeroIntegerisZeroBytesb) {
        memset(&(dstp->u.r.p[marpaESLIFJSONDecodeDepositCallbackContextp->allocl]), '\0', marpaESLIFJSONDecodeDepositCallbackContextp->allocl * sizeof(marpaESLIFValueResult_t));
      } else {
        for (indicel = marpaESLIFJSONDecodeDepositCallbackContextp->allocl; indicel < nextAllocl; indicel++) {
          dstp->u.r.p[indicel] = marpaESLIFValueResultUndef;
        }
      }
      marpaESLIFJSONDecodeDepositCallbackContextp->allocl = nextAllocl;
    }

    dstp->u.r.sizel = nextSizel;
  }

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Setting value of type %s at array indice %ld", _marpaESLIF_value_types(srcp->type), (unsigned long) (dstp->u.r.sizel - 1));
  dstp->u.r.p[dstp->u.r.sizel - 1] = *srcp;
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFJSONDecodeSetHashCallbackv(marpaESLIFJSONDecodeDepositCallbackContext_t *marpaESLIFJSONDecodeDepositCallbackContextp, marpaESLIFValueResult_t *dstp, marpaESLIFValueResult_t *srcp)
/*****************************************************************************/
{
  static const char           *funcs                                 = "_marpaESLIFJSONDecodeSetHashCallbackv";
  size_t                       nextSizel;
  size_t                       nextAllocl;
  marpaESLIFValueResultPair_t *marpaESLIFValueResultPairTmpp;
  size_t                       indicel;
  short                        rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  if (marpaESLIFJSONDecodeDepositCallbackContextp->keyb) {
    if (marpaESLIFJSONDecodeDepositCallbackContextp->allocl <= 0) {
      /* First time */

      /* It is not a hasard that MARPAESLIF_VALUE_TYPE_UNDEF is 0... */
      if (marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp->NULLisZeroBytesb && marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp->ZeroIntegerisZeroBytesb) {
        dstp->u.t.p = (marpaESLIFValueResultPair_t *) calloc(1, sizeof(marpaESLIFValueResultPair_t));
        if (dstp->u.t.p == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp, "calloc failure, %s", strerror(errno));
          goto err;
        }
      } else {
        dstp->u.t.p = (marpaESLIFValueResultPair_t *) malloc(sizeof(marpaESLIFValueResultPair_t));
        if (dstp->u.t.p == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp, "calloc failure, %s", strerror(errno));
          goto err;
        }
        dstp->u.t.p->key   = marpaESLIFValueResultUndef;
        dstp->u.t.p->value = marpaESLIFValueResultUndef;
      }

      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Allocated destination %p->u.t.p to %p", dstp, dstp->u.t.p);
      dstp->contextp           = NULL;
      dstp->representationp    = NULL;
      dstp->type               = MARPAESLIF_VALUE_TYPE_TABLE;
      dstp->u.t.shallowb       = 0;
      dstp->u.t.freeUserDatavp = marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp;
      dstp->u.t.freeCallbackp  = _marpaESLIF_generic_freeCallbackv;
      dstp->u.t.sizel          = 1;

      marpaESLIFJSONDecodeDepositCallbackContextp->allocl = 1;

    } else {
      nextSizel = dstp->u.t.sizel + 1;
      /* Paranoid mode */
      if (nextSizel < dstp->u.t.sizel) {
        MARPAESLIF_ERROR(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp, "size_t turnaround when computing nextSizel");
        goto err;
      }

      if (nextSizel > marpaESLIFJSONDecodeDepositCallbackContextp->allocl) {
        nextAllocl = marpaESLIFJSONDecodeDepositCallbackContextp->allocl * 2;
        /* Paranoid mode */
        if (nextAllocl < marpaESLIFJSONDecodeDepositCallbackContextp->allocl * 2) {
          MARPAESLIF_ERROR(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp, "size_t turnaround when computing nextAllocl");
          goto err;
        }
        marpaESLIFValueResultPairTmpp = (marpaESLIFValueResultPair_t *) realloc(dstp->u.t.p, nextAllocl * sizeof(marpaESLIFValueResultPair_t));
        if (marpaESLIFValueResultPairTmpp == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp, "realloc failure, %s", strerror(errno));
          goto err;
        }

        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Reallocated destination %p->u.t.p from %p to %p", dstp, dstp->u.t.p, marpaESLIFValueResultPairTmpp);
        dstp->u.t.p = marpaESLIFValueResultPairTmpp;

        /* It is not a hasard that MARPAESLIF_VALUE_TYPE_UNDEF is 0... */
        if (marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp->NULLisZeroBytesb && marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp->ZeroIntegerisZeroBytesb) {
          memset(&(dstp->u.t.p[marpaESLIFJSONDecodeDepositCallbackContextp->allocl]), '\0', marpaESLIFJSONDecodeDepositCallbackContextp->allocl * sizeof(marpaESLIFValueResultPair_t));
        } else {
          for (indicel = marpaESLIFJSONDecodeDepositCallbackContextp->allocl; indicel < nextAllocl; indicel++) {
            dstp->u.t.p[indicel].key = marpaESLIFValueResultUndef;
            dstp->u.t.p[indicel].value = marpaESLIFValueResultUndef;
          }
        }

        marpaESLIFJSONDecodeDepositCallbackContextp->allocl = nextAllocl;
      }
      dstp->u.t.sizel = nextSizel;
    }

    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Setting key of type %s at array indice %ld", _marpaESLIF_value_types(srcp->type), (unsigned long) (dstp->u.t.sizel - 1));
    dstp->u.t.p[dstp->u.t.sizel - 1].key = *srcp;
    marpaESLIFJSONDecodeDepositCallbackContextp->keyb = 0;
  } else {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Setting value of type %s at array indice %ld", _marpaESLIF_value_types(srcp->type), (unsigned long) (dstp->u.t.sizel - 1));
    dstp->u.t.p[dstp->u.t.sizel - 1].value = *srcp;
    marpaESLIFJSONDecodeDepositCallbackContextp->keyb = 1;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodePropagateValueb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep)
/*****************************************************************************/
{
  static const char             *funcs = "_marpaESLIFJSONDecodePropagateValueb";
  short                          rcb;
  marpaESLIFJSONDecodeDeposit_t  marpaESLIFJSONDecodeDeposit;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  if (! _marpaESLIFJSONDecodeDepositStackGetLastb(marpaESLIFJSONDecodeContextp, &marpaESLIFJSONDecodeDeposit)) {
    goto err;
  }

  if (! marpaESLIFJSONDecodeDeposit.actionp(marpaESLIFJSONDecodeDeposit.contextp, marpaESLIFJSONDecodeDeposit.dstp, currentValuep)) {
    goto err;
  }

  /* Value has been propagated - undef origin */
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Setting current value *%p to marpaESLIFValueResultUndef", currentValuep);
  *currentValuep = marpaESLIFValueResultUndef;
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFJSONDecodeManageEventsb(marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp, marpaESLIFValueResult_t *currentValuep)
/*****************************************************************************/
{
  static const char                            *funcs = "_marpaESLIFJSONDecodeManageEventsb";
  size_t                                        eventArrayl;
  marpaESLIFEvent_t                            *eventArrayp;
  size_t                                        eventl;
  marpaESLIFJSONDecodeDeposit_t                 marpaESLIFJSONDecodeDeposit;
  marpaESLIFValueResult_t                      *dstp;
  marpaESLIFJSONDecodeDepositCallbackContext_t *marpaESLIFJSONDecodeDepositCallbackContextp;
  char                                         *pauses;
  size_t                                        pausel;
  char                                         *tmps;
  marpaESLIFAlternative_t                       alternative;
  short                                         rcb;

  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_INC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "start");

  if (MARPAESLIF_UNLIKELY(! _marpaESLIFRecognizer_eventb(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, &eventArrayl, &eventArrayp))) {
    goto err;
  }

  for (eventl = 0; eventl < eventArrayl; eventl++) {
    switch (eventArrayp[eventl].events[0]) {
    case '0':
      /*------------------------------------------------
        :terminal ::= '[' pause => after event => 0_inc
        :terminal ::= '{' pause => after event => 0_inc
        ------------------------------------------------*/
      dstp = (marpaESLIFValueResult_t *) malloc(sizeof(marpaESLIFValueResult_t));
      if (dstp == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Allocated destination dstp to %p", dstp);
      *dstp = marpaESLIFValueResultUndef;

      marpaESLIFJSONDecodeDepositCallbackContextp = (marpaESLIFJSONDecodeDepositCallbackContext_t *) malloc(sizeof(marpaESLIFJSONDecodeDepositCallbackContext_t));
      if (marpaESLIFJSONDecodeDepositCallbackContextp == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Allocated callback context to %p", marpaESLIFJSONDecodeDepositCallbackContextp);
      marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFp                  = marpaESLIFJSONDecodeContextp->marpaESLIFp;
      marpaESLIFJSONDecodeDepositCallbackContextp->marpaESLIFJSONDecodeContextp = marpaESLIFJSONDecodeContextp;
      marpaESLIFJSONDecodeDepositCallbackContextp->keyb                         = 1;
      marpaESLIFJSONDecodeDepositCallbackContextp->allocl                       = 0;
      
      switch (eventArrayp[eventl].symbols[1]) {
      case '[':
        marpaESLIFJSONDecodeDeposit.dstp     = dstp;
        marpaESLIFJSONDecodeDeposit.contextp = marpaESLIFJSONDecodeDepositCallbackContextp;
        marpaESLIFJSONDecodeDeposit.actionp  = _marpaESLIFJSONDecodePushArrayCallbackv;
        break;
      case '{':
        marpaESLIFJSONDecodeDeposit.dstp     = dstp;
        marpaESLIFJSONDecodeDeposit.contextp = marpaESLIFJSONDecodeDepositCallbackContextp;
        marpaESLIFJSONDecodeDeposit.actionp   = _marpaESLIFJSONDecodeSetHashCallbackv;
        break;
      default:
        MARPAESLIF_ERROR(marpaESLIFJSONDecodeContextp->marpaESLIFp, "Invalid item symbols");
        goto err;
      }
      if (! _marpaESLIFJSONDecodeDepositStackPushb(marpaESLIFJSONDecodeContextp, &marpaESLIFJSONDecodeDeposit)) {
        goto err;
      }
      break;
    case '1':
      /* ------------------------------------------------
         :terminal ::= ']' pause => after event => 1_dec$
         :terminal ::= '}' pause => after event => 1_dec$
         ------------------------------------------------*/
      if (! _marpaESLIFJSONDecodeDepositStackGetLastb(marpaESLIFJSONDecodeContextp, &marpaESLIFJSONDecodeDeposit)) {
        goto err;
      }
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Setting current value *%p to *%p of type %s", currentValuep, marpaESLIFJSONDecodeDeposit.dstp, _marpaESLIF_value_types(marpaESLIFJSONDecodeDeposit.dstp->type));
      *currentValuep = *(marpaESLIFJSONDecodeDeposit.dstp);

      /* By definition dstp and its context were allocated */
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Freeing dstp %p", marpaESLIFJSONDecodeDeposit.dstp);
      free(marpaESLIFJSONDecodeDeposit.dstp);
      MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Freeing contextp %p", marpaESLIFJSONDecodeDeposit.contextp);
      free(marpaESLIFJSONDecodeDeposit.contextp);

      /* Pop stack and propagate the value */
      _marpaESLIFJSONDecodeDepositStackPopb(marpaESLIFJSONDecodeContextp);
      if (! _marpaESLIFJSONDecodePropagateValueb(marpaESLIFJSONDecodeContextp, currentValuep)) {
        goto err;
      }
      break;
    case '2':
      /*-----------------------------------------------
        :lexeme ::= CHAR pause => after event => 2_CHAR

        Note that by definition currentValuep has been initialized in 5_DQUOTE_START
        -----------------------------------------------*/
      if (! _marpaESLIFRecognizer_lexeme_last_pauseb(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, "CHAR", &pauses, &pausel)) {
        goto err;
      }
      if (currentValuep->u.s.sizel <= 0) {
        currentValuep->u.s.p = (unsigned char *) malloc(pausel);
        if (currentValuep->u.s.p == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Allocated current value %p->u.s.p to %p", currentValuep, currentValuep->u.s.p);
      } else {
        tmps = (unsigned char *) realloc(currentValuep->u.s.p, currentValuep->u.s.sizel + pausel);
        if (tmps == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "realloc failure, %s", strerror(errno));
          goto err;
        }
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Reallocated current value %p->u.s.p from %p to %p", currentValuep, currentValuep->u.s.p, tmps);
        currentValuep->u.s.p = tmps;
      }
      memcpy(currentValuep->u.s.p + currentValuep->u.s.sizel, pauses, pausel);
      currentValuep->u.s.shallowb  = 0;
      currentValuep->u.s.sizel    += pausel;
      break;
    case '3':
      /*-------------------------------------------------------------
        :lexeme   ::= CNTRL_CHAR pause => after event => 3_CNTRL_CHAR
        
        Note that by definition currentValuep has been initialized in 5_DQUOTE_START
        -------------------------------------------------------------*/
      if (! _marpaESLIFRecognizer_lexeme_last_pauseb(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, "CNTRL_CHAR", &pauses, &pausel)) {
        goto err;
      }
      if (currentValuep->u.s.sizel <= 0) {
        currentValuep->u.s.p = (unsigned char *) malloc(pausel);
        if (currentValuep->u.s.p == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Allocated current value %p->u.s.p to %p", currentValuep, currentValuep->u.s.p);
      } else {
        tmps = (unsigned char *) realloc(currentValuep->u.s.p, currentValuep->u.s.sizel + pausel);
        if (tmps == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFJSONDecodeContextp->marpaESLIFp, "realloc failure, %s", strerror(errno));
          goto err;
        }
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "Reallocated current value %p->u.s.p from %p to %p", currentValuep, currentValuep->u.s.p, tmps);
        currentValuep->u.s.p = tmps;
      }
      memcpy(currentValuep->u.s.p + currentValuep->u.s.sizel, pauses, pausel);
      currentValuep->u.s.shallowb  = 0;
      currentValuep->u.s.sizel    += pausel;
      break;
    case '4':
      /* ---------------------------------------------------------
         :lexeme   ::= CONSTANT pause => after event => 4_CONSTANT
         ---------------------------------------------------------*/
      if (! _marpaESLIFRecognizer_lexeme_last_pauseb(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, "CONSTANT", &pauses, &pausel)) {
        goto err;
      }
      if (! _marpaESLIFJSONDecodeSetConstantb(marpaESLIFJSONDecodeContextp, currentValuep, pauses, pausel)) {
        goto err;
      }
      break;
    case '5':
      /* ------------------------------------------------------------------
         :lexeme   ::= DQUOTE_START pause => before event => 5_DQUOTE_START
         ------------------------------------------------------------------*/
      if (! _marpaESLIFRecognizer_hook_discard_switchb(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp)) {
        goto err;
      }

      alternative.lexemes        = "DQUOTE_START";
      alternative.value          = marpaESLIFValueResultUndef;
      alternative.grammarLengthl = 1;

      if (! _marpaESLIFRecognizer_lexeme_readb(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, &alternative, 1 /* lengthl */)) {
        goto err;
      }

      currentValuep->type               = MARPAESLIF_VALUE_TYPE_STRING;
      currentValuep->u.s.p              = (char *) MARPAESLIF_EMPTY_STRING;
      currentValuep->u.s.shallowb       = 1;
      currentValuep->u.s.freeUserDatavp = marpaESLIFJSONDecodeContextp->marpaESLIFp;
      currentValuep->u.s.freeCallbackp  = _marpaESLIF_generic_freeCallbackv;
      currentValuep->u.s.sizel          = 0;
      currentValuep->u.s.encodingasciis = (char *) MARPAESLIF_UTF8_STRING;
      break;
    case '6':
      /* ------------------------------------------------------------------
         :lexeme   ::= DQUOTE_END pause => before event => 6_DQUOTE_END
         ------------------------------------------------------------------*/
      if (! _marpaESLIFRecognizer_hook_discard_switchb(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp)) {
        goto err;
      }
      alternative.lexemes        = "DQUOTE_END";
      alternative.value          = marpaESLIFValueResultUndef;
      alternative.grammarLengthl = 1;
      if (! _marpaESLIFRecognizer_lexeme_readb(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, &alternative, 1 /* lengthl */)) {
        goto err;
      }
      if (! _marpaESLIFJSONDecodePropagateValueb(marpaESLIFJSONDecodeContextp, currentValuep)) {
        goto err;
      }
      break;
    case '7':
      /* ------------------------------------------------------------------
         :lexeme   ::= NUMBER pause => before event => 7_NUMBER
         ------------------------------------------------------------------*/
      if (! _marpaESLIFRecognizer_lexeme_last_pauseb(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, "NUMBER", &pauses, &pausel)) {
        goto err;
      }
      if (! _marpaESLIFJSONDecodeSetNumberb(marpaESLIFJSONDecodeContextp, currentValuep, pauses, pausel)) {
        goto err;
      }
      break;
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  MARPAESLIFRECOGNIZER_CALLSTACKCOUNTER_DEC(marpaESLIFJSONDecodeContextp->marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFJSONDecodeValueResultImportb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  static const char             *funcs                        = "_marpaESLIFJSONDecodeValueResultImportb";
  marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp = (marpaESLIFJSONDecodeContext_t *) userDatavp;

  /* Proxy to user-defined importb */
  return marpaESLIFJSONDecodeContextp->marpaESLIFValueOptionp->importerp(marpaESLIFValuep,
                                                                         marpaESLIFJSONDecodeContextp->marpaESLIFValueOptionp->userDatavp,
                                                                         marpaESLIFValueResultp);
}

/*****************************************************************************/
static short _marpaESLIFJSONDecodeRepresentationb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp, char **encodingasciisp, marpaESLIFRepresentationDispose_t *disposeCallbackpp, short *stringbp)
/*****************************************************************************/
{
  static const char             *funcs                        = "_marpaESLIFJSONDecodeRepresentationb";
  marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp = (marpaESLIFJSONDecodeContext_t *) userDatavp;
  marpaESLIFValueOption_t       *marpaESLIFValueOptionp       = marpaESLIFJSONDecodeContextp->marpaESLIFValueOptionp;
  short                          rcb;

  /* Proxy to caller's representation */
  rcb = marpaESLIFValueResultp->representationp(marpaESLIFValueOptionp->userDatavp,
                                                marpaESLIFValueResultp,
                                                inputcpp,
                                                inputlp,
                                                encodingasciisp,
                                                &(marpaESLIFJSONDecodeContextp->representationDisposep),
                                                stringbp);

  *disposeCallbackpp = _marpaESLIFJSONDecodeRepresentationDisposev;

  return rcb;
}

/*****************************************************************************/
static void _marpaESLIFJSONDecodeRepresentationDisposev(void *userDatavp, char *inputcp, size_t inputl, char *encodingasciis)
/*****************************************************************************/
{
  static const char             *funcs                        = "_marpaESLIFJSONDecodeRepresentationDisposev";
  marpaESLIFJSONDecodeContext_t *marpaESLIFJSONDecodeContextp = (marpaESLIFJSONDecodeContext_t *) userDatavp;
  marpaESLIFValueOption_t       *marpaESLIFValueOptionp       = marpaESLIFJSONDecodeContextp->marpaESLIFValueOptionp;

  /* Proxy to caller's representation disposer */
  if (marpaESLIFJSONDecodeContextp->representationDisposep != NULL) {
    marpaESLIFJSONDecodeContextp->representationDisposep(marpaESLIFValueOptionp->userDatavp,
                                                         inputcp,
                                                         inputl,
                                                         encodingasciis);
  }
}

/*****************************************************************************/
static short _marpaESLIFJSONEncodeValueResultImportb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  static const char             *funcs                        = "_marpaESLIFJSONEncodeValueResultImportb";
  marpaESLIFJSONEncodeContext_t *marpaESLIFJSONEncodeContextp = (marpaESLIFJSONEncodeContext_t *) userDatavp;

  /* Proxy to user-defined importb */
  return marpaESLIFJSONEncodeContextp->marpaESLIFValueOptionp->importerp(marpaESLIFValuep,
                                                                         marpaESLIFJSONEncodeContextp->marpaESLIFValueOptionp->userDatavp,
                                                                         marpaESLIFValueResultp);
}

/*****************************************************************************/
static short _marpaESLIFJSONEncodeRepresentationb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp, char **encodingasciisp, marpaESLIFRepresentationDispose_t *disposeCallbackpp, short *stringbp)
/*****************************************************************************/
{
  static const char             *funcs                        = "_marpaESLIFJSONEncodeRepresentationb";
  marpaESLIFJSONEncodeContext_t *marpaESLIFJSONEncodeContextp = (marpaESLIFJSONEncodeContext_t *) userDatavp;
  marpaESLIFValueOption_t       *marpaESLIFValueOptionp       = marpaESLIFJSONEncodeContextp->marpaESLIFValueOptionp;
  short                          rcb;

  /* Proxy to caller's representation */
  rcb = marpaESLIFValueResultp->representationp(marpaESLIFValueOptionp->userDatavp,
                                                marpaESLIFValueResultp,
                                                inputcpp,
                                                inputlp,
                                                encodingasciisp,
                                                &(marpaESLIFJSONEncodeContextp->representationDisposep),
                                                stringbp);

  *disposeCallbackpp = _marpaESLIFJSONEncodeRepresentationDisposev;

  return rcb;
}

/*****************************************************************************/
static void _marpaESLIFJSONEncodeRepresentationDisposev(void *userDatavp, char *inputcp, size_t inputl, char *encodingasciis)
/*****************************************************************************/
{
  static const char             *funcs                        = "_marpaESLIFJSONEncodeRepresentationDisposev";
  marpaESLIFJSONEncodeContext_t *marpaESLIFJSONEncodeContextp = (marpaESLIFJSONEncodeContext_t *) userDatavp;
  marpaESLIFValueOption_t       *marpaESLIFValueOptionp       = marpaESLIFJSONEncodeContextp->marpaESLIFValueOptionp;

  /* Proxy to caller's representation disposer */
  if (marpaESLIFJSONEncodeContextp->representationDisposep != NULL) {
    marpaESLIFJSONEncodeContextp->representationDisposep(marpaESLIFValueOptionp->userDatavp,
                                                         inputcp,
                                                         inputl,
                                                         encodingasciis);
  }
}
