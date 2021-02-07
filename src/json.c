#include <locale.h>
#include <stdlib.h>

#undef  FILENAMES
#define FILENAMES "json.c" /* For logging */

static void                                 _marpaESLIFJSONReaderDisposev(void *userDatavp, char *inputcp, size_t inputl, short eofb, short characterStreamb, char *encodings, size_t encodingl);
static short                                _marpaESLIFJSONReaderb(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);
static marpaESLIFRecognizerEventCallback_t  _marpaESLIFJSONRecognizerEventActionResolverp(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions);
static short                                _marpaESLIFJSONRecognizerEventCallbackb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFEvent_t *eventArrayp, size_t eventArrayl, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static short                                _marpaESLIFJSON_incb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static short                                _marpaESLIFJSON_decb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static marpaESLIFValueRuleCallback_t        _marpaESLIFJSONValueRuleActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static short                                _marpaESLIFJSONValueResultImportb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static short                                _marpaESLIFJSON_membersb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                                _marpaESLIFJSON_numberb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                                _marpaESLIFJSON_charsb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                                _marpaESLIFJSON_charb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                                _marpaESLIFJSON_constantb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                                _marpaESLIFJSON_positive_infinityb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                                _marpaESLIFJSON_negative_infinityb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                                _marpaESLIFJSON_positive_nanb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                                _marpaESLIFJSON_negative_nanb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                                _marpaESLIFJSON_proposalb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFJSONProposalAction_t proposalp, char *strings, size_t stringl, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static void                                 _marpaESLIFJSONRepresentationDisposev(void *userDatavp, char *inputcp, size_t inputl, char *encodingasciis);
static short                                _marpaESLIFJSONRepresentationb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp, char **encodingasciisp, marpaESLIFRepresentationDispose_t *disposeCallbackpp);

typedef struct marpaESLIFJSONContext {
  size_t                             currentDepthl;
  marpaESLIFJSONDecodeOption_t      *marpaESLIFJSONDecodeOptionp;
  marpaESLIFRecognizerOption_t      *marpaESLIFRecognizerOptionp;
  marpaESLIFValueOption_t           *marpaESLIFValueOptionp;
  /* For number parsing, we are unfortunately dependant on locale */
  char                              decimalPointc;
  marpaESLIFReaderDispose_t         readerDisposep;
  marpaESLIFRepresentationDispose_t representationDisposep;
  /* At every action we use this buffer to see if a number can */
  /* be converted to a true non-floating pointer.              */
  char                             *numbers;
  size_t                            numberallocl;
  char                             *integers;
  size_t                            integerallocl;
  short                             strictb;
} marpaESLIFJSONContext_t;

static const char *MARPAESLIFJSON_DQUOTE          = "\"";
static const char *MARPAESLIFJSON_BACKSLASH       = "\\";
static const char *MARPAESLIFJSON_SLASH           = "/";
static const char *MARPAESLIFJSON_BACKSPACE       = "\x08";
static const char *MARPAESLIFJSON_FORMFEED        = "\x0C";
static const char *MARPAESLIFJSON_LINEFEED        = "\x0A";
static const char *MARPAESLIFJSON_CARRIAGE_RETURN = "\x0D";
static const char *MARPAESLIFJSON_HORIZONTAL_TAB  = "\x09";

static const char *marpaESLIFJSON_encode_extended_grammars =
  "input ::= INPUT action => ::jsonf\n"
  "INPUT   ~ [\\s\\S]\n"
  ;

static const char *marpaESLIFJSON_decode_extended_grammars =
  "#\n"
  "# Default action is to propagate the first RHS value\n"
  "#\n"
  ":default ::= action => ::shift event-action => marpaESLIFJsonEventAction fallback-encoding => UTF-8\n"
  "\n"
  "                   #######################################################\n"
  "                   # >>>>>>>>>>>>>>>> Strict JSON Grammar <<<<<<<<<<<<<<<<\n"
  "                   #######################################################\n"
  "\n"
  "# ----------------\n"
  "# Start is a value\n"
  "# ----------------\n"
  ":start ::= value\n"
  "\n"
  "# -------------------\n"
  "# Composite separator\n"
  "# -------------------\n"
  ":terminal ::= ','                                                                              symbol-action => ::undef    # We do not mind about this symbol\n"
  "comma     ::= ','\n"
  "\n"
  "# ----------\n"
  "# JSON value\n"
  "# ----------\n"
  "value    ::= string\n"
  "           | number\n"
  "           | object\n"
  "           | array\n"
  "           | constant\n"
  "constant ::= /true|false|null/                                                                 action         => constant\n"
  "\n"
  "# -----------\n"
  "# JSON object\n"
  "# -----------\n"
  ":terminal ::= '{'                                                                              symbol-action => ::undef    # We do not mind about this symbol\n"
  ":terminal ::= '}'                                                                              symbol-action => ::undef    # We do not mind about this symbol\n"
  "object    ::= '{' members '}'                                                                  action         => ::copy[1]\n"
  "members   ::= pair*                                                                            action         => members   # Returns { @{pair1}, ..., @{pair2} }\n"
  "                                                                                               separator      => commas    # ... separated by comma(s)\n"
  "                                                                                               proper         => 0         # ... with eventual trailing separator\n"
  "                                                                                               hide-separator => 1         # ... and hide separator in the action\n"
  "\n"
  ":terminal ::= ':'                                                                              symbol-action => ::undef    # We do not mind about this symbol\n"
  "pair      ::= string (-':'-) value                                                             action         => ::row     # Returns [ string, value ]\n"
  "\n"
  "# -----------\n"
  "# JSON Arrays\n"
  "# -----------\n"
  ":terminal ::= '['                                                                              symbol-action => ::undef    # We do not mind about this symbol\n"
  ":terminal ::= ']'                                                                              symbol-action => ::undef    # We do not mind about this symbol\n"
  "array     ::= '[' elements ']'                                                                 action         => ::copy[1] # Returns elements\n"
  "elements  ::= value*                                                                           action         => ::row     # Returns [ value1, ..., valuen ]\n"
  "                                                                                               separator      => commas    # ... separated by comma(s)\n"
  "                                                                                               proper         => 0        # ... with eventual trailing separator\n"
  "                                                                                               hide-separator => 1         # ... and hide separator in the action\n"
  "\n"
  "# -----------\n"
  "# JSON Number\n"
  "# -----------\n"
  "# Take care: we allow a JSON number to start with non-significant digits\n"
  "number ::= /[+-]?(?:[0-9]+)(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?/                                    action => number\n"
  "\n"
  "# -----------\n"
  "# JSON String\n"
  "# -----------\n"
  ":terminal ::= '\"'                                                                             symbol-action => ::undef    # We do not mind about this symbol\n"
  "string    ::= '\"' chars '\"'                                                                  action => ::copy[1]\n"
  "\n"
  ":terminal ::= '\"' pause => after event => :discard[switch]\n"
  "\n"
  "chars   ::= char*                                                                              action => chars\n"
  "char    ::= /(?:[^\"\\\\\\x00-\\x1F]+)|(?:\\\\[\"\\\\\\/bfnrt])|(?:(?:\\\\u[[:xdigit:]]{4})+)/ action => char # Take care: PCRE2 [:cntrl:] includes DEL character\n"
  "\n"
  "# ------------------------------------------------------------\n"
  "# Discard: Unsignificant whitespaces, Perl comment C++ comment\n"
  "# ------------------------------------------------------------\n"
  ":discard ::= /(?:[\\x{9}\\x{A}\\x{D}\\x{20}]+)|(?:(?:(?:#)(?:[^\\n]*)(?:\\n|\\z)))|(?:(?:(?:(?:\\/\\/)(?:[^\\n]*)(?:\\n|\\z))|(?:(?:\\/\\*)(?:(?:[^\\*]+|\\*(?!\\/))*)(?:\\*\\/))))/u\n"
  "# :discard ::= /[\\x{9}\\x{A}\\x{D}\\x{20}]+/\n"
  "# :discard ::= /(?:(?:#)(?:[^\\n]*)(?:\\n|\\z))/u\n"
  "# :discard ::= /(?:(?:(?:\\/\\/)(?:[^\\n]*)(?:\\n|\\z))|(?:(?:\\/\\*)(?:(?:[^\\*]+|\\*(?!\\/))*)(?:\\*\\/)))/\n"
  "\n"
  "# --------------------------\n"
  "# Unlimited commas extension\n"
  "# --------------------------\n"
  "commas   ::= comma+\n"
  "\n"
  "# --------------------------\n"
  "# Max depth limit (does not change the grammar)\n"
  "# --------------------------\n"
  ":terminal ::= '[' pause => after event => inc[]\n"
  ":terminal ::= ']' pause => after event => dec[]\n"
  ":terminal ::= '{' pause => after event => inc[]\n"
  ":terminal ::= '}' pause => after event => dec[]\n"
  "\n"
  "# ----------------\n"
  "# Infinity extension\n"
  "# ----------------\n"
  "number   ::= '-Infinity':i                                                                     action => negative_infinity\n"
  "           | '-Inf':i                                                                          action => negative_infinity\n"
  "           | '+Infinity':i                                                                     action => positive_infinity\n"
  "           | '+Inf':i                                                                          action => positive_infinity\n"
  "           | 'Infinity':i                                                                      action => positive_infinity\n"
  "           | 'Inf':i                                                                           action => positive_infinity\n"
  "\n"
  "# ----------------\n"
  "# Nan extension\n"
  "# ----------------\n"
  "number   ::= '-NaN':i                                                                          action => negative_nan\n"
  "           | 'NaN':i                                                                           action => positive_nan\n"
  "           | '+NaN':i                                                                          action => positive_nan\n"
  "\n"
  "# -----------------\n"
  "# Control character\n"
  "# -----------------\n"
  "char      ::= /[\\x00-\\x1F]/                                                          # Because [:cntrl:] includes DEL (x7F)\n"
  "\n"
  ;

static const char *marpaESLIFJSON_encode_strict_grammars =
  "input ::= INPUT action => ::json\n"
  "INPUT   ~ [\\s\\S]\n"
  ;
