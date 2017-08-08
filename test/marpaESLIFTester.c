#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static marpaESLIFValueRuleCallback_t   ruleActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static marpaESLIFValueSymbolCallback_t symbolActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static short                           default_meta_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                           default_lexeme_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static short                           inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);
static short                           eventManagerb(int *eventCountip, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericLogger_t *genericLoggerp);
static void                            genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static short                           alternativeRepresentation(void *userDatavp, marpaESLIFValueResult_t *valueResultp, char **inputcpp, size_t *inputlp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);

typedef struct marpaESLIFTester_context {
  genericLogger_t *genericLoggerp;
  char            *inputs;
  size_t           inputl;
} marpaESLIFTester_context_t;

const static char *exceptions = "\n"
  ":start ::= start\n"
  ":discard ::= whitespace event => :symbol\n"
  ":discard ::= arobace event => discard_arobace\n"
  "event ^start = predicted start\n"
  "event start[] = nulled start\n"
  "event start$ = completed start\n"
  "start ::= thisstart - startException\n"
  "\n"
  "thisstart ~ chars '!'\n"
  "\n"
  "chars ~ char*\n"
  "\n"
  ":lexeme ::= <char> pause => before event => ^[a-zA-Z0-9_:]\n"
  ":lexeme ::= <char> pause => after event => [a-zA-Z0-9_:]$\n"
  "char ~ [a-zA-Z0-9_:]\n"
  "\n"
  "startException ~ chars ':' chars\n"
  "\n"
  "event ^whitespace = predicted whitespace\n"
  "event whitespace[] = nulled whitespace\n"
  "event whitespace$ = completed whitespace\n"
  "whitespace ::= [\\s]\n"
  "event ^arobace = predicted arobace\n"
  "event arobace[] = nulled arobace\n"
  "event arobace$ = completed arobace\n"
  "arobace ::= '@'\n"
  ;

/* Built-in actions extensions */
static const char *actionsArrayp[] = {
  "'%[]'",
  "'%{}'",
  "'%undef'",
  "'%true'",
  "'%false'",
};

