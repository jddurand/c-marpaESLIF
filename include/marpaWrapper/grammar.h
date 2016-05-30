#ifndef MARPAWRAPPER_GRAMMAR
#define MARPAWRAPPER_GRAMMAR

#include <stddef.h>
#include <stdarg.h>

#include "marpaWrapper/export.h"
#include "genericLogger.h"

/* ---------------- */
/* Opaque structure */
/* ---------------- */
typedef struct marpaWrapperGrammar       marpaWrapperGrammar_t;
typedef struct marpaWrapperGrammarSymbol marpaWrapperGrammarSymbol_t;
typedef struct marpaWrapperGrammarRule   marpaWrapperGrammarRule_t;

/* ------------------------------------------ */
/* For every symbol there can be three events */
/* ------------------------------------------ */
typedef enum marpaWrapperGrammarEventType {
  MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE       = 0x00,
  MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION = 0x01,
  MARPAWRAPPERGRAMMAR_EVENTTYPE_NULLED     = 0x02,
  MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION = 0x04,
} marpaWrapperGrammarEventType_t;

/* A triggered event is for a symbol, with an event type */
typedef struct marpaWrapperGrammarEvent {
  enum {
    MARPAWRAPPERGRAMMAR_EVENT_COMPLETED,
    MARPAWRAPPERGRAMMAR_EVENT_NULLED,
    MARPAWRAPPERGRAMMAR_EVENT_EXPECTED
  } eventType;
  marpaWrapperGrammarSymbol_t *symbolp;
} marpaWrapperGrammarEvent_t;

/* and this is managed with an event callback */
typedef void (*marpaWrapperGrammarEventCallback_t)(void                       *datavp,
						   marpaWrapperGrammarEvent_t *eventp);


/* ------------------ */
/* Options per symbol */
/* ------------------ */
typedef struct marpaWrapperGrammarSymbolOption {
  void                              *datavp;                /* Default: NULL. User's opaque data pointer for this symbol  */
  short                              terminalb;             /* Default: 0. Eventually force symbol to be terminal         */
  short                              startb;                /* Default: 0. Eventually force symbol to be the start symbol */
  int                                eventSeti;             /* Default: MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE.               */
  marpaWrapperGrammarEventCallback_t eventCallbackp;        /* Default: NULL */
  void                              *eventCallbackDatavp;   /* Default: NULL */
} marpaWrapperGrammarSymbolOption_t;

/* ---------------- */
/* Options per rule */
/* ---------------- */
typedef struct marpaWrapperGrammarRuleOption {
  void                        *datavp;         /* Default: NULL. User's opaque data pointer for this rule */
  int                          ranki;          /* Default: 0. Rank                                        */
  short                        nullRanksHighb; /* Default: 0. Null variant pattern                        */
  short                        sequenceb;      /* Default: 0. Sequence ?                                  */
  marpaWrapperGrammarSymbol_t *separatorSymbolp; /* Default: NULL. Eventual separator symbol              */
  short                        properb;        /* Default: 0. Proper flag                                 */
  int                          minimumi;       /* Default: 0. Mininimum - must be 0 or 1                  */
} marpaWrapperGrammarRuleOption_t;

/* --------------- */
/* General options */
/* --------------- */
typedef struct marpaWrapperGrammarOption {
  genericLogger_t                   *genericLoggerp;             /* Default: NULL.                                      */
  short                              warningIsErrorb;            /* Default: 0. Have precedence over warningIsIgnoredb  */
  short                              warningIsIgnoredb;          /* Default: 0.                                         */
  marpaWrapperGrammarEventCallback_t eventDefaultCallbackp;      /* Default: NULL */
  void                              *eventDefaultCallbackDatavp; /* Default: NULL */
} marpaWrapperGrammarOption_t;

#ifdef __cplusplus
extern "C" {
#endif
  marpaWrapper_EXPORT marpaWrapperGrammar_t       *marpaWrapperGrammar_newp(marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp);
  marpaWrapper_EXPORT void                         marpaWrapperGrammar_freev(marpaWrapperGrammar_t *marpaWrapperGrammarp);

  marpaWrapper_EXPORT marpaWrapperGrammarSymbol_t *marpaWrapperGrammar_newSymbolp(marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp);
  marpaWrapper_EXPORT marpaWrapperGrammarRule_t   *marpaWrapperGrammar_newRulep(marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaWrapperGrammarRuleOption_t *marpaWrapperGrammarRuleOptionp,
										marpaWrapperGrammarSymbol_t *lhsSymbolp,
										size_t rhsSymboll, marpaWrapperGrammarSymbol_t **rhsSymbolpp
										);
  /* Handy methods to create symbols and rules that I find more user-friendly */
  marpaWrapper_EXPORT marpaWrapperGrammarSymbol_t *marpaWrapperGrammar_newSymbolExtp(marpaWrapperGrammar_t *marpaWrapperGrammarp, void *datavp, short terminalb, short startb, int eventSeti,
										     marpaWrapperGrammarEventCallback_t eventCallbackp,
										     void                              *eventCallbackDatavp);
  marpaWrapper_EXPORT marpaWrapperGrammarRule_t   *marpaWrapperGrammar_newRuleExtp(marpaWrapperGrammar_t *marpaWrapperGrammarp, void *datavp, int ranki, short nullRanksHighb,
										   marpaWrapperGrammarSymbol_t *lhsSymbolp, ...);
  marpaWrapper_EXPORT marpaWrapperGrammarRule_t   *marpaWrapperGrammar_newSequenceExtp(marpaWrapperGrammar_t *marpaWrapperGrammarp, void *datavp, int ranki, short nullRanksHighb,
										       marpaWrapperGrammarSymbol_t *lhsSymbolp,
										       marpaWrapperGrammarSymbol_t *rhsSymbolp, int minimumi,
										       marpaWrapperGrammarSymbol_t *separatorSymbolp, short properb);
  
  marpaWrapper_EXPORT short                        marpaWrapperGrammar_precomputeb(marpaWrapperGrammar_t *marpaWrapperGrammarp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAWRAPPER_GRAMMAR */