static const char *marpaESLIFJSON_decode_strict_grammars =
  "#\n"
  "# Default action is to propagate the first RHS value\n"
  "#\n"
  ":default ::= action => ::shift event-action => marpaESLIFJsonEventAction fallback-encoding => UTF-8\n"
  "\n"
  "                   #######################################################\n"
  "                   # >>>>>>>>>>>>>>>> Strict JSON Grammar <<<<<<<<<<<<<<<<\n"
  "                   #######################################################\n"
  "\n"
  "# ----------------\n"
  "# Start is a value\n"
  "# ----------------\n"
  ":start ::= value\n"
  "\n"
  "# -------------------\n"
  "# Composite separator\n"
  "# -------------------\n"
  ":terminal ::= ','                                                                              symbol-action => ::undef    # We do not mind about this symbol\n"
  "comma     ::= ','\n"
  "\n"
  "# ----------\n"
  "# JSON value\n"
  "# ----------\n"
  "value    ::= string\n"
  "           | number\n"
  "           | object\n"
  "           | array\n"
  "           | constant\n"
  "constant ::= /true|false|null/                                                                 action         => constant\n"
  "\n"
  "# -----------\n"
  "# JSON object\n"
  "# -----------\n"
  ":terminal ::= '{'                                                                              symbol-action => ::undef    # We do not mind about this symbol\n"
  ":terminal ::= '}'                                                                              symbol-action => ::undef    # We do not mind about this symbol\n"
  "object    ::= '{' members '}'                                                                  action         => ::copy[1]\n"
  "members   ::= pair*                                                                            action         => members   # Returns { @{pair1}, ..., @{pair2} }\n"
  "                                                                                               separator      => comma     # ... separated by comma\n"
  "                                                                                               proper         => 1         # ... with no trailing separator\n"
  "                                                                                               hide-separator => 1         # ... and hide separator in the action\n"
  "\n"
  ":terminal ::= ':'                                                                              symbol-action => ::undef    # We do not mind about this symbol\n"
  "pair      ::= string (-':'-) value                                                             action         => ::row     # Returns [ string, value ]\n"
  "\n"
  "# -----------\n"
  "# JSON Arrays\n"
  "# -----------\n"
  ":terminal ::= '['                                                                              symbol-action => ::undef    # We do not mind about this symbol\n"
  ":terminal ::= ']'                                                                              symbol-action => ::undef    # We do not mind about this symbol\n"
  "array     ::= '[' elements ']'                                                                 action         => ::copy[1] # Returns elements\n"
  "elements  ::= value*                                                                           action         => ::row     # Returns [ value1, ..., valuen ]\n"
  "                                                                                               separator      => comma     # ... separated by comma\n"
  "                                                                                               proper         => 1         # ... with no trailing separator\n"
  "                                                                                               hide-separator => 1         # ... and hide separator in the action\n"
  "\n"
  "# -----------\n"
  "# JSON Number\n"
  "# -----------\n"
  "number ::= /-?(?:0|[1-9][0-9]*)(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?/                             action => number\n"
  "\n"
  "# -----------\n"
  "# JSON String\n"
  "# -----------\n"
  ":terminal ::= '\"'                                                                             symbol-action => ::undef    # We do not mind about this symbol\n"
  "string    ::= '\"' chars '\"'                                                                  action => ::copy[1]\n"
  "\n"
  ":terminal ::= '\"' pause => after event => :discard[switch]\n"
  "\n"
  "chars   ::= char*                                                                              action => chars\n"
  "char    ::= /(?:[^\"\\\\\\x00-\\x1F]+)|(?:\\\\[\"\\\\\\/bfnrt])|(?:(?:\\\\u[[:xdigit:]]{4})+)/ action => char # Take care: PCRE2 [:cntrl:] includes DEL character\n"
  "\n"
  "# -------------------------\n"
  "# Unsignificant whitespaces\n"
  "# -------------------------\n"
  ":discard ::= /[\\x{9}\\x{A}\\x{D}\\x{20}]+/\n"
  "\n"
  "# --------------------------\n"
  "# Max depth limit (does not change the grammar)\n"
  "# --------------------------\n"
  ":terminal ::= '[' pause => after event => inc[]\n"
  ":terminal ::= ']' pause => after event => dec[]\n"
  ":terminal ::= '{' pause => after event => inc[]\n"
  ":terminal ::= '}' pause => after event => dec[]\n"
  "\n"
  ;

#define MARPAESLIFJSON_DST_OR_VALCHAR(dst, valchar) do {                \
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
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Unsupported hexadecimal character '%c' (0x%lx)", _valchar, (unsigned long) _valchar); \
      goto err;                                                         \
    }                                                                   \
  } while (0)

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

  marpaESLIFJSONp = _marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption, NULL /* marpaESLIfGrammarPreviousp */);
  if (MARPAESLIF_UNLIKELY(marpaESLIFJSONp == NULL)) {
    goto err;
  }

  /* Remember if this is strict JSON grammar */
  marpaESLIFJSONp->jsonStrictb = strictb;
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

  marpaESLIFJSONp = _marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption, NULL /* marpaESLIFGrammarPreviousp */);
  if (MARPAESLIF_UNLIKELY(marpaESLIFJSONp == NULL)) {
    goto err;
  }

  /* Remember if this is strict JSON grammar */
  marpaESLIFJSONp->jsonStrictb = strictb;
  goto done;

 err:
  marpaESLIFGrammar_freev(marpaESLIFJSONp);
  marpaESLIFJSONp = NULL;

 done:
  return marpaESLIFJSONp;
}

/*****************************************************************************/
short marpaESLIFJSON_decodeb(marpaESLIFGrammar_t *marpaESLIFGrammarJSONp, marpaESLIFJSONDecodeOption_t *marpaESLIFJSONDecodeOptionp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp)
/*****************************************************************************/
{
  marpaESLIFRecognizer_t       *marpaESLIFRecognizerp = NULL;
  marpaESLIFValue_t            *marpaESLIFValuep      = NULL;
  short                         rcb;
  marpaESLIFRecognizerOption_t  marpaESLIFRecognizerOption;
  marpaESLIFValueOption_t       marpaESLIFValueOption;
  marpaESLIFJSONContext_t       marpaESLIFJSONContext;
  short                         continueb;

  /* Whatever happens, we take entire control on the callbacks so that we have our own context on top of it */
  marpaESLIFJSONContext.currentDepthl               = 0;
  marpaESLIFJSONContext.marpaESLIFJSONDecodeOptionp = marpaESLIFJSONDecodeOptionp;
  marpaESLIFJSONContext.marpaESLIFRecognizerOptionp = marpaESLIFRecognizerOptionp;
  marpaESLIFJSONContext.marpaESLIFValueOptionp      = marpaESLIFValueOptionp;
  marpaESLIFJSONContext.decimalPointc               = MARPAESLIF_DECIMAL_POINT(marpaESLIFGrammarJSONp->marpaESLIFp);
  marpaESLIFJSONContext.readerDisposep              = NULL;
  marpaESLIFJSONContext.representationDisposep      = NULL;
  marpaESLIFJSONContext.numbers                     = NULL;
  marpaESLIFJSONContext.numberallocl                = 0;
  marpaESLIFJSONContext.integers                    = NULL;
  marpaESLIFJSONContext.integerallocl               = 0;
  marpaESLIFJSONContext.strictb                     = marpaESLIFGrammarJSONp->jsonStrictb;

  if (MARPAESLIF_UNLIKELY((marpaESLIFGrammarJSONp == NULL) || (marpaESLIFJSONDecodeOptionp == NULL) || (marpaESLIFRecognizerOptionp == NULL) || (marpaESLIFRecognizerOptionp->readerCallbackp == NULL) || (marpaESLIFValueOptionp == NULL))) {
    errno = EINVAL;
    goto err;
  }

  marpaESLIFRecognizerOption                      = *marpaESLIFRecognizerOptionp;
  marpaESLIFRecognizerOption.userDatavp           = &marpaESLIFJSONContext;
  marpaESLIFRecognizerOption.readerCallbackp      = _marpaESLIFJSONReaderb;
  marpaESLIFRecognizerOption.ifActionResolverp    = NULL; /* Our grammar has no if-action anyway */
  marpaESLIFRecognizerOption.eventActionResolverp = _marpaESLIFJSONRecognizerEventActionResolverp;
  marpaESLIFRecognizerOption.regexActionResolverp = NULL;
  marpaESLIFRecognizerOption.importerp            = NULL;

  marpaESLIFValueOption                           = *marpaESLIFValueOptionp;
  marpaESLIFValueOption.userDatavp                = &marpaESLIFJSONContext;
  marpaESLIFValueOption.ruleActionResolverp       = _marpaESLIFJSONValueRuleActionResolverp;
  marpaESLIFValueOption.symbolActionResolverp     = NULL; /* We use the default "::transfer" */
  marpaESLIFValueOption.importerp                 = ((marpaESLIFValueOptionp != NULL) && (marpaESLIFValueOptionp->importerp != NULL)) ? _marpaESLIFJSONValueResultImportb : NULL;
  marpaESLIFValueOption.highRankOnlyb             = 1; /* Fixed */
  marpaESLIFValueOption.orderByRankb              = 1; /* Fixed */
  marpaESLIFValueOption.ambiguousb                = 0; /* Fixed */
  marpaESLIFValueOption.nullb                     = 0; /* Fixed */
  marpaESLIFValueOption.maxParsesi                = 0; /* Fixed */

  if (marpaESLIFJSONDecodeOptionp->maxDepthl == 0) {
    /* No need of any event: the grammar parse method is the fastest */
    rcb = marpaESLIFGrammar_parseb(marpaESLIFGrammarJSONp, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, NULL /* exhaustedbp */);
    goto done;
  }

  /* We need to loop using a recognizer to catch the events */
  marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarJSONp, &marpaESLIFRecognizerOption);
  if (MARPAESLIF_UNLIKELY(marpaESLIFRecognizerp == NULL)) {
    goto err;
  }
  if (MARPAESLIF_UNLIKELY(! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, 1 /* initialEventsb */, &continueb, NULL /* exhaustedbp */))) {
    goto err;
  }
  while (continueb) {
    if (MARPAESLIF_UNLIKELY(! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 0 /* deltaLengthl */, &continueb, NULL /* exhaustedbp */))) {
      goto err;
    }
  }
  marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
  if (MARPAESLIF_UNLIKELY(marpaESLIFValuep == NULL)) {
    goto err;
  }
  /* Set-up proxy representation */
  marpaESLIFValuep->proxyRepresentationp = _marpaESLIFJSONRepresentationb;
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
  if (marpaESLIFJSONContext.numbers != NULL) {
    free(marpaESLIFJSONContext.numbers);
  }
  if (marpaESLIFJSONContext.integers != NULL) {
    free(marpaESLIFJSONContext.integers);
  }
  return rcb;
}