int main() {
  marpaESLIF_t                 *marpaESLIFp        = NULL;
  marpaESLIFGrammar_t          *marpaESLIFGrammarp = NULL;
  marpaESLIFOption_t            marpaESLIFOption;
  marpaESLIFGrammarOption_t     marpaESLIFGrammarOption;
  int                           exiti;
  int                           ngrammari;
  char                         *grammarshows;
  int                           leveli;
  genericLogger_t              *genericLoggerp;
  marpaESLIFTester_context_t    marpaESLIFTester_context;
  marpaESLIFRecognizerOption_t  marpaESLIFRecognizerOption;
  marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp;
  marpaESLIFRecognizer_t       *marpaESLIFRecognizerp = NULL;
  marpaESLIFValueOption_t       marpaESLIFValueOption;
  marpaESLIFValueOption_t      *marpaESLIFValueOptionp;
  marpaESLIFValue_t            *marpaESLIFValuep = NULL;
  short                         continueb;
  short                         exhaustedb;
  const static char            *inputs = "abc! 123de@:@f";
  short                         rcValueb;
  int                           eventCounti = 0;
  size_t                        nLexemel;
  size_t                        lexemel;
  char                        **lexemesArrayp;
  marpaESLIFValueResult_t       marpaESLIFValueResult;
  int                          *ruleip;
  size_t                        rulel;
  size_t                        rulei;
  marpaESLIFRuleProperty_t      ruleProperty;
  int                          *symbolip;
  size_t                        symboll;
  size_t                        symboli;
  marpaESLIFSymbolProperty_t    symbolProperty;
  size_t                        rhsl;
  marpaESLIFGrammarProperty_t   grammarProperty;
  
  genericLoggerp = genericLogger_newp(genericLoggerCallback, NULL /* userDatavp */, GENERICLOGGER_LOGLEVEL_DEBUG);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  /* Grammar builtin actions extension */
  if (! marpaESLIF_extend_builtin_actionb(marpaESLIFp, (char **) actionsArrayp, sizeof(actionsArrayp)/sizeof(actionsArrayp[0]))) {
    goto err;
  }
  
  /* Dump ESLIF grammar */
  if (marpaESLIFGrammar_ngrammarib(marpaESLIF_grammarp(marpaESLIFp), &ngrammari)) {
    for (leveli = 0; leveli < ngrammari; leveli++) {
      if (marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIF_grammarp(marpaESLIFp), &grammarshows, leveli, NULL)) {
        GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "ESLIF grammar at level %d:", leveli);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n\n%s", grammarshows);
      }
    }
  }

  marpaESLIFGrammarOption.bytep               = (void *) exceptions;
  marpaESLIFGrammarOption.bytel               = strlen(exceptions);
  marpaESLIFGrammarOption.encodings           = "UTF-8";
  marpaESLIFGrammarOption.encodingl           = (marpaESLIFGrammarOption.encodings != NULL) ? strlen(marpaESLIFGrammarOption.encodings) : 0;
  marpaESLIFGrammarOption.encodingOfEncodings = "ASCII";
  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);

  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

  if (marpaESLIFGrammar_eslifp(marpaESLIFGrammarp) != marpaESLIFp) {
    GENERICLOGGER_ERROR(marpaESLIFOption.genericLoggerp, "marpaESLIFGrammar_eslifp != marpaESLIFp");
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

  /* Grammar introspection */
  if (marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    for (leveli = 0; leveli < ngrammari; leveli++) {
      GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "---------------------------------------");
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "TEST grammar introspection at level %d:", leveli);
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "---------------------------------------\n\n%s", grammarshows);
      if (marpaESLIFGrammar_grammarproperty_by_levelb(marpaESLIFGrammarp, &grammarProperty, leveli, NULL)) {
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... Grammar No %d", leveli);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Level                 : %d", grammarProperty.leveli);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Grammar description   : %s", grammarProperty.descp->asciis);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... LATM ?                : %d", (int) grammarProperty.latmb);
        if (grammarProperty.defaultSymbolActionp != NULL) {
          switch (grammarProperty.defaultSymbolActionp->type) {
          case MARPAESLIF_ACTION_TYPE_NAME:
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Default symbol action : %s (type: name)", grammarProperty.defaultSymbolActionp->u.names);
            break;
          case MARPAESLIF_ACTION_TYPE_STRING:
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Default symbol action : %s (type: string)", grammarProperty.defaultSymbolActionp->u.stringp->asciis);
            break;
          default:
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Default symbol action : ??? (type: %d)", grammarProperty.defaultSymbolActionp->type);
            break;
          }
        } else {
          GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "... ... Default symbol action : ??? (NULL)");
        }
        if (grammarProperty.defaultRuleActionp != NULL) {
          switch (grammarProperty.defaultRuleActionp->type) {
          case MARPAESLIF_ACTION_TYPE_NAME:
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Default rule action   : %s (type: name)", grammarProperty.defaultRuleActionp->u.names);
            break;
          case MARPAESLIF_ACTION_TYPE_STRING:
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Default rule action   : %s (type: string)", grammarProperty.defaultRuleActionp->u.stringp->asciis);
            break;
          default:
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Default rule action   : ??? (type: %d)", grammarProperty.defaultRuleActionp->type);
            break;
          }
        } else {
          GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "... ... Default rule action   : ??? (NULL)");
        }
        if (grammarProperty.defaultFreeActionp != NULL) {
          switch (grammarProperty.defaultFreeActionp->type) {
          case MARPAESLIF_ACTION_TYPE_NAME:
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Default free action   : %s (type: name)", grammarProperty.defaultFreeActionp->u.names);
            break;
          case MARPAESLIF_ACTION_TYPE_STRING:
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Default free action   : %s (type: string)", grammarProperty.defaultFreeActionp->u.stringp->asciis);
            break;
          default:
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Default free action   : ??? (type: %d)", grammarProperty.defaultFreeActionp->type);
            break;
          }
        } else {
          GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "... ... Default free action   : ??? (NULL)");
        }
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Start symbol Id       : %d", grammarProperty.starti);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Discard symbol Id     : %d", grammarProperty.discardi);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Number of symbols     : %ld", (unsigned long) grammarProperty.nsymboll);
        for (symboll = 0; symboll < grammarProperty.nsymboll; symboll++) {
          GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... ... Symbol Id %ld       : %d", (unsigned long) symboll, grammarProperty.symbolip[symboll]);
        }
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Number of rules       : %ld", (unsigned long) grammarProperty.nrulel);
        for (rulel = 0; rulel < grammarProperty.nrulel; rulel++) {
          GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... ... Rule Id %ld         : %d", (unsigned long) rulel, grammarProperty.ruleip[rulel]);
        }
      }
      if (marpaESLIFGrammar_rulearray_by_levelb(marpaESLIFGrammarp, &ruleip, &rulel, leveli, NULL)) {
        for (rulei = 0; rulei < rulel; rulei++) {
          if (marpaESLIFGrammar_ruleproperty_by_levelb(marpaESLIFGrammarp, rulei, &ruleProperty, leveli, NULL)) {
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... Rule No %d", rulei);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Rule Id               : %d", ruleProperty.idi);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Rule description      : %s", ruleProperty.descp->asciis);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Rule show             : %s", ruleProperty.asciishows);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... LHS Id                : %d", ruleProperty.lhsi);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Separator Id          : %d", ruleProperty.separatori);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Number of RHS         : %ld", (unsigned long) ruleProperty.nrhsl);
            for (rhsl = 0; rhsl < ruleProperty.nrhsl; rhsl++) {
              GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... ... RHS No %ld         : %d", (unsigned long) rhsl, ruleProperty.rhsip[rhsl]);
            }
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Exception Id          : %d", ruleProperty.exceptioni);
            if (ruleProperty.actionp != NULL) {
              switch (ruleProperty.actionp->type) {
              case MARPAESLIF_ACTION_TYPE_NAME:
                GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Action                : %s (type: name)", ruleProperty.actionp->u.names);
                break;
              case MARPAESLIF_ACTION_TYPE_STRING:
                GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Action                : %s (type: string)", ruleProperty.actionp->u.stringp->asciis);
                break;
              default:
                GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Action                : ??? (type: %d)", ruleProperty.actionp->type);
                break;
              }
            } else {
              GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "... ... Action                : ??? (NULL)");
            }
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Discard event         : %s", (ruleProperty.discardEvents != NULL) ? ruleProperty.discardEvents : "");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Discard initial state : %d", ruleProperty.discardEventb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Rank                  : %d", ruleProperty.ranki);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Null ranks high ?     : %d", (int) ruleProperty.nullRanksHighb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Sequence ?            : %d", (int) ruleProperty.sequenceb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Proper ?              : %d", (int) ruleProperty.properb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Sequence minimum      : %d", ruleProperty.minimumi);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Internal ?            : %d", (int) ruleProperty.internalb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Accessible ?          : %s", ((ruleProperty.propertyBitSet & MARPAESLIF_RULE_IS_ACCESSIBLE) == MARPAESLIF_RULE_IS_ACCESSIBLE) ? "yes" : "no");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Nullable ?            : %s", ((ruleProperty.propertyBitSet & MARPAESLIF_RULE_IS_NULLABLE) == MARPAESLIF_RULE_IS_NULLABLE) ? "yes" : "no");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Nulling ?             : %s", ((ruleProperty.propertyBitSet & MARPAESLIF_RULE_IS_NULLING) == MARPAESLIF_RULE_IS_NULLING) ? "yes" : "no");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Loop ?                : %s", ((ruleProperty.propertyBitSet & MARPAESLIF_RULE_IS_LOOP) == MARPAESLIF_RULE_IS_LOOP) ? "yes" : "no");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Productive ?          : %s", ((ruleProperty.propertyBitSet & MARPAESLIF_RULE_IS_PRODUCTIVE) == MARPAESLIF_RULE_IS_PRODUCTIVE) ? "yes" : "no");
          }
        }
      }
      if (marpaESLIFGrammar_symbolarray_by_levelb(marpaESLIFGrammarp, &symbolip, &symboll, leveli, NULL)) {
        for (symboli = 0; symboli < symboll; symboli++) {
          if (marpaESLIFGrammar_symbolproperty_by_levelb(marpaESLIFGrammarp, symboli, &symbolProperty, leveli, NULL)) {
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... Symbol No %d", symboli);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... type                  : %d", symbolProperty.type);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... startb                : %d", (int) symbolProperty.startb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... discardb              : %d", (int) symbolProperty.discardb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... discardRhsb           : %d", (int) symbolProperty.discardRhsb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... lhsb                  : %d", (int) symbolProperty.lhsb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... topb                  : %d", (int) symbolProperty.topb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... idi                   : %d", symbolProperty.idi);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Description           : %s", symbolProperty.descp->asciis);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... eventBefores          : %s", symbolProperty.eventBefores != NULL ? symbolProperty.eventBefores : "??? (NULL)");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... eventBeforeb          : %d", (int) symbolProperty.eventBeforeb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... eventAfters           : %s", symbolProperty.eventAfters != NULL ? symbolProperty.eventAfters : "??? (NULL)");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... eventAfterb           : %d", (int) symbolProperty.eventAfterb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... eventPredicteds       : %s", symbolProperty.eventPredicteds != NULL ? symbolProperty.eventPredicteds : "??? (NULL)");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... eventPredictedb       : %d", (int) symbolProperty.eventPredictedb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... eventNulleds          : %s", symbolProperty.eventNulleds != NULL ? symbolProperty.eventNulleds : "??? (NULL)");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... eventNulledb          : %d", (int) symbolProperty.eventNulledb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... eventCompleteds       : %s", symbolProperty.eventCompleteds != NULL ? symbolProperty.eventCompleteds : "??? (NULL)");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... eventCompletedb       : %d", (int) symbolProperty.eventCompletedb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... discardEvents         : %s", symbolProperty.discardEvents != NULL ? symbolProperty.discardEvents : "??? (NULL)");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... discardEventb         : %d", (int) symbolProperty.discardEventb);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... lookupResolvedLeveli  : %d", symbolProperty.lookupResolvedLeveli);
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... priorityi             : %d", symbolProperty.priorityi);
            if (symbolProperty.nullableActionp != NULL) {
              switch (symbolProperty.nullableActionp->type) {
              case MARPAESLIF_ACTION_TYPE_NAME:
                GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... nullableAction        : %s (type: name)", symbolProperty.nullableActionp->u.names);
                break;
              case MARPAESLIF_ACTION_TYPE_STRING:
                GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... nullableAction        : %s (type: string)", symbolProperty.nullableActionp->u.stringp->asciis);
                break;
              default:
                GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... nullableAction        : ??? (type: %d)", symbolProperty.nullableActionp->type);
                break;
              }
            } else {
              GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "... ... nullableAction        : ??? (NULL)");
            }
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Accessible ?          : %s", ((symbolProperty.propertyBitSet & MARPAESLIF_SYMBOL_IS_ACCESSIBLE) == MARPAESLIF_SYMBOL_IS_ACCESSIBLE) ? "yes" : "no");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Nullable ?            : %s", ((symbolProperty.propertyBitSet & MARPAESLIF_SYMBOL_IS_NULLABLE) == MARPAESLIF_SYMBOL_IS_NULLABLE) ? "yes" : "no");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Nulling ?             : %s", ((symbolProperty.propertyBitSet & MARPAESLIF_SYMBOL_IS_NULLING) == MARPAESLIF_SYMBOL_IS_NULLING) ? "yes" : "no");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Productive ?          : %s", ((symbolProperty.propertyBitSet & MARPAESLIF_SYMBOL_IS_PRODUCTIVE) == MARPAESLIF_SYMBOL_IS_PRODUCTIVE) ? "yes" : "no");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Start ?               : %s", ((symbolProperty.propertyBitSet & MARPAESLIF_SYMBOL_IS_START) == MARPAESLIF_SYMBOL_IS_START) ? "yes" : "no");
            GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "... ... Terminal ?            : %s", ((symbolProperty.propertyBitSet & MARPAESLIF_SYMBOL_IS_TERMINAL) == MARPAESLIF_SYMBOL_IS_TERMINAL) ? "yes" : "no");
          }
        }
      }
    }
  }

  /* Try recognizer */
  marpaESLIFTester_context.genericLoggerp = genericLoggerp;
  marpaESLIFTester_context.inputs         = (char *) inputs;
  marpaESLIFTester_context.inputl         = strlen(inputs);

  marpaESLIFRecognizerOption.userDatavp                = &marpaESLIFTester_context;
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp = inputReaderb;
  marpaESLIFRecognizerOption.disableThresholdb         = 0;
  marpaESLIFRecognizerOption.exhaustedb                = 1;
  marpaESLIFRecognizerOption.newlineb                  = 1;
  marpaESLIFRecognizerOption.trackb                    = 0;
  marpaESLIFRecognizerOption.bufsizl                   = 0;
  marpaESLIFRecognizerOption.buftriggerperci           = 50;
  marpaESLIFRecognizerOption.bufaddperci               = 50;

  marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
  if (marpaESLIFRecognizerp == NULL) {
    goto err;
  }

  if (marpaESLIFRecognizer_grammarp(marpaESLIFRecognizerp) == NULL) {
    GENERICLOGGER_ERROR(marpaESLIFOption.genericLoggerp, "marpaESLIFRecognizer_grammarp returned NULL");
    goto err;
  }

  marpaESLIFRecognizerOptionp = marpaESLIFRecognizer_optionp(marpaESLIFRecognizerp);
  if (marpaESLIFRecognizerOptionp == NULL) {
    goto err;
  }
  if ((marpaESLIFRecognizerOption.userDatavp                != marpaESLIFRecognizerOption.userDatavp) ||
      (marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp != marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp) ||
      (marpaESLIFRecognizerOption.disableThresholdb         != marpaESLIFRecognizerOption.disableThresholdb) ||
      (marpaESLIFRecognizerOption.exhaustedb                != marpaESLIFRecognizerOption.exhaustedb) ||
      (marpaESLIFRecognizerOption.newlineb                  != marpaESLIFRecognizerOption.newlineb) ||
      (marpaESLIFRecognizerOption.trackb                    != marpaESLIFRecognizerOption.trackb) ||
      (marpaESLIFRecognizerOption.bufsizl                   != marpaESLIFRecognizerOption.bufsizl) ||
      (marpaESLIFRecognizerOption.buftriggerperci           != marpaESLIFRecognizerOption.buftriggerperci) ||
      (marpaESLIFRecognizerOption.bufaddperci               != marpaESLIFRecognizerOption.bufaddperci)) {
    GENERICLOGGER_ERROR(marpaESLIFOption.genericLoggerp, "marpaESLIFRecognizer_optionp does have the correct content");
    goto err;
  }

  /* genericLogger_logLevel_seti(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE); */
  if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, 1 /* initialEventsb */, &continueb, &exhaustedb)) {
    goto err;
  }
  /* Lexemes expected ? */
  if (! marpaESLIFRecognizer_lexeme_expectedb(marpaESLIFRecognizerp, &nLexemel, &lexemesArrayp)) {
    goto err;
  }
  for (lexemel = 0; lexemel < nLexemel; lexemel++) {
    GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Lexeme expected: %s", lexemesArrayp[lexemel]);
  }
  if (! eventManagerb(&eventCounti, marpaESLIFRecognizerp, genericLoggerp)) {
    goto err;
  }
  while (continueb) {
    if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 0, &continueb, &exhaustedb)) {
      goto err;
    }
    /* Lexemes expected ? */
    if (! marpaESLIFRecognizer_lexeme_expectedb(marpaESLIFRecognizerp, &nLexemel, &lexemesArrayp)) {
      goto err;
    }
    for (lexemel = 0; lexemel < nLexemel; lexemel++) {
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Lexeme expected: %s", lexemesArrayp[lexemel]);
    }
    if (! eventManagerb(&eventCounti, marpaESLIFRecognizerp, genericLoggerp)) {
      goto err;
    }
  }

  /* Try value */
  
  marpaESLIFValueOption.userDatavp             = NULL;
  marpaESLIFValueOption.ruleActionResolverp    = NULL;
  marpaESLIFValueOption.symbolActionResolverp  = NULL;
  marpaESLIFValueOption.freeActionResolverp    = NULL;
  marpaESLIFValueOption.highRankOnlyb          = 1;
  marpaESLIFValueOption.orderByRankb           = 1;
  marpaESLIFValueOption.ambiguousb             = 0;
  marpaESLIFValueOption.nullb                  = 0;
  marpaESLIFValueOption.maxParsesi             = 0;

  marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
  if (marpaESLIFValuep == NULL) {
    goto err;
  }

  if (marpaESLIFValue_recognizerp(marpaESLIFValuep) != marpaESLIFRecognizerp) {
    GENERICLOGGER_ERROR(marpaESLIFOption.genericLoggerp, "marpaESLIFValue_recognizerp != marpaESLIFRecognizerp");
    goto err;
  }

  marpaESLIFValueOptionp = marpaESLIFValue_optionp(marpaESLIFValuep);
  if (marpaESLIFValueOptionp == NULL) {
    goto err;
  }
  if ((marpaESLIFValueOption.userDatavp            != marpaESLIFValueOption.userDatavp) ||
      (marpaESLIFValueOption.ruleActionResolverp   != marpaESLIFValueOption.ruleActionResolverp) ||
      (marpaESLIFValueOption.symbolActionResolverp != marpaESLIFValueOption.symbolActionResolverp) ||
      (marpaESLIFValueOption.freeActionResolverp   != marpaESLIFValueOption.freeActionResolverp) ||
      (marpaESLIFValueOption.highRankOnlyb         != marpaESLIFValueOption.highRankOnlyb) ||
      (marpaESLIFValueOption.orderByRankb          != marpaESLIFValueOption.orderByRankb) ||
      (marpaESLIFValueOption.ambiguousb            != marpaESLIFValueOption.ambiguousb) ||
      (marpaESLIFValueOption.nullb                 != marpaESLIFValueOption.nullb) ||
      (marpaESLIFValueOption.maxParsesi            != marpaESLIFValueOption.maxParsesi)) {
    GENERICLOGGER_ERROR(marpaESLIFOption.genericLoggerp, "marpaESLIFValue_optionp does have the correct content");
    goto err;
  }

  while (1) {
    rcValueb = marpaESLIFValue_valueb(marpaESLIFValuep, &marpaESLIFValueResult);
    if (rcValueb < 0) {
      goto err;
    } else if (rcValueb == 0) {
      break;
    } else {
      /* We use the default that is ::concat, i.e. it is guaranteed to a NUL byte terminated array */
      /* and in our case we made sure input is ASCII compliant... */
      if (marpaESLIFValueResult.u.p != NULL) {
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Value result: %s", marpaESLIFValueResult.u.p);
        free(marpaESLIFValueResult.u.p);
      } else {
        GENERICLOGGER_WARN(marpaESLIFOption.genericLoggerp, "Value result is NULL");
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
  return exiti;
}

/****************************************************************************/
static marpaESLIFValueRuleCallback_t ruleActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/****************************************************************************/
{
  marpaESLIFTester_context_t    *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;
  marpaESLIFGrammar_t           *marpaESLIFGrammarp        = marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep));
  marpaESLIFValueRuleCallback_t  marpaESLIFValueRuleCallbackp;
  int                            leveli;

  if (! marpaESLIFGrammar_grammar_currentb(marpaESLIFGrammarp, &leveli, NULL /* descp */)) {
    GENERICLOGGER_ERROR(marpaESLIFTester_contextp->genericLoggerp, "marpaESLIFGrammar_grammar_currentb failure");
    goto err;
  }

  /* We have only one level here */
  if (leveli != 0) {
    GENERICLOGGER_ERRORF(marpaESLIFTester_contextp->genericLoggerp, "leveli = %d", leveli);
    goto err;
  }
  if (strcmp(actions, "default_meta_actionb") == 0) {
    marpaESLIFValueRuleCallbackp = default_meta_actionb;
  } else {
    GENERICLOGGER_ERRORF(marpaESLIFTester_contextp->genericLoggerp, "Unsupported action \"%s\"", actions);
    goto err;
  }

  goto done;

 err:
  marpaESLIFValueRuleCallbackp = NULL;
 done:
  return marpaESLIFValueRuleCallbackp;
}

