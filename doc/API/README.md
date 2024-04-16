# NAME

marpaESLIF\_API - Extended Marpa's Scanless Interface API

# DESCRIPTION

marpaESLIF is a Scanless Interface expressed in a BNF format, that is using [marpaWrapper](https://github.com/jddurand/c-marpaWrapper), itself being a thin interface on top of [libmarpa](https://jeffreykegler.github.io/Marpa-web-site/libmarpa.html) parser.

The whole API is available with this include:

```
#include <marpaESLIF.h>
```

There are four namespaces:

- ESLIF

    The main object.

- ESLIFGrammar

    Grammar creation and inspection.

- ESLIFRecognizer

    Parsing phase.

- ESLIFValue

    Valuation phase.

and a universal type:

```
typedef struct marpaESLIFString {
  char   *bytep;            /* pointer bytes */
  size_t  bytel;            /* number of bytes */
  char   *encodingasciis;   /* Encoding of bytes, itself in ASCII encoding, NUL byte terminated */
  char   *asciis;           /* ASCII transliteration - never NULL if bytep is not NULL */
} marpaESLIFString_t;
```

which describe what is a _string_:
this is a sequence of `bytel` bytes,
starting at `bytep` in memory,
in eventual `encodingasciis` encoding,
and transliterated as much as possible into `asciis`.

## ESLIF

Any application must start by instanciating a main ESLIF. This object is thread-safe, re-entrant, and can be reused as many times as needed, in contrary to other namespaces. The recommended usage is to create an ESLIF instance once for the entire process lifetime.

### `marpaESLIF_newp`

```
typedef struct marpaESLIFOption {
  genericLogger_t *genericLoggerp;  /* Logger. Default: NULL */
} marpaESLIFOption_t;

marpaESLIF_t *marpaESLIF_newp(marpaESLIFOption_t *marpaESLIFOptionp);
```

This method is returning a `marpaESLIF` object, or `NULL` in case of failure. The parameter `marpaESLIFOptionp` is a pointer to a `marpaESLIFOption_t` structure, or `NULL`. The `marpaESLIFOption_t` has, as of this version, the single following component:

- `genericLoggerp`

    A pointer to a [genericLogger](https://github.com/jddurand/c-genericLogger) instance. If it is `NULL`, then ESLIF will never log. Otherwise, all namespaces will inherit this logger object and use it.

### `marpaESLIF_versionb`

```
short marpaESLIF_versionb(marpaESLIF_t *marpaESLIFp, char **versionsp);
```

Puts in `versionsp`, if not `NULL`, a pointer to a `NUL` terminated _ASCII_ string containing ESLIF version, in the form `MAJOR.MINOR.PATCH`.

Returns a true value on failure, a false value on failure.

### `marpaESLIF_versionMajorb`

```
short marpaESLIF_versionMajorb(marpaESLIF_t *marpaESLIFp, int *versionMajorip);
```

Puts in `versionMajorip`, if not `NULL`, the major version of ESLIF.

Returns a true value on failure, a false value on failure.

### `marpaESLIF_versionMinorb`

```
short marpaESLIF_versionMinorb(marpaESLIF_t *marpaESLIFp, int *versionMinorip);
```

Puts in `versionMinorip`, if not `NULL`, the minor version of ESLIF.

Returns a true value on failure, a false value on failure.

### `marpaESLIF_versionPatchb`

```
short marpaESLIF_versionPatchb(marpaESLIF_t *marpaESLIFp, int *versionPatchip);
```

Puts in `versionPatchip`, if not `NULL`, the patch version of ESLIF.

Returns a true value on failure, a false value on failure.

### `marpaESLIF_extend_builtin_actionb`

```
short marpaESLIF_extend_builtin_actionb(marpaESLIF_t *marpaESLIFp,
                                        char **actionsArrayp,
                                        size_t actionsArrayl);
```

ESLIF grammar built-in action names extension is possible using this method, where:

- `actionsArrayp`

    is a pointer to an array of new actions built-ins, defined as terminals as per the ESLIF grammar. Action extensions are always of type `MARPAESLIF_ACTION_TYPE_NAME`.

- `actionsArrayl`

    is the number of elements in the array.

For example:

```
static const char *actionsArrayp[] = {
  "'%[]'",
  "'%{}'",
  "'%undef'",
  "'%true'",
  "'%false'",
};

marpaESLIF_extend_builtin_actionb(marpaESLIFp, (char **) actionsArrayp, sizeof(actionsArrayp)/sizeof(actionsArrayp[0]))) {
```

will add `%[]`, `%{}` etc... built-in action names, that will _not_ be managed by ESLIF anyway, but by the caller's valuation callback: the action name is a parameter of this callback. The implementation is hardcoding every new buil-in as a new rule in the BNF in this form:

```perl
<action name> ::= NEW_BUILTIN action => ::ascii
```

### `marpaESLIF_optionp`

```
marpaESLIFOption_t *marpaESLIF_optionp(marpaESLIF_t *marpaESLIFp);
```

Returns the options used to create the `marpaESLIFp` instance, or `NULL` if failure. The returned pointer must _never_ be freed.

### `marpaESLIF_grammarp`

```
marpaESLIFGrammar_t *marpaESLIF_grammarp(marpaESLIF_t *marpaESLIFp);
```

Returns the ESLIF grammar itself, or `NULL` if failure. The returned pointer must _never_ be freed.

### `marpaESLIF_numberb`

```
marpaESLIF_EXPORT short marpaESLIF_numberb(marpaESLIF_t *marpaESLIFp, char *s, size_t sizel, marpaESLIFValueResult_t *marpaESLIFValueResultp, short *confidencebp);
```

Tries to parse the _ASCII_ string pointed by `s` of size `sizel` bytes (if `sizel` is 0, ESLIF will compute automatically the length) as a JSON relaxed number, i.e. with possible leading `+` and zeroes. Returns in `marpaESLIFValueResultp`, when not `NULL`, the result, and in `confidencebp`, when not `NULL`, a true number if ESLIF is sure. The value type in `marpaESLIFValueResultp` is guaranteed to be one of these:

- MARPAESLIF\_VALUE\_TYPE\_DOUBLE
- MARPAESLIF\_VALUE\_TYPE\_SHORT
- MARPAESLIF\_VALUE\_TYPE\_INT
- MARPAESLIF\_VALUE\_TYPE\_LONG
- MARPAESLIF\_VALUE\_TYPE\_LONG\_LONG
- MARPAESLIF\_VALUE\_TYPE\_LONG\_DOUBLE
- MARPAESLIF\_VALUE\_TYPE\_UNDEF

`MARPAESLIF_VALUE_TYPE_UNDEF` indicates a failure, and always have `*confidencebp` to `0`.

Returns a true value on success, a false value otherwise.

### `marpaESLIFValueResult_isinfb`

```
short marpaESLIFValueResult_isinfb(marpaESLIF_t *marpaESLIFp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
```

Returns a true value if `marpaESLIFp` and `marpaESLIFValueResultp` arguments are correct and if `marpaESLIFValueResultp` hosts an infinity number.

### `marpaESLIFValueResult_isnanb`

```
short marpaESLIFValueResult_isnanb(marpaESLIF_t *marpaESLIFp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
```

Returns a true value if `marpaESLIFp` and `marpaESLIFValueResultp` arguments are correct and if `marpaESLIFValueResultp` hosts a NaN.

### `marpaESLIFValueResult_is_positive_nanb`

```
short marpaESLIFValueResult_is_positive_nanb(marpaESLIF_t *marpaESLIFp, marpaESLIFValueResult_t *marpaESLIFValueResultp, short *confidencebp);
```

Returns a true value if `marpaESLIFp` and `marpaESLIFValueResultp` arguments are correct and if `marpaESLIFValueResultp` hosts a positive NaN number. ESLIF returns a true value in `confidenceb` when it is sure, a false otherwise. The false value happens when getting the `NaN` is not sure about the sign.

### `marpaESLIFValueResult_is_negative_nanb`

```
short marpaESLIFValueResult_is_negative_nanb(marpaESLIF_t *marpaESLIFp, marpaESLIFValueResult_t *marpaESLIFValueResultp, short *confidencebp);
```

Returns a true value if `marpaESLIFp` and `marpaESLIFValueResultp` arguments are correct and if `marpaESLIFValueResultp` hosts a negative NaN number. ESLIF returns a true value in `confidenceb` when it is sure, a false otherwise. The false value happens when getting the `NaN` is not sure about the sign.

### `marpaESLIF_freev`

```
void marpaESLIF_freev(marpaESLIF_t *marpaESLIFp);
```

The destructor of a marpaESLIF instance `marpaESLIFp` instance. No-op if `marpaESLIFp` is `NULL`.

## ESLIFGrammar

An _ESLIFGrammar_ is a sparse array of grammars, each is identified by a _level_, that is the indice in this array, i.e. `0`, `1`, and so on.

Most of the APIs in this section have two forms, one for the _current_ grammar, one for a grammar at a wanted _indice_ or _description_. A _description_ is a string identifying a grammar, set using the `:desc` keyword, e.g.:

```
:desc :[0]: 'Description of the grammar at level 0'
:desc :[7]: 'Description of the grammar at level 7'
```

The default current grammar is always at level `0`, that must exist. Level <0> is naturally considered like the _top level_ grammar. Nevertheless it is possible to instanciate a parsing at a _different_ level, then the _current grammar_ in derived namespaces like the recognizer or the valuation is _the level you asked for_. For example:

```
A :[0]:= X          # Could have been A ::= X, '::=' is an alias for ':[0]:'
X :[1]:= Y@+2       # Could have been X ~ Y@+1, '~' is an alias for ':[1]:'
Y :[3]:= 'Z'
```

declares grammars at level `0`, `1` and `3`. Grammar at level `2` is undefined.

### `marpaESLIFGrammar_newp`

```
typedef struct marpaESLIFGrammarOption {
  void   *bytep;               /* Input */
  size_t  bytel;               /* Input length in byte unit */
  char   *encodings;           /* Input encoding. Default: NULL */
  size_t  encodingl;           /* Length of encoding itself. Default: 0 */
} marpaESLIFGrammarOption_t;

marpaESLIFGrammar_t *marpaESLIFGrammar_newp(marpaESLIF_t              *marpaESLIFp,
                                            marpaESLIFGrammarOption_t *marpaESLIFGrammarOptionp);
```

Creates a grammar object instance, with parameters:

- `marpaESLIFp`

    An &lt;required> ESLIF object instance.

- `marpaESLIFGrammarOptionp`

    A _required_ pointer to a `marpaESLIFGrammarOption_t` structure, defined as:

    - `bytep`

        A pointer to the start of the grammar. Must be non-NULL.

    - `bytel`

        Number of **bytes**. Must be > 0.

    - `encodings`

        Pointer to a buffer containing encoding information of the grammar, in ASCII. Can be NULL.

    - `encodingl`

        Number of bytes of the buffer containing encoding information. Can be 0.

Grammars themselves are assumed to always fit within a single block of memory. So there is no _grammar reader_ here: we assume that a single buffer starting at `bytep`, spanning over `bytel` bytes, is always possible and enough for a grammar definition.

Returns `NULL` if failure, a grammar object instance on success.

### `marpaESLIFGrammar_eslifp`

```
marpaESLIF_t *marpaESLIFGrammar_eslifp(marpaESLIFGrammar_t *marpaESLIFGrammarp);
```

Returns the ESLIF parent object instance of `marpaESLIFGrammarp`, or `NULL` if failure. It never returns `NULL` if `marpaESLIFGrammarp` is not `NULL`.

### `marpaESLIFGrammar_optionp`

```
marpaESLIFGrammarOption_t *marpaESLIFGrammar_optionp(marpaESLIFGrammar_t *marpaESLIFGrammarp);
```

Returns the options used to create the `marpaESLIFGrammarp` instance, or `NULL` if failure. The returned pointer must _never_ be freed.

### `marpaESLIFGrammar_ngrammarib`

```
short marpaESLIFGrammar_ngrammarib(marpaESLIFGrammar_t *marpaESLIFGrammarp,
                                   int                 *ngrammarip);
```

Puts the number of sub-grammars in the value pointed by `ngrammarip`, which can be `NULL`. The number of grammars is the highest level plus one, and it is **allowed** to have undefined grammars at any indice but level 0. For example:

```
A :[0]:= X          # First grammar
X :[1]:= Y@+2       # Second grammar
                    # Third grammar (undefined)
Y :[3]:= 'Z'        # Fourth grammar
```

would return `4`: there are four grammars, the third one being undefined.

Returns a true value on failure, a false value on failure.

### `marpaESLIFGrammar_defaultsb`

```
typedef enum marpaESLIFActionType {
  MARPAESLIF_ACTION_TYPE_NAME = 0,
  MARPAESLIF_ACTION_TYPE_STRING,
  MARPAESLIF_ACTION_TYPE_LUA,
  MARPAESLIF_ACTION_TYPE_LUA_FUNCTION
} marpaESLIFActionType_t;

typedef struct marpaESLIFAction {
  marpaESLIFActionType_t type;
  union {
    char               *names;
    marpaESLIFString_t *stringp;
    char               *luas;
    char               *luaFunctions;
  } u;
} marpaESLIFAction_t;

typedef struct marpaESLIFGrammarDefaults {
  marpaESLIFAction_t *defaultRuleActionp;   /* Default action for rules */
  marpaESLIFAction_t *defaultSymbolActionp; /* Default action for symbols */
  marpaESLIFAction_t *defaultEventActionp;  /* Default action for events */
  marpaESLIFAction_t *defaultRegexActionp;  /* Default action for regex */
  char               *defaultEncodings;     /* Default encoding */
  char               *fallbackEncodings;    /* Fallback encoding */
} marpaESLIFGrammarDefaults_t;

short marpaESLIFGrammar_defaultsb(marpaESLIFGrammar_t         *marpaESLIFGrammarp,
                                  marpaESLIFGrammarDefaults_t *marpaESLIFGrammarDefaultsp);
```

Puts in the structure pointed by `marpaESLIFGrammarDefaultsp`, if it is not `NULL`, the current grammar defaults, all of them being a structure of type `marpaESLIFAction_t` or `NULL` when not set. An action can be of two types:

- `MARPAESLIF_ACTION_TYPE_NAME`

    A named action, that the end-user will be responsible to resolve to valid a function pointer. Action name itself is an ASCII NUL terminated string accessible in `u.names`.

- `MARPAESLIF_ACTION_TYPE_STRING`

    A string literal. Then the action result is forced to be the UTF-8 corresponding string, always pushed with type `MARPAESLIF_VALUE_TYPE_ARRAY` on the stack. String literals are illegals for the free action, legal for rule and symbol actions. String itself is a `marpaESLIFString_t` structure pointer accessible in `u.stringp`.

- `MARPAESLIF_ACTION_TYPE_LUA`

    A string literal. Then the action result is forced to be a lua function, using an embedded lua interpreter that ESLIF provides. Such function should be enclosed in a statement like e.g.:

    ```
    <luascript>
      function thefunction(arguments)
      end
    </luascript>
    ```

    where there will be as many arguments as there are Right-Hand side values.

- `MARPAESLIF_ACTION_TYPE_LUA_FUNCTION`

    A function lua 5.3.4 source. The action is an anonymous function that is reloaded and executed everytime, using an embedded lua interpreter that ESLIF provides. Such function should be declare like e.g.:

    ```perl
    action => ::lua->function(s)
                       -- Lua semantics!
                       print('Argument: '..tostring(s))
                       return s
                     end
    ```

    Lua 5.3.4 semantics are applied _immediately_ after the opening parenthesis, up to the end of a function body parsing as per the Lua 5.3.4 grammar. ESLIF semantics takes over after the `end` lua keyword that corresponds to the end of the lua function.

- defaultRuleActionp

    Default action for symbols, and correspond to the grammar keyword `action`. Can be a name of a string literal.

- defaultSymbolActionp

    Default action for symbols, and correspond to the grammar keyword `symbol-action`. Can be a name of a string literal.

- defaultEventActionp

    Default action for events, and correspond to the grammar keyword `event-action`. Can be a name of a string literal.

- defaultRegexActionp

    Default action for regex callout, and correspond to the grammar keyword `regex-action`. Can be a name of a string literal.

The keywords mentionned upper are set within a _:default_ statement, e.g.:

```perl
:default ::= action        => myRuleAction
             symbol-action => ::u8"String Literal"
```

Returns a true value on failure, a false value on failure.

### `marpaESLIFGrammar_defaults_by_levelb`

```
short marpaESLIFGrammar_defaults_by_levelb(marpaESLIFGrammar_t         *marpaESLIFGrammarp,
                                           marpaESLIFGrammarDefaults_t *marpaESLIFGrammarDefaultsp,
                                           int                          leveli,
                                           marpaESLIFString_t          *descp);
```

Put in the structure pointed by `marpaESLIFGrammarDefaultsp`, if it is not `NULL`, the grammar at level `leveli` and/or description `descp` defaults, where a `NULL` value for a default means it is not set.

Returns a true value on failure, a false value on failure.

### `marpaESLIFGrammar_defaults_setb`

```
short marpaESLIFGrammar_defaults_setb(marpaESLIFGrammar_t         *marpaESLIFGrammarp,
                                      marpaESLIFGrammarDefaults_t *marpaESLIFGrammarDefaultsp);
```

Sets all grammar defaults using the structure pointed by `marpaESLIFGrammarDefaultsp`, if it is not `NULL`. A `NULL` value for a default has the same effect as if original grammar never had the associated keyword.

Returns a true value on failure, a false value on failure.

### `marpaESLIFGrammar_defaults_by_level_setb`

```
short marpaESLIFGrammar_defaults_by_level_setb(marpaESLIFGrammar_t         *marpaESLIFGrammarp,
                                               marpaESLIFGrammarDefaults_t *marpaESLIFGrammarDefaultsp,
                                               int                          leveli,
                                               marpaESLIFString_t          *descp);
```

Puts in the structure pointed by `marpaESLIFGrammarDefaultsp`, if it is not `NULL`. A `NULL` value for a default has the same effect as if original grammar never had the associated keyword.

Returns a true value on failure, a false value on failure.

### `marpaESLIFGrammar_grammar_by_levelb`

```
short marpaESLIFGrammar_grammar_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp,
                                          int                  leveli,
                                          marpaESLIFString_t  *descp,
                                          int                 *levelip,
                                          marpaESLIFString_t **descpp);
```

Put in the values pointed by `levelip` and `descpp`, which can be both `NULL`, the level and description of wanted grammar from `marpaESLIFGrammarp`:

- `leveli`

    is the eventual explicit wanted level, ignored if lower than `0`.

- `descp`

    is the eventual explicit description, ignored if `NULL`.

When both `leveli` and `descp` are set, `descp` have precedence over `leveli`.

Returns a true value on failure, a false value on failure.

### `marpaESLIFGrammar_grammarproperty_currentb`

```
typedef struct marpaESLIFGrammarProperty {
  int                    leveli;                       /* Grammar level */
  marpaESLIFString_t    *descp;                        /* Grammar description (auto-generated if none) */
  short                  latmb;                        /* LATM ? */
  marpaESLIFAction_t    *defaultSymbolActionp;         /* Default action for symbols - never NULL */
  marpaESLIFAction_t    *defaultRuleActionp;           /* Default action for rules - never NULL */
  marpaESLIFAction_t    *defaultEventActionp;          /* Default action for event - can be NULL */
  marpaESLIFAction_t    *defaultRegexActionp;          /* Default action for regex - can be NULL */
  int                    starti;                       /* Start symbol Id - always >= 0 */
  int                    discardi;                     /* Discard symbol Id (-1 if none) */
  size_t                 nsymboll;                     /* Number of symbols - always > 0*/
  int                   *symbolip;                     /* Array of symbols Ids - never NULL */
  size_t                 nrulel;                       /* Number of rules - always > 0*/
  int                   *ruleip;                       /* Array of rule Ids - never NULL */
} marpaESLIFGrammarProperty_t;

marpaESLIFGrammar_grammarproperty_currentb(marpaESLIFGrammar_t *marpaESLIFGrammarp,
                                           marpaESLIFGrammarProperty_t *grammarPropertyp);
```

Put in the value pointed by `grammarPropertyp`, if it is not `NULL`, current `marpaESLIFGrammarp`'s grammar properties.

### `marpaESLIFGrammar_grammarproperty_by_levelb`

```
marpaESLIFGrammar_grammarproperty_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp,
                                            marpaESLIFGrammarProperty_t *grammarPropertyp,
                                            int leveli,
                                            marpaESLIFString_t *descp);
```

Put in the value pointed by `grammarPropertyp`, if it is not `NULL`, `marpaESLIFGrammarp`'s grammar's at level `leveli` or with description `descp` properties.

### `marpaESLIFGrammar_rulearray_currentb`

```
short marpaESLIFGrammar_rulearray_currentb(marpaESLIFGrammar_t  *marpaESLIFGrammarp,
                                           int                 **ruleipp,
                                           size_t               *rulelp);
```

Puts in the values pointed by `ruleipp` and `rulelp` a pointer to an array of integer, and the number of them, respectively, of current grammar. User must **not** try to free `*ruleipp`. Please note that this is exactly the same information as the tuple `{nrulel,ruleip`} returned by `marpaESLIFGrammar_grammarproperty_currentb`.

Returns a true value on failure, a false value on failure. `*ruleipp` is never `NULL` in case of success.

### `marpaESLIFGrammar_rulearray_by_levelb`

```
short marpaESLIFGrammar_rulearray_by_levelb(marpaESLIFGrammar_t  *marpaESLIFGrammarp,
                                            int                 **ruleipp,
                                            size_t               *rulelp,
                                            int                   leveli,
                                            marpaESLIFString_t   *descp);
```

Put in the values pointed by `ruleipp` and `rulelp` a pointer to an array of integer, and the number of them, respectively, of the grammar referenced by `leveli` or `descp`. 

- `leveli`

    is the eventual explicit wanted level, ignored if lower than `0`.

- `descp`

    is the eventual explicit description, ignored if `NULL`.

When both `leveli` and `descp` are set, `descp` have precedence over `leveli`.

Returns a true value on failure, a false value on failure. `*ruleipp` is never `NULL` in case of success.

### `marpaESLIFGrammar_ruleproperty_currentb`

```
typedef enum marpaESLIFRulePropertyBit {
  MARPAESLIF_RULE_IS_ACCESSIBLE = 0x01,
  MARPAESLIF_RULE_IS_NULLABLE   = 0x02,
  MARPAESLIF_RULE_IS_NULLING    = 0x04,
  MARPAESLIF_RULE_IS_LOOP       = 0x08,
  MARPAESLIF_RULE_IS_PRODUCTIVE = 0x10
} marpaESLIFRulePropertyBit_t;

typedef struct marpaESLIFRuleProperty {
  int                    idi;                          /* Rule Id */
  marpaESLIFString_t    *descp;                        /* Rule alternative name (auto-generated if none) */
  char                  *asciishows;                   /* Rule show (ASCII) */
  int                    lhsi;                         /* LHS symbol Id */
  int                    separatori;                   /* Eventual separator symbol Id (-1 if none) */
  size_t                 nrhsl;                        /* Number of RHS, 0 in case of a nullable */
  int                   *rhsip;                        /* Array of RHS Ids, NULL in case of a nullable */
  int                    exceptioni;                   /* Exception symbol Id (-1 if none) */
  marpaESLIFAction_t    *actionp;                      /* Action */
  char                  *discardEvents;                /* Discard event name - shallowed with its RHS */
  short                  discardEventb;                /* Discard event initial state: 0: off, 1: on */
  int                    ranki;                        /* Rank */
  short                  nullRanksHighb;               /* Null ranks high ? */
  short                  sequenceb;                    /* Sequence ? */
  short                  properb;                      /* Proper ? */
  int                    minimumi;                     /* minimum in case of sequence ? */
  short                  internalb;                    /* This rule is internal */
  int                    propertyBitSet;               /* C.f. marpaESLIFRulePropertyBit_t */
} marpaESLIFRuleProperty_t;


short marpaESLIFGrammar_ruleproperty_currentb(marpaESLIFGrammar_t *marpaESLIFGrammarp,
                                              int rulei,
                                              marpaESLIFRuleProperty_t *rulePropertyp);
```

Puts in the value pointed by `rulePropertyp`, if it is not `NULL`, properties of rule number `rulei` of current `marpaESLIFGrammarp`'s grammar.

Returns a true value on failure, a false value on failure.

### `marpaESLIFGrammar_ruleproperty_by_levelb`

```
short marpaESLIFGrammar_ruleproperty_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp,
                                               int rulei,
                                               marpaESLIFRuleProperty_t *rulePropertyp,
                                               int leveli,
                                               marpaESLIFString_t *descp);
```

Puts in the value pointed by `rulePropertyp`, if it is not `NULL`, properties of rule number `rulei` of `marpaESLIFGrammarp`'s grammar's level `leveli` or description `descp`.

Returns a true value on failure, a false value on failure.

### `marpaESLIFGrammar_ruledisplayform_currentb`

```
short marpaESLIFGrammar_ruledisplayform_currentb(marpaESLIFGrammar_t  *marpaESLIFGrammarp,
                                                 int                   rulei,
                                                 char                **ruledisplaysp);
```

Put in the value pointed by `ruledisplaysp`, which can be `NULL`, a NUL-terminated _ASCII encoded_ version of rule name at indice `rulei` of the current grammar.

Returns a true value on failure, a false value on failure.

Example of a rule display form:

```
<lstring>
```

### `marpaESLIFGrammar_ruledisplayform_by_levelb`

```
short marpaESLIFGrammar_ruledisplayform_by_levelb(marpaESLIFGrammar_t  *marpaESLIFGrammarp,
                                                  int                   rulei,
                                                  char                **ruledisplaysp,
                                                  int                   leveli,
                                                  marpaESLIFString_t   *descp);
```

Put in the value pointed by `ruledisplaysp`, which can be `NULL`, a NUL-terminated _ASCII encoded_ version of rule name at indice `rulei` of the wanted grammar from `marpaESLIFGrammarp`:

- `leveli`

    is the eventual explicit wanted level, ignored if < `0`.

- `descp`

    is the eventual explicit description, ignored if `NULL`.

`descp` have precedence of `leveli`.

Return a true value on failure, a false value on failure.

### `marpaESLIFGrammar_symbolarray_currentb`

```
short marpaESLIFGrammar_symbolarray_currentb(marpaESLIFGrammar_t  *marpaESLIFGrammarp,
                                             int                 **symbolipp,
                                             size_t               *symbollp);
```

Puts in the values pointed by `symbolipp` and `symbollp` a pointer to an array of integer, and the number of them, respectively, of current grammar. User must **not** try to free `*symbolipp`. Please note that this is exactly the same information as the tuple `{nsymboll,symbolip`} returned by `marpaESLIFGrammar_grammarproperty_currentb`.