/*****************************************************************************/
short marpaESLIFJSON_encodeb(marpaESLIFGrammar_t *marpaESLIFGrammarJSONp, marpaESLIFValueResult_t *marpaESLIFValueResultp, marpaESLIFValueOption_t *marpaESLIFValueOptionp)
/*****************************************************************************/
/* Strict or not, encode grammar is always at level 10                       */
/*****************************************************************************/
{
  marpaESLIFRecognizer_t       *marpaESLIFRecognizerp = NULL;
  marpaESLIFValue_t            *marpaESLIFValuep      = NULL;
  short                         rcb;
  marpaESLIFAlternative_t       marpaESLIFAlternative;
  marpaESLIFValueOption_t       marpaESLIFValueOption;
  marpaESLIFJSONContext_t       marpaESLIFJSONContext;

  marpaESLIFJSONContext.currentDepthl               = 0; /* Not used */
  marpaESLIFJSONContext.marpaESLIFJSONDecodeOptionp = NULL; /* Not used */
  marpaESLIFJSONContext.marpaESLIFRecognizerOptionp = NULL; /* Not used */
  marpaESLIFJSONContext.marpaESLIFValueOptionp      = marpaESLIFValueOptionp;
  marpaESLIFJSONContext.decimalPointc               = '.'; /* Not used */
  marpaESLIFJSONContext.readerDisposep              = NULL;
  marpaESLIFJSONContext.representationDisposep      = NULL;
  marpaESLIFJSONContext.numbers                     = NULL;
  marpaESLIFJSONContext.numberallocl                = 0;
  marpaESLIFJSONContext.integers                    = NULL;
  marpaESLIFJSONContext.integerallocl               = 0;
  marpaESLIFJSONContext.strictb                     = marpaESLIFGrammarJSONp->jsonStrictb;

  if (MARPAESLIF_UNLIKELY((marpaESLIFGrammarJSONp == NULL) || (marpaESLIFValueResultp == NULL) || (marpaESLIFValueOptionp == NULL))) {
    errno = EINVAL;
    goto err;
  }

  marpaESLIFValueOption                           = *marpaESLIFValueOptionp;
  marpaESLIFValueOption.userDatavp                = &marpaESLIFJSONContext;
  marpaESLIFValueOption.ruleActionResolverp       = NULL; /* Not used */
  marpaESLIFValueOption.symbolActionResolverp     = NULL; /* We use the native ::transfer action */
  marpaESLIFValueOption.importerp                 = ((marpaESLIFValueOptionp != NULL) && (marpaESLIFValueOptionp->importerp != NULL)) ? _marpaESLIFJSONValueResultImportb : NULL;
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
  marpaESLIFValuep->proxyRepresentationp = _marpaESLIFJSONRepresentationb;
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
  if (marpaESLIFJSONContext.numbers != NULL) {
    free(marpaESLIFJSONContext.numbers);
  }
  if (marpaESLIFJSONContext.integers != NULL) {
    free(marpaESLIFJSONContext.integers);
  }
  return rcb;
}

/*****************************************************************************/
static void _marpaESLIFJSONReaderDisposev(void *userDatavp, char *inputcp, size_t inputl, short eofb, short characterStreamb, char *encodings, size_t encodingl)
/*****************************************************************************/
{
  marpaESLIFJSONContext_t      *marpaESLIFJSONContextp      = (marpaESLIFJSONContext_t *) userDatavp;
  marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp = marpaESLIFJSONContextp->marpaESLIFRecognizerOptionp;

  /* Proxy to caller's read disposer */
  if (marpaESLIFJSONContextp->readerDisposep != NULL) {
    marpaESLIFJSONContextp->readerDisposep(marpaESLIFRecognizerOptionp->userDatavp, inputcp, inputl, eofb, characterStreamb, encodings, encodingl);
  }
}

/*****************************************************************************/
static short _marpaESLIFJSONReaderb(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp)
/*****************************************************************************/
{
  short                         rcb;
  marpaESLIFJSONContext_t      *marpaESLIFJSONContextp      = (marpaESLIFJSONContext_t *) userDatavp;
  marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp = marpaESLIFJSONContextp->marpaESLIFRecognizerOptionp;

  /* Proxy to caller's recognizer */
  rcb = marpaESLIFRecognizerOptionp->readerCallbackp(marpaESLIFRecognizerOptionp->userDatavp, inputcpp, inputlp, eofbp, characterStreambp, encodingsp, encodinglp, &(marpaESLIFJSONContextp->readerDisposep));

  *disposeCallbackpp = _marpaESLIFJSONReaderDisposev;

  return rcb;
}

/*****************************************************************************/
static marpaESLIFRecognizerEventCallback_t _marpaESLIFJSONRecognizerEventActionResolverp(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions)
/*****************************************************************************/
{
  marpaESLIFRecognizerEventCallback_t  rcp;

  if (MARPAESLIF_LIKELY(strcmp(actions, "marpaESLIFJsonEventAction") == 0)) {
    rcp = _marpaESLIFJSONRecognizerEventCallbackb;
  } else {
    MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Unrecognized event-action \"%s\"", actions);
    goto err;
  }

  goto done;

 err:
  rcp = NULL;
  
 done:
  return rcp;
}

/*****************************************************************************/
static short _marpaESLIFJSONRecognizerEventCallbackb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFEvent_t *eventArrayp, size_t eventArrayl, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp)
/*****************************************************************************/
{
  size_t i;
  short  rcb;

  for (i = 0; i < eventArrayl; i++) {
    if (eventArrayp[i].events != NULL) {
      if (strcmp(eventArrayp[i].events, "inc[]") == 0) {
        if (MARPAESLIF_UNLIKELY(! _marpaESLIFJSON_incb(userDatavp, marpaESLIFRecognizerp, marpaESLIFValueResultBoolp))) {
          goto err;
        }
      } else if (strcmp(eventArrayp[i].events, "dec[]") == 0) {
        if (MARPAESLIF_UNLIKELY(! _marpaESLIFJSON_decb(userDatavp, marpaESLIFRecognizerp, marpaESLIFValueResultBoolp))) {
          goto err;
        }
      } else {
        MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Unrecognized event \"%s\"", eventArrayp[i].events);
        goto err;
      }
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFJSON_incb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp)
/*****************************************************************************/
{
  marpaESLIFJSONContext_t *marpaESLIFJSONContextp = (marpaESLIFJSONContext_t *) userDatavp;
  size_t                   currentDepthl = marpaESLIFJSONContextp->currentDepthl;

  if (++currentDepthl < marpaESLIFJSONContextp->currentDepthl) {
    MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "size_t turnaround when computing currentDepthl");
    *marpaESLIFValueResultBoolp = MARPAESLIFVALUERESULTBOOL_FALSE;
  } else {
    marpaESLIFJSONContextp->currentDepthl = currentDepthl;
    if (currentDepthl > marpaESLIFJSONContextp->marpaESLIFJSONDecodeOptionp->maxDepthl) {
      MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Maximum depth %ld reached", (unsigned long) marpaESLIFJSONContextp->marpaESLIFJSONDecodeOptionp->maxDepthl);
      errno = EINVAL;
      *marpaESLIFValueResultBoolp = MARPAESLIFVALUERESULTBOOL_FALSE;
    } else {
      *marpaESLIFValueResultBoolp = MARPAESLIFVALUERESULTBOOL_TRUE;
    }
  }

  return 1;
}

/*****************************************************************************/
static short _marpaESLIFJSON_decb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp)
/*****************************************************************************/
{
  marpaESLIFJSONContext_t *marpaESLIFJSONContextp = (marpaESLIFJSONContext_t *) userDatavp;
  size_t                   currentDepthl = marpaESLIFJSONContextp->currentDepthl;

  if (--currentDepthl > marpaESLIFJSONContextp->currentDepthl) {
    MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "size_t turnaround when computing currentDepthl");
    *marpaESLIFValueResultBoolp = MARPAESLIFVALUERESULTBOOL_FALSE;
    errno = EINVAL;
  } else {
    marpaESLIFJSONContextp->currentDepthl = currentDepthl;
    *marpaESLIFValueResultBoolp = MARPAESLIFVALUERESULTBOOL_TRUE;
  }

  return 1;
}

/*****************************************************************************/
static marpaESLIFValueRuleCallback_t _marpaESLIFJSONValueRuleActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  marpaESLIFValueRuleCallback_t  rcp;

  if (strcmp(actions, "members") == 0) {
    rcp = _marpaESLIFJSON_membersb;
  } else if (strcmp(actions, "number") == 0) {
    rcp = _marpaESLIFJSON_numberb;
  } else if (strcmp(actions, "char") == 0) {
    rcp = _marpaESLIFJSON_charb;
  } else if (strcmp(actions, "chars") == 0) {
    rcp = _marpaESLIFJSON_charsb;
  } else if (strcmp(actions, "constant") == 0) {
    rcp = _marpaESLIFJSON_constantb;
  } else if (strcmp(actions, "positive_infinity") == 0) {
    rcp = _marpaESLIFJSON_positive_infinityb;
  } else if (strcmp(actions, "negative_infinity") == 0) {
    rcp = _marpaESLIFJSON_negative_infinityb;
  } else if (strcmp(actions, "positive_nan") == 0) {
    rcp = _marpaESLIFJSON_positive_nanb;
  } else if (strcmp(actions, "negative_nan") == 0) {
    rcp = _marpaESLIFJSON_negative_nanb;
  } else {
    MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Unrecognized action \"%s\"", actions);
    goto err;
  }

  goto done;

 err:
  rcp = NULL;
  
 done:
  return rcp;
}