/****************************************************************************/
static marpaESLIFValueSymbolCallback_t symbolActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/****************************************************************************/
{
  marpaESLIFTester_context_t     *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;
  marpaESLIFGrammar_t            *marpaESLIFGrammarp        = marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep));
  marpaESLIFValueSymbolCallback_t marpaESLIFValueSymbolCallbackp;
  int                             leveli;

  if (! marpaESLIFGrammar_grammar_currentb(marpaESLIFGrammarp, &leveli, NULL /* descp */)) {
    GENERICLOGGER_ERROR(marpaESLIFTester_contextp->genericLoggerp, "marpaESLIFGrammar_grammar_currentb failure");
    goto err;
  }


  /* We have only one level here */
  if (leveli != 0) {
    GENERICLOGGER_ERRORF(marpaESLIFTester_contextp->genericLoggerp, "leveli = %d", leveli);
    goto err;
  }
  if (strcmp(actions, "default_lexeme_actionb") == 0) {
    marpaESLIFValueSymbolCallbackp = default_lexeme_actionb;
  } else {
    GENERICLOGGER_ERRORF(marpaESLIFTester_contextp->genericLoggerp, "Unsupported action \"%s\"", actions);
    goto err;
  }

  goto done;

 err:
  marpaESLIFValueSymbolCallbackp = NULL;
 done:
  return marpaESLIFValueSymbolCallbackp;
}

