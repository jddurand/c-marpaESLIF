#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp);
static short importb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);

typedef struct marpaESLIFTester_context {
  genericLogger_t *genericLoggerp;
  char            *inputs;
  size_t           inputl;
} marpaESLIFTester_context_t;

const static char *dsl = "\n"
  "#\n"
  "# Default action is to propagate the first RHS value\n"
  "#\n"
  ":default ::= action => ::shift\n"
  "\n"
  "                   #######################################################\n"
  "                   # >>>>>>>>>>>>>>>> Strict JSON Grammar <<<<<<<<<<<<<<<<\n"
  "                   #######################################################\n"
  "\n"
  "# -----------------------------------------\n"
  "# Start is a value that we want stringified\n"
  "# -----------------------------------------\n"
  ":start ::= value2string\n"
  "value2string ::= value action => ::json\n"
  "\n"
  "# -------------------\n"
  "# Composite separator\n"
  "# -------------------\n"
  "comma    ::= ','                                  action         => ::undef   # No-op anyway, override ::shift (default action)\n"
  "\n"
  "# ----------\n"
  "# JSON value\n"
  "# ----------\n"
  "value    ::= string                                                           # ::shift (default action)\n"
  "           | number                                                           # ::shift (default action)\n"
  "           | object                                                           # ::shift (default action)\n"
  "           | array                                                            # ::shift (default action)\n"
  "           | 'true'                               action => ::true            # built-in true action\n"
  "           | 'false'                              action => ::false           # built-in false action\n"
  "           | 'null'                               action => ::undef\n"
  "\n"
  "# -----------\n"
  "# JSON object\n"
  "# -----------\n"
  "object   ::= (-'{'-) members (-'}'-)                                          # ::shift (default action)\n"
  "members  ::= pairs*                               action         => ::lua->lua_members   # Returns { string1 => value1, ..., stringn => valuen }\n"
  "                                                  separator      => comma     # ... separated by comma\n"
  "                                                  proper         => 1         # ... with no trailing separator\n"
  "                                                  hide-separator => 1         # ... and hide separator in the action\n"
  "                                                  \n"
  "pairs    ::= string (-':'-) value                 action         => ::row     # Returns [ string, value ]\n"
  "\n"
  "# -----------\n"
  "# JSON Arrays\n"
  "# -----------\n"
  "array    ::= (-'['-) elements (-']'-)                                         # ::shift (default action)\n"
  "elements ::= value*                               action => ::row             # Returns [ value1, ..., valuen ]\n"
  "                                                  separator      => comma     # ... separated by comma\n"
  "                                                  proper         => 1         # ... with no trailing separator\n"
  "                                                  hide-separator => 1         # ... and hide separator in the action\n"
  "                                                  \n"
  "\n"
  "# ------------\n"
  "# JSON Numbers\n"
  "# ------------\n"
  "number ::= NUMBER                                                             # ::shift (default action)\n"
  "\n"
  ":lexeme ::= NUMBER if-action => ::lua->lua_number_ifaction symbol-action => ::lua->lua_number\n"
  "NUMBER   ~ _INT\n"
  "         | _INT _FRAC\n"
  "         | _INT _EXP\n"
  "         | _INT _FRAC _EXP\n"
  "_INT     ~ _DIGIT\n"
  "         | _DIGIT19 _DIGITS\n"
  "         | '-' _DIGIT\n"
  "         | '-' _DIGIT19 _DIGITS\n"
  "_DIGIT   ~ [0-9]\n"
  "_DIGIT19 ~ [1-9]\n"
  "_FRAC    ~ '.' _DIGITS\n"
  "_EXP     ~ _E _DIGITS\n"
  "_DIGITS  ~ /[0-9]+/\n"
  "_E       ~ /e[+-]?/i\n"
  "\n"
  "# -----------\n"
  "# JSON String\n"
  "# -----------\n"
  "string     ::= '\"' chars '\"' action => ::copy[1]\n"
  ":terminal ::= '\"' pause => after event => :discard[switch]                           # Implementation of discard enabling using reserved ':discard[switch]' keyword\n"
  "\n"
  "chars   ::= filled                                  action => ::lua->lua_chars\n"
  "filled  ::= char+                                   action => ::concat                # Returns join('', char1, ..., charn)\n"
  "chars   ::=                                         action => ::lua->lua_empty_string # Prefering empty string instead of undef\n"
  "char    ::= /[^\"\\\\\\x00-\\x1F]+/                                                   # ::shift (default action) - take care PCRE2 [:cntrl:] includes DEL character\n"
  "          | '\\\\' '\"'                             action => ::copy[1]               # Returns double quote, already ok in data\n"
  "          | '\\\\' '\\\\'                           action => ::copy[1]               # Returns backslash, already ok in data\n"
  "          | '\\\\' '/'                              action => ::copy[1]               # Returns slash, already ok in data\n"
  "          | '\\\\' 'b'                              action => ::u8\"\\x{08}\"\n"
  "          | '\\\\' 'f'                              action => ::u8\"\\x{0C}\"\n"
  "          | '\\\\' 'n'                              action => ::u8\"\\x{0A}\"\n"
  "          | '\\\\' 'r'                              action => ::u8\"\\x{0D}\"\n"
  "          | '\\\\' 't'                              action => ::u8\"\\x{09}\"\n"
  "          | /(?:\\\\u[[:xdigit:]]{4})+/             action => ::lua->lua_unicode\n"
  "\n"
  "\n"
  "# -------------------------\n"
  "# Unsignificant whitespaces\n"
  "# -------------------------\n"
  ":discard    ::= whitespaces event => whitespaces$\n"
  "whitespaces ::= /[\\x{9}\\x{A}\\x{D}\\x{20}]+/\n"
  "\n"
  "                   #######################################################\n"
  "                   # >>>>>>>>>>>>>>>>>> JSON Extensions <<<<<<<<<<<<<<<<<<\n"
  "                   #######################################################\n"
  "\n"
  "# --------------------------\n"
  "# Unlimited commas extension\n"
  "# --------------------------\n"
  "# /* Unlimited commas */commas   ::= comma+\n"
  "\n"
  "# --------------------------\n"
  "# Perl comment extension\n"
  "# --------------------------\n"
  ":discard ::= perl_comment event => perl_comment$\n"
  "perl_comment ::= /(?:(?:#)(?:[^\\n]*)(?:\\n|\\z))/u\n"
  "\n"
  "# --------------------------\n"
  "# C++ comment extension\n"
  "# --------------------------\n"
  "# /* C++ comment */:discard ::= /(?:(?:(?:\\/\\/)(?:[^\\n]*)(?:\\n|\\z))|(?:(?:\\/\\*)(?:(?:[^\\*]+|\\*(?!\\/))*)(?:\\*\\/)))/\n"
  "\n"
  "# ----------------\n"
  "# Number extension\n"
  "# ----------------\n"
  "#\n"
  "# number ::= /\\-?(?:(?:[1-9]?[0-9]+)|[0-9])(?:\\.[0-9]+)?(?:[eE](?:[+-])?[0-9]+)?/ # /* bignum */action => ::lua->lua_number\n"
  "\n"
  "# /* nan */number   ::= '-NaN':i                               action => ::lua->lua_nan\n"
  "# /* nan */number   ::=  'NaN':i                               action => ::lua->lua_nan\n"
  "# /* nan */number   ::= '+NaN':i                               action => ::lua->lua_nan\n"
  "# /* inf */number   ::= '-Infinity':i                          action => ::lua->lua_negative_infinity\n"
  "# /* inf */number   ::=  'Infinity':i                          action => ::lua->lua_positive_infinity\n"
  "# /* inf */number   ::= '+Infinity':i                          action => ::lua->lua_positive_infinity\n"
  "# /* inf */number   ::= '-Inf':i                               action => ::lua->lua_negative_infinity\n"
  "# /* inf            ::=  'Inf':i                               action => ::lua->lua_positive_infinity\n"
  "# /* inf */number   ::= '+Inf':i                               action => ::lua->lua_positive_infinity\n"
  "\n"
  "# -----------------\n"
  "# Control character\n"
  "# -----------------\n"
  "# /* cntrl */char      ::= /[\\x00-\\x1F]/                                                          # Because [:cntrl:] includes DEL (x7F)\n"
  "\n"
  "# -----------------\n"
  "# Lua actions      \n"
  "# -----------------\n"
  "<luascript>\n"
  "\n"
  "  -----------------------------------\n"
  "  function lua_number_ifaction(number)\n"
  "  -----------------------------------\n"
  "    return tonumber(number) ~= nil\n"
  "  end\n"
  "\n"
  "  -----------------------------------\n"
  "  function lua_members(...)\n"
  "  -----------------------------------\n"
  "    local _result = niledtablekv()\n"
  "    for _i=1,select('#', ...) do\n"
  "      local _pair = select(_i, ...)\n"
  "      local _key = _pair[1]\n"
  "      local _value = _pair[2]\n"
  "      _result[_key] = _value\n"
  "    end\n"
  "    return _result\n"
  "  end\n"
  "\n"
  "  -----------------------------------\n"
  "  function lua_number(number)\n"
  "  -----------------------------------\n"
  "    local _result = tonumber(number)\n"
  "    return _result\n"
  "  end\n"
  "\n"
  "  -----------------------------------\n"
  "  function lua_empty_string()\n"
  "  -----------------------------------\n"
  "    local _result = ''\n"
  "    _result:encoding('UTF-8')\n"
  "    return _result\n"
  "  end\n"
  "\n"
  "  -----------------------------------\n"
  "  function lua_chars(chars)\n"
  "  -----------------------------------\n"
  "    local _result = chars\n"
  "    -- Concatenation of lexemes always produces an UTF-8 string\n"
  "    _result:encoding('UTF-8')\n"
  "    return _result\n"
  "  end\n"
  "\n"
  "  -----------------------------------\n"
  "  function lua_unicode(u)\n"
  "  -----------------------------------\n"
  "    local _hex = {}\n"
  "    local _maxpos = string.len(u)\n"
  "    local _nextArrayIndice = 1\n"
  "    local _pos = 1\n"
  "\n"
  "    -- Per def u is a sequence of \\\\u[[:xdigit:]]{4} i.e. 6 'characters', ahem bytes\n"
  "    while (_pos < _maxpos) do\n"
  "       -- Extract the [[:xdigit:]]{4} part\n"
  "      local _codepointAsString = string.sub(u, _pos + 2, _pos + 5)\n"
  "      local _codepoint = tonumber(_codepointAsString, 16)\n"
  "      _hex[_nextArrayIndice] = _codepoint\n"
  "      _nextArrayIndice = _nextArrayIndice + 1\n"
  "      _pos = _pos + 6\n"
  "    end\n"
  "\n"
  "    local _unicode = ''\n"
  "    local _high\n"
  "    local _low\n"
  "    local _codepoint\n"
  "    while (#_hex > 0) do\n"
  "      if (#_hex > 1) then\n"
  "        _high, _low = table.unpack(_hex, 1, 2)\n"
  "        -- UTF-16 surrogate pair ?\n"
  "        if ((_high >= 0xD800) and (_high <= 0xDBFF) and (_low >= 0xDC00) and (_low <= 0xDFFF)) then\n"
  "          _codepoint = ((_high - 0xD800) * 0x400) + (_low - 0xDC00) + 0x10000\n"
  "          table.remove(_hex, 1)\n"
  "          table.remove(_hex, 1)\n"
  "        else\n"
  "          _codepoint = _high\n"
  "          table.remove(_hex, 1)\n"
  "        end\n"
  "      else\n"
  "        _codepoint = table.remove(_hex, 1)\n"
  "      end\n"
  "      _unicode = _unicode..utf8.char(_codepoint)\n"
  "    end\n"
  "\n"
  "    _unicode:encoding('UTF-8')\n"
  "    return _unicode\n"
  "  end\n"
  "  -----------------------------------\n"
  "  function table_print (tt, indent, done) -- FOR DEBUG\n"
  "  -----------------------------------\n"
  "    done = done or {}\n"
  "    indent = indent or 0\n"
  "    if type(tt) == \"table\" then\n"
  "      for key, value in pairs (tt) do\n"
  "        io.write(string.rep (\" \", indent)) -- indent it\n"
  "        if type (value) == \"table\" and not done [value] then\n"
  "          done [value] = true\n"
  "          io.write(string.format(\"[%s] => table\\n\", tostring (key)));\n"
  "          io.write(string.rep (\" \", indent+4)) -- indent it\n"
  "          io.write(\"(\\n\");\n"
  "          table_print (value, indent + 7, done)\n"
  "          io.write(string.rep (\" \", indent+4)) -- indent it\n"
  "          io.write(\")\\n\");\n"
  "        else\n"
  "          io.write(string.format(\"[%s] => %s\\n\",\n"
  "              tostring (key), tostring(value)))\n"
  "        end\n"
  "      end\n"
  "    else\n"
  "      io.write(tostring(tt) .. \"\\n\")\n"
  "    end\n"
  "  end\n"
  "  io.stdout:setvbuf('no')\n"
  "</luascript>\n"
  ;