Returns a true value on failure, a false value on failure. `*symbolipp` is never `NULL` in case of success.

### `marpaESLIFGrammar_symbolarray_by_levelb`

```
short marpaESLIFGrammar_symbolarray_by_levelb(marpaESLIFGrammar_t  *marpaESLIFGrammarp,
                                              int                 **symbolipp,
                                              size_t               *symbollp,
                                              int                   leveli,
                                              marpaESLIFString_t   *descp);
```

Put in the values pointed by `symbolipp` and `symbollp` a pointer to an array of integer, and the number of them, respectively, of the grammar referenced by `leveli` or `descp`. 

- `leveli`

    is the eventual explicit wanted level, ignored if lower than `0`.

- `descp`

    is the eventual explicit description, ignored if `NULL`.

When both `leveli` and `descp` are set, `descp` have precedence over `leveli`.

Returns a true value on failure, a false value on failure. `*symbolipp` is never `NULL` in case of success.

### `marpaESLIFGrammar_symbolproperty_currentb`

```perl
typedef enum marpaESLIFSymbolPropertyBit {
  MARPAESLIF_SYMBOL_IS_ACCESSIBLE = 0x01,
  MARPAESLIF_SYMBOL_IS_NULLABLE   = 0x02,
  MARPAESLIF_SYMBOL_IS_NULLING    = 0x04,
  MARPAESLIF_SYMBOL_IS_PRODUCTIVE = 0x08,
  MARPAESLIF_SYMBOL_IS_START      = 0x10,
  MARPAESLIF_SYMBOL_IS_TERMINAL   = 0x20
} marpaESLIFSymbolPropertyBit_t;

typedef enum marpaESLIFSymbolType {
  MARPAESLIF_SYMBOLTYPE_TERMINAL,
  MARPAESLIF_SYMBOLTYPE_META
} marpaESLIFSymbolType_t;

typedef struct marpaESLIFSymbolProperty {
  marpaESLIFSymbolType_t       type;                   /* Symbol type */
  short                        startb;                 /* Start symbol ? */
  short                        discardb;               /* Discard LHS symbol (i.e. :discard) ? */
  short                        discardRhsb;            /* Discard RHS symbol ? */
  short                        lhsb;                   /* Is an LHS somewhere in its grammar ? */
  short                        topb;                   /* Is a top-level symbol in its grammar - implies lhsb */
  int                          idi;                    /* Marpa ID */
  marpaESLIFString_t          *descp;                  /* Symbol description */
  char                        *eventBefores;           /* Pause before */
  short                        eventBeforeb;           /* Pause before initial state: 0: off, 1: on */
  char                        *eventAfters;            /* Pause after */
  short                        eventAfterb;            /* Pause after initial state: 0: off, 1: on */
  char                        *eventPredicteds;        /* Event name for prediction */
  short                        eventPredictedb;        /* Prediction initial state: 0: off, 1: on */
  char                        *eventNulleds;           /* Event name for nulled */
  short                        eventNulledb;           /* Nulled initial state: 0: off, 1: on */
  char                        *eventCompleteds;        /* Event name for completion */
  short                        eventCompletedb;        /* Completion initial state: 0: off, 1: on */
  char                        *discardEvents;          /* Discard event name - shallow pointer to a :discard rule's discardEvents */
  short                        discardEventb;          /* Discard event initial state: 0: off, 1: on - copy of :discard's rule value */
  int                          lookupResolvedLeveli;   /* Resolved grammar level */
  int                          priorityi;              /* Symbol priority */
  short                        verboseb;               /* Symbol is verbose */
  marpaESLIFAction_t          *nullableActionp;        /* Nullable semantic */
  int                          propertyBitSet;
  int                          eventBitSet;
  marpaESLIFAction_t          *symbolActionp;          /* Symbol specific action */
  marpaESLIFAction_t          *ifActionp;              /* Symbol if action */
} marpaESLIFSymbolProperty_t;

short marpaESLIFGrammar_symbolproperty_currentb(marpaESLIFGrammar_t *marpaESLIFGrammarp,
                                                int symboli,
                                                marpaESLIFSymbolProperty_t *symbolPropertyp);
```

Puts in the value pointed by `symbolPropertyp`, if it is not `NULL`, properties of symbol number `symboli` of current `marpaESLIFGrammarp`'s grammar.

Returns a true value on failure, a false value on failure.

### `marpaESLIFGrammar_symbolproperty_by_levelb`

```
short marpaESLIFGrammar_symbolproperty_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp,
                                                 int symboli,
                                                 marpaESLIFSymbolProperty_t *symbolPropertyp,
                                                 int leveli,
                                                 marpaESLIFString_t *descp);
```

Puts in the value pointed by `symbolPropertyp`, if it is not `NULL`, properties of symbol number `symboli` of `marpaESLIFGrammarp`'s grammar's level `leveli` or description `descp`.

Returns a true value on failure, a false value on failure.

### `marpaESLIFGrammar_symboldisplayform_currentb`

```
short marpaESLIFGrammar_symboldisplayform_currentb(marpaESLIFGrammar_t  *marpaESLIFGrammarp,
                                                   int                   symboli,
                                                   char                **symboldisplaysp);
```

Put in the value pointed by `symboldisplaysp`, which can be `NULL`, a NUL-terminated _ASCII encoded_ version of symbol name at indice `symboli` of the current grammar from `marpaESLIFGrammarp`.

Return a true value on failure, a false value on failure.

Example of a symbol display form:

```
<in_string>
```

### `marpaESLIFGrammar_symboldisplayform_by_levelb`

```
short marpaESLIFGrammar_symboldisplayform_by_levelb(marpaESLIFGrammar_t  *marpaESLIFGrammarp,
                                                    int                   symboli,
                                                    char                **symboldisplaysp,
                                                    int                   leveli,
                                                    marpaESLIFString_t   *descp);
```

Put in the value pointed by `symboldisplaysp`, which can be `NULL`, a NUL-terminated _ASCII encoded_ version of symbol name at indice `symboli` of the wanted grammar from `marpaESLIFGrammarp`:

- `leveli`

    is the eventual explicit wanted level, ignored if < `0`.

- `descp`

    is the eventual explicit description, ignored if `NULL`.

`descp` have precedence of `leveli`.

Return a true value on failure, a false value on failure.

### `marpaESLIFGrammar_ruleshowform_currentb`

```
short marpaESLIFGrammar_ruleshowform_currentb(marpaESLIFGrammar_t  *marpaESLIFGrammarp,
                                              int                   rulei,
                                              char                **ruleshowsp);
```

Put in the value pointed by `ruleshowsp`, which can be `NULL`, a NUL-terminated _ASCII encoded_ transliteration of rule definition at indice `rulei` of the current grammar from `marpaESLIFGrammarp`.

Return a true value on failure, a false value on failure.

Example of a rule show form:

```
<lstring> ~ <quote> <in_string> <quote>
```

### `marpaESLIFGrammar_ruleshowform_by_levelb`

```
short marpaESLIFGrammar_ruleshowform_by_levelb(marpaESLIFGrammar_t  *marpaESLIFGrammarp,
                                                  int                   rulei,
                                                  char                **ruleshowsp,
                                                  int                   leveli,
                                                  marpaESLIFString_t   *descp);
```

Put in the value pointed by `ruleshowsp`, which can be `NULL`, a NUL-terminated _ASCII encoded_ transliteration of rule definition at indice `rulei` of the wanted grammar from `marpaESLIFGrammarp`:

- `leveli`

    is the eventual explicit wanted level, ignored if < `0`.

- `descp`

    is the eventual explicit description, ignored if `NULL`.