/*****************************************************************************/
static short _marpaESLIFJSON_membersb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* members  ::= pair* action => members   # Returns { @{pair1}, ..., @{pair2} } */
  marpaESLIFJSONContext_t       *marpaESLIFJSONContextp = (marpaESLIFJSONContext_t *) userDatavp;
  short                          disallowDupkeysb       = marpaESLIFJSONContextp->marpaESLIFJSONDecodeOptionp->disallowDupkeysb;
  marpaESLIFValueResult_t        marpaESLIFValueResult;
  marpaESLIFValueResult_t       *marpaESLIFValueResultp;
  int                            i;
  int                            j;
  int                            currentTableIndicei;
  short                         *origshallowbp[2];
  short                          rcb;
  marpaESLIFValueResultString_t *previousKeyp;
  marpaESLIFValueResultString_t *currentKeyp;
  marpaESLIF_string_t           *stringp;

  marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_TABLE;
  marpaESLIFValueResult.contextp           = NULL;
  marpaESLIFValueResult.representationp    = NULL;
  marpaESLIFValueResult.u.t.p              = NULL;
  marpaESLIFValueResult.u.t.freeUserDatavp = marpaESLIFValuep->marpaESLIFRecognizerp;
  marpaESLIFValueResult.u.t.freeCallbackp  = _marpaESLIF_generic_freeCallbackv;
  marpaESLIFValueResult.u.t.shallowb       = 0;
  marpaESLIFValueResult.u.t.sizel          = 0;

  /* We receive a list of rows that we want to flatten */
  if ((! nullableb) && (argni >= arg0i)) {
    marpaESLIFValueResult.u.t.sizel = argni - arg0i + 1;
    marpaESLIFValueResult.u.t.p = (marpaESLIFValueResultPair_t *) malloc(marpaESLIFValueResult.u.t.sizel * sizeof(marpaESLIFValueResultPair_t));
    if (MARPAESLIF_UNLIKELY(marpaESLIFValueResult.u.t.p == NULL)) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }

    for (i = arg0i, currentTableIndicei = 0; i<= argni; i++, currentTableIndicei++) {
      marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, i);
      if (MARPAESLIF_UNLIKELY(marpaESLIFValueResultp == NULL)) {
        goto err;
      }

      /* No need to check that - this is ok per definition */
      /*
      if (MARPAESLIF_UNLIKELY(marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_ROW)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "marpaESLIFValueResultp->type is not ROW (got %d, %s)", marpaESLIFValueResultp->type, _marpaESLIF_value_types(marpaESLIFValueResultp->type));
        goto err;
      }

      if (MARPAESLIF_UNLIKELY(marpaESLIFValueResultp->u.r.sizel != 2)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "row size is %ld and not 2", (unsigned long) marpaESLIFValueResultp->u.r.sizel);
        goto err;
      }
      */

      /* We have to take care of members's shallow status: the table becomes the owner in any case */
      for (j = 0; j < 2; j++) {
        origshallowbp[j] = NULL;
        switch (marpaESLIFValueResultp->u.r.p[j].type) {
        case MARPAESLIF_VALUE_TYPE_PTR:
          origshallowbp[j] = &(marpaESLIFValueResultp->u.r.p[j].u.p.shallowb);
          break;
        case MARPAESLIF_VALUE_TYPE_ARRAY:
          origshallowbp[j] = &(marpaESLIFValueResultp->u.r.p[j].u.a.shallowb);
          break;
        case MARPAESLIF_VALUE_TYPE_STRING:
          origshallowbp[j] = &(marpaESLIFValueResultp->u.r.p[j].u.s.shallowb);
          break;
        case MARPAESLIF_VALUE_TYPE_ROW:
          origshallowbp[j] = &(marpaESLIFValueResultp->u.r.p[j].u.t.shallowb);
          break;
        case MARPAESLIF_VALUE_TYPE_TABLE:
          origshallowbp[j] = &(marpaESLIFValueResultp->u.r.p[j].u.t.shallowb);
          break;
        default:
          break;
        }
      }

      marpaESLIFValueResult.u.t.p[currentTableIndicei].key   = marpaESLIFValueResultp->u.r.p[0];
      marpaESLIFValueResult.u.t.p[currentTableIndicei].value = marpaESLIFValueResultp->u.r.p[1];

      if (origshallowbp[0] != NULL) {
        *(origshallowbp[0]) = 1;
      }

      if (origshallowbp[1] != NULL) {
        *(origshallowbp[1]) = 1;
      }

      if (disallowDupkeysb && (currentTableIndicei > 0)) {
        /* By definition keys are always strings coming from parsing, i.e. STRING type with encoding UTF-8 */
        currentKeyp = &(marpaESLIFValueResult.u.t.p[currentTableIndicei].key.u.s);
        for (j = 0; j < currentTableIndicei; j++) {
          previousKeyp = &(marpaESLIFValueResult.u.t.p[j].key.u.s);
          if (((previousKeyp->p == NULL) && (currentKeyp->p == NULL))
              ||
              ((previousKeyp->p != NULL) && (currentKeyp->p != NULL)
               &&
               (previousKeyp->sizel == currentKeyp->sizel)
               &&
               (
                (previousKeyp->sizel == 0)
                ||
                (memcmp((char *) previousKeyp->p, (char *) currentKeyp->p, previousKeyp->sizel) == 0)
                )
               )
              ) {
            stringp = _marpaESLIF_string_newp(marpaESLIFValuep->marpaESLIFp, previousKeyp->encodingasciis, (char *) previousKeyp->p, previousKeyp->sizel);
            if (stringp == NULL) {
              /* Say there is a duplicate key anway */
              MARPAESLIF_ERROR(marpaESLIFValuep->marpaESLIFp, "Duplicate key");
            } else {
              MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Duplicate key %s", stringp->asciis);
              _marpaESLIF_string_freev(stringp, 0 /* onStstackb */);
            }
            goto err;
          }
        }
      }
    }
  }

  if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  if (marpaESLIFValueResult.u.t.p != NULL) {
    free(marpaESLIFValueResult.u.t.p);
  }
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFJSON_numberb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  static const char                  *funcs = "_marpaESLIFJSON_numberb";
  marpaESLIFJSONContext_t            *marpaESLIFJSONContextp = (marpaESLIFJSONContext_t *) userDatavp;
  char                               *tmps;
  char                               *endptrendp;
  char                               *p;
  char                               *q;
  char                               *pmin;
  char                               *pmax;
  char                                dotc;
  char                                exponentc;
  char                               *exponentp;
  long                                exponentl;
  char                               *dotp;
  size_t                              numberOfUnsignificantDigitl;
  char                               *endptrp;
  char                               *numbers;
  size_t                              numberl;
  size_t                              decimall;
  short                               isFloatb;
  short                               isNegb;
  size_t                              charsl;
  size_t                              prevCharsl;
  size_t                              l;
  size_t                              shiftl;
  char                               *arrayp;
  size_t                              arrayl;
#if defined(MARPAESLIF_HAVE_LONG_LONG) && defined(C_STRTOLL)
  long long                           valuell;
#else
  long                                valuel;
#endif
#if (defined(C_STRTOLD) && defined(MARPAESLIF_HUGE_VALL)) || (defined(C_STRTOD) && defined(MARPAESLIF_HUGE_VAL))
  short                               decimalPointb;
  char                               *decimalPoints;
#  if defined(C_STRTOLD) && defined(MARPAESLIF_HUGE_VALL)
  long double                         valueld;
#  else
  double                              valued;