/****************************************************************************/
static short default_meta_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/****************************************************************************/
{
  return 0;
}

/****************************************************************************/
static short default_lexeme_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/****************************************************************************/
{
  return 0;
}

/*****************************************************************************/
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp)
/*****************************************************************************/
{
  marpaESLIFTester_context_t *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;

  *inputsp              = marpaESLIFTester_contextp->inputs;
  *inputlp              = marpaESLIFTester_contextp->inputl;
  *eofbp                = 1;
  *characterStreambp    = 0; /* We say this is not a stream of characters - regexp will adapt and to UTF correctness if needed */
  *encodingOfEncodingsp = NULL;
  *encodingsp           = NULL;
  *encodinglp           = 0;

  return 1;
}

/*****************************************************************************/
static short eventManagerb(int *eventCountip, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericLogger_t *genericLoggerp)
/*****************************************************************************/
{
  marpaESLIFEvent_t      *eventArrayp;
  size_t                  eventArrayl;
  size_t                  eventArrayIteratorl;
  short                   rcb;
  char                   *inputs;
  size_t                  inputl;
  short                   eofb;
  marpaESLIFAlternative_t marpaESLIFAlternative;

  (*eventCountip)++;

  if (! marpaESLIFRecognizer_eventb(marpaESLIFRecognizerp, &eventArrayl, &eventArrayp)) {
    goto err;
  }

  for (eventArrayIteratorl = 0; eventArrayIteratorl < eventArrayl; eventArrayIteratorl++) {
    switch (eventArrayp[eventArrayIteratorl].type) {
    case MARPAESLIF_EVENTTYPE_COMPLETED:
      GENERICLOGGER_INFOF(genericLoggerp, "[%3d] Event %s for symbol <%s>", *eventCountip, eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols);
      break;
    case MARPAESLIF_EVENTTYPE_NULLED:
      GENERICLOGGER_INFOF(genericLoggerp, "[%3d] Event %s for symbol <%s>", *eventCountip, eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols);
      break;
    case MARPAESLIF_EVENTTYPE_PREDICTED:
      GENERICLOGGER_INFOF(genericLoggerp, "[%3d] Event %s for symbol <%s>", *eventCountip, eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols);
      break;
    case MARPAESLIF_EVENTTYPE_BEFORE:
      if (! marpaESLIFRecognizer_isEofb(marpaESLIFRecognizerp, &eofb)) {
        goto err;
      }
      if (! marpaESLIFRecognizer_inputb(marpaESLIFRecognizerp, &inputs, &inputl)) {
        goto err;
      }
      GENERICLOGGER_INFOF(genericLoggerp, "[%3d] Event %s for symbol <%s> (character is %c (0x%lx), eofb is %d)", *eventCountip, eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols, *inputs, (unsigned long) *inputs, (int) eofb);
      if (strcmp(eventArrayp[eventArrayIteratorl].events, "^[a-zA-Z0-9_:]") == 0) {
        GENERICLOGGER_INFOF(genericLoggerp, "[%3d] ... Pushing single alternative <%s>", *eventCountip, eventArrayp[eventArrayIteratorl].symbols);
        marpaESLIFAlternative.lexemes               = eventArrayp[eventArrayIteratorl].symbols;
        marpaESLIFAlternative.value.type            = MARPAESLIF_VALUE_TYPE_CHAR;
        marpaESLIFAlternative.value.u.c             = *inputs;
        marpaESLIFAlternative.value.contexti        =  0; /* Not used */
        marpaESLIFAlternative.value.sizel           =  0; /* Not used */
        /* We push a MARPAESLIF_VALUE_TYPE_CHAR : default representation is ok */
        marpaESLIFAlternative.value.representationp = NULL;
        /* marpaESLIFAlternative.value.representationp = alternativeRepresentation; */
        marpaESLIFAlternative.grammarLengthl        = 1;
        if (! marpaESLIFRecognizer_lexeme_readb(marpaESLIFRecognizerp, &marpaESLIFAlternative, 1 /* Length in the real input */)) {
          goto err;
        }
        /* Complete can generate again events! */
        /* We have no risk because a given symbol instance can never generate two events: if it has been completed */
        /* and it will be predicted again, then the completion event wins. */
        if (! eventManagerb(eventCountip, marpaESLIFRecognizerp, genericLoggerp)) {
          goto err;
        }
      }
      break;
    case MARPAESLIF_EVENTTYPE_AFTER:
      if (! marpaESLIFRecognizer_isEofb(marpaESLIFRecognizerp, &eofb)) {
        goto err;
      }
      if (! marpaESLIFRecognizer_inputb(marpaESLIFRecognizerp, &inputs, &inputl)) {
        goto err;
      }
      GENERICLOGGER_INFOF(genericLoggerp, "[%3d] Event %s for symbol <%s> (inputl=%ld, eofbp is %d)", *eventCountip, eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols, (unsigned long) inputl, (int) eofb);
      break;
    case MARPAESLIF_EVENTTYPE_EXHAUSTED:
      GENERICLOGGER_INFOF(genericLoggerp, "[%3d] >>> Exhausted event", *eventCountip);
      break;
    case MARPAESLIF_EVENTTYPE_DISCARD:
      GENERICLOGGER_INFOF(genericLoggerp, "[%3d] Event %s for symbol <%s>", *eventCountip, eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols);
      break;
    default:
      if (eventArrayp[eventArrayIteratorl].type != MARPAESLIF_EVENTTYPE_NONE) {
        /* Should NEVER happen */
        GENERICLOGGER_WARNF(genericLoggerp, "[%3d] Unsupported event type %d", *eventCountip, eventArrayp[eventArrayIteratorl].type);
      }
      break;
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
static void genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  switch (logLeveli) {
  case GENERICLOGGER_LOGLEVEL_TRACE:
    fprintf(stdout, "<TRACE> %s\n", msgs); fflush(stdout);
    break;
  case GENERICLOGGER_LOGLEVEL_DEBUG:
    fprintf(stdout, "<DEBUG> %s\n", msgs); fflush(stdout);
    break;
  case GENERICLOGGER_LOGLEVEL_INFO:
    fprintf(stdout, "<INFO> %s\n", msgs); fflush(stdout);
    break;
  case GENERICLOGGER_LOGLEVEL_NOTICE:
    fprintf(stdout, "<NOTICE> %s\n", msgs); fflush(stdout);
    break;
  case GENERICLOGGER_LOGLEVEL_WARNING:
    fprintf(stdout, "<WARNING> %s\n", msgs); fflush(stdout);
    break;
  case GENERICLOGGER_LOGLEVEL_ERROR:
    fprintf(stderr, "<ERROR> %s\n", msgs); fflush(stderr);
    break;
  case GENERICLOGGER_LOGLEVEL_CRITICAL:
    fprintf(stdout, "<CRITICAL> %s\n", msgs); fflush(stdout);
    break;
  case GENERICLOGGER_LOGLEVEL_ALERT:
    fprintf(stdout, "<ALERT %s\n", msgs); fflush(stdout);
    break;
  case GENERICLOGGER_LOGLEVEL_EMERGENCY:
    fprintf(stdout, "<EMERGENCY> %s\n", msgs); fflush(stdout);
    break;
  default:
    fprintf(stdout, "<UNKNOWN LEVEL> %s\n", msgs); fflush(stdout);
    break;
  }
}

/*****************************************************************************/
static short alternativeRepresentation(void *userDatavp, marpaESLIFValueResult_t *valueResultp, char **inputcpp, size_t *inputlp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp)
/*****************************************************************************/
{
  /* We know that we are pushing a CHAR */

  *inputcpp          = &valueResultp->u.c;
  *inputlp           = 1;
  *characterStreambp = 1;
}