`descp` have precedence of `leveli`.

Return a true value on failure, a false value on failure.

### `marpaESLIFGrammar_grammarshowform_currentb`

```
short marpaESLIFGrammar_grammarshowform_currentb(marpaESLIFGrammar_t  *marpaESLIFGrammarp,
                                                 int                   grammari,
                                                 char                **grammarshowsp);
```

Put in the value pointed by `grammarshowsp`, which can be `NULL`, a NUL-terminated _ASCII encoded_ transliteration of grammar definition at indice `grammari` of the current grammar from `marpaESLIFGrammarp`. This is very exhaustive, and give internal information about all symbols and rules. See the NOTES section.

Return a true value on failure, a false value on failure.

### `marpaESLIFGrammar_grammarshowform_by_levelb`

```
short marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammar_t  *marpaESLIFGrammarp,
                                                  int                   grammari,
                                                  char                **grammarshowsp,
                                                  int                   leveli,
                                                  marpaESLIFString_t   *descp);
```

Put in the value pointed by `grammarshowsp`, which can be `NULL`, a NUL-terminated _ASCII encoded_ transliteration of grammar definition at indice `grammari` of the wanted grammar from `marpaESLIFGrammarp`:

- `leveli`

    is the eventual explicit wanted level, ignored if < `0`.

- `descp`

    is the eventual explicit description, ignored if `NULL`.

`descp` have precedence of `leveli`.

This is very exhaustive, and give internal information about all symbols and rules. See the NOTES section.

Return a true value on failure, a false value on failure.

### `marpaESLIFGrammar_grammarshowscriptb`
  short marpaESLIFGrammar\_grammarshowscriptb(marpaESLIFGrammar\_t \*marpaESLIFGrammarp, char \*\*grammarscriptsp);

Put in the value pointed by `grammarscriptsp`, which can be `NULL`, a NUL-terminated ASCII string containing all lua statements.

Return a true value on failure, a false value on failure.

### `marpaESLIFGrammar_parseb`

```
short marpaESLIFGrammar_parseb(marpaESLIFGrammar_t          *marpaESLIFGrammarp,
                               marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp,
                               marpaESLIFValueOption_t      *marpaESLIFValueOptionp,
                               short                        *isExhaustedbp);
```

Please refer to the documentation of:

- `marpaESLIFRecognizer_newp`

    for the `marpaESLIFRecognizerOption_t` structure documentation.

- `marpaESLIFValue_newp`

    for the `marpaESLIFValueOption_t` structures documentation.

A short form of parsing and valuation, in which _no event_ but _if_ or _regexp_ callback is possible: the entire scanning is left to the marpaESLIF engine. The grammar at level `0` is used.

The value pointed by `isExhaustedbp`, when not `NULL`, is filled with a true value if the parsing exhausted (exhaustion roughly mean that the parsing stopped without consuming all the data), a false value otherwise. This flag does not depend on the existence of an exhaustion event or not.

Returns a true value on success, a false value otherwise.

### `marpaESLIFGrammar_parse_by_levelb`

```
short marpaESLIFGrammar_parse_by_levelb(marpaESLIFGrammar_t          *marpaESLIFGrammarp,
                                        marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp,
                                        marpaESLIFValueOption_t      *marpaESLIFValueOptionp,
                                        short                        *isExhaustedbp,
                                        int                           leveli,
                                        marpaESLIFString_t           *descp);
```

Same functionnality as `marpaESLIFGrammar_parseb` for a wanted grammar from `marpaESLIFGrammarp`, with these additional parameters:

- `leveli`

    is the eventual explicit wanted level, ignored if < `0`.

- `descp`

    is the eventual explicit description, ignored if `NULL`.

`descp` have precedence of `leveli`.

### `marpaESLIFGrammar_freev`

```
void marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp);
```

The destructor of a marpaESLIFGrammar instance `marpaESLIFGrammarp` instance. No-op if `marpaESLIFGrammarp` is `NULL`.

## ESLIF Value Result

Before going on with the recognizer and valation namespaces, the notion of value result should be explicited first.

A Value result holds two notions:

- A value type
- How the value is _represented_

Supported value types are in the enumeration `marpaESLIFValueType_t`:

```
typedef enum marpaESLIFValueType {
  MARPAESLIF_VALUE_TYPE_UNDEF = 0,
  MARPAESLIF_VALUE_TYPE_CHAR,
  MARPAESLIF_VALUE_TYPE_SHORT,
  MARPAESLIF_VALUE_TYPE_INT,
  MARPAESLIF_VALUE_TYPE_LONG,
  MARPAESLIF_VALUE_TYPE_FLOAT,
  MARPAESLIF_VALUE_TYPE_DOUBLE,
  MARPAESLIF_VALUE_TYPE_PTR,
  MARPAESLIF_VALUE_TYPE_ARRAY,
  MARPAESLIF_VALUE_TYPE_BOOL,
  MARPAESLIF_VALUE_TYPE_STRING,
  MARPAESLIF_VALUE_TYPE_ROW,
  MARPAESLIF_VALUE_TYPE_TABLE,
  MARPAESLIF_VALUE_TYPE_LONG_DOUBLE
#ifdef MARPAESLIF_HAVE_LONG_LONG
  ,MARPAESLIF_VALUE_TYPE_LONG_LONG
#endif
} marpaESLIFValueType_t;
```

The names are quite explicit:

- MARPAESLIF\_VALUE\_TYPE\_UNDEF

    Value is undefined.

- MARPAESLIF\_VALUE\_TYPE\_CHAR

    Value type is `marpaESLIFValueResultChar_t`, typedef'ed to: `char`.

- MARPAESLIF\_VALUE\_TYPE\_SHORT

    Value type is `marpaESLIFValueResultShort_t`, typedef'ed to: `short`.

- MARPAESLIF\_VALUE\_TYPE\_INT

    Value type is `marpaESLIFValueResultInt_t`, typedef'ed to: `int`.

- MARPAESLIF\_VALUE\_TYPE\_LONG

    Value type is `marpaESLIFValueResultLong_t`, typedef'ed to: `long`.

- MARPAESLIF\_VALUE\_TYPE\_FLOAT

    Value type is `marpaESLIFValueResultFloat_t`, typedef'ed to: `float`.

- MARPAESLIF\_VALUE\_TYPE\_DOUBLE

    Value type is `marpaESLIFValueResultDouble_t`, typedef'ed to: `double`.

- MARPAESLIF\_VALUE\_TYPE\_PTR

    Value type is `marpaESLIFValueResultPtr_t`, typedef'ed to: `struct { void *p; short shallowb; }`.

- MARPAESLIF\_VALUE\_TYPE\_ARRAY

    Value type is `marpaESLIFValueResultArray_t`, typedef'ed to: `struct { void *p; short shallowb; size_t sizel; }`.

- MARPAESLIF\_VALUE\_TYPE\_BOOL

    Value type is `marpaESLIFValueResultBool_t`, typedef'ed to: `enum`. Recommended values should be `MARPAESLIFVALUERESULTBOOL_FALSE` (i.e. `0`) or `MARPAESLIFVALUERESULTBOOL_TRUE` (i.e. `1`).

- MARPAESLIF\_VALUE\_TYPE\_STRING

    Value type is `marpaESLIFValueResultString_t`, typedef'ed to: `struct { unsigned char *p; short shallowb; size_t sizel; char *encodingasciis; }`. `p` and `encodingasciis` _must_ be set, even for an empty string.

- MARPAESLIF\_VALUE\_TYPE\_ROW

    Value type is `marpaESLIFValueResultRow_t`, naively corresponding the notion of array in any traditional language, and typedef'ed to:

    ```
    typedef struct marpaESLIFValueResultRow {
      marpaESLIFValueResult_t             *p;
      void                                *freeUserDatavp;
      marpaESLIFValueResultFreeCallback_t  freeCallbackp;
      short                                shallowb;
      size_t                               sizel;
    } marpaESLIFValueResultRow_t;
    ```

    where `p` is an array of `marpaESLIFValueResult_t` values, of size `sizel`, that can be zero.

- MARPAESLIF\_VALUE\_TYPE\_TABLE

    Value type is `marpaESLIFValueResultTable_t`, naively corresponding the notion of hash in any language that supports it, i.e. a key and a value, typedef'ed to:

    ```
    typedef struct marpaESLIFValueResultPair marpaESLIFValueResultPair_t;
    typedef struct marpaESLIFValueResultTable {
      marpaESLIFValueResultPair_t         *p;
      void                                *freeUserDatavp;
      marpaESLIFValueResultFreeCallback_t  freeCallbackp;
      short                                shallowb;
      size_t                               sizel;
    } marpaESLIFValueResultTable_t;
    struct marpaESLIFValueResultPair {
      marpaESLIFValueResult_t    key;
      marpaESLIFValueResult_t    value;
    };
    ```

    where `p` is an array of pairs of `marpaESLIFValueResult_t` key and value, of size `sizel`, that can be zero.

    In contrary to most languages, the key can be of any type, it is not restricted to a string.

- MARPAESLIF\_VALUE\_TYPE\_LONG\_DOUBLE

    Value type is `marpaESLIFValueResultLongDouble_t`, typedef'ed to: `long double`.

- MARPAESLIF\_VALUE\_TYPE\_LONG\_LONG

    Value type is `marpaESLIFValueResultLongLong_t`, typedef'ed to: `long long`, when your compiler has support for this native C type.

A Value result is of type `marpaESLIFValueResult_t` with more meta data:

```perl
typedef void (*marpaESLIFRepresentationDispose_t)(void *userDatavp,
                                                  char *inputcp,
                                                  size_t inputl,
                                                  char *encodingasciis);
typedef short (*marpaESLIFRepresentation_t)(void *userDatavp,
                                            marpaESLIFValueResult_t *marpaESLIFValueResultp,
                                            char **inputcpp,
                                            size_t *inputlp,
                                            char **encodingasciisp,
                                            marpaESLIFRepresentationDispose_t *disposeCallbackpp,
                                            short *stringbp);
typedef struct marpaESLIFValueResult {
  void                      *contextp;          /* Free value meaningful only to the user */
  marpaESLIFRepresentation_t representationp;   /* How a user-land alternative is represented if it was in the input */
  marpaESLIFValueType_t      type;              /* Type for tagging the following union */
  union {
    marpaESLIFValueResultChar_t        c;            /* Value is a char */
    marpaESLIFValueResultShort_t       b;            /* Value is a short */
    marpaESLIFValueResultInt_t         i;            /* Value is an int */
    marpaESLIFValueResultLong_t        l;            /* Value is a long */
    marpaESLIFValueResultFloat_t       f;            /* Value is a float */
    marpaESLIFValueResultDouble_t      d;            /* Value is a double */
    marpaESLIFValueResultPtr_t         p;            /* Value is a pointer */
    marpaESLIFValueResultArray_t       a;            /* Value is an array */
    marpaESLIFValueResultBool_t        y;            /* Value is a boolean */
    marpaESLIFValueResultString_t      s;            /* Value is a string */
    marpaESLIFValueResultRow_t         r;            /* Value is a row of values */
    marpaESLIFValueResultTable_t       t;            /* Value is a row of values, where sizel is even */
    marpaESLIFValueResultLongDouble_t ld;            /* Value is a long double */
  #ifdef MARPAESLIF_HAVE_LONG_LONG
    marpaESLIFValueResultLongLong_t   ll;            /* Value is a long long */
  #endif
  } u;
} marpaESLIFValueResult_t;
```

which means:

- contextp

    This can be anything that is meaningul for the end-user. It must be different than `NULL`, reserved to the marpaESLIF engine.

- representationp

    Associated representation in terms of input: this has an immediate impact on the `::concat` and `::convert[ENCODING]` actions:

    ```perl
    A ::= B C action => ::concat
    D ::= E F action => ::convert[ENCODING]
    ```

    where `::concat` is a binary concatenation, and `::convert[ENCODING]` is a string conversion:

    - Representation callback exists: The binary content of the representation is taken as-is.

        The callback can give a disposal function pointer that is guaranteed to be called when ESLIF do not need representation data anymore.

    - No representation callback: The following default rules apply:

        ```
        |-------------------------------------------------------------------------------|
        | Value type                    | ::concat              | ::convert[ENCODING]   |
        |-------------------------------------------------------------------------------|
        | undefined                     | N/A                   | N/A                   |
        | marpaESLIFValueResultChar_t   | binary value          | formatted as %c       |
        | marpaESLIFValueResultShort_t  | binary value          | formatted as %d       |
        | marpaESLIFValueResultInt_t    | binary value          | formatted as %d       |
        | marpaESLIFValueResultLong_t   | binary value          | formatted as %ld      |
        | marpaESLIFValueResultFloat_t  | binary value          | formatted as %f (1)   |
        | marpaESLIFValueResultDouble_t | binary value          | formatted as %f (2)   |
        | marpaESLIFValueResultPtr_t    | u.p.p, size void*     | formatted as %p       |
        | marpaESLIFValueResultArray_t  | u.a.p, size u.a.sizel | u.a.p, size u.a.sizel |
        | marpaESLIFValueResultBool_t   | binary value          | formatted as %d       |
        | marpaESLIFValueResultString_t | u.s.p, size u.s.sizel | %s in ENCODING (3)    |
        |-------------------------------------------------------------------------------|
        ```

        - (1) Float

            Best effort formatting, using a string format that takes into account C's `FLT_DIG` constant if any.

        - (2) Double

            Best effort formatting, using a string format that takes into account C's `DBL_DIG` constant if any.

        - (3) String

            String transcoding, result is appended as if it was a binary concat.

- type

    This is the type itself, and has an impact on the C's `union` that follows. Please note that:

    - sizel

        Some value type are containers, and `sizel` gives the size in byte unit.

    - shallowb

        This has an impact on internal value stack manipulation: everytime ESLIF tries to set a value, it is looking if this is going to replace something at the same stack indice. If the value to be replaced as this stack indice is not shallowed, free callback is called.

## ESLIFRecognizer

One of the most important thing to understand is the notion of _lexeme_: a _lexeme_ is a _meta symbol_ referencing another _meta symbol_ in another grammar. As such, it is a _terminal_ from the grammar point of view.

Internally, marpaESLIF have two terminal implementations:

- explicit terminal

    These are terminals defined explicitely in the grammar as strings, character classes or regular expressions. These terminals have no name, and will be handled automatically by the recognizer during a scanning phase.

- meta symbol

    These are symbols that have no LHS counterpart in the current grammar, thus must be known to another grammar. We call them lexemes, and the other grammar is used as a _lexing grammar_, totqlly managed by ESLIF, _as if_ this was an explicit terminal.

This mean that if the end-user want to have control on the parsing, he &lt;must> make sure that he will declare as a lexeme any part of the grammar on which he migh want to have some influence.

The subtility, so, is in the declaration of terminals. For instance, rules like this:

```perl
<sequence rule> ::= <something>* separator => [\s]+
<something>     ::= /[a-z]+/
<something>     ::= 'X'
```

aren't declaring any lexeme: both `[\s]+`, `/[a-z]+/` and `'X'` are _explicit_ terminals in the same grammar level. There is no associated name, and the user will not be able to say "I want to push an alternative whose content is `'X'` myself". On the other hand:

```perl
<sequence rule> ::= <something>* separator => my_separator
<something>     ::= /[a-z]+/
<something>     ::= my_string

my_separator      ~ [\s]+
my_string         ~ 'X'
```

declares the symbols `my_separator` and `my_string` as lexemes: they are not in any LHS of the grammar at level 0 (the one with `::=` declarations), they refer to a _grammar used as lexer_ that is at level 1 (the one with `~` declarations). The user will be able to push himself alternatives using the symbol names `my_separator` and `my_string`. Taking the example of the `'X'` string, the user have the freedom to push _something else_ but `'X'`, because he took over the lexing phase. The only important thing from the top grammar point of view, is that the _terminal_ whose name is `my_string` is an accepted alternative. Still, the end-user will not be able to push an alternative that would correspond to the expression `/[a-z]+/` at the `something` location.

Since when a user push alternative himself lexing is bypassed, this mean that the _representation_ of this alternative is also totally under the user's control. Indeed, nothing prevent the user to push the alternative named `my_string` and say its representation is something else but `X`. This is the purpose of the `marpaESLIFValueResult` structure: it is describing what ESLIF is calling a _result_, and makes a clear distinction between a result and its representation.

Being able to affect the parse tree is only possible anywhen the recognizer holds on, i.e. anywhen there is an _event_.

At any location in the grammar, possible alternatives can be of the two major types: _explicit terminals_ and lexemes (i.e. a _meta symbol_ refering another grammar). If the two types are present, and if at least one _explicit terminal_ matches, then the longests _explicit terminal_ will have absolute precedence over any lexeme. In other terms, _explicit terminals_ have an implicit absolute priority when they appear directly in a rule definition.

### `marpaESLIFRecognizer_newp`