#  endif
#endif
  marpaESLIFValueResult_t             marpaESLIFValueResult;
  marpaESLIFValueResult_t            *marpaESLIFValueResultInputp;
  short                               rcb;

  /* Input is of type array by definition, UTF-8 encoded */
  marpaESLIFValueResultInputp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, arg0i);
  if (MARPAESLIF_UNLIKELY(marpaESLIFValueResultInputp == NULL)) {
    goto err;
  }

  /* We do not really mind about the '+' eventual sign. Only the '-' sign triggers */
  /* a special processing, that may fall into a true floating pointer number to    */
  /* preserve the signedness of zero.                                              */
  arrayp = marpaESLIFValueResultInputp->u.a.p;
  arrayl = marpaESLIFValueResultInputp->u.a.sizel;

  if (arrayp[0] == '+') {
    MARPAESLIF_TRACE(marpaESLIFValuep->marpaESLIFp, funcs, "Removing leading '+' sign");
    ++arrayp;
    --arrayl;
  }

  /* We want to know if this can be expressed as a non-floating pointer number */
  if (marpaESLIFJSONContextp->numbers == NULL) {
    marpaESLIFJSONContextp->numbers = (char *) malloc(arrayl + 1); /* + 1 for the NUL byte */
    if (marpaESLIFJSONContextp->numbers == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
    }
    numbers = marpaESLIFJSONContextp->numbers;
    marpaESLIFJSONContextp->numberallocl = arrayl;
  } else if (marpaESLIFJSONContextp->numberallocl < arrayl) {
    tmps = (char *) realloc(marpaESLIFJSONContextp->numbers, arrayl + 1); /* + 1 for the NUL byte */
    if (tmps == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "realloc failure, %s", strerror(errno));
        goto err;
    }
    numbers = marpaESLIFJSONContextp->numbers = tmps;
    marpaESLIFJSONContextp->numberallocl = arrayl;
  } else {
    numbers = marpaESLIFJSONContextp->numbers;
  }

  numberl = arrayl;
  memcpy(numbers, arrayp, numberl);
  numbers[numberl] = '\0';

  /* From now on the work area is numbers, with numberl ASCII characters, ending with a '\0' at indice numberl */
  endptrendp = numbers + numberl;

  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: %ld bytes", numbers, (unsigned long) numberl);

  /* Look for the eventual exponent */
  exponentp = NULL;
  for (p = endptrendp - 1; p >= numbers; p--) {
    exponentc = *p;
    if ((exponentc == 'e') || (exponentc == 'E')) {
      exponentp = p;
      break;
    }
  }

  /* Look for the eventual dot */
  dotp = NULL;
  for (p = numbers; p < endptrendp; p++) {
    dotc = *p;
    if (dotc == '.') {
      dotp = p;
      break;
    }
  }

  /* Remove non significant digits on the left, not possible with the strict grammar */
  if (! marpaESLIFJSONContextp->strictb) {

    /* Locate where the scanning will start */
    pmin = numbers;
    if (*pmin == '-') {
      pmin++;
    }

    /* Locate where the scanning will stop */
    if (dotp != NULL) {
      /* Dot character is present */
      pmax = dotp;
    } else if (exponentp != NULL) {
      /* No dot character but there is the exponent character */
      pmax = exponentp;
    } else {
      /* The string is made only with digits */
      pmax = endptrendp;
    }

    numberOfUnsignificantDigitl = 0;
    for (p = pmin; p < pmax; p++) {
      if (*p != '0') {
        break;
      }
      numberOfUnsignificantDigitl++;
    }

    if ((numberOfUnsignificantDigitl > 0) && (p == pmax)) {
      /* We want to retain at least one digit before the dot, e.g. we do not want to remove everything */

      /* If we match:  */
      /* 000000.456000 */
      /* ^pmin         */
      /*       ^pmax   */
      /*       ^p      */
      /*               */
      /* we change to: */
      /* 000000.456000 */
      /* ^pmin         */
      /*       ^pmax   */
      /*      ^p       */
      if (--numberOfUnsignificantDigitl > 0) {
        --p;
      }
    }

    if (numberOfUnsignificantDigitl > 0) {
      /* Note that is guaranteed that p < pmax */

      /* 000123.456000 */
      /* ^pmin         */
      /*       ^pmax   */
      /*    ^p         */

      MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: Removing %ld non significant left digits", numbers, (unsigned long) numberOfUnsignificantDigitl);
      memmove(pmin, p, endptrendp - p + 1); /* + 1 for the NUL byte */

      /* Impact of the memmove() */
      numberl -= numberOfUnsignificantDigitl;
      endptrendp -= numberOfUnsignificantDigitl;
      if (dotp != NULL) {
        dotp -= numberOfUnsignificantDigitl;
      }
      if (exponentp != NULL) {
        exponentp -= numberOfUnsignificantDigitl;
      }

      MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: Now %ld bytes", numbers, (unsigned long) numberl);
    }
  }

  /* Remove non significant digits after the dot character. */
  if (dotp != NULL) {
    pmin = dotp + 1;
    if (exponentp != NULL) {
      pmax = exponentp;
    } else {
      pmax = endptrendp;
    }

    numberOfUnsignificantDigitl = 0;
    /* We voluntarily say p > pmin so that */
    /* we always retain at least one digit */
    /* after the dot.                      */
    for (p = pmax - 1; p > pmin; p--) {
      if (*p != '0') {
        break;
      }
      numberOfUnsignificantDigitl++;
    }

    if ((numberOfUnsignificantDigitl > 0) || ((p == pmin) && (*p == '0'))) {
      /* It is guaranteed that numberOfUnsignificantDigitl is < total number of digits after the dot. */

      if ((p == pmin) && (*p == '0')) {
        /* Special case of (p == pmin) && (*p == '0'), then it means that it something like e.g.; */
        /* 123.000000      */
        /*    ^dotp        */
        /*     ^pmin       */
        /*           ^pmax */
        /*     ^p          */
        numberOfUnsignificantDigitl += 2;
        MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: Removing the dot part", numbers, (unsigned long) numberOfUnsignificantDigitl);
        memmove(dotp, pmax, endptrendp - pmax + 1); /* + 1 for the NUL byte */
        dotp = NULL;
      } else {
        /* 123.456000      */
        /*     ^pmin       */
        /*           ^pmax */
        /*       ^p        */
        MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: Removing %ld non significant right digits", numbers, (unsigned long) numberOfUnsignificantDigitl);
        ++p;
        memmove(p, pmax, endptrendp - pmax + 1); /* + 1 for the NUL byte */
      }

      /* Impact of the memmove() */
      numberl -= numberOfUnsignificantDigitl;
      endptrendp -= numberOfUnsignificantDigitl;
      if (exponentp != NULL) {
        exponentp -= numberOfUnsignificantDigitl;
      }

      MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: Now %ld bytes", numbers, (unsigned long) numberl);
    }
  }

  /* We now have a number with no unsignificant digit. We want to know if this is a true floating point number. */

  /* If there is an exponent, take its value - we assume that using a long is fair enough. */
  if (exponentp == NULL) {
    exponentl = 0;
  } else {
    endptrp = NULL;
    errno = 0;    /* To distinguish success/failure after call */
    exponentl = strtol(exponentp + 1, &endptrp, 10);
    /* Note that the exponent in a JSON number always have at least one digit */
    if ((endptrp != endptrendp) || (errno != 0)) {
      MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: Exponent parsing failure", numbers, errno != 0 ? strerror(errno) : "bad final pointer");
      goto proposal;
    }
  }

  if (dotp == NULL) {
    decimall = 0;
  } else {
    pmin = dotp + 1;
    if (exponentp != NULL) {
      pmax = exponentp;
    } else {
      pmax = endptrendp;
    }
    decimall = pmax - pmin;
  }

  /* Check the eventual signedness */
  isNegb = (numbers[0] == '-') ? 1 : 0;

  /* Count the total number of digits needed to represent this non-floating number.                              */
  /* Take care: decimall is unsigned; exponentl is signed and will be converted to unsigned if we do comparison. */
  /* The value of charsl is guaranteed to be set only if isFloatb == 0.                                          */
  if (decimall == 0) {
    if (exponentl < 0) {
      isFloatb = 1;
    } else {
      isFloatb = 0;
      if (exponentp != NULL) {
        /* [-]123E[+]789 */
        charsl = exponentp - numbers;
        prevCharsl = charsl;
        charsl += exponentl;
        if (MARPAESLIF_UNLIKELY(charsl < prevCharsl)) { /* Turnaround */
          goto proposal;
        }
      } else {
        /* [-]123 */
        charsl = numberl;
      }
    }
  } else {
    /* Per def decimall here is > 0 */
    if (exponentl <= 0) {
      isFloatb = 1;
    } else {
      /* decimall is > 0, exponentl is > 0 and unsigned automatic conversion will not change its value */
      /* I may change to a temporary variable of another type or use compiler's #pragma because        */
      /* sometimes there is a warning.                                                                 */
      if (exponentl < decimall) {
        isFloatb = 1;
      } else {
        /* [-]123.456E[+]789 */
        isFloatb = 0;
        charsl = dotp - numbers;
        prevCharsl = charsl;
        charsl += exponentl;
        if (MARPAESLIF_UNLIKELY(charsl < prevCharsl)) { /* Turnaround */
          goto proposal;
        }
      }
    }
  }

  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: %ld decimals, exponent value is %ld => %sa true floating point number", numbers, (unsigned long) decimall, (long) exponentl, isFloatb ? "" : "not ");

  if (isFloatb) {
    /* A floating point number always trigger the proposal */
    goto proposal;
  }

  /* We have a special case in our algorithm: the representation -0 or -0Exx where xx >= 0  */
  /* Since the sign of zero can only be handled by a floating point number, and since zero  */
  /* is always exactly represented by the later, this special case is moved to the proposal */
  /* where we use floating pointer.                                                         */
  if ((numbers[0] == '-') && (numbers[1] == '0')) {
    /* It is a signed zero. This test is enough because we removed all non significant digits on */
    /* the left side, keeping at most one digit. If this digit is '0' we are done.               */
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: signed zero detected, forcing true floating point number", numbers);
    marpaESLIFValueResult.contextp        = NULL;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_DOUBLE;
    marpaESLIFValueResult.u.d             = -0.;
    goto fast_proposal;
  }

  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: %ld characters are needed to completely represent this non-floating pointer number", numbers, (unsigned long) charsl);

  /* Is it too long for the largest non-floating pointer integer that we have */
  if (isNegb) {
#ifdef MARPAESLIF_HAVE_LONG_LONG
    if (charsl > marpaESLIFValuep->marpaESLIFp->llongmincharsl) {
      MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: charsl is %ld > %ld (LLONG_MIN) : go to proposal", numbers, (unsigned long) charsl, (unsigned long) marpaESLIFValuep->marpaESLIFp->llongmincharsl);
      goto proposal;
    }
#else
    if (charsl > marpaESLIFValuep->marpaESLIFp->longmincharsl) {
      MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: charsl is %ld > %ld (LONG_MIN) : go to proposal", numbers, (unsigned long) charsl, (unsigned long) marpaESLIFValuep->marpaESLIFp->longmincharsl);
      goto proposal;
    }
#endif
  } else {
#ifdef MARPAESLIF_HAVE_LONG_LONG
    if (charsl > marpaESLIFValuep->marpaESLIFp->llongmaxcharsl) {
      MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: charsl is %ld > %ld (LLONG_MAX) : go to proposal", numbers, (unsigned long) charsl, (unsigned long) marpaESLIFValuep->marpaESLIFp->llongmaxcharsl);
      goto proposal;
    }
#else
    if (charsl > marpaESLIFValuep->marpaESLIFp->longmaxcharsl) {
      MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: charsl is %ld > %ld (LONG_MAX) : go to proposal", numbers, (unsigned long) charsl, (unsigned long) marpaESLIFValuep->marpaESLIFp->longmaxcharsl);
      goto proposal
    }
#endif
  }

  /* No need to check on turnaround on charsl here: no system ever set a number of supported digits to such max size_t value */  
  if (marpaESLIFJSONContextp->integers == NULL) {
    marpaESLIFJSONContextp->integers = (char *) malloc(charsl + 1);
    if (marpaESLIFJSONContextp->integers == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    marpaESLIFJSONContextp->integerallocl = charsl;
  } else if (marpaESLIFJSONContextp->integerallocl < charsl) {
    tmps = (char *) realloc(marpaESLIFJSONContextp->integers, charsl + 1);
    if (tmps == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "realloc failure, %s", strerror(errno));
      goto err;
    }
    marpaESLIFJSONContextp->integers = tmps;
    marpaESLIFJSONContextp->integerallocl = charsl;
  }

  /* The only possibities are: */
  /* [-]123                    */
  /* [-]123E[+]789             */
  /* [-]123.456E[+]789         */
  p = marpaESLIFJSONContextp->integers;
  if (dotp == NULL) {
    if (exponentp == NULL) {
      /* [-]123                */
      memcpy(p, numbers, numberl);
    } else {
      /* [-]123E[+]789        */
      l = exponentp - numbers;
      memcpy(p, numbers, l);
      q = p + l;
      /* exponentl is positive by definition here.                                                     */
      /* I may change to a temporary variable of another type or use compiler's #pragma because        */
      /* sometimes there is a warning.                                                                 */
      for (l = 0; l < exponentl; l++, q++) {
        *q = '0';
      }
    }
  } else {
    /* By definition decimall and exponentl are positive */
    /* [-]123.456E[+]789      */
    l = dotp - numbers;
    memcpy(p, numbers, l);
    q = p + l;

    l = exponentp - dotp - 1;
    memcpy(q, dotp + 1, l);
    q += l;

    shiftl = exponentl - decimall;
    for (l = 0; l < shiftl; l++, q++) {
      *q = '0';
    }
  }

  marpaESLIFJSONContextp->integers[charsl] = '\0';
  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: Transformed for parsing to %s", numbers, marpaESLIFJSONContextp->integers);
  endptrendp = marpaESLIFJSONContextp->integers + charsl;

