#ifndef MARPAESLIF_INTERNAL_BOOTSTRAP_ACTIONS_H
#define MARPAESLIF_INTERNAL_BOOTSTRAP_ACTIONS_H

static void                            _marpaESLIF_bootstrap_freeCallbackv(void *userDatavp, int contexti, void *p);
static marpaESLIFValueRuleCallback_t   _marpaESLIF_bootstrap_ruleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static marpaESLIFValueSymbolCallback_t _marpaESLIF_bootstrap_symbolActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);


#endif /* MARPAESLIF_INTERNAL_BOOTSTRAP_ACTIONS_H */