```perl
typedef void  (*marpaESLIFReaderDispose_t)(void *userDatavp,
                                           char *inputcp,
                                           size_t inputl,
                                           short eofb,
                                           short characterStreamb,
                                           char *encodings,
                                           size_t encodingl);
typedef short (*marpaESLIFReader_t)(void    *userDatavp,
                                    char   **inputcpp,
                                    size_t  *inputlp,
                                    short   *eofbp,
                                    short   *characterStreambp,
                                    char   **encodingsp,
                                    size_t  *encodinglp,
                                    marpaESLIFReaderDispose_t *disposeCallbackpp);
/* Recognizer callback definitions */

/* if-action callback: received a marpaESLIFValueResultp of type ARRAY and return a boolean */
typedef short (*marpaESLIFRecognizerIfCallback_t)(void *userDatavp,
                                                  marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                                  marpaESLIFValueResult_t *marpaESLIFValueResultp,
                                                  marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
/* if-action callback resolver */
typedef marpaESLIFRecognizerIfCallback_t (*marpaESLIFRecognizerIfActionResolver_t)(void *userDatavp,
                                                                                   marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                                                                   char *actions);

/* event-action callback: received an array of marpaESLIFEvent_t and return a boolean */
typedef short (*marpaESLIFRecognizerEventCallback_t)(void *userDatavp,
                                                     marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                                     marpaESLIFEvent_t *eventArrayp,
                                                     size_t eventArrayl,
                                                     marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
/* event-action callback resolver */
typedef marpaESLIFRecognizerEventCallback_t (*marpaESLIFRecognizerEventActionResolver_t)(void *userDatavp,
                                                                                         marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                                                                         char *actions);

/* regex-action callback: received a hash and return an integer */
typedef short (*marpaESLIFRecognizerRegexCallback_t)(void *userDatavp,
                                                     marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                                     marpaESLIFValueResult_t *marpaESLIFCalloutBlockp,
                                                     marpaESLIFValueResultInt_t *marpaESLIFValueResultOutp);
/* regex-action callback resolver */
typedef marpaESLIFRecognizerRegexCallback_t (*marpaESLIFRecognizerRegexActionResolver_t)(void *userDatavp,
                                                                                         marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                                                                         char *actions);

/* End-user recognizer importer facility */
typedef short (*marpaESLIFRecognizerImport_t)(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                              void *userDatavp,
                                              marpaESLIFValueResult_t *marpaESLIFValueResultp,
                                              short haveUndefb);

typedef struct marpaESLIFRecognizerOption {
  void                *userDatavp;                  /* User specific context */
  marpaESLIFReader_t   readerCallbackp;             /* Reader callback */
  short                disableThresholdb;           /* Disable threshold warning.  Recommended: 0 */
  short                exhaustedb;                  /* Exhaustion event.           Recommended: 0 */
  short                newlineb;                    /* Count line/column numbers.  Recommended: 0 */
  short                trackb;                      /* Absolute position tracking. Recommended: 0 */
  size_t               bufsizl;                     /* Minimum stream buffer size. Recommended: 0 */
  /* Excess number of bytes, in % of bufsizl, where stream buffer size is reduced */
  unsigned int         buftriggerperci;             /*                             Recommended: 50 */
  /* Policy of minimum of bytes for increase, in % of current allocated size, when */
  /*  stream buffer size augment policy in % */
  unsigned int         bufaddperci;                 /*                             Recommended: 50 */
  marpaESLIFRecognizerIfActionResolver_t ifActionResolverp; /* symbol if action resolver. */
  marpaESLIFRecognizerEventActionResolver_t eventActionResolverp; /* Will return the function doing the wanted event action */
  marpaESLIFRecognizerRegexActionResolver_t regexActionResolverp; /* Will return the function doing the wanted regex callout action */
  marpaESLIFRecognizerImport_t              importerp;           /* Importer facility */
} marpaESLIFRecognizerOption_t;

marpaESLIFRecognizer_t *marpaESLIFRecognizer_newp(marpaESLIFGrammar_t          *marpaESLIFGrammarp,
                                                  marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp);
```

Creates a recognizer out of a `marpaESLIFGrammarp` grammar instance.

The `marpaESLIFReader_t` defines the prototype of a reader, referenced in the required parameter `marpaESLIFRecognizerOptionp`. Its arguments are:

- `userDatavp`

    User opaque pointer, set in `marpaESLIFRecognizerOptionp` and propagated as-is to the reader. There is no default value.

- `inputcpp`

    The place where the user will set the pointer of the next available buffer. Default is `NULL`.

- `inputlp`

    The place where the user will set the number of bytes of the next available buffer. Default is `0`.

- `eofbp`

    The place where the user will set a flag saying this the end of data. Default is a false value.

- `characterStreambp`

    The place where the user will set a flag saying this buffer _must_ be a stream of characters. Default is a false value.

- `encodingsp`

    The place where the user will give a pointer to a buffer containing encoding information, in ASCII encoding. Default is `NULL`.

- `encodinglp`

    The place where the user will give a size of the buffer starting at `*encodingsp`. Default is `0`.

- `disposeCallbackpp`

    A user-defined function pointer that, if not `NULL`, is guaranteed to be called as soon as ESLIF do not need anymore what the reader returned.

Mixing binary and character streams is possible but only at the chunk level, where a chunk is the data buffer returned by the reader callback. It is up the user to be able to determine the boundaries. Here is how the character stream flag and encoding information is used by marpaESLIF at the return of the reader callback:

- 1 - User say this is a character stream
    - A - Previous read was also a character streams
        - a - User gave encoding (`*encodingsp` is not `NULL`)

            If previous encoding is different, it is flushed and a new one is starting. Else previous encoding continue, this more will support incomplete character from the previous chunk.

        - b - User gave no encoding (`*encodingsp` is `NULL`)

            It is assumed that previous encoding continue
    - B - Previous read was not a stream of characters (or there was no previous read)

        If user gave encoding information it is used, else encoding is guessed.
- 2 - User say this is not a character stream
    - A - Previous read was a character stream

        It is flushed, and binary data is appended as-is.

    - B - Previous read was not a character stream (or there was no previous read)

        Binary data is appended as-is.

_If_ the grammar requires UTF-8 understanding of the buffer (either by using characters with code points greater than `0xFF` in terminals, or explicitely setting the `c` modifier to regular expressions - all of that meaning that PCRE2 expressions have the PCRE2\_UTF flag), and `If` the chunk of data was not declared as a character stream: it is assumed that it is in UTF-8 encoding. PCRE2 will check that and yell if it does not contain only valid UTF-8 characters.

The parameter `marpaESLIFRecognizerOptionp` is a pointer to a `marpaESLIFRecognizerOption_t` structure, where:

- `userDatavp`

    User context, propagated as-is to reader callback.

- `readerCallbackp`

    Pointer to the reader callback. If `NULL`, the stream will be considered complete (i.e. it reached end-of-file) at the very beginning.