#if defined(MARPAESLIF_HAVE_LONG_LONG) && defined(C_STRTOLL)
  endptrp = NULL;
  errno = 0;    /* To distinguish success/failure after call */
  valuell = C_STRTOLL(marpaESLIFJSONContextp->integers, &endptrp, 10);
  /* Note that the exponent in a JSON number always have at least one digit */
  if ((endptrp != endptrendp) || (errno != 0)) {
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: %s parsing failure, %s", numbers, marpaESLIFJSONContextp->integers, errno != 0 ? strerror(errno) : "bad final pointer");
    goto proposal;
  }
  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: %s parsing success", numbers, marpaESLIFJSONContextp->integers);
  /* Can we promote it to a less higher thingy ? */
  if ((SHRT_MIN <= valuell) && (valuell <= SHRT_MAX)) {
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: %d fits in a SHORT", numbers, (int) valuell);
    marpaESLIFValueResult.contextp        = NULL;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_SHORT;
    marpaESLIFValueResult.u.b             = (short) valuell;
  } else if ((INT_MIN <= valuell) && (valuell <= INT_MAX)) {
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: %d fits in an INT", numbers, (int) valuell);
    marpaESLIFValueResult.contextp        = NULL;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_INT;
    marpaESLIFValueResult.u.i             = (int) valuell;
  } else if ((LONG_MIN <= valuell) && (valuell <= LONG_MAX)) {
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: %ld fits in a LONG", numbers, (long) valuell);
    marpaESLIFValueResult.contextp        = NULL;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_LONG;
    marpaESLIFValueResult.u.l             = (long) valuell;
  } else {
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: " MARPAESLIF_LONG_LONG_FMT " remains a LONG LONG", numbers, valuell);
    marpaESLIFValueResult.contextp        = NULL;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_LONG_LONG;
    marpaESLIFValueResult.u.ll            = valuell;
  }
#else
  endptrp = NULL;
  errno = 0;    /* To distinguish success/failure after call */
  valuel = strtol(marpaESLIFJSONContextp->integers, &endptrp, 10);
  /* Note that the exponent in a JSON number always have at least one digit */
  if ((endptrp != endptrendp) || (errno != 0)) {
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: %s parsing failure, %s", numbers, marpaESLIFJSONContextp->integers, errno != 0 ? strerror(errno) : "bad final pointer");
    goto proposal;
  }
  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: %s parsing success", numbers, marpaESLIFJSONContextp->integers);
  /* Can we promote it to a less higher thingy ? */
  if ((SHRT_MIN <= valuel) && (valuel <= SHRT_MAX)) {
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: %d fits in a SHORT", numbers, (int) valuel);
    marpaESLIFValueResult.contextp        = NULL;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_SHORT;
    marpaESLIFValueResult.u.b             = (short) valuel;
  } else if ((INT_MIN <= valuel) && (valuel <= INT_MAX)) {
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: %d fits in an INT", numbers, (int) valuel);
    marpaESLIFValueResult.contextp        = NULL;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_INT;
    marpaESLIFValueResult.u.i             = (int) valuel;
  } else if ((LONG_MIN <= valuel) && (valuel <= LONG_MAX)) {
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: %ld remains a LONG", numbers, valuel);
    marpaESLIFValueResult.contextp        = NULL;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_LONG;
    marpaESLIFValueResult.u.l             = valuel;
  }
#endif
  goto fast_proposal;

 proposal:
  /* In the proposal we go back to the original string, as if nothing has happened. Only the eventual leading '+' */
  /* remains removed. It is never needed.                                                                         */
#if defined(C_STRTOLD) && defined(MARPAESLIF_HUGE_VALL)
  endptrendp = numbers + numberl;
  /* Do we have to change the decimal point representation ? */
  decimalPoints = strchr(numbers, '.');
  decimalPointb = ((decimalPoints != NULL) && (*decimalPoints != marpaESLIFJSONContextp->decimalPointc)) ? 1 : 0;
  if (decimalPointb) {
    *decimalPoints = marpaESLIFJSONContextp->decimalPointc;
  }

  endptrp = NULL;
  errno = 0;    /* To distinguish success/failure after call */
  valueld = C_STRTOLD(numbers, &endptrp);
  if (! ((endptrp != endptrendp) /* Parsing error */
         ||
         ((errno == ERANGE) && ((valueld == MARPAESLIF_HUGE_VALL) || (valueld == -MARPAESLIF_HUGE_VALL))) /* Overflow */
         ||
         ((valueld == 0.) && (errno != 0)) /* Underflow */
         )) {
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: long double parsing success", numbers);
    marpaESLIFValueResult.contextp        = NULL;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_LONG_DOUBLE;
    marpaESLIFValueResult.u.ld            = valueld;
  } else {
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: long double parsing failure, %s", numbers, errno != 0 ? strerror(errno) : "bad final pointer");
    marpaESLIFValueResult.contextp        = NULL;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_UNDEF;
  }
  if (decimalPointb) {
    *decimalPoints = '.';
  }
#else /* C_STRTOLD && MARPAESLIF_HUGE_VALL */
#  if defined(C_STRTOD) && defined(MARPAESLIF_HUGE_VAL)
  endptrendp = numbers + numberl;
  /* Do we have to change the decimal point representation ? */
  decimalPoints = strchr(numbers, '.');
  decimalPointb = ((decimalPoints != NULL) && (*decimalPoints != marpaESLIFJSONContextp->decimalPointc)) ? 1 : 0;
  if (decimalPointb) {
    *decimalPoints = marpaESLIFJSONContextp->decimalPointc;
  }

  endptrp = NULL;
  errno = 0;    /* To distinguish success/failure after call */
  valued = C_STRTOD(numbers, &endptrp);
  if (! ((endptrp != endptrendp) /* Parsing error */
         ||
         ((errno == ERANGE) && ((valued == MARPAESLIF_HUGE_VAL) || (valued == -MARPAESLIF_HUGE_VAL))) /* Overflow */
         ||
         ((valued == 0.) && (errno != 0)) /* Underflow */
         )) {
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: double parsing success", numbers);
    marpaESLIFValueResult.contextp        = NULL;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_DOUBLE;
    marpaESLIFValueResult.u.d             = valued;
  } else {
    MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: double parsing failure, %s", numbers, errno != 0 ? strerror(errno) : "bad final pointer");
    marpaESLIFValueResult.contextp        = NULL;
    marpaESLIFValueResult.representationp = NULL;
    marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_UNDEF;
  }
  if (decimalPointb) {
    *decimalPoints = '.';
  }
#  else /* C_STRTOD && MARPAESLIF_HUGE_VAL */
  MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "%s: No lib call available for parsing", numbers);
  marpaESLIFValueResult.contextp        = NULL;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_UNDEF;