typedef struct valueContext {
  genericLogger_t *genericLoggerp;
} valueContext_t;

int main() {
  marpaESLIF_t                *marpaESLIFp        = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp = NULL;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  int                          exiti;
  int                          ngrammari;
  char                        *grammarshows;
  int                          leveli;
  genericLogger_t             *genericLoggerp;
  marpaESLIFTester_context_t   marpaESLIFTester_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = NULL;
  short                        continueb;
  short                        exhaustedb;
  int                          i;
  marpaESLIFValue_t           *marpaESLIFValuep = NULL;
  marpaESLIFValueOption_t      marpaESLIFValueOption;
  valueContext_t               valueContext;
  char                        *lastDiscards;
  size_t                       lastDiscardl;

  const static char           *inputs[] = {
    "{\n"
    "  \"Image\": {\n"
    "    \"IDs\": [116]\n"
    "  }\n"
    "}",
    "[\n"
    "  {\n"
    "     \"precision\": \"zip\",\n"
    "     \"Country\":   \"US\"\n"
    "  }\n"
    "]",
    "[\"a\",\"b\"]",
    "[\"\"]",
    "[\"\\uD801\\udc37\"]",
    "[\"\\ud83d\\ude39\\ud83d\\udc8d\"]",
    "\"\\ud83d\\ude39\\ud83d\\udc8d\"",
    "\"ASCII String\"",
    "{\"test\":\"1\"}",
    "{\"test\":true}",
    "{\"test\":false}",
    "{\"test\":null, \"test2\":\"hello world\"}",
    "{\"test\":\"1.25\"}",
    "{\"test\":1.25}",
    "{\"test\":\"1.25e4\"}",
    "{\"test\":1.25e4}",
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
    "  # \"in_reply_to_status_id_str\" : \"281400879465238529\",\n"
    "  \"in_reply_to_status_id_str\" : \"281400879\",\n"
    "  \"geo\" : {\n"
    "  },\n"
    "  # \"id_str\" : \"281405942321532929\",\n"
    "  \"id_str\" : \"281405942\",\n"
    "  \"in_reply_to_user_id\" : 61233,\n"
    "  \"text\" : \"@monkchips Ouch. Some regrets are harsher than others.\",\n"
    "  # \"id\" : 281405942321532929,\n"
    "  \"id\" : 281405942,\n"
    "  # \"in_reply_to_status_id\" : 2814008794,\n"
    "  \"in_reply_to_status_id\" : 281400879,\n"
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
    "1.12",
    "true",
    "false",
    "\"x\"",
    "{\"test\":\"ASCII String\"}",
    "{\"test\":[1,2,3]}",
    "{\"test\":\"\\u1234\"}",
    "{\"test\":[1,2,3,null,5]}",
    "{\"test\":[1,2,3,[null],5]}",
    "{\"geo\" : {}}",
    "{\"geo\" : []}",
    "{}",
    "[]",
    "{\"1\\u12343\":2}",
    "{\"test\":null}",
    "987654321 # Last discarded data is a perl comment"
  };

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);
  if (genericLoggerp == NULL) {
    perror("GENERICLOGGER_NEW");
    goto err;
  }

  valueContext.genericLoggerp = genericLoggerp;

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  marpaESLIFGrammarOption.bytep               = (void *) dsl;
  marpaESLIFGrammarOption.bytel               = strlen(dsl);
  marpaESLIFGrammarOption.encodings           = NULL;
  marpaESLIFGrammarOption.encodingl           = 0;

  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

  /* Dump grammar */
  if (marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    for (leveli = 0; leveli < ngrammari; leveli++) {
      if (marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammarp, &grammarshows, leveli, NULL)) {
        GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "TEST grammar at level %d:", leveli);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n\n%s", grammarshows);
      }
    }
  }

  for (i = 0; i < (sizeof(inputs)/sizeof(inputs[0])); i++) {
    marpaESLIFTester_context.genericLoggerp = genericLoggerp;
    marpaESLIFTester_context.inputs         = (char *) inputs[i];
    marpaESLIFTester_context.inputl         = (inputs[i] != NULL) ? strlen(inputs[i]) : 0;

    marpaESLIFRecognizerOption.userDatavp        = &marpaESLIFTester_context;
    marpaESLIFRecognizerOption.readerCallbackp   = inputReaderb;
    marpaESLIFRecognizerOption.disableThresholdb = 0;
    marpaESLIFRecognizerOption.exhaustedb        = 0;
    marpaESLIFRecognizerOption.newlineb          = 1;
    marpaESLIFRecognizerOption.trackb            = 1;
    marpaESLIFRecognizerOption.bufsizl           = 0;
    marpaESLIFRecognizerOption.buftriggerperci   = 50;
    marpaESLIFRecognizerOption.bufaddperci       = 50;
    marpaESLIFRecognizerOption.ifActionResolverp = NULL;

    /* Free previous round */
    if (marpaESLIFValuep != NULL) {
      marpaESLIFValue_freev(marpaESLIFValuep);
      marpaESLIFValuep = NULL;
    }
    if (marpaESLIFRecognizerp != NULL) {
      marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
      marpaESLIFRecognizerp = NULL;
    }
    
    marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
    if (marpaESLIFRecognizerp == NULL) {
      goto err;
    }

    if (marpaESLIFTester_context.inputs == NULL) {
      break;
    }

    GENERICLOGGER_INFO(genericLoggerp, "Scanning JSON");
    GENERICLOGGER_INFO(genericLoggerp, "-------------");
    GENERICLOGGER_INFOF(genericLoggerp, "%s", inputs[i]);
    GENERICLOGGER_INFO(genericLoggerp, "-------------");
    /* Scan the input */
    /* genericLogger_logLevel_seti(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE); */
    if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, 1 /* initialEventsb */, &continueb, &exhaustedb)) {
      goto err;
    }

    while (continueb) {
      /* Resume */
      if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 0, &continueb, &exhaustedb)) {
        goto err;
      }
    }

    /* Get last discarded value (note: trackb was on) */
    if (! marpaESLIFRecognizer_discard_lastb(marpaESLIFRecognizerp, &lastDiscards, &lastDiscardl)) {
      goto err;
    }
    if ((lastDiscards != NULL) && (lastDiscardl > 0)) {
      GENERICLOGGER_INFOF(genericLoggerp, "Last discarded data: <%s>", lastDiscards);
    }
    
    /* Call for valuation, letting marpaESLIF free the result */
    marpaESLIFValueOption.userDatavp            = &valueContext; /* User specific context */
    marpaESLIFValueOption.ruleActionResolverp   = NULL; /* Will return the function doing the wanted rule action */
    marpaESLIFValueOption.symbolActionResolverp = NULL; /* Will return the function doing the wanted symbol action */
    marpaESLIFValueOption.importerp             = importb;
    marpaESLIFValueOption.highRankOnlyb         = 1;    /* Default: 1 */
    marpaESLIFValueOption.orderByRankb          = 1;    /* Default: 1 */
    marpaESLIFValueOption.ambiguousb            = 0;    /* Default: 0 */
    marpaESLIFValueOption.nullb                 = 0;    /* Default: 0 */
    marpaESLIFValueOption.maxParsesi            = 0;    /* Default: 0 */
    marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
    if (marpaESLIFValuep == NULL) {
      goto err;
    }
    /* genericLogger_logLevel_seti(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE); */
    while (1) {
      short rcValueb = marpaESLIFValue_valueb(marpaESLIFValuep);
      if (rcValueb < 0) {
	goto err;
      } else if (rcValueb == 0) {
	break;
      }
    }
  }

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
  marpaESLIFValue_freev(marpaESLIFValuep);
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
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
static short importb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  valueContext_t *valueContextp = (valueContext_t *) userDatavp;

  switch (marpaESLIFValueResultp->type) {
  case MARPAESLIF_VALUE_TYPE_UNDEF:
    GENERICLOGGER_NOTICE(valueContextp->genericLoggerp, "Result type is undef");
    break;
  case MARPAESLIF_VALUE_TYPE_CHAR:
    GENERICLOGGER_NOTICEF(valueContextp->genericLoggerp, "Result type is char: %c", marpaESLIFValueResultp->u.c);
    break;
  case MARPAESLIF_VALUE_TYPE_SHORT:
    GENERICLOGGER_NOTICEF(valueContextp->genericLoggerp, "Result type is short: %d", (int) marpaESLIFValueResultp->u.b);
    break;
  case MARPAESLIF_VALUE_TYPE_INT:
    GENERICLOGGER_NOTICEF(valueContextp->genericLoggerp, "Result type is int: %d", marpaESLIFValueResultp->u.i);
    break;
  case MARPAESLIF_VALUE_TYPE_LONG:
    GENERICLOGGER_NOTICEF(valueContextp->genericLoggerp, "Result type is long: %ld", marpaESLIFValueResultp->u.l);
    break;
  case MARPAESLIF_VALUE_TYPE_FLOAT:
    GENERICLOGGER_NOTICEF(valueContextp->genericLoggerp, "Result type is float: %f", (double) marpaESLIFValueResultp->u.f);
    break;
  case MARPAESLIF_VALUE_TYPE_DOUBLE:
    GENERICLOGGER_NOTICEF(valueContextp->genericLoggerp, "Result type is double: %f", marpaESLIFValueResultp->u.d);
    break;
  case MARPAESLIF_VALUE_TYPE_PTR:
    GENERICLOGGER_NOTICEF(valueContextp->genericLoggerp, "Result type is ptr: %p", marpaESLIFValueResultp->u.p.p);
    break;
  case MARPAESLIF_VALUE_TYPE_ARRAY:
    GENERICLOGGER_NOTICEF(valueContextp->genericLoggerp, "Result type is array: {%p,%ld}", marpaESLIFValueResultp->u.a.p, (unsigned long) marpaESLIFValueResultp->u.a.sizel);
    break;
  case MARPAESLIF_VALUE_TYPE_BOOL:
    GENERICLOGGER_NOTICEF(valueContextp->genericLoggerp, "Result type is bool: %d", (int) marpaESLIFValueResultp->u.b);
    break;
  case MARPAESLIF_VALUE_TYPE_STRING:
    GENERICLOGGER_NOTICEF(valueContextp->genericLoggerp, "Result type is string: %s, encoding: %s", marpaESLIFValueResultp->u.s.p, marpaESLIFValueResultp->u.s.encodingasciis);
    break;
  case MARPAESLIF_VALUE_TYPE_ROW:
    GENERICLOGGER_NOTICEF(valueContextp->genericLoggerp, "Result type is row: {%p,%ld}", marpaESLIFValueResultp->u.r.p, (unsigned long) marpaESLIFValueResultp->u.r.sizel);
    break;
  case MARPAESLIF_VALUE_TYPE_TABLE:
    GENERICLOGGER_NOTICEF(valueContextp->genericLoggerp, "Result type is table: {%p,%ld}", marpaESLIFValueResultp->u.t.p, (unsigned long) marpaESLIFValueResultp->u.t.sizel);
    break;
  default:
    GENERICLOGGER_ERRORF(valueContextp->genericLoggerp, "Unsupported result type %d", marpaESLIFValueResultp->type);
    break;
  }

  return 1;
}