- `disableThresholdb`

    Disable treshold warning generated by [libmarpa](https://jeffreykegler.github.io/Marpa-web-site/libmarpa.html). Recommended value is a false value, because when such warning arises, it usually indicate some problem with the grammar.

- `exhaustedb`

    Support of exhaustion event. Recommended value is a false value, because the most common usage of parsing data is to parse the full buffer up to its end of data. Nevertheless is is quite easy to find applications that support the exhaustion, for example parsing of binary structures that contain sub-structures, and so on. Setting this flag is also the _only_ way to have the `marpaESLIFGrammar_parseb` method successful even if not all data is consumed. The end user might still not have an exhaustion event triggered, this is why the later is propagated to the end user only if exhaustedb is a positive value. Please see the exhaustion item in the NOTES section.

- `newlineb`

    Support of newline count. This is used when the parser fails to produce ab accurate report with line and number information. Works at the granularity of data chunk, and is meaningless if data chunk is not declared as a character stream. Any Unicode line ending sequence is considered.

- `trackb`

    Support of absolute position tracking. The recognizer have to track absolute position to successfuly process the `marpaESLIFRecognizer_last_completedb()` method (see below). Nevertheless, maintaining such information has a cost and is not fully reliable because there is no attempt to check for an eventual turnaround of the associated internal variables. You should put this option to a true value only if you plan to use `marpaESLIFRecognizer_last_completedb()`.

- `bufsizl`

    Default number of bytes of the internal buffer in marpaESLIF, in which _all_ data is stored and appended. If `0`, the value used when compiling marpaESLIF library is used, itself defaulting to 1Mb. Recommended value is `0`.

- `buftriggerperci`

    In order to prevent internal buffer to grow indefinitely, `buftriggerperci` is the percentage of `bufsizl` that give a trigger on the number of unused bytes at which the internal buffer is reduced to its initial size `bufsizl`. Recommended value is `50`.

- `bufaddperci`

    To prevent too many memory allocations, when marpaESLIF need to increase its internal buffer, it will do so by requesting `bufaddperci` percent of current allocated size more bytes. Recommended value: `50`.

- `ifActionResolverp`

    If a lexeme or a terminal is a candidate for automatic parsing (_scan_ or _parse_ modes), this callback must not be `NULL` and return a function pointer. The later will be be called with a `marpaESLIFValueResultp` argument that is guaranteed to be of type `ARRAY`, and must return in `marpaESLIFValueResultBoolp`, guaranteed to not be `NULL`, a boolean value saying of the lexeme or terminal can be taken in account in the parse tree.

    The function will return a true value in case of success, a false value otherwise.

- `eventActionResolverp`

    If the grammar has set a generic event callback via `:default event-action`, this callback must not be `NULL` and return a function pointer. The later will be be called with a `actions` pointer containing an `NUL` terminated ASCII string representing the name of the event action.

- `regexActionResolverp`

    If the grammar has set a generic regular expression callback via `:default regex-action`, this callback must not be `NULL` and return a function pointer. The later will be be called with a `marpaESLIFCalloutBlockp` pointer containing regular expression information, and a pointer to a `marpaESLIFValueResultInt_t` value that will be reinjected into PCRE2, following [PCRE2 callout](https://www.pcre.org/current/doc/html/pcre2callout.html) specification, i.e. (quote from PCRE2 documentation):

    - If the value is zero, matching proceeds as normal.
    - If the value is greater than zero, matching fails at the current point, but the testing of other matching possibilities goes ahead, just as if a lookahead assertion had failed.
    - If the value is less than zero, the match is abandoned, and the matching function returns the negative value.

        Note that in this case, if the negative value is lower than `PCRE2_ERROR_INTERNAL_DUPMATCH`, ESLIF will emit a warning and change it to `PCRE2_ERROR_CALLOUT`

    The `marpaESLIFCalloutBlockp` is guaranteed to have the MARPAESLIF\_VALUE\_TYPE\_TABLE type, with these keys:

    - `callout_number`

        A `MARPAESLIF_VALUE_TYPE_LONG` or `MARPAESLIF_VALUE_TYPE_UNDEF` value. `MARPAESLIF_VALUE_TYPE_LONG` is when the callout is in the form `(?Cnumber)`, `MARPAESLIF_VALUE_TYPE_UNDEF` when the callout is the form `(?C"string")`.

    - `callout_string`

        A `MARPAESLIF_VALUE_TYPE_ARRAY` or `MARPAESLIF_VALUE_TYPE_UNDEF` value. `MARPAESLIF_VALUE_TYPE_ARRAY` is when the callout is in the form `(?C"string")`, `MARPAESLIF_VALUE_TYPE_UNDEF` when the callout is the form `(?Cnumber)`. `MARPAESLIF_VALUE_TYPE_ARRAY` points to the inner string.

    - `subject`

        A `MARPAESLIF_VALUE_TYPE_ARRAY` pointing to current subject bytes.

    - `pattern`

        A `MARPAESLIF_VALUE_TYPE_ARRAY` pointing to current PCRE2 pattern bytes.

    - `capture_top`

        A `MARPAESLIF_VALUE_TYPE_LONG` giving the maximum current capture

    - `capture_last`

        A `MARPAESLIF_VALUE_TYPE_LONG` giving the most recently closed capture.

    - `offset_vector`

        A `MARPAESLIF_VALUE_TYPE_ROW` of `MARPAESLIF_VALUE_TYPE_LONG`s giving offsets within the subject.

    - `mark`

        A `MARPAESLIF_VALUE_TYPE_LONG` or `MARPAESLIF_VALUE_TYPE_UNDEF` giving current mark offset.

    - `start_match`

        A `MARPAESLIF_VALUE_TYPE_LONG` giving current match start attempt offset.

    - `current_position`

        A `MARPAESLIF_VALUE_TYPE_LONG` giving current subject offset.

    - `next_item`

        A `MARPAESLIF_VALUE_TYPE_ARRAY` or `MARPAESLIF_VALUE_TYPE_UNDEF` giving next item the pattern.

    - `grammar_level`

        A `MARPAESLIF_VALUE_TYPE_INT` giving current grammar level.

    - `symbol_id`

        A `MARPAESLIF_VALUE_TYPE_INT` giving the current symbol identifier within current grammar.

- `importerp`

    User-defined importer facility function during the recognizer phase. Please refer to the IMPORT VALUE section in this document.

This method returns a recognizer pointer in case of success, `NULL` in case of failure.

### `marpaESLIFRecognizer_newFromp`

```
marpaESLIFRecognizer_t *marpaESLIFRecognizer_newFromp(marpaESLIFGrammar_t    *marpaESLIFGrammarp,
                                                      marpaESLIFRecognizer_t *marpaESLIFRecognizerSharedp);
```

Creates a recognizer out of a `marpaESLIFGrammarp` grammar instance, that is using explicitly the _same_ settings as `marpaESLIFRecognizerSharedp` and shares the later's stream.

From there, any update to the new recognizer's stream or `marpaESLIFRecognizerSharedp`'s stream are synchronous and shared. Running the new recognizer and the shared recognizer in parallel is not supported.

The input stream callback will use `marpaESLIFRecognizerSharedp` settings.

It is the application responsibility to make sure that the lifetime of `marpaESLIFRecognizerSharedp` is &lt;greater> than newly created recognizer instance.

Child recognizer inherits a shallow copy of `marpaESLIFRecognizerSharedp`'s context.

This method returns a recognizer pointer in case of success, `NULL` in case of failure.

### `marpaESLIFRecognizer_set_exhausted_flagb`

```
short marpaESLIFRecognizer_set_exhausted_flagb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                               short                   exhaustedb);
```

The only recognizer option that can be changed at runtime is the `exhaustedb` flag. The later is important when the recognizer cannot accept any new data (i.e. parse is exhausted) but unprocessed data remains: if `exhaustedb` is a false value, then an error is raised. Please see the exhaustion item in the NOTES section.

This method returns a true value in case of success, a false value otherwise.

### `marpaESLIFRecognizer_shareb`

```
short marpaESLIFRecognizer_shareb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                  marpaESLIFRecognizer_t *marpaESLIFRecognizerSharedp);
```

A recognizer can be instructed to share the stream with another `marpaESLIFRecognizerSharedp` at any time. When `marpaESLIFRecognizerSharedp` is `NULL`, sharing is stopped. Running the new recognizer and the shared recognizer in parallel is not allowed.

It is the application responsibility to make sure that the lifetime of `marpaESLIFRecognizerSharedp` is &lt;greater> than `marpaESLIFRecognizerp`.

Any `marpaESLIFReader_t` callback output will after the stream of both recognizers, as if they were both owning the internal buffers attached to the stream. In particular the input position of both recognizers are affected: they will _move_ in parallel. If you want `marpaESLIFRecognizerSharedp` to not be affected, then you must use the `marpaESLIFRecognizer_peekb` method.

It is illegal to have `marpaESLIFRecognizerp` equal to `marpaESLIFRecognizerSharedp`.

This method returns a true value in case of success, a false value otherwise.

### `marpaESLIFRecognizer_peekb`

```
short marpaESLIFRecognizer_peekb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                 marpaESLIFRecognizer_t *marpaESLIFRecognizerPeekedp);
```

The recognizer `marpaESLIFRecognizerp` is instructed to share the stream with `marpaESLIFRecognizerPeekedp`, _and_ is guaranteed that the internal stream buffer can only grow until it is unpeeked. This guarantees that `marpaESLIFRecognizerPeekedp` is not affected by any `marpaESLIFRecognizerp`'s reader callback. When `marpaESLIFRecognizerPeekedp` is `NULL`, peek is stopped. 

It is legal to have `marpaESLIFRecognizerp` equal to `marpaESLIFRecognizerPeekedp`, this mean that the user wants to "freeze" the stream, allowing only the internal buffers to grow.

It is the application responsibility to make sure that the lifetime of `marpaESLIFRecognizerPeekedp` is &lt;greater> than `marpaESLIFRecognizerp`.

This method returns a true value in case of success, a false value otherwise.

### `marpaESLIFRecognizer_optionp`

```
marpaESLIFRecognizerOption_t *marpaESLIFRecognizer_optionp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
```

Returns the options used to create the `marpaESLIFRecognizerp` instance, or `NULL` if failure. The returned pointer must _never_ be freed.

### `marpaESLIFRecognizer_scanb`

```
short marpaESLIFRecognizer_scanb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                 short initialEventsb,
                                 short *isCanContinuebp,
                                 short *isExhaustedbp);
```

Start a scanless parsing using the `marpaESLIFRecognizerp` instance. Only one such call can be done in the whole lifetime of a recognizer. Parameters are:

- initialEventsb

    Traditionnally initial events are not of use and are skipped. When true, this flag enable such events.

- isCanContinuebp

    The scanning will pause as soon as there is an event or an error. When not `NULL`, the value pointed by `isCanContinuebp` say that if scanning can continue if it is a true value.

- isExhaustedbp

    If not `NULL`, this value pointed by `isExhaustedbp` will say if the parse is exhausted, regardless if there is an exhaustion event or not.

A typical usage of the `marpaESLIFRecognizer_scanb` method is:

```perl
short isCanContinuebp;
short exhaustedb;

if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp,
                                 0 /* initialEventsb */,
                                 &isCanContinuebp,
                                 &exhaustedb)) {
  goto error_processing;
}
while (isCanContinuebp) {
  /* See below for the events, pause and resume methods */
}
```

This method returns a true value on success, a false value on failure, and can generate grammar and pause events.

### `marpaESLIFRecognizer_resumeb`

```
short marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                   size_t deltaLengthl,
                                   short *isCanContinuebp,
                                   short *isExhaustedbp);
```

Resume the scanning. All parameters have the same meaning as for `marpaESLIFRecognizer_scanb`, with an additional `deltaLengthl` that say to the recognizer to skip this given number of byte before going on. A typical usage is:

```perl
short isCanContinuebp;
short exhaustedb;

if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp,
                                 0 /* initialEventsb */,
                                 &isCanContinuebp,
                                 &exhaustedb)) {
  goto error_processing;
}
while (isCanContinuebp) {
  /* See below for the events and pause */
  /* ... */
  /* Resume */
  if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp,
                                     0,
                                     &isCanContinuebp,
                                     &exhaustedb)) {
    goto error_processing;
  }
}
```

Calling `marpaESLIFRecognizer_resumeb` is legal for any recognizer that already called &lt;marpaESLIFRecognizer\_scanb> or did at least one call to `marpaESLIFRecognizer_alternativeb` or `marpaESLIFRecognizer_alternative_readb`.

This method can generate grammar and pause events.

Returns a true value on success, a false value on failure.

### `marpaESLIFRecognizer_alternativeb`

```
typedef struct marpaESLIFAlternative {
  char                   *names  ;        /* Symbol name */
  marpaESLIFValueResult_t value;          /* Value */
  size_t                  grammarLengthl; /* Length in the grammar (1 == token-stream model) */
} marpaESLIFAlternative_t;

short marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                        marpaESLIFAlternative_t *marpaESLIFAlternativep);
```

It is possible to inject symbols outside of the internal scanner. They are then are described using a `marpaESLIFAlternative_t` structure, containing:

- names

    The symbol name as per the grammar. Must be different than `NULL`.

- value

    A structure describing the value. Please refer to the "ESLIF Value Result" section for explanation.

- grammarLengthl

    The number of grammar tokens taken by this alternative. A highly recommended value is `1`, which correspond to the tradional "token-stream" model. This number must be greater than zero, and putting a value greater than `1`, will make the parser believe that your alternative has consumed as many terminals.

Returns a true value on success, a false value on failure.

### `marpaESLIFRecognizer_alternative_completeb`

```
short marpaESLIFRecognizer_alternative_completeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                                 size_t lengthl);
```

Say to the recognizer pointed `marpaESLIFRecognizerp` by that the current set of alternatives is complete, and that the stream should advance by `length` bytes, where bytes is really the number of bytes in the internal recognizer buffer, i.e. an UTF-8 sequence of code points in case of a character-oriented stream. The reader callback may be called if necessary, in case the caller say to advance by more bytes than what is currently available. It is an error to specify more bytes than the recognizer can get (and eventually transform, in case of character-stream oriented buffer) from the reader. A a value of `0` for the length is legal.

If a _after_ event is triggered, `marpaESLIFRecognizer_name_last_pauseb` will show as many bytes as `lengthl`. And because it is guaranteed that `marpaESLIFRecognizer_name_last_pauseb` returns a pointer to a NUL-terminated buffer, the extreme case is that last pause can be an empty string "", which is legal when the alternative has been pushed using a user-defined value or it is correspond to a pseudo terminal.

This method can generate grammar and pause events.

Returns a true value on success, a false value on failure.

### `marpaESLIFRecognizer_alternative_readb`

```
short marpaESLIFRecognizer_alternative_readb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                             marpaESLIFAlternative_t *marpaESLIFAlternativep);
                                             size_t lengthl);
```

A short-hand version of alternative management that is pushing an alternative and do completion in one call.

This method can generate grammar and pause events.

Returns a true value on success, a false value on failure.

### `marpaESLIFRecognizer_name_tryb`

```
short marpaESLIFRecognizer_name_tryb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                     char *names,
                                     short *matchbp);
```

Try to match the symbol with name `names` that must be non-`NULL` against the input seen by the recognizer. Returns in `matchbp`, when not `NULL`, a true value if there is a match, a false value otherwise. This method generates no event.

This function should never fail, a false value indicates something fatal happened or the arguments are not correct.

### `marpaESLIFRecognizer_discardb`

```
short marpaESLIFRecognizer_discardb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *discardlp);
```

Applies :discard on the stream, returning in `discardlp`, when not `NULL`, a value that is > 0 if discard was successful.

The discarded data itself can be retreived _only_ if there is an associated completion event, e.g.:

```perl
:discard                       ::= /[\\s]+/\n event => whitespace$
:discard                       ::= /(?:(?:#)(?:[^\\n]*)(?:\\n|\\z))/u event => perl_comment$
:discard                       ::= /(?:(?:(?:\\/\\/)(?:[^\\n]*)(?:\\n|\\z))|(?:(?:\\/\\*)(?:(?:[^\\*]+|\\*(?!\\/))*)(?:\\*\\/)))/u event => cplusplus_comment$
```

Returns a true value on success, a false value otherwise.

### `marpaESLIFRecognizer_discard_tryb`

```
short marpaESLIFRecognizer_discard_tryb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                        short *matchbp);
```

Try to match the :discard rule against the input seen by the recognizer. Returns in `matchbp`, when not `NULL`, a true value if there is a match, a false value otherwise. This method generates no event.

This function should never fail, a false value indicates something fatal happened or the arguments are not correct.

### `marpaESLIFRecognizer_name_expectedb`

```
short marpaESLIFRecognizer_name_expectedb(marpaESLIFRecognizer_t   *marpaESLIFRecognizerp,
                                          size_t                   *nNamelp,
                                          char                   ***namesArraypp);
```

For the current recognizer `marpaESLIFRecognizerp`, get the list of _names_ expected. `*nNamelp`, if not `NULL`, is filled with the number of expected names, and `*namesArraypp`, if not `NULL`, will contain the array of NUL-terminated ASCII symbol names.

This function should never fail, a false value indicates something fatal happened or the arguments are not correct.

### `marpaESLIFRecognizer_isEofb`

```
short marpaESLIFRecognizer_eofb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *eofbp);
```

Returns in the value pointed by `eofbp` the current EOF state, when it is not `NULL`.

This function should never fail, a false value indicates something fatal happened or the arguments are not correct.

### `marpaESLIFRecognizer_isStartCompleteb`

```
short marpaESLIFRecognizer_isStartCompleteb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                            short *completebp);
```

Returns in the value pointed by `completebp` a true value if grammar reached start symbol completion, when it is not `NULL`.

This function should never fail, a false value indicates something fatal happened or the arguments are not correct.

### `marpaESLIFRecognizer_isExhaustedb`

```
short marpaESLIFRecognizer_isExhaustedb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *isExhaustedbp);
```

Returns in the value pointed by `isExhaustedbp` the current grammar exhaustion state, when it is not `NULL`. This method can be called at any time. It does not conflict with the `isExhaustedbp` parameter in `marpaESLIFGrammar_parseb()`, `marpaESLIFGrammar_parse_by_levelb()`, , `marpaESLIFGrammar_scan()` or `marpaESLIFGrammar_resumeb()`, that provide the same parameter for convienence.

This function should never fail, a false value indicates something fatal happened or the arguments are not correct.

### `marpaESLIFRecognizer_isCanContinueb`

```
short marpaESLIFRecognizer_isCanContinueb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *isCanContinueb);
```

Returns in the value pointed by `isCanContinueb` the current recognizer continuation flag state, when it is not `NULL`. This method can be called at any time. It does not conflict with the `isExhaustedbp` parameter in `marpaESLIFGrammar_parseb()`, `marpaESLIFGrammar_parse_by_levelb()`, , `marpaESLIFGrammar_scan()` or `marpaESLIFGrammar_resumeb()`, that provide the same parameter for convienence.

This function should never fail, a false value indicates something fatal happened or the arguments are not correct.

### `marpaESLIFRecognizer_event_onoffb`

```
typedef enum marpaESLIFEventType {
  MARPAESLIF_EVENTTYPE_NONE       = 0x00,
  MARPAESLIF_EVENTTYPE_COMPLETED  = 0x01, /* Grammar event */
  MARPAESLIF_EVENTTYPE_NULLED     = 0x02, /* Grammar event */
  MARPAESLIF_EVENTTYPE_PREDICTED  = 0x04, /* Grammar event */
  MARPAESLIF_EVENTTYPE_BEFORE     = 0x08, /* Just before symbol is commited */
  MARPAESLIF_EVENTTYPE_AFTER      = 0x10, /* Just after symbol is commited */
  MARPAESLIF_EVENTTYPE_EXHAUSTED  = 0x20, /* Exhaustion */
  MARPAESLIF_EVENTTYPE_DISCARD    = 0x40  /* Discard */
} marpaESLIFEventType_t;

short marpaESLIFRecognizer_event_onoffb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                        char                   *symbols,
                                        marpaESLIFEventType_t   eventSeti,
                                        short                   onoffb);
```

For the recognizer pointed by `marpaESLIFRecognizerp`, switches on or off the event types `eventSeti` (which is a bit mask of event type), depending of `onoffb` value, at symbol with name `symbols` (a NUL terminated ASCII string) of current grammar. Setting on or off a non-existing event is a no-op. There are eight type of events, described with the enum `marpaESLIFEventType_t`:

- `MARPAESLIF_EVENTTYPE_NONE`

    No event -;

- MARPAESLIF\_EVENTTYPE\_COMPLETED

    Completion event. This can happen only for meta symbol names. In the grammar this is declared with:

    ```
    event start$ = completed symbolname
    ```

- MARPAESLIF\_EVENTTYPE\_NULLED

    Nulled event. This can happen only for meta symbol names. In the grammar this is declared with:

    ```
    event start$ = nulled symbolname
    ```

- MARPAESLIF\_EVENTTYPE\_PREDICTED

    Prediction event. This can happen only for meta symbol names. In the grammar this is declared with:

    ```
    event start$ = predicted symbolname
    ```

- MARPAESLIF\_EVENTTYPE\_BEFORE

    _symbol before_ event. This can happen only for lexemes or terminals that are predicted by the scanning. In the grammar this is declared with:

    ```perl
    :lexeme   ::= lexemename          pause => before event => eventname
    :terminal ::= terminaldescription pause => before event => eventname
    ```

- MARPAESLIF\_EVENTTYPE\_AFTER

    _symbol before_ event. This can happen only for lexemes or terminals after they are pushed as alternatives by the scanning. In the grammar this is declared with:

    ```perl
    :lexeme   ::= lexemename          pause => after  event => eventname
    :terminal ::= terminaldescription pause => after  event => eventname
    ```

- MARPAESLIF\_EVENTTYPE\_EXHAUSTED

    Exhaustion event. It has no name, and is possible only if the recognizer is instanciated with the `exhaustedb` flag being a true value in the `marpaESLIFRecognizerOption_t` structure.

- MARPAESLIF\_EVENTTYPE\_DISCARD

    Discard event. This can happen only for `:discard` rules, and happen everytime a `:discard` rule is completed at the top-level grammar. In the grammar this is declared with:

    ```perl
    :discard ::= whitespace event => discard_whitespace
    ```

This function should never fail, a false value indicates something fatal happened or the arguments are not correct.

### `marpaESLIFRecognizer_eventb`

```
typedef struct marpaESLIFEvent {
  marpaESLIFEventType_t type;
  char                 *symbols; /* Symbol name, NULL if exhausted event, ':discard' if discard event */
  char                 *events;  /* Event name, always NULL if exhaustion eent */
} marpaESLIFEvent_t;

short marpaESLIFRecognizer_eventb(marpaESLIFRecognizer_t  *marpaESLIFRecognizerp,
                                  size_t                  *eventArraylp,
                                  marpaESLIFEvent_t      **eventArraypp);
```

Retreive the current list of events within the recognizer pointed by `marpaESLIFRecognizerp`, putting in the value pointed by `eventArraylp`, if it is not NULL, the number of event, and in the value pointed by `eventArraypp`, if it not NULL, the list of these events. An event is always a structure of type `marpaESLIFEvent_t` containing:

- type

    The type of event. C.f. upper section for a description of `marpaESLIFRecognizer_t` type.

- symbols

    The name of the symbol that triggered the event. Always a NUL-terminated ASCII string, except for exhaustion where it is `NULL`.

- events

    The name of the event. Always a NUL-terminated ASCII string, except for exhaustion where it is `NULL`.

This function should never fail, a false value indicates something fatal happened or the arguments are not correct.

### `marpaESLIFRecognizer_progressLogb`

```
short marpaESLIFRecognizer_progressLogb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                        int                     starti,
                                        int                     endi,
                                        genericLoggerLevel_t    logleveli);
```

Send to the logger configured at the marpaESLIF instanciation step, the current progress of parsing of the `marpaESLIFRecognizerp` recognizer instance. Logging is do at level `logleveli` (c.f. [genericLogger](https://github.com/jddurand/c-genericLogger) documentation), and from `starti` to `endi` indices. The `starti` and `endi` indices are Earleme Set Ids, and obey the Perl language convention, i.e. a negative indice means it far back from the end. The format of the strings generated is totally under the [marpaWrapper](https://github.com/jddurand/c-marpaWrapper) library. This method should be used when the end user want to have a progress report during valuation, c.f. the `marpaESLIFValue_value_startb` and `marpaESLIFValue_value_lengthb` methods.

This function should never fail, a false value indicates something fatal happened or the arguments are not correct.

### `marpaESLIFRecognizer_progressb`

```
typedef struct marpaESLIFRecognizerProgress {
  int earleySetIdi;
  int earleySetOrigIdi;
  int rulei;
  int positioni;
  int earlemei;
  int earlemeOrigi;
} marpaESLIFRecognizerProgress_t;

short marpaESLIFRecognizer_progressb(marpaESLIFRecognizer_t          *marpaESLIFRecognizerp,
                                     int                              starti,
                                     int                              endi,
                                     size_t                          *progresslp,
                                     marpaESLIFRecognizerProgress_t **progresspp);
```

Asks for the raw version progress, and from `starti` to `endi` indices. The `starti` and `endi` indices are Earleme Set Ids, and obey the Perl language convention, i.e. a negative indice means it far back from the end. If not `NULL`, returns in `progresslp` the number of progress items, themselves stored in `progresspp` when the later is not `NULL`. A progress items is a `marpaESLIFRecognizerProgress_t` structure composed of:

- earleySetId

    The Earley Set Id.

- earleySetOrigId

    The origin Earley Set Id.

- rule

    The rule number.

- position

    The position in the rule, where a negative number or a number bigger than the length of the rule means the rule is completed, `0` means the rule is predicted, else the rule is being run.

Returns a true value on success, a false value otherwise.

### marpaESLIFRecognizer\_latestEarleySetIdb>

```
short marpaESLIFRecognizer_latestEarleySetIdb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                              int *latestEarleySetIdip);
```

When `latestEarleySetIdip` is not `NULL`, it contains the latest Earley Set Id.

Returns a true value on success, a false value otherwise.

### marpaESLIFRecognizer\_earlemeb

```
short marpaESLIFRecognizer_earlemeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                    int earleySetIdi,
                                    int *earlemeip);
```

When `earlemeip` is not `NULL`, it contains the earleme Id corresponding to the Earley Set Id `earleySetIdi`. The later follows the Perl language convention, i.e. a negative indice means it far back from the end.

Returns a true value on success, a false value otherwise.

### `marpaESLIFRecognizer_inputb`

```
short marpaESLIFRecognizer_inputb(marpaESLIFRecognizer_t  *marpaESLIFRecognizerp,
                                  char                   **inputsp,
                                  size_t                  *inputlp);
```

For the recognizer instance pointed by `marpaESLIFRecognizerp`, returns the current internal buffer state, i.e.:

- inputsp

    If not `NULL`, will contain the current input internal buffer pointer.

- inputlp

    If not `NULL`, will contain the current input internal buffer length.

These pointers are intentionaly not having a _const_ keyword, though it is obvious that any attempt to modify `*inputsp` or `*inputlp` is likely to make the process crash. The internal buffer may _not_ correspond to what the callback returned in case there is character conversion: in this case the internal buffer contains UTF-8 character sequences.

Returns a true value on success, a false value on failure.

### `marpaESLIFRecognizer_errorb`

```
short marpaESLIFRecognizer_errorb(marpaESLIFRecognizer_t  *marpaESLIFRecognizerp);
```

Generates an error report for the recognizer instance pointed by `marpaESLIFRecognizerp`.

Returns a true value on success, a false value on failure.

### `marpaESLIFRecognizer_locationb`

```
short marpaESLIFRecognizer_locationb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                     size_t *linelp,
                                     size_t *columnlp);
```

For the recognizer instance pointed by `marpaESLIFRecognizerp`, returns the current line and number, i.e.:

- linelp

    If not `NULL`, will contain the current absolute line number.

- columnlp

    If not `NULL`, will contain the current absolute column number.

Please note that this is in effect only if the recognizer option have a true value for `newlineb`, and for character chunks.

Returns a true value on success, a false value on failure.

### `marpaESLIFRecognizer_name_last_pauseb`

```perl
short marpaESLIFRecognizer_name_last_pauseb(marpaESLIFRecognizer_t  *marpaESLIFRecognizerp,
                                            char                    *names,
                                            char                   **pausesp,
                                            size_t                  *pauselp);
```

For the recognizer instance pointed by `marpaESLIFRecognizerp`, returns a pointer to the last portion of input stream that generated a _pause_ event for symbol `names` in the value pointed by `pausesp` if it is not NULL, and the length in the value pointed by `pauselp` if it is not NULL. For convenience if the last pause is not NULL, the buffer holding the pause information is always NUL-terminated, even if the `\0` NUL character is _not_ in the returned length. In case of user-defined alternatives, the empty string "" is returned in `pausesp`.

This function should never fail, a false value indicates something fatal happened or the arguments are not correct.

### `marpaESLIFRecognizer_name_last_tryb`

```
short marpaESLIFRecognizer_name_last_tryb(marpaESLIFRecognizer_t  *marpaESLIFRecognizerp,
                                          char                    *names,
                                          char                   **trysp,
                                          size_t                  *trylp);
```

For the recognizer instance pointed by `marpaESLIFRecognizerp`, returns a pointer to the last portion of input stream into the value pointed by `trysp`, when it is non-`NULL`, and corresponding length in the value pointed by `trylp` when it is is non-`NULL`, of the last successful try for symbol `names`, via a call to `marpaESLIFRecognizer_name_tryb`. If `*trysp` is non-NULL, it is guaranteed that `*trylp` is greater than zero, and the block of memory is NUL-terminated, even if the additional NUL byte will not be included into `trylp`.

This function should never fail, a false value indicates something fatal happened or the arguments are not correct.

### `marpaESLIFRecognizer_discard_last_tryb`

```
short marpaESLIFRecognizer_discard_last_tryb(marpaESLIFRecognizer_t  *marpaESLIFRecognizerp,
                                             char                   **trysp,
                                             size_t                  *trylp);
```

For the recognizer instance pointed by `marpaESLIFRecognizerp`, returns a pointer to the last portion of input stream into the value pointed by `trysp`, when it is non-`NULL`, and corresponding length in the value pointed by `trylp` when it is is non-`NULL`, of the last successful discard try, via a call to `marpaESLIFRecognizer_discard_tryb`. If `*trysp` is non-NULL, it is guaranteed that `*trylp` is greater than zero, and the block of memory is NUL-terminated, even if the additional NUL byte will not be included into `trylp`.

This function should never fail, a false value indicates something fatal happened or the arguments are not correct.

### `marpaESLIFRecognizer_discard_lastb`

```
short marpaESLIFRecognizer_discard_lastb(marpaESLIFRecognizer_t  *marpaESLIFRecognizerp,
                                         char                   **lastsp,
                                         size_t                  *lastlp);
```

For the recognizer instance pointed by `marpaESLIFRecognizerp`, returns a pointer to the last portion of input stream into the value pointed by `lastsp`, when it is non-`NULL`, and corresponding length in the value pointed by `lastsp` when it is is non-`NULL`, of the last successful discard. If `*lastsp` is non-NULL, it is guaranteed that `*lastlp` is greater than zero, and the block of memory is NUL-terminated, even if the additional NUL byte will not be included into `lastlp`.

For performance reasons, last discard data is available only if the recognizer interface has a true value for `trackb`, _and_ if there is a discard event for the `:discard` rule that matched.

This function should never fail, a false value indicates something fatal happened or the arguments are not correct.

### `marpaESLIFRecognizer_readb`

```
short marpaESLIFRecognizer_readb(marpaESLIFRecognizer_t  *marpaESLIFRecognizerp,
                                 char                   **inputsp,
                                 size_t                  *inputlp);
```

Forces a call to the reader callback. All parameters have the semantic as in the method `marpaESLIFRecognizer_inputb`.

Returns a true value on success, a false value on failure.

### `marpaESLIFRecognizer_last_completedb`

```
short marpaESLIFRecognizer_last_completedb(marpaESLIFRecognizer_t  *marpaESLIFRecognizerp,
                                           char                    *names,
                                           char                   **offsetpp,
                                           size_t                  *lengthlp);
```

Convenience method that, for the recognizer instance pointed by `marpaESLIFRecognizerp`, returns the _absolute offset_ since the beginning of the scanning in the value pointed by `offsetpp` if it not `NULL`, and the length in the value pointed by `lengthlp` if it is not `NULL`, of the last completion for symbol `names`, that must not be `NULL`. This _absolute offset_ is computed without consideration of any turnaround, thus if the amount of data that has been scanned is larger what a `char *` can adress, the offset value will not be reliable. Same remark for the length.

Returns a true value on success, a false value on failure.

### `marpaESLIFRecognizer_hook_discardb`

```
marpaESLIFRecognizer_hook_discardb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                   short discardOnOffb);
```

Hook the recognizer to enable or disable the use of `:discard` if it exists. Default mode is on. This is a _permanent_ setting.

Returns a true value on success, a false value on failure.

### `marpaESLIFRecognizer_freev`

```
void marpaESLIFRecognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
```

The destructor of a marpaESLIFRecognizer instance `marpaESLIFRecognizerp` instance. No-op if `marpaESLIFRecognizerp` is `NULL`.

### `marpaESLIFJSON_encode_newp`

```
marpaESLIFGrammar_t *marpaESLIFJSON_encode_newp(marpaESLIF_t *marpaESLIFp, short strictb);
```

ESLIF embeds a JSON grammar encoder, where `strictb` says it is strictly the grammar as per [JSON Specification](http://json.org). The non strict mode enables:

- Infinity and NaN

### `marpaESLIFJSON_encodeb`

```
short  marpaESLIFJSON_encodeb(marpaESLIFGrammar_t *marpaESLIFGrammarJSONp, marpaESLIFValueResult_t *marpaESLIFValueResultp, marpaESLIFValueOption_t *marpaESLIFValueOptionp);
```

### `marpaESLIFJSON_decode_newp`

```
marpaESLIFGrammar_t *marpaESLIFJSON_decode_newp(marpaESLIF_t *marpaESLIFp, short strictb);
```

ESLIF embeds a JSON grammar decoder, where `strictb` says it is strictly the grammar as per [JSON Specification](http://json.org). The non strict mode enables:

- extras comma separators
- Perl and C++ comments
- Infinity and NaN
- Leading zeroes in number
- Case-insensitive for constants

### `marpaESLIFJSON_decodeb`

```perl
typedef short (*marpaESLIFJSONProposalAction_t)(void *userDatavp, char *strings, size_t stringl, marpaESLIFValueResult_t *marpaESLIFValueResultp, short confidenceb);

typedef struct marpaESLIFJSONDecodeOption {
  short                                        disallowDupkeysb;                /* Do not allow duplicate key in an object */
  size_t                                       maxDepthl;                       /* Maximum depth - 0 if no maximum */
  short                                        noReplacementCharacterb;         /* No replacement character for invalid UTF-16 surrogates */
  marpaESLIFJSONProposalAction_t               positiveInfinityActionp;         /* +Infinity action */
  marpaESLIFJSONProposalAction_t               negativeInfinityActionp;         /* -Infinity action */
  marpaESLIFJSONProposalAction_t               positiveNanActionp;              /* +Nan action */
  marpaESLIFJSONProposalAction_t               negativeNanActionp;              /* -Nan action */
  marpaESLIFJSONProposalAction_t               numberActionp;                   /* Number action */
} marpaESLIFJSONDecodeOption_t;

short marpaESLIFJSON_decodeb(marpaESLIFGrammar_t *marpaESLIFGrammarJSONp,
                             marpaESLIFJSONDecodeOption_t *marpaESLIFJSONDecodeOptionp,
                             marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp,
                              marpaESLIFValueOption_t *marpaESLIFValueOptionp);
```

Concerning numbers, since every host might want to have its own representation, every proposal callback that is not `NULL` will be called. The proposal will be in the `marpaESLIFValueResultp` pointer, guaranteed to not be `NULL`, and the caller can safely modify it. The `confidenceb` will be a true value if ESLIF is sure it represents correctly the number, a false value otherwise.

For infinity and NaN callbacks, when `confidenceb` is a true value, `marpaESLIFValueResultp` type is always `MARPAESLIF_VALUE_TYPE_FLOAT`, else it is `MARPAESLIF_VALUE_TYPE_UNDEF`.

For other numbers, `confidenceb` is a true value `marpaESLIFValueResultp` type is an integer type, or the double type and when the double value is exactly 0. Else this will be a double type, or `MARPAESLIF_VALUE_TYPE_UNDEF`.

In general it is safe to leave untouched the `marpaESLIFValueResultp` if `confidenceb` is a true value, else it is recommended to change it.

Take care of the context in the callbacks:

- Decoder option callbacks

    They are called in the context of a recognizer. So they are using `marpaESLIFJSONDecodeOptionp-`userDatavp> in their context.

- Import callback

    It is called in the context of a valuator. So they are using `marpaESLIFValueOptionp-`userDatavp> in their context.

- FLOAT

    The float representation of

- Infinity
- Nan
- Number

## ESLIFValue

At any point in the parsing lifetime it is possible to call for parse tree values. Traditionnaly this is happening when all data has been scanned, or when the parsing has exhausted.

### `marpaESLIFValue_newp`

```perl
typedef short (*marpaESLIFValueRuleCallback_t)  (void              *userDatavp,
                                                 marpaESLIFValue_t *marpaESLIFValuep,
                                                 int                arg0i,
                                                 int                argni,
                                                 int                resulti,
                                                 short              nullableb);

typedef short (*marpaESLIFValueSymbolCallback_t)(void              *userDatavp,
                                                 marpaESLIFValue_t *marpaESLIFValuep,
                                                 char              *bytep,
                                                 size_t             bytel,
                                                 int                resulti);

typedef void  (*marpaESLIFValueFreeCallback_t)  (void              *userDatavp,
                                                 int                contexti,
                                                 void              *p,
                                                 size_t             sizel);

typedef marpaESLIFValueRuleCallback_t   (*marpaESLIFValueRuleActionResolver_t)  (void              *userDatavp,
                                                                                 marpaESLIFValue_t *marpaESLIFValuep,
                                                                                 char              *actions);

typedef marpaESLIFValueSymbolCallback_t (*marpaESLIFValueSymbolActionResolver_t)(void              *userDatavp,
                                                                                 marpaESLIFValue_t *marpaESLIFValuep,
                                                                                 char              *actions);

/* End-user valuator importer facility */
typedef short (*marpaESLIFValueImport_t)(marpaESLIFValue_t *marpaESLIFValuep,
                                         void *userDatavp,
                                         marpaESLIFValueResult_t *marpaESLIFValueResultp,
                                         short haveUndefb);

typedef struct marpaESLIFValueOption {
  void                                 *userDatavp;            /* User specific context */
  marpaESLIFValueRuleActionResolver_t   ruleActionResolverp;   /* Returns pointer to rule action */
  marpaESLIFValueSymbolActionResolver_t symbolActionResolverp; /* Returns pointer to symbol action */
  marpaESLIFValueImport_t               importerp;             /* Will ask end-user to import a marpaESLIFValueResult */
  short                                 highRankOnlyb;         /* Recommended: 1 */
  short                                 orderByRankb;          /* Recommended: 1 */
  short                                 ambiguousb;            /* Recommended: 0 */
  short                                 nullb;                 /* Recommended: 0 */
  int                                   maxParsesi;            /* Recommended: 0 */
} marpaESLIFValueOption_t;

marpaESLIFValue_t *marpaESLIFValue_newp(marpaESLIFRecognizer_t  *marpaESLIFRecognizerp,
                                        marpaESLIFValueOption_t *marpaESLIFValueOptionp);
```

where `userDatavp` is a user opaque value, holding user specific context. Action resolvers are looked up later in this section. The other parameters are:

- ruleActionResolverp

    If there a custom rule, this callback must not be `NULL` and return a function pointer that will be used to perform valuation. This function must be return a true value otherwise valuation will stop.

    See below.

- symbolActionResolverp

    If there a custom generic `:default symbol-action`, this callback must not be `NULL` and return a function pointer that will be used to perform symbol value transition. This function must be return a true value otherwise valuation will stop.

    See below.

- importerp

    See the IMPORT VALUE section in this document.

- highRankOnlyb

    Actions can have a _rank_, e.g.:

    ```perl
    my_rule ::= I J 'string terminal' rank => 2
    my_rule ::= K L 'string terminal' rank => 1
    ```

    say that `my_rule` have two alternatives, with different ranks.

    _If_ the next parameter `orderByRankb` is a true value, then only rule with highest rank will be taken into account. This is a nice nice way to have a grammar ambiguous by nature, though having a non-ambiguous parse tree value from it.

- orderByRankb

    Specify if the rank is to be used.

- ambiguousb

    Specify if parse tree valuation can be ambiguous. Application's choice -;

- nullb

    Allow a parse tree valuation to produce nothing. It is recommended to set this to a false value.

- maxParsesi

    In case of ambiguity, there can be a lot of parse tree valuation results. If greater than zero, `maxParsesi` is a higher limit on such number.

Valuation lifetime is on the top-level grammar and has two main entry points:

- Valuation of terminals

    Terminals are what comes directly from the input stream. From grammar point of view, this correspond to string, character class, regular expressions, or lexemes.

- Valuation of rules

    Indeed, the valuation lifetime of something like e.g.:

    ```
    this_rule ::= 'string terminal'
    ```

    is:

    - 1 - Evaluate `'string terminal'`
    - 2 - Evaluate `this_rule`

    and nothing impose that the valuation result of `my_rule` would be same as `'string terminal'` valuation result.

    The functions holding valuation results are totally unknown from marpaESLIF, which is always asking to user-space for a function pointer of the correct type, whenever needed. For that it is using _action resolvers_. An action resolver is always called for a given _action name_ `actions`, guaranteed to a NUL-terminated ASCII string. In other terms marpaESLIF will always call the actions resolver like this:

    - For rules

        ```perl
        ruleActionp = ruleActionResolverp(userDatavp, marpaESLIFValuep, actions)
        ```

    - For symbols

        ```perl
        symbolActionp = symbolActionResolverp(userDatavp, marpaESLIFValuep, actions)
        ```

    and nothing impose the action resolvers to always return the same function pointer for a given NUL-terminated ASCII `action` string.

    The value result itself is also not under marpaESLIF control, though the later requires the user to manipulate a _stack_, and to assign to every entry in the stack a _value type_, and a _value context_. So there is a notion of _user stack_, which start at indice number `0`, and whose length is determined by the parse tree value location. The context of every _value_ in the stack is nothing else but a free number different than zero that should describe what it is, and has a meaning only for the end-user. It is important to set a correct context when assigning values to the stack, because this is how the _memory free_ function pointer, another function leaving in user space, will know how to react. For example a rule like this:

    ```perl
    my_rule ::= I J 'string terminal' action => my_action
    ```

    could be called with output stack indice `0` and input stack indices `0` for `I`, `1` for `J`, and `2` for `K`. It is the end user that knows what it is all about, through the famous _context_. Suppose that types of data would be a pointer (`PTR` below) with context value `0x12`, an integer (`INT` below) with context value `0x34`, and a short (`SHORT` below) with context value `0x56`, and that the end user decided that the result is a long with context `0x78`, then a representation of the stack is:

    ```perl
    |---------------------------------------------------|
    |            | my_rule =    I    J 'string terminal'|
    |---------------------------------------------------|
    |Stack indice|       0 =    0    1                 2|
    |        Type|    LONG    PTR  INT             SHORT|
    |     Context|    0x78   0x12 0x34              0x56|
    |---------------------------------------------------|
    ```

    You will note that:

    - The context of the terminal is a short !?

        The <'string terminal'> terminal is coming from input stream. There is _no_ associated symbol, and the function that manage symbols is covered by this grammar statement:

        ```perl
        :default ::= symbol-action => my_symbol_action
        ```

        marpaESLIF called the symbol action resolver `symbolActionResolverp` for action `"my_symbol_action"`, the later returning function pointer `my_symbol_actionp`, and marpaESLIF called this function pointer like this:

        ```perl
        my_symbol_actionp(userDatavp, marpaESLIFValuep, p, l, 2)
        ```

        where `p` is a pointer to a copy of input stream `'string terminal'`, of size `l`, asking for the result to be pushed at stack indice number `2`. It is the user that decided that it will put a short value at indice 2, with context `0x56`.

    - You talk about value types and context in the stack, but **how** can this be set?

        marpaESLIF provide stack manipulation functions, and it is simply impossible for the end-user to manipulate the stack without using these functions. Stack itself is maintained inside marpaESLIF. Please see all `marpaESLIFValue_stack_xxx` functions documented later.

    - Stack number indice `0` is in _both input and output_ indices

        This mean that, at any time, a stack indice can be replaced by another. This is a decision of the parse tree engine, based on rule definitions.

        Suppose that valuation of rule `my_rule`, there is already something at indice `0` in the stack and it is a pointer!? marpaESLIF will _automatically_ call for _memory free_ action on this indice if the pointer is not shallowed, before replacing it by the `my_rule` result. The _free action_ is set at the grammar level:

        ```perl
        :default ::= free-action => my_free_action
        ```

    Full history of our example rule `my_rule` action is then:

    - 1. Call the action resolver for `"my_action"`

        The function call is equivalent to

        ```perl
        ruleActionResolverp(userDatavp, marpaESLIFValuep, "my_action")
        ```

        We suppose that it returned a function pointer to `my_actionp`, a function that exist in user-space.

    - 2. Call `my_actionp`

        The function call is

        ```perl
        my_actionp(userDatavp, marpaESLIFValuep, 0/* arg0i */, 2/* argni */, 0/* resulti */, 0/* nullableb */)
        ```

        What is this `nullableb` flag!? Well, it means what it means: sometimes a rule is nullable, for example the empty rule:

        ```perl
        nullableRule ::= action => my_action2;
        ```

        or a sequence rule:

        ```perl
        nullableRule ::= item* action => my_action2
        ```

        Then, and only in these cases, the `nullableb` flag is a true value, `arg0i` and `argni` will be both `-1`, e.g. (with `my_action2p` being the rule resolver result for action `"my_action2"`):

        ```perl
        my_action2p(userDatavp, marpaESLIFValuep, -1/* arg0i */, -1/* argni */, 0/* resulti */, 1/* nullableb */)
        ```

        Ok, so finally `my_actionp` is called, and it decided to return a value of type _long_ and context `0x78`.

    - 3. Put user data at stack indice `0` and context `0x78`

    Nothing differentiate symbol actions v.s. rule action except that there is no range of indices in input, only a single indice.

    Returns `NULL` is no valuation is possible at this stage, else return a non-`NULL` value.

- Final result

    ESLIF never keeps in memory the _final_ result. Before freeing its internal stack, it will ask the user to do an import using `importp` marpaESLIFValueResult's option, if set.

    The goal of the transformer is to get into user's space a managable equivalent to an ESLIF value. Please refer to the [marpaESLIF\_BINDINGS](https://metacpan.org/pod/marpaESLIF_BINDINGS) bindings documentation.

### `marpaESLIFValue_recognizerp`

```
marpaESLIFRecognizer_t *marpaESLIFValue_recognizerp(marpaESLIFValue_t *marpaESLIFValuep);
```

Returns the marpaESLIF's recognizer instance used to create the `marpaESLIFValuep` instance.

### `marpaESLIFValue_optionp`

```
marpaESLIFValueOption_t *marpaESLIFValue_optionp(marpaESLIFValue_t *marpaESLIFValuep);
```

Returns the options used to create the `marpaESLIFValuep` instance, or `NULL` if failure. The returned pointer must _never_ be freed.

### `marpaESLIFValue_valueb`

```
short marpaESLIFValue_valueb(marpaESLIFValue_t       *marpaESLIFValuep,
                             marpaESLIFValueResult_t *marpaESLIFValueResultp);
```

Calls for parse tree valuation. Parameter `marpaESLIFValueResultp` can be `NULL`, meaning that the end-user is not interested by the value. Otherwise, this a pointer to a structure of type `marpaESLIFValueResult_t` that will host the result. Technically, please note that a final parse tree valuation is &lt;always> at stack indice number `0`:

The return value of `marpaESLIFValue_valueb` is always:

- 1 on success
- 0 on end
- -1 on failure

I.e. a typical usage is:

```
while (1) {
  short rcValueb = marpaESLIFValue_valueb(marpaESLIFValuep, NULL);
  if (rcValueb < 0) {
    goto err;
  } else if (rcValueb == 0) {
    break;
  }
}
```

### `marpaESLIFValue_value_startb`

```
short marpaESLIFValue_value_startb(marpaESLIFValue_t *marpaESLIFValuep, int *startip);
```

Taken directly from [marpaWrapper](https://github.com/jddurand/c-marpaWrapper) interface, this is returning the Start Earleme Set Ids of current valuation phase in the value pointed by `startip`, if the later is not `NULL`. This should be used to produce progress reports if wanted.

This method cannot be called outside a valuation callback. C.f. `marpaESLIFValue_value_lengthb` for an example.

Returns a true value on success, else a false value.

### `marpaESLIFValue_value_lengthb`

```
short marpaESLIFValue_value_lengthb(marpaESLIFValue_t *marpaESLIFValuep, int *lengthip);
```

Returns the number of Earleme Set Ids in the value pointed by `lengthip`, if the later is not `NULL`.

The length is greater than zero only when current Start Earleme Set Id is at a non-nullable symbol. This method cannot be called outside a valuation callback.

A typical usage example is:

```
int starti;
int lengthi;

if (marpaESLIFValue_value_startb(marpaESLIFValuep, &starti) &&
    marpaESLIFValue_value_lengthb(marpaESLIFValuep, &lengthi)) {
    marpaESLIFRecognizer_progressLogb(marpaESLIFValue_recognizerp(marpaESLIFValuep),
                                      starti,
                                      /* lengthi is zero when this is a nullable symbol */
                                      (lengthi > 0) ? starti+lengthi-1 : starti,
                                      GENERICLOGGER_LOGLEVEL_ERROR);
  }
```

Returns a true value on success, else a false value.

### `marpaESLIFValue_contextb`

```
short marpaESLIFValue_contextb(marpaESLIFValue_t  *marpaESLIFValuep,
                               char              **symbolsp,
                               int                *symbolip,
                               char              **rulesp,
                               int                *ruleip);
```

Puts in the value pointed by `symbolsp` if it is not `NULL`, the current symbol name if valuation occurs on a symbol, or in the value pointed by `rulesp` if it is not `NULL`, the current current rule name if valuation occurs on a rule. Idem for `symbolip` and `ruleip` concerning symbol Id and rule Id, respectively, where the value `-1` is filled for cases that do not apply.

Returns a true value on success, a false value on failure.

### `marpaESLIFValue_freev`

```
void marpaESLIFValue_freev(marpaESLIFValue_t *marpaESLIFValuep);
```

The destructor of a marpaESLIFValue instance `marpaESLIFValuep` instance. No-op if `marpaESLIFValuep` is `NULL`.

## Stack manipulation

In the valuation callbacks, stack is manipulated using the functions described below. With no exception, they all return a true value on success, a false value on failure, and all have in common current marpaESLIF valuation context `marpaESLIFValuep`.

The _getAndForget_ versions are dangerous in the sense that there is a risk of memory leak if your application forget to free a memory popped from the stack with such method. Nevertheless, they are also very handy when wou _know_ this is a pointer that you want ESLIF to totally forget about.

### `marpaESLIFValue_stack_setb`

```
short marpaESLIFValue_stack_setb(marpaESLIFValue_t *marpaESLIFValuep,
                                 int indicei,
                                 marpaESLIFValueResult_t *marpaESLIFValueResultp);
```

Puts the `marpaESLIFValueResultp` pointer at stack number `indicei`.

Returns a true value on success, a false value otherwise.

In case `marpaESLIFValueResultp` is a container, any non-shallow pointer will have to occur once only.

### `marpaESLIFValue_stack_getb`

```
marpaESLIFValue_stack_getb(marpaESLIFValue_t *marpaESLIFValuep,
                           int indicei,
                           marpaESLIFValueResult_t *marpaESLIFValueResultp);
```

Copies the value result at indice `indicei` into the `marpaESLIFValueResultp` pointer.

Returns a true value on success, a false value otherwise.

### `marpaESLIFValue_stack_getp`

```
marpaESLIFValueResult_t *marpaESLIFValue_stack_getp(marpaESLIFValue_t *marpaESLIFValuep,
                                                    int indicei);
```

Gets a direct pointer to the value result at indice `indicei` within the value stack. Handy method for performance.

Returns a non-`NULL` value on success, `NULL` otherwise.

### `marpaESLIFValue_stack_forgetb`

```
short marpaESLIFValue_stack_forgetb(marpaESLIFValue_t *marpaESLIFValuep,
                                    int indicei);
```

Says to ESLIF to forget about any eventual value result at indice `indicei`.

Returns a true value on success, a false value otherwise.

### `marpaESLIFValue_stack_getAndForgetb`

```
short marpaESLIFValue_stack_getAndForgetb(marpaESLIFValue_t *marpaESLIFValuep,
                                          int indicei,
                                          marpaESLIFValueResult_t *marpaESLIFValueResultp);
```

Copies the value result at indice `indicei` into the `marpaESLIFValueResultp` pointer and tells &lt;marpaESLIF> to forget about it.

Returns a true value on success, a false value otherwise.

### `marpaESLIFValue_marpaESLIFValueResult_freeb`

```
marpaESLIF_EXPORT short marpaESLIFValue_marpaESLIFValueResult_freeb(marpaESLIFValue_t *marpaESLIFValuep,
                                                                    marpaESLIFValueResult_t *marpaESLIFValueResultp,
                                                                    short deepb);
```

Handy method to be used when you use `marpaESLIFValue_stack_forgetb`. If `deepb` is a false value, only the first immediate pointer, if any, inside `marpaESLIFValueResultp` will be freed when not shallow. If `deepb` is a true value, loops over all inner marpaESLIFValueResult's and call for disposal of every non-shadowed element. This method never free `marpaESLIFValueResultp` itself because it may be on the stack. It is user's responsibility to handle this case.

### `marpaESLIFRecognizer_marpaESLIFValueResult_freeb`

```
marpaESLIF_EXPORT short marpaESLIFRecognizer_marpaESLIFValueResult_freeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                                                         marpaESLIFValueResult_t *marpaESLIFValueResultp,
                                                                         short deepb);
```

Handy method equivalent to `marpaESLIFValue_marpaESLIFValueResult_freeb` but in a recognizer context.

### `marpaESLIF_ftos`

```
char *marpaESLIF_ftos(marpaESLIF_t *marpaESLIFp, float f);
```

Tries to convert accurately `f` to a string. Result a `NUL` terminated ASCII string that the user will have to free if success, `NULL` if failure. This should not be called everytime because it is very slow.

### `marpaESLIF_dtos`

```
char *marpaESLIF_dtos(marpaESLIF_t *marpaESLIFp, double d);
```

Tries to convert accurately `d` to a string. Result a `NUL` terminated ASCII string that the user will have to free if success, `NULL` if failure. This should not be called everytime because it is very slow.

### `marpaESLIF_ldtos`

```
char *marpaESLIF_ldtos(marpaESLIF_t *marpaESLIFp, long double ld);
```

Tries to convert accurately `ld` to a string. Result a `NUL` terminated ASCII string that the user will have to free if success, `NULL` if failure. This should not be called everytime because it is very slow.

## External symbols

```perl
typedef short (*marpaESLIFSymbolImport_t)(marpaESLIFSymbol_t *marpaESLIFSymbolp,
                                          void *userDatavp,
                                          marpaESLIFValueResult_t *marpaESLIFValueResultp,
                                          short haveUndefb);

typedef struct marpaESLIFSymbolOption {
  void                     *userDatavp;          /* User specific context */
  marpaESLIFSymbolImport_t  importerp;           /* If end-user want to import a marpaESLIFValueResult */
} marpaESLIFSymbolOption_t;

marpaESLIFSymbol_t          *marpaESLIFSymbol_string_newp(marpaESLIF_t *marpaESLIFp,
                                                          marpaESLIFString_t *stringp,
                                                          char *modifiers,
                                                          marpaESLIFSymbolOption_t *marpaESLIFSymbolOptionp);
marpaESLIFSymbol_t          *marpaESLIFSymbol_regex_newp(marpaESLIF_t *marpaESLIFp,
                                                         marpaESLIFString_t *stringp,
                                                         char *modifiers,
                                                         marpaESLIFSymbolOption_t *marpaESLIFSymbolOptionp);

/* For meta external symbol, the start symbol of the given grammar duplicated */
marpaESLIFSymbol_t          *marpaESLIFSymbol_meta_newp(marpaESLIF_t *marpaESLIFp,
                                                        marpaESLIFGrammar_t *marpaESLIFGrammarp,
                                                        marpaESLIFSymbolOption_t *marpaESLIFSymbolOptionp);
marpaESLIFSymbol_t          *marpaESLIFSymbol_meta_new_by_levelp(marpaESLIF_t *marpaESLIFp,
                                                                 marpaESLIFGrammar_t *marpaESLIFGrammarp,
                                                                 int leveli,
                                                                 marpaESLIFString_t *descp,
                                                                 marpaESLIFSymbolOption_t *marpaESLIFSymbolOptionp);

/* An external symbol can be used directly inside the recognizer phase in the current input stream. The later will automatically expand if needed */
/* as in normal recognizer lifetime. */
/* It can be also used outside of any grammar on a free input string */
/* If there is match, *matchbp will contain a true value, else a false value */
/* If there is match and importer is set, the later is called */
short                        marpaESLIFSymbol_tryb(marpaESLIFSymbol_t *marpaESLIFSymbolp,
                                                   char *inputs,
                                                   size_t inputl,
                                                   short *matchbp);
short                        marpaESLIFRecognizer_symbol_tryb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp,
                                                              marpaESLIFSymbol_t *marpaESLIFSymbolp,
                                                              short *matchbp);
void                         marpaESLIFSymbol_freev(marpaESLIFSymbol_t *marpaESLIFSymbolp);
```

`Out-of-grammar` symbols of type terminal, i.e. strings or regular expressions, can be created at any time, exactly as if they where present in the grammar: they a described with a `stringp` and can contain `modifiers`, which can be `NULL`.

For strings and regular expressions, the `modifiers`, when not `NULL`, must be compliant with the grammar specification for strings and regular expressions, i.e.:

- Strings

    Only `c` or `i` modifiers are allowed

- Regular expressions

    Only `e`, `i`, `j`, `m`, `n`, `s`, `x`, `D`, `J`, `U`, `a`, `N`, `u`, `b`, `c` or `A` modifiers are allowed

For meta symbols, also they can belong to any `marpaESLIFGrammarp`, only the grammar terminals can be tried with `marpaESLIFSymbol_tryb` and `marpaESLIFRecognizer_symbol_tryb`.

For strings and regular expressions, the `stringp` argument is without the delimiters, e:g:

- String

    ```
    123  for the string "123"
    "123 for the string "\"123" or '"123'
    ```

- Regular expressions

    ```
    123   for the regular expression /123/
    1[23] for the regular expression /1[23]/
    ```

Please note that `marpaESLIFRecognizer_symbol_tryb()` will use the symbol's importer, if any, not the recognizer's importer.

# NOTES

- Symbol names

    The name of a lexeme or a terminal symbol is be default how it is writen. Nevertheless this can overwriten using the

    ```perl
    name => wantedName
    ```

    adverb in the grammar, e.g.:

    ```perl
    :terminal ::= /X/ name => XRegexp
    :lexeme   ::= X   name => XLexeme
    ```

    It is highly recommended to not set the same name to multiple symbols.

- Exhaustion

    ESLIF considers two exhaustion cases:

    - Grammar exhaustion

        The grammar is truely exhausted, i.e. it cannot accept any alternative.

    - Start completion and nothing is accepted

        During the scan or resume phases, if no alternative is found, marpa low-level does not say recognizer is exhausted, but the recognizer progress has reached start symbol completion, then ESLIF also considers that the grammar is exhausted.

- REENTRANT AND THREAD-SAFETY

    All the objects are re-entrant but not thread-safe.

- Float values

    when a `MARPAESLIF_VALUE_TYPE_DOUBLE` or `MARPAESLIF_VALUE_TYPE_LONG_DOUBLE` type is pushed to the stack, marpaESLIF will always tries to downgrade to `float`, or `double` or `float`, respectively, if there is no loss of information. This is in particular sensible to the `MARPAESLIF_VALUE_TYPE_LONG_DOUBLE` type, because the latter is not always well supported by all external languages, e.g. Java.

    Special values like infinity or NaN, if supported, are trapped will always be overwriten to a `MARPAESLIF_VALUE_TYPE_FLOAT` type with their corresponding signed or NaN `float` native representation.

- Type of internal rules

    It is guaranteed that ESLIF built-in actions (i.e. `::ascii`, `::shift`, etc..) never produces a `marpaESLIFValueResult` of type `PTR`. This mean that the type <PTR> is solely used for data exchange with hosts that cannot export or import using built-in ESLIF types.

- Grammar show form

    The grammar show form is a concatenation of rule show forms and internal information, for every valid grammar level. For every grammar, the following is outputed:

    - Meta grammar settings

        For example:

        ```perl
        /*
         * **********************
         * Meta-grammar settings:
         * **********************
         */
        :start ::= json
        :default ::= action => ::concat
                     symbol-action => ::shift
                     latm => 1
        ```

    - Event settings

        For example:

        ```perl
        /*
         * ***************
         * Event settings:
         * ***************
         */
        :lexeme ::= <lstring> pause => after event => lstring$=on
        ```

    - Rules

        For example:

        ```perl
        /*
         * ******
         * Rules:
         * ******
         */
        <json> ::= <object>
        <json> ::= <array>
        <object> ::= '{' <members> '}' action => do_object
        etc...
        ```

    - Lexemes

        A _lexeme_ is a symbol that is referencing another symbol of the same name but in &lt;another> grammar level, for example:

        ```
        # --------
        # Lexemes:
        # --------
        #
        # <number> ::= <number>@+1
        # <lstring> ::= <lstring>@+1
        # <whitespace> ::= <whitespace>@+1
        ```

    - Rule properties

        For every rule, Marpa parser internal properties are given, together with the rule description, and components of the rule in terms in symbol indices, for example:

        ```perl
        # -----------------
        # Rules properties:
        # -----------------
        #
        # Rule No 0
        #   Properties: ACCESSIBLE, PRODUCTIVE
        #   Definition: <json> ::= <object>
        #   Components:  LHS = RHS[]
        #                  0 = 1
        # ...
        #
        # Rule No 2
        #   Properties: ACCESSIBLE, PRODUCTIVE
        #   Definition: <object> ::= '{' <members> '}' action => do_object
        #   Components:  LHS = RHS[]
        #                  1 = 3 4 5
        # Rule No 3
        #   Properties: ACCESSIBLE, NULLABLE, PRODUCTIVE
        #   Definition: <members> ::= <pair>* separator => /[,]/ action => do_array
        #   Components:  LHS = RHS[]
        #                  4 = 6
        #
        # ...
        ```

        Exceptions are shown like this:

        ```
        #
        # Rule No 2
        #   Properties: ACCESSIBLE, NULLABLE, PRODUCTIVE
        #   Definition: <start> ::= <thisstart> - <startException>
        #   Components:  LHS = RHS[] - EXCEPTION
        #                  0 = 4 - 5
        #
        ```

    - Symbol properties

        For every symbol, its ESLIF type, Marpa parser properties and definition are given. When it is a terminal, corresponding regular expression definition is given, as well as eventual JIT usage (c.f. PCRE2 information to understand what it is about). For example:

        ```
        # -------------------
        # Symbols properties:
        # -------------------
        #
        # Symbol No 0
        #         Type: ESLIF META
        #   Properties: ACCESSIBLE, PRODUCTIVE, START
        #         Name: <json>
        #
        # ...
        #
        # Symbol No 3
        #         Type: ESLIF TERMINAL
        #   Properties: ACCESSIBLE, PRODUCTIVE, TERMINAL
        #      Pattern: \{
        #        Flags: PCRE2_ANCHORED
        #          JIT: complete=yes, partial=yes
        #
        # ...
        #
        # Symbol No 23
        #         Type: ESLIF TERMINAL
        #   Properties: PRODUCTIVE, TERMINAL
        #      Pattern:
        #     0x000000: 5b 5c 73 5d                                     [\s]
        #        Flags: PCRE2_ANCHORED
        #          JIT: complete=yes, partial=yes
        ```

        There are two types of terminal:

        - Strings

            Strings are always converted to a pattern that is fully ASCII compatible, in which ESLIF decide to eventually escape some characters, or to use them in their `\x{...}` code point version. Therefore the definition of the PCRE2 pattern is outputed as-is.

        - Regular expression

            This type include character class. In such a case, the UTF-8 byte sequence corresponding to what the grammar is defining is sent directly to PCRE2, so the definition is the dump of this UTF-8 byte sequence.

# IMPORT VALUE

Both recognizer and valuation support the notion of importer. An importer is optional in the recognizer's phase, and usually vital for the valuation phase. Except the namespace, they have very similar signatures.

Automatic imports rely on the `importerp` option in their respective constructors:

```perl
marpaESLIFRecognizerImport_t importerp; /* Importer facility */
marpaESLIFValueImport_t      importerp; /* Will ask end-user to import a marpaESLIFValueResult */
```

These callbacks must return function pointers that have this signature (function names here are just examples):

```perl
short userDefinedRecognizerImporter(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, short haveUndefb);
short userDefinedValueImporter(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, short haveUndefb);
```

i.e. they are called with their respective namespace and user-defined `userDatavp` that is set in their respective constructors.

The flag `haveUndefb` is always:

- a true value

    when the import is on the type:

    - `MARPAESLIF_VALUE_TYPE_UNDEF`
    - `MARPAESLIF_VALUE_TYPE_ROW`

        and one of the elements is of type `MARPAESLIF_VALUE_TYPE_UNDEF`

    - `MARPAESLIF_VALUE_TYPE_TABLE`

        and one of the elements (that can be the key or the value) is of type `MARPAESLIF_VALUE_TYPE_UNDEF`.

- a false value

    In all other cases.

On-demande imports are also possible using these methods:

```
short marpaESLIFValue_importb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResultp);
short marpaESLIFRecognizer_importb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
```

that are just proxies to `userDefinedRecognizerImporter` and `userDefinedValueImporter`, respectively.

An importer is a stack safe recursive mechanism that establishes the following data contract with the implementation:

- `MARPAESLIF_VALUE_TYPE_ROW` and `MARPAESLIF_VALUE_TYPE_TABLE` marpaESLIFValueResult's types are flatified:
    - `MARPAESLIF_VALUE_TYPE_ROW`
        - All members are imported first in order. In other words, the row has `marpaESLIFValueResultp-`u.r.sizel> items, so:

            ```
            marpaESLIFValueResultp->u.r.p[0]
            ...
            marpaESLIFValueResultp->u.r.p[marpaESLIFValueResultp->u.r.sizel-1]
            ```

            are imported.

        - Then the marpaESLIFValueResult itself, of type `MARPAESLIF_VALUE_TYPE_ROW` is imported:

            ```
            marpaESLIFValueResultp /* Type is MARPAESLIF_VALUE_TYPE_ROW */
            ```
    - `MARPAESLIF_VALUE_TYPE_TABLE`
        - All key/value members are imported first in order. In other words, the table has `marpaESLIFValueResultp-`u.t.sizel> items, so:

            ```
            &(marpaESLIFValueResultp->u.t.p[0].key)
            &(marpaESLIFValueResultp->u.t.p[0].value)
            ...
            &(marpaESLIFValueResultp->u.t.p[marpaESLIFValueResultp->u.t.sizel-1].key)
            &(marpaESLIFValueResultp->u.t.p[marpaESLIFValueResultp->u.t.sizel-1].value)
            ```

            are imported.

        - Then the marpaESLIFValueResult itself, of type `MARPAESLIF_VALUE_TYPE_TABLE` is imported:

            ```
            marpaESLIFValueResultp /* Type is MARPAESLIF_VALUE_TYPE_TABLE */
            ```
- Other marpaESLIFValueResult types are imported as-is, because they are just proxies to native C data types, they are not containers.

This mean that the importer callback should maintain an internal stack of its transformed marpaESLIFValueResult\_t items everytime it is called and:

- push to its stack anything that is not a `MARPAESLIF_VALUE_TYPE_ROW` nor a `MARPAESLIF_VALUE_TYPE_TABLE`
- pop from its stack `u.r.sizel` elements when it receives a `MARPAESLIF_VALUE_TYPE_ROW` callback, transforming them to what is an array for the host language
- pop from its stack `u.t.sizel * 2` elements when it receives a `MARPAESLIF_VALUE_TYPE_TABLE` callback, transforming them to what is a hash for the host language
- pop from its stack any other type, transforming it to something meaningful in the host language

At the end it must have exactly one element in its internal stack.

Current bindings exist for the Lua, Java and Perl languages, with this default implementation:

```
Type                                  Lua Type          Java (JNI) Type       Perl (XS) Type

MARPAESLIF_VALUE_TYPE_UNDEF           LUA_TNIL          NULL                  &PL_sv_undef
MARPAESLIF_VALUE_TYPE_CHAR            LUA_TSTRING       Depends [5]           PV*
MARPAESLIF_VALUE_TYPE_SHORT           LUA_TNUMBER       Depends [6]           Depends [12]
MARPAESLIF_VALUE_TYPE_INT             LUA_TNUMBER       Depends [6]           Depends [12]
MARPAESLIF_VALUE_TYPE_LONG            LUA_TNUMBER       Depends [6]           Depends [12]
MARPAESLIF_VALUE_TYPE_FLOAT           LUA_TNUMBER       Depends [7]           NV
MARPAESLIF_VALUE_TYPE_DOUBLE          LUA_TNUMBER       Depends [8]           NV
MARPAESLIF_VALUE_TYPE_PTR             Depends [1]       Depends [9]           Depends [13]
MARPAESLIF_VALUE_TYPE_BOOL            LUA_TBOOLEAN      java.lang.Bool        Depends [14]
MARPAESLIF_VALUE_TYPE_STRING          LUA_TSTRING [2]   java.lang.String      Depends [15]
MARPAESLIF_VALUE_TYPE_ROW             LUA_TTABLE [3]    object[]              AV*
MARPAESLIF_VALUE_TYPE_TABLE           LUA_TTABLE [4]    java.util.HashMap     HV*
MARPAESLIF_VALUE_TYPE_LONG_DOUBLE     LUA_TNUMBER       Depends [10]          Depends [16]
MARPAESLIF_VALUE_TYPE_LONG_LONG       LUA_TNUMBER       Depends [11]          Depends [17]

MARPAESLIF_VALUE_TYPE_ARRAY           LUA_TSTRING       byte[]                PV*
or [18]
MARPAESLIF_VALUE_TYPE_ARRAY           LUA_XSTRING       DirectyByteBuffer     readonly PV*
```

- \[1\] Context dependant, can be a lua specific value, injected by hand during recognizer phase for example, falling back to a LUA\_TUSERDATA
- \[2\] Encoding information is attached to internal string. Since strings in Lua are unique, one string can have only one encoding.
- \[3\] A special niled table that indexed by number
- \[4\] A special niled table indexed by strings
- \[5\] java.lang.Byte or java.lang.Short or java.lang.Integer or java.lang.Long or java.math.BigInteger, depends on `CHAR_BIT`
- \[6\] java.lang.Float (includes +/-Inf and Nan) or java.lang.Double or java.math.BigDecimal, depends on the C native data size
- \[7\] java.lang.Float or java.lang.Double or java.math.BigDecimal, depends on the C native data size
- \[8\] java.lang.Float or java.lang.Double or java.math.BigDecimal
- \[9\] java.lang.Object, if a lua specific value, injected by hand during recognizer phase for example, falling back to java.lang.Long
- \[10\] java.lang.Float or java.lang.Double or java.math.BigDecimal
- \[11\] java.lang.Byte or java.lang.Short or java.lang.Integer or java.lang.Long or java.math.BigInteger
- \[12\] IV or Math::BigInt
- \[13\] SV\*, if be a perl specific value, injected by hand during recognizer phase for example, falling back PTR2IV()
- \[14\] $MarpaX::ESLIF::true or $MarpaX::ESLIF::false
- \[15\] PV\* or MarpaX::ESLIF::String instance
- \[16\] NV or Math::BigFloat
- \[17\] IV or Math::BigInt
- \[18\] In recognizer callbacks, every container is an array. For languages binding efficiency, no copy is done is direct access to memory is given via:
    - Perl language

        A readonly `SV*` that is a `PV*` pointing directly to the memory region.

    - Lua language

        A [xstring-lua](https://github.com/chipdude/xstring-lua)-like object, where only four methods exists:

        - `len(s)`

            Returns the number of bytes of the region

        - `sub(s, [i, [j]])`

            Returns a new _xtring_ instance mapped to the given subregion.

        - `byte(s, [i, [j]])`

            Returns numeric code of the given subregion.

        - `string(s)`

            Returns a true Lua string out of this xstring. This operation will involve a memory copy.

        Any attempt to stringify automatically this xstring will result in a creation of a true Lua string.

# Strings and Array lifetime

Here are the rules for strings and arrays lifetime:

- `marpaESLIF_versionb`

    The ASCII string returned in `*versionsp` is persistent until ESLIF is freed.

- `marpaESLIFAction` type

    ESLIF guarantees that all string pointers in `marpaESLIFAction` are persistent until the associated grammar is freed.

- `marpaESLIFGrammar_grammar_by_levelb`

    ESLIF guarantees that the string returned in `*descpp` is persistent until the associated grammar is freed.

- `marpaESLIFGrammar_grammarproperty_currentb` 

    Any action and string returned in persistent until the associated grammar is freed.

- `marpaESLIFGrammar_ruleproperty_currentb` and `marpaESLIFGrammar_ruleproperty_by_levelb`

    Any action and string returned in persistent until the associated grammar is freed.

- `marpaESLIFGrammar_ruledisplayform_currentb` and `marpaESLIFGrammar_ruledisplayform_by_levelb`

    The _ASCII_ string returned in `*ruledisplaysp` is persistent until the associated grammar is freed.

- `marpaESLIFGrammar_symbolproperty_currentb` and `marpaESLIFGrammar_symbolproperty_by_levelb`

    Any action and string returned in persistent until the associated grammar is freed.

- `marpaESLIFGrammar_symboldisplayform_currentb` and `marpaESLIFGrammar_symboldisplayform_by_levelb`

    The _ASCII_ string returned in `*symboldisplaysp` is persistent until the associated grammar is freed.

- `marpaESLIFGrammar_ruleshowform_currentb` and `marpaESLIFGrammar_ruleshowform_by_levelb`

    The _ASCII_ string returned in `*ruleshowsp` is persistent until the associated grammar is freed.

- `marpaESLIFGrammar_grammarshowform_currentb` and `marpaESLIFGrammar_grammarshowform_by_levelb`

    The _ASCII_ string returned in `*grammarshowsp` is persistent until the associated grammar is freed.

- `marpaESLIFGrammar_grammarshowscriptb`

    The _ASCII_ string returned in `*grammarscriptsp` is persistent until the associated grammar is freed.

- `marpaESLIFValue_contextb`

    The _ASCII_ strings returned in `*symbolsp` and `*rulesp` are persistent until the associated grammar is freed.

- Recognizer callbacks

    All string and arrays are valid during the callback and are guaranteed to not change `if` ESLIF recognizer state is not affected by any user call. Calls that `may` affect ESLIF recognizer state are:

    - `marpaESLIFRecognizer_scanb`
    - `marpaESLIFRecognizer_resumeb`
    - `marpaESLIFRecognizer_alternative_readb`
    - `marpaESLIFRecognizer_alternative_completeb`
    - `marpaESLIFRecognizer_readb`

    Cases where ESLIF always returns the direct memory pointers are:

    - `marpaESLIFRecognizer_inputb()`
    - `marpaESLIFRecognizerIfCallback_t` structures in if callbacks
    - `marpaESLIFRecognizerEventCallback_t` structures in event callbacks
    - `marpaESLIFRecognizerRegexCallback_t` structures in regular expression callbacks
    - `marpaESLIFRecognizerImport_t()`
    - `marpaESLIFRecognizer_symbol_tryb()`

    The memory pointers in the above methods are _always_ of type `MARPAESLIF_VALUE_TYPE_ARRAY`, ESLIF bindings then guarantees that:

    - Perl language

        A read-only `SV*` is used to access the memory content.

    - Lua language

        A read-only &lt;external string> implementation, very closed to [xstring-lua](https://github.com/chipdude/xstring-lua) is used to access the memory content. Take care these are _not_ true strings from Lua point of view. The following methods exist on such object:

        - self:len()

            Returns the byte length.

        - self:sub(offset\[, length\])

            Returns a new external string instance starting at offset of length bytes. It obeys the same rules as native Lua native string.sub(s, offset\[, length\]).

        - self:byte()

            Returns an array of integers representing the values starting at offset of length bytes. It obeys the same rules as native Lua native string.byte(s, offset\[, length\]).

        - self:string()

            Returns a true Lua string. This method will trigger a _copy into Lua_.

        The explicit stringification tostring() on an xstring is calling self:string().

    - Java language

        A `DirectByteBuffer` is used to access the memory content. Any attempt to change memory will result in unpredicted behaviour.

    In conclusion, if the end-user is asking the recognizer anything that returns a direct pointer to its internal memory, the end-user must take into account that any call that will change ESLIF state will invalidate the previous result. So he should consider that this is a one-shot valid thing. For example, a sequence like:

    ```
    Get input using marpaESLIFRecognizer_inputb
    Test input first byte
    Read a grammar terminal of size one byte using marpaESLIFRecognizer_alternative_readb
    Test input second byte
    ```

    is wrong, and must be changed to:

    ```
    Get input using marpaESLIFRecognizer_inputb
    Test input first byte
    Read a grammar terminal of size one byte using marpaESLIFRecognizer_alternative_readb
    Get input using marpaESLIFRecognizer_inputb
    Test input first byte
    ```

- Imports
    - `marpaESLIFValue_importb`

        Any data after the call is freed. The end-user must import strings and arrays in its namespace.

    - `marpaESLIFRecognizer_importb`

        The strings and arrays will be valid until ESLIF recognizer state is changed.

# SEE ALSO

[genericLogger](https://github.com/jddurand/c-genericLogger),  [PCRE2](http://www.pcre.org/), [libmarpa](https://jeffreykegler.github.io/Marpa-web-site/libmarpa.html), [marpaWrapper](https://github.com/jddurand/c-marpaWrapper), [marpaESLIF\_BINDINGS](https://github.com/jddurand/c-marpaESLIF/tree/master/src/bindings)