#  endif  /* C_STRTOD && MARPAESLIF_HUGE_VAL */
#endif /* C_STRTOLD && MARPAESLIF_HUGE_VALL */

  if (MARPAESLIF_UNLIKELY(! _marpaESLIFJSON_proposalb(userDatavp,
                                                      marpaESLIFValuep,
                                                      marpaESLIFJSONContextp->marpaESLIFJSONDecodeOptionp->numberActionp,
                                                      numbers,
                                                      numberl,
                                                      &marpaESLIFValueResult))) {
    goto err;
  }

 fast_proposal:
  rcb = _marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult);
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFJSON_charb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  static const char *funcs       = "_marpaESLIFJSON_charb";
  /* char    ::= /(?:[^\"\\\\\\x00-\\x1F]+)|(?:\\\\[\"\\\\\\/bfnrt])/ */

  /* It is guaranteed to be an ARRAY with size > 0.                   */
  /* Cases are:                                                       */
  /* - First character is     '\': this is an escaped character       */
  /* - First character is not '\': the whole match is ASCII chars     */
  marpaESLIF_uint32_t     *uint32p = NULL;
  size_t                   uint32l;
  unsigned char           *dstp = NULL;
  size_t                   dstl;
  marpaESLIFJSONContext_t *marpaESLIFJSONContextp = (marpaESLIFJSONContext_t *) userDatavp;
  marpaESLIFValueResult_t *marpaESLIFValueResultp;
  marpaESLIFValueResult_t  marpaESLIFValueResult;
  marpaESLIF_uint32_t      c;
  char                    *p;
  unsigned char           *q;
  size_t                   i;
  size_t                   j;
  short                    rcb;

  marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, arg0i);
  if (MARPAESLIF_UNLIKELY(marpaESLIFValueResultp == NULL)) {
    goto err;
  }

  /* We re-process marpaESLIFValueResultp only when this is an escaped character */
  if (marpaESLIFValueResultp->u.a.p[0] == '\\') {
    switch (marpaESLIFValueResultp->u.a.p[1]) {
    case '"':
      marpaESLIFValueResult.contextp           = NULL;
      marpaESLIFValueResult.representationp    = NULL;
      marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_ARRAY;
      marpaESLIFValueResult.u.a.freeUserDatavp = NULL;
      marpaESLIFValueResult.u.a.freeCallbackp  = NULL;
      marpaESLIFValueResult.u.a.shallowb       = 1;
      marpaESLIFValueResult.u.a.sizel          = 1;
      marpaESLIFValueResult.u.a.p              = (char *) MARPAESLIFJSON_DQUOTE;
      break;
    case '\\':
      marpaESLIFValueResult.contextp           = NULL;
      marpaESLIFValueResult.representationp    = NULL;
      marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_ARRAY;
      marpaESLIFValueResult.u.a.freeUserDatavp = NULL;
      marpaESLIFValueResult.u.a.freeCallbackp  = NULL;
      marpaESLIFValueResult.u.a.shallowb       = 1;
      marpaESLIFValueResult.u.a.sizel          = 1;
      marpaESLIFValueResult.u.a.p              = (char *) MARPAESLIFJSON_BACKSLASH;
      break;
    case '/':
      marpaESLIFValueResult.contextp           = NULL;
      marpaESLIFValueResult.representationp    = NULL;
      marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_ARRAY;
      marpaESLIFValueResult.u.a.freeUserDatavp = NULL;
      marpaESLIFValueResult.u.a.freeCallbackp  = NULL;
      marpaESLIFValueResult.u.a.shallowb       = 1;
      marpaESLIFValueResult.u.a.sizel          = 1;
      marpaESLIFValueResult.u.a.p              = (char *) MARPAESLIFJSON_SLASH;
      break;
    case 'b':
      marpaESLIFValueResult.contextp           = NULL;
      marpaESLIFValueResult.representationp    = NULL;
      marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_ARRAY;
      marpaESLIFValueResult.u.a.freeUserDatavp = NULL;
      marpaESLIFValueResult.u.a.freeCallbackp  = NULL;
      marpaESLIFValueResult.u.a.shallowb       = 1;
      marpaESLIFValueResult.u.a.sizel          = 1;
      marpaESLIFValueResult.u.a.p              = (char *) MARPAESLIFJSON_BACKSPACE;
      break;
    case 'f':
      marpaESLIFValueResult.contextp           = NULL;
      marpaESLIFValueResult.representationp    = NULL;
      marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_ARRAY;
      marpaESLIFValueResult.u.a.freeUserDatavp = NULL;
      marpaESLIFValueResult.u.a.freeCallbackp  = NULL;
      marpaESLIFValueResult.u.a.shallowb       = 1;
      marpaESLIFValueResult.u.a.sizel          = 1;
      marpaESLIFValueResult.u.a.p              = (char *) MARPAESLIFJSON_FORMFEED;
      break;
    case 'r':
      marpaESLIFValueResult.contextp           = NULL;
      marpaESLIFValueResult.representationp    = NULL;
      marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_ARRAY;
      marpaESLIFValueResult.u.a.freeUserDatavp = NULL;
      marpaESLIFValueResult.u.a.freeCallbackp  = NULL;
      marpaESLIFValueResult.u.a.shallowb       = 1;
      marpaESLIFValueResult.u.a.sizel          = 1;
      marpaESLIFValueResult.u.a.p              = (char *) MARPAESLIFJSON_LINEFEED;
      break;
    case 'n':
      marpaESLIFValueResult.contextp           = NULL;
      marpaESLIFValueResult.representationp    = NULL;
      marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_ARRAY;
      marpaESLIFValueResult.u.a.freeUserDatavp = NULL;
      marpaESLIFValueResult.u.a.freeCallbackp  = NULL;
      marpaESLIFValueResult.u.a.shallowb       = 1;
      marpaESLIFValueResult.u.a.sizel          = 1;
      marpaESLIFValueResult.u.a.p              = (char *) MARPAESLIFJSON_CARRIAGE_RETURN;
      break;
    case 't':
      marpaESLIFValueResult.contextp           = NULL;
      marpaESLIFValueResult.representationp    = NULL;
      marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_ARRAY;
      marpaESLIFValueResult.u.a.freeUserDatavp = NULL;
      marpaESLIFValueResult.u.a.freeCallbackp  = NULL;
      marpaESLIFValueResult.u.a.shallowb       = 1;
      marpaESLIFValueResult.u.a.sizel          = 1;
      marpaESLIFValueResult.u.a.p              = (char *) MARPAESLIFJSON_HORIZONTAL_TAB;
      break;
    default:
      /* It is a sequence of '\uXXXX' by definition, i.e. 6 bytes - so there are (marpaESLIFValueResultp->u.a.sizel / 6) hex digits */
      uint32l = marpaESLIFValueResultp->u.a.sizel / 6;
      uint32p = (marpaESLIF_uint32_t *) malloc(sizeof(marpaESLIF_uint32_t) * uint32l);
      if (MARPAESLIF_UNLIKELY(uint32p == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }

      for (i = 0, p = marpaESLIFValueResultp->u.a.p + 2; i < uint32l; i++, p += 2) {
        c = 0;

        MARPAESLIFJSON_DST_OR_VALCHAR(c, *p++);
        c <<= 4;
        MARPAESLIFJSON_DST_OR_VALCHAR(c, *p++);
        c <<= 4;
        MARPAESLIFJSON_DST_OR_VALCHAR(c, *p++);
        c <<= 4;
        MARPAESLIFJSON_DST_OR_VALCHAR(c, *p++);

        uint32p[i] = c;
      }

      /* Worst case is four UTF-8 bytes */
      dstl = uint32l * 4;
      dstp = (unsigned char *) malloc(sizeof(unsigned char) * dstl);
      if (MARPAESLIF_UNLIKELY(dstp == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }

      /* Based on efi_utf16_to_utf8 from Linux kernel */
      q = dstp;
      for (i = 0, j = 1; i < uint32l; i++, j++) {
        c = uint32p[i];

        if ((j < uint32l) && (c >= 0xD800) && (c <= 0xDBFF) && (uint32p[j] >= 0xDC00) && (uint32p[j] <= 0xDFFF)) {
          /* Surrogate UTF-16 pair */
          c = 0x10000 + ((c & 0x3FF) << 10) + (uint32p[j] & 0x3FF);
          ++i;
          ++j;
        }

        if ((c >= 0xD800) && (c <= 0xDFFFF)) {
          if (marpaESLIFJSONContextp->marpaESLIFJSONDecodeOptionp->noReplacementCharacterb) {
            MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Invalid UTF-16 character \\%c%c%c%c%c", marpaESLIFValueResultp->u.a.p[(i * 6) + 1], marpaESLIFValueResultp->u.a.p[(i * 6) + 2], marpaESLIFValueResultp->u.a.p[(i * 6) + 3], marpaESLIFValueResultp->u.a.p[(i * 6) + 4], marpaESLIFValueResultp->u.a.p[(i * 6) + 5]);
            goto err;
          } else {
            MARPAESLIF_TRACEF(marpaESLIFValuep->marpaESLIFp, funcs, "Invalid UTF-16 character \\%c%c%c%c%c replaced by 0xFFFD", marpaESLIFValueResultp->u.a.p[(i * 6) + 1], marpaESLIFValueResultp->u.a.p[(i * 6) + 2], marpaESLIFValueResultp->u.a.p[(i * 6) + 3], marpaESLIFValueResultp->u.a.p[(i * 6) + 4], marpaESLIFValueResultp->u.a.p[(i * 6) + 5]);
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

      marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_ARRAY;
      marpaESLIFValueResult.contextp           = NULL;
      marpaESLIFValueResult.representationp    = NULL;
      marpaESLIFValueResult.u.a.p              = (char *) dstp;
      marpaESLIFValueResult.u.a.sizel          = q - dstp;
      marpaESLIFValueResult.u.s.freeUserDatavp = marpaESLIFValuep->marpaESLIFRecognizerp;
      marpaESLIFValueResult.u.s.freeCallbackp  = _marpaESLIF_generic_freeCallbackv;
      marpaESLIFValueResult.u.s.shallowb       = 0;
      break;
    }

    marpaESLIFValueResultp = &marpaESLIFValueResult;
  }

  if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, marpaESLIFValueResultp))) {
    goto err;
  }

  /* dstp, if allocated, is now managed by ESLIF */
  dstp = NULL;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (uint32p != NULL) {
    free(uint32p);
  }
  if (dstp != NULL) {
    free(dstp);
  }
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFJSON_charsb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  /* chars   ::= char* */
  /* We own entirely all chars, therefore we know that it is valid UTF-8: we do not need ::concat[UTF-8], but */
  /* just to concatenate all chars. */
  unsigned char           *p = NULL;
  size_t                   sizel;
  short                    shallowb;
  int                      i;
  marpaESLIFValueResult_t  marpaESLIFValueResult;
  marpaESLIFValueResult_t *marpaESLIFValueResultp;
  unsigned char           *q;
  short                    rcb;

  if (nullableb) {
    /* This will catch the empty string case */
    marpaESLIFValueResult.contextp           = NULL;
    marpaESLIFValueResult.representationp    = NULL;
    marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_STRING;
    marpaESLIFValueResult.u.s.p              = (unsigned char *) MARPAESLIF_EMPTY_STRING;
    marpaESLIFValueResult.u.s.freeUserDatavp = NULL;
    marpaESLIFValueResult.u.s.freeCallbackp  = NULL;
    marpaESLIFValueResult.u.s.shallowb       = 1;
    marpaESLIFValueResult.u.s.sizel          = 0;
    marpaESLIFValueResult.u.s.encodingasciis = (char *) MARPAESLIF_UTF8_STRING;
  } else {
    /* These are all ARRAYs of size > 0 */
    /* When arg0i == argni then the whole thing is already available in a single bloc, no need to re-allocate */
    if (arg0i == argni) {
      /* We just transform this marpaESLIFValueResult from string type to array type */
      if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_getAndForgetb(marpaESLIFValuep, arg0i, &marpaESLIFValueResult))) {
        goto err;
      }

      p        = (unsigned char *) marpaESLIFValueResult.u.a.p;
      sizel    = marpaESLIFValueResult.u.a.sizel;
      shallowb = marpaESLIFValueResult.u.a.shallowb;

      marpaESLIFValueResult.contextp           = NULL;
      marpaESLIFValueResult.representationp    = NULL;
      marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_STRING;
      marpaESLIFValueResult.u.s.p              = p;
      marpaESLIFValueResult.u.s.freeUserDatavp = marpaESLIFValuep->marpaESLIFRecognizerp;
      marpaESLIFValueResult.u.s.freeCallbackp  = _marpaESLIF_generic_freeCallbackv;
      marpaESLIFValueResult.u.s.shallowb       = shallowb;
      marpaESLIFValueResult.u.s.sizel          = sizel;
      marpaESLIFValueResult.u.s.encodingasciis = (char *) MARPAESLIF_UTF8_STRING;

    } else {

      sizel = 0;
      for (i = arg0i; i<= argni; i++) {
        marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, i);
        if (MARPAESLIF_UNLIKELY(marpaESLIFValueResultp == NULL)) {
          goto err;
        }
        sizel += marpaESLIFValueResultp->u.a.sizel;
      }

      p = (unsigned char *) malloc(sizel + 1); /* + 1 for a hiden NUL byte */
      if (MARPAESLIF_UNLIKELY(p == NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }

      marpaESLIFValueResult.contextp           = NULL;
      marpaESLIFValueResult.representationp    = NULL;
      marpaESLIFValueResult.type               = MARPAESLIF_VALUE_TYPE_STRING;
      marpaESLIFValueResult.u.s.p              = p;
      marpaESLIFValueResult.u.s.freeUserDatavp = marpaESLIFValuep->marpaESLIFRecognizerp;
      marpaESLIFValueResult.u.s.freeCallbackp  = _marpaESLIF_generic_freeCallbackv;
      marpaESLIFValueResult.u.s.shallowb       = 0;
      marpaESLIFValueResult.u.s.sizel          = sizel;
      marpaESLIFValueResult.u.s.encodingasciis = (char *) MARPAESLIF_UTF8_STRING;

      q = p;
      for (i = arg0i; i<= argni; i++) {
        marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, i);
        if (MARPAESLIF_UNLIKELY(marpaESLIFValueResultp == NULL)) {
          goto err;
        }
        sizel = marpaESLIFValueResultp->u.a.sizel;
        memcpy(q, marpaESLIFValueResultp->u.a.p, sizel);
        q += sizel;
      }
      *q = '\0'; /* The hiden NUL byte */
    }
  }

  if (MARPAESLIF_UNLIKELY(! _marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult))) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  if (p != NULL) {
    free(p);
  }
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFJSON_constantb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  marpaESLIFValueResult_t *marpaESLIFValueResultp;

  /* We know this is an array that is either 'true', 'false' or 'null' */
  marpaESLIFValueResultp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, arg0i);
  if (marpaESLIFValueResultp == NULL) {
    return 0;
  } else {

    switch (marpaESLIFValueResultp->u.a.p[0]) {
    case 't':
      return _marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &(marpaESLIFValuep->marpaESLIFp->marpaESLIFValueResultTrue));
    case 'f':
      return _marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &(marpaESLIFValuep->marpaESLIFp->marpaESLIFValueResultFalse));
    case 'n':
      return _marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, (marpaESLIFValueResult_t *) &marpaESLIFValueResultUndef);
    default:
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "Invalid first character '%c'", marpaESLIFValueResultp->u.a.p[0]);
      return 0;
    }
  }
}

/*****************************************************************************/
static short _marpaESLIFJSONValueResultImportb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  marpaESLIFJSONContext_t *marpaESLIFJSONContextp = (marpaESLIFJSONContext_t *) userDatavp;

  /* Proxy to user-defined importb */
  return marpaESLIFJSONContextp->marpaESLIFValueOptionp->importerp(marpaESLIFValuep, marpaESLIFJSONContextp->marpaESLIFValueOptionp->userDatavp, marpaESLIFValueResultp);
}

/*****************************************************************************/
static short _marpaESLIFJSON_positive_infinityb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  marpaESLIFJSONContext_t *marpaESLIFJSONContextp = (marpaESLIFJSONContext_t *) userDatavp;
  marpaESLIFValueResult_t  marpaESLIFValueResult;
  marpaESLIFValueResult_t *marpaESLIFValueResultInputp;
  short                    rcb;

  /* Input is of type array by definition, UTF-8 encoded */
  marpaESLIFValueResultInputp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, arg0i);
  if (MARPAESLIF_UNLIKELY(marpaESLIFValueResultInputp == NULL)) {
    goto err;
  }

#ifdef MARPAESLIF_INFINITY
  marpaESLIFValueResult.contextp        = NULL;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_FLOAT;
  marpaESLIFValueResult.u.f             = MARPAESLIF_INFINITY;
#else
  marpaESLIFValueResult.contextp        = NULL;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_UNDEF;
#endif

  if (MARPAESLIF_UNLIKELY(! _marpaESLIFJSON_proposalb(userDatavp,
                                                      marpaESLIFValuep,
                                                      marpaESLIFJSONContextp->marpaESLIFJSONDecodeOptionp->positiveInfinityActionp,
                                                      marpaESLIFValueResultInputp->u.a.p,
                                                      marpaESLIFValueResultInputp->u.a.sizel,
                                                      &marpaESLIFValueResult))) {
    goto err;
  }

  rcb = _marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult);
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFJSON_negative_infinityb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  marpaESLIFJSONContext_t *marpaESLIFJSONContextp = (marpaESLIFJSONContext_t *) userDatavp;
  marpaESLIFValueResult_t  marpaESLIFValueResult;
  marpaESLIFValueResult_t *marpaESLIFValueResultInputp;
  short                    rcb;

  /* Input is of type array by definition, UTF-8 encoded */
  marpaESLIFValueResultInputp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, arg0i);
  if (MARPAESLIF_UNLIKELY(marpaESLIFValueResultInputp == NULL)) {
    goto err;
  }

#ifdef MARPAESLIF_INFINITY
  marpaESLIFValueResult.contextp        = NULL;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_FLOAT;
  marpaESLIFValueResult.u.f             = -MARPAESLIF_INFINITY;
#else
  marpaESLIFValueResult.contextp        = NULL;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_UNDEF;
#endif

  if (MARPAESLIF_UNLIKELY(! _marpaESLIFJSON_proposalb(userDatavp,
                                                      marpaESLIFValuep,
                                                      marpaESLIFJSONContextp->marpaESLIFJSONDecodeOptionp->negativeInfinityActionp,
                                                      marpaESLIFValueResultInputp->u.a.p,
                                                      marpaESLIFValueResultInputp->u.a.sizel,
                                                      &marpaESLIFValueResult))) {
    goto err;
  }

  rcb = _marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult);
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFJSON_positive_nanb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  marpaESLIFJSONContext_t *marpaESLIFJSONContextp = (marpaESLIFJSONContext_t *) userDatavp;
  marpaESLIFValueResult_t  marpaESLIFValueResult;
  marpaESLIFValueResult_t *marpaESLIFValueResultInputp;
  short                    rcb;

  /* Input is of type array by definition, UTF-8 encoded */
  marpaESLIFValueResultInputp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, arg0i);
  if (MARPAESLIF_UNLIKELY(marpaESLIFValueResultInputp == NULL)) {
    goto err;
  }

#ifdef MARPAESLIF_NAN
  marpaESLIFValueResult.contextp        = NULL;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_FLOAT;
  marpaESLIFValueResult.u.f             = MARPAESLIF_NAN;
#else
  marpaESLIFValueResult.contextp        = NULL;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_UNDEF;
#endif

  if (MARPAESLIF_UNLIKELY(! _marpaESLIFJSON_proposalb(userDatavp,
                                                      marpaESLIFValuep,
                                                      marpaESLIFJSONContextp->marpaESLIFJSONDecodeOptionp->positiveNanActionp,
                                                      marpaESLIFValueResultInputp->u.a.p,
                                                      marpaESLIFValueResultInputp->u.a.sizel,
                                                      &marpaESLIFValueResult))) {
    goto err;
  }

  rcb = _marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult);
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFJSON_negative_nanb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  marpaESLIFJSONContext_t *marpaESLIFJSONContextp = (marpaESLIFJSONContext_t *) userDatavp;
  marpaESLIFValueResult_t  marpaESLIFValueResult;
  marpaESLIFValueResult_t *marpaESLIFValueResultInputp;
  short                    rcb;

  /* Input is of type array by definition, UTF-8 encoded */
  marpaESLIFValueResultInputp = _marpaESLIFValue_stack_getp(marpaESLIFValuep, arg0i);
  if (MARPAESLIF_UNLIKELY(marpaESLIFValueResultInputp == NULL)) {
    goto err;
  }

#ifdef MARPAESLIF_NAN
  marpaESLIFValueResult.contextp        = NULL;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_FLOAT;
  marpaESLIFValueResult.u.f             = MARPAESLIF_NAN;
#else
  marpaESLIFValueResult.contextp        = NULL;
  marpaESLIFValueResult.representationp = NULL;
  marpaESLIFValueResult.type            = MARPAESLIF_VALUE_TYPE_UNDEF;
#endif

  if (MARPAESLIF_UNLIKELY(! _marpaESLIFJSON_proposalb(userDatavp,
                                                      marpaESLIFValuep,
                                                      marpaESLIFJSONContextp->marpaESLIFJSONDecodeOptionp->negativeNanActionp,
                                                      marpaESLIFValueResultInputp->u.a.p,
                                                      marpaESLIFValueResultInputp->u.a.sizel,
                                                      &marpaESLIFValueResult))) {
    goto err;
  }

  rcb = _marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult);
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static void _marpaESLIFJSONRepresentationDisposev(void *userDatavp, char *inputcp, size_t inputl, char *encodingasciis)
/*****************************************************************************/
{
  marpaESLIFJSONContext_t *marpaESLIFJSONContextp = (marpaESLIFJSONContext_t *) userDatavp;
  marpaESLIFValueOption_t *marpaESLIFValueOptionp = marpaESLIFJSONContextp->marpaESLIFValueOptionp;

  /* Proxy to caller's representation disposer */
  if (marpaESLIFJSONContextp->representationDisposep != NULL) {
    marpaESLIFJSONContextp->representationDisposep(marpaESLIFValueOptionp->userDatavp, inputcp, inputl, encodingasciis);
  }
}

/*****************************************************************************/
static short _marpaESLIFJSONRepresentationb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp, char **encodingasciisp, marpaESLIFRepresentationDispose_t *disposeCallbackpp)
/*****************************************************************************/
{
  short                    rcb;
  marpaESLIFJSONContext_t *marpaESLIFJSONContextp = (marpaESLIFJSONContext_t *) userDatavp;
  marpaESLIFValueOption_t *marpaESLIFValueOptionp = marpaESLIFJSONContextp->marpaESLIFValueOptionp;

  /* Proxy to caller's representation */
  rcb = marpaESLIFValueResultp->representationp(marpaESLIFValueOptionp->userDatavp, marpaESLIFValueResultp, inputcpp, inputlp, encodingasciisp, &(marpaESLIFJSONContextp->representationDisposep));

  *disposeCallbackpp = _marpaESLIFJSONRepresentationDisposev;

  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFJSON_proposalb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFJSONProposalAction_t proposalp, char *strings, size_t stringl, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  marpaESLIFJSONContext_t *marpaESLIFJSONContextp;
  marpaESLIFValueOption_t *marpaESLIFValueOptionp;
  short                    rcb;

  if (proposalp != NULL) {
    marpaESLIFJSONContextp = (marpaESLIFJSONContext_t *) userDatavp;
    marpaESLIFValueOptionp = marpaESLIFJSONContextp->marpaESLIFValueOptionp;

    if (MARPAESLIF_UNLIKELY(! proposalp(marpaESLIFValueOptionp->userDatavp, strings, stringl, marpaESLIFValueResultp))) {
      MARPAESLIF_ERRORF(marpaESLIFValuep->marpaESLIFp, "%s: Callback failure", strings);
      goto err;
    }
  } else if (marpaESLIFValueResultp->type == MARPAESLIF_VALUE_TYPE_UNDEF) {
    MARPAESLIF_WARNF(marpaESLIFValuep->marpaESLIFp, "%s: Parsing failure, using undefined value", strings);
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}
