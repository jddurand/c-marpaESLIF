/* Java JNI bindings for marpaESLIF */

#include <jni.h>
#include <limits.h>
/* Should never happen */
#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include <marpaESLIF.h>
#include <genericStack.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <float.h>
#include <genericLogger.h>

/* ---------------- */
/* Exported methods */
/* ---------------- */
JNIEXPORT jint         JNICALL JNI_OnLoad                                                      (JavaVM *vmp, void* reservedp);
JNIEXPORT void         JNICALL JNI_OnUnLoad                                                    (JavaVM *vmp, void* reservedp);

JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIF_jniNew                              (JNIEnv *envp, jobject eslifp, jint indice);
JNIEXPORT jstring      JNICALL Java_org_parser_marpa_ESLIF_jniJavaVersion                      (JNIEnv *envp, jclass classp);
JNIEXPORT jint         JNICALL Java_org_parser_marpa_ESLIF_jniJavaVersionMajor                 (JNIEnv *envp, jclass classp);
JNIEXPORT jint         JNICALL Java_org_parser_marpa_ESLIF_jniJavaVersionMinor                 (JNIEnv *envp, jclass classp);
JNIEXPORT jint         JNICALL Java_org_parser_marpa_ESLIF_jniJavaVersionPatch                 (JNIEnv *envp, jclass classp);
JNIEXPORT jstring      JNICALL Java_org_parser_marpa_ESLIF_jniVersion                          (JNIEnv *envp, jobject eslifp);
JNIEXPORT jint         JNICALL Java_org_parser_marpa_ESLIF_jniVersionMajor                     (JNIEnv *envp, jobject eslifp);
JNIEXPORT jint         JNICALL Java_org_parser_marpa_ESLIF_jniVersionMinor                     (JNIEnv *envp, jobject eslifp);
JNIEXPORT jint         JNICALL Java_org_parser_marpa_ESLIF_jniVersionPatch                     (JNIEnv *envp, jobject eslifp);
/* JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIF_jniFree                             (JNIEnv *envp, jobject eslifp); */
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFGrammar_jniNew                       (JNIEnv *envp, jobject eslifGrammarp, jbyteArray utf8byteArrayp);
JNIEXPORT jint         JNICALL Java_org_parser_marpa_ESLIFGrammar_jniNgrammar                  (JNIEnv *envp, jobject eslifGrammarp);
JNIEXPORT jint         JNICALL Java_org_parser_marpa_ESLIFGrammar_jniCurrentLevel              (JNIEnv *envp, jobject eslifGrammarp);
JNIEXPORT jstring      JNICALL Java_org_parser_marpa_ESLIFGrammar_jniCurrentDescription        (JNIEnv *envp, jobject eslifGrammarp);
JNIEXPORT jstring      JNICALL Java_org_parser_marpa_ESLIFGrammar_jniDescriptionByLevel        (JNIEnv *envp, jobject eslifGrammarp, jint level);
JNIEXPORT jintArray    JNICALL Java_org_parser_marpa_ESLIFGrammar_jniCurrentRuleIds            (JNIEnv *envp, jobject eslifGrammarp);
JNIEXPORT jintArray    JNICALL Java_org_parser_marpa_ESLIFGrammar_jniCurrentSymbolIds          (JNIEnv *envp, jobject eslifGrammarp);
JNIEXPORT jintArray    JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleIdsByLevel            (JNIEnv *envp, jobject eslifGrammarp, jint level);
JNIEXPORT jintArray    JNICALL Java_org_parser_marpa_ESLIFGrammar_jniSymbolIdsByLevel          (JNIEnv *envp, jobject eslifGrammarp, jint level);
JNIEXPORT jintArray    JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleDisplay               (JNIEnv *envp, jobject eslifGrammarp, jint rule);
JNIEXPORT jintArray    JNICALL Java_org_parser_marpa_ESLIFGrammar_jniSymbolDisplay             (JNIEnv *envp, jobject eslifGrammarp, jint symbol);
JNIEXPORT jintArray    JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleShow                  (JNIEnv *envp, jobject eslifGrammarp, jint rule);
JNIEXPORT jintArray    JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleDisplayByLevel        (JNIEnv *envp, jobject eslifGrammarp, jint level, jint rule);
JNIEXPORT jintArray    JNICALL Java_org_parser_marpa_ESLIFGrammar_jniSymbolDisplayByLevel      (JNIEnv *envp, jobject eslifGrammarp, jint level, jint symbol);
JNIEXPORT jintArray    JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleShowByLevel           (JNIEnv *envp, jobject eslifGrammarp, jint level, jint rule);
JNIEXPORT jstring      JNICALL Java_org_parser_marpa_ESLIFGrammar_jniShow                      (JNIEnv *envp, jobject eslifGrammarp);
JNIEXPORT jstring      JNICALL Java_org_parser_marpa_ESLIFGrammar_jniShowByLevel               (JNIEnv *envp, jobject eslifGrammarp, jint level);
JNIEXPORT jboolean     JNICALL Java_org_parser_marpa_ESLIFGrammar_jniParse                     (JNIEnv *envp, jobject eslifGrammarp, jobject eslifRecognizerInterfacep, jobject eslifValueInterfacep);
JNIEXPORT jobject      JNICALL Java_org_parser_marpa_ESLIFGrammar_jniProperties                (JNIEnv *envp, jobject eslifGrammarp);
JNIEXPORT jobject      JNICALL Java_org_parser_marpa_ESLIFGrammar_jniPropertiesByLevel         (JNIEnv *envp, jobject eslifGrammarp, jint level);
JNIEXPORT jobject      JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleProperties            (JNIEnv *envp, jobject eslifGrammarp, jint rule);
JNIEXPORT jobject      JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRulePropertiesByLevel     (JNIEnv *envp, jobject eslifGrammarp, jint level, jint rule);
JNIEXPORT jobject      JNICALL Java_org_parser_marpa_ESLIFGrammar_jniSymbolProperties          (JNIEnv *envp, jobject eslifGrammarp, jint symbol);
JNIEXPORT jobject      JNICALL Java_org_parser_marpa_ESLIFGrammar_jniSymbolPropertiesByLevel   (JNIEnv *envp, jobject eslifGrammarp, jint level, jint symbol);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFGrammar_jniFree                      (JNIEnv *envp, jobject eslifGrammarp);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniNew                    (JNIEnv *envp, jobject eslifRecognizerp, jobject eslifGrammarp);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniShare                  (JNIEnv *envp, jobject eslifRecognizerp, jobject eslifRecognizerSharedp);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniUnshare                (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniPeek                   (JNIEnv *envp, jobject eslifRecognizerp, jobject eslifRecognizerPeekedp);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniUnpeek                 (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT jboolean     JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniScan                   (JNIEnv *envp, jobject eslifRecognizerp, jboolean initialEvents);
JNIEXPORT jboolean     JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniResume                 (JNIEnv *envp, jobject eslifRecognizerp, jint deltaLength);
JNIEXPORT jboolean     JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniAlternative            (JNIEnv *envp, jobject eslifRecognizerp, jstring namep, jobject objectp, jint grammarLengthi);
JNIEXPORT jboolean     JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniAlternativeComplete    (JNIEnv *envp, jobject eslifRecognizerp, jint lengthi);
JNIEXPORT jboolean     JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniAlternativeRead        (JNIEnv *envp, jobject eslifRecognizerp, jstring namep, jobject objectp, jint lengthi, jint grammarLengthi);
JNIEXPORT jboolean     JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniNameTry                (JNIEnv *envp, jobject eslifRecognizerp, jstring namep);
JNIEXPORT jboolean     JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniDiscardTry             (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT jobjectArray JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniNameExpected           (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT jboolean     JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniIsEof                  (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT jboolean     JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniIsStartComplete        (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT jboolean     JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniIsExhausted            (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniSetExhaustedFlag       (JNIEnv *envp, jobject eslifRecognizerp, jboolean flag);
JNIEXPORT jboolean     JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniIsCanContinue          (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT jboolean     JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniRead                   (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT jbyteArray   JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniInput                  (JNIEnv *envp, jobject eslifRecognizerp, jint offset, jint length);
JNIEXPORT jlong        JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniInputLength            (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniError                  (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT jbyteArray   JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniNameLastPause          (JNIEnv *envp, jobject eslifRecognizerp, jstring namep);
JNIEXPORT jbyteArray   JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniNameLastTry            (JNIEnv *envp, jobject eslifRecognizerp, jstring namep);
JNIEXPORT jbyteArray   JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniDiscardLastTry         (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT jbyteArray   JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniDiscardLast            (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT jobjectArray JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniEvent                  (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT jobjectArray JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniProgress               (JNIEnv *envp, jobject eslifRecognizerp, jint starti, jint endi);
JNIEXPORT jlong        JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniLastCompletedOffset    (JNIEnv *envp, jobject eslifRecognizerp, jstring namep);
JNIEXPORT jlong        JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniLastCompletedLength    (JNIEnv *envp, jobject eslifRecognizerp, jstring namep);
JNIEXPORT jlong        JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniLine                   (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT jlong        JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniColumn                 (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniEventOnOff             (JNIEnv *envp, jobject eslifRecognizerp, jstring symbolp, jobjectArray eventTypesp, jboolean onOff);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniHookDiscard            (JNIEnv *envp, jobject eslifRecognizerp, jboolean onOff);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniHookDiscardSwitch      (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniProgressLog            (JNIEnv *envp, jobject eslifRecognizerp, jint starti, jint endi, jobject levelp);
JNIEXPORT jobject      JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniSymbolTry              (JNIEnv *envp, jobject eslifRecognizerp, jobject eslifSymbolp);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniFree                   (JNIEnv *envp, jobject eslifRecognizerp);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFValue_jniNew                         (JNIEnv *envp, jobject eslifValuep, jobject eslifRecognizerp);
JNIEXPORT jboolean     JNICALL Java_org_parser_marpa_ESLIFValue_jniValue                       (JNIEnv *envp, jobject eslifValuep);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFValue_jniFree                        (JNIEnv *envp, jobject eslifValuep);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFJSONEncoder_jniNew                   (JNIEnv *envp, jobject eslifJSONEncoderp, jboolean strict);
JNIEXPORT jstring      JNICALL Java_org_parser_marpa_ESLIFJSONEncoder_jniEncode                (JNIEnv *envp, jobject eslifJSONEncoderp, jobject o);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFJSONEncoder_jniFree                  (JNIEnv *envp, jobject eslifJSONEncoderp);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFJSONDecoder_jniNew                   (JNIEnv *envp, jobject eslifJSONDecoderp, jboolean strict);
JNIEXPORT jobject      JNICALL Java_org_parser_marpa_ESLIFJSONDecoder_jniDecode                (JNIEnv *envp, jobject eslifJSONDecoderp, jobject eslifRecognizerInterfacep, jobject eslifDecodeOptionp);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFJSONDecoder_jniFree                  (JNIEnv *envp, jobject eslifJSONDecoderp);

JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFSymbol_jniNew                        (JNIEnv *envp, jobject eslifSymbolp, jstring type, jbyteArray utf8byteArrayp, jstring modifiers);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFSymbol_jniMetaNew                    (JNIEnv *envp, jobject eslifSymbolp, jobject eslifGrammarp, jstring symbolp);
JNIEXPORT jbyteArray   JNICALL Java_org_parser_marpa_ESLIFSymbol_jniTest                       (JNIEnv *envp, jobject eslifSymbolp, jbyteArray byteArrayp);
JNIEXPORT void         JNICALL Java_org_parser_marpa_ESLIFSymbol_jniFree                       (JNIEnv *envp, jobject eslifSymbolp);

/* ---------- */
/* Structures */
/* ---------- */
typedef struct marpaESLIFJavaGenericLoggerContext {
  jint      indice;
} marpaESLIFJavaGenericLoggerContext_t;

typedef struct marpaESLIFJavaClassCache {
  char     *classs;
  jclass    classp;
  short     requiredb;
} marpaESLIFJavaClassCache_t;

typedef struct marpaESLIFJavaMethodCache {
  marpaESLIFJavaClassCache_t *marpaESLIFJavaClassCachep;
  char                       *methods;
  char                       *signatures;
  short                       staticb;
  jmethodID                   methodp;
  short                       requiredb;
} marpaESLIFJavaMethodCache_t;

typedef struct marpaESLIFJavaFieldCache {
  marpaESLIFJavaClassCache_t *marpaESLIFJavaClassCachep;
  char                       *fields;
  char                       *signatures;
  short                       staticb;
  jfieldID                    fieldp;
  short                       requiredb;
} marpaESLIFJavaFieldCache_t;

typedef struct marpaESLIFJavaRecognizerContext {
  jobject                      eslifRecognizerInterfacep;  /* Current recognizer interface instance - this can change at every call */
  jbyteArray                   byteArrayp;
  jstring                      encodingp;
  genericStack_t               _symbolStackp;
  genericStack_t              *symbolStackp;
  marpaESLIFJavaClassCache_t   classCache;                    /* Cache of current class */
  marpaESLIFJavaMethodCache_t *methodCachep;                  /* Cache of method IDs */
  size_t                       methodCacheSizel;
  char                        *actions;                       /* shallow copy of last resolved name */
  jmethodID                    methodp;                       /* Current resolved method ID */
  genericStack_t               _objectStack;
  genericStack_t              *objectStackp;
} marpaESLIFJavaRecognizerContext_t;

typedef struct marpaESLIFJavaValueContext {
  jobject                        eslifValueInterfacep;          /* Current eslifValueInterface instance */
  jobject                        eslifGrammarp;                 /* Current eslifGrammarp instance */
  marpaESLIFJavaClassCache_t     classCache;                    /* Cache of current class */
  marpaESLIFJavaMethodCache_t   *methodCachep;                  /* Cache of method IDs */
  size_t                         methodCacheSizel;
  jmethodID                      methodp;                       /* Current resolved method ID */
  char                          *actions;                       /* shallow copy of last resolved name */
  genericStack_t                 _objectStack;
  genericStack_t                *objectStackp;
} marpaESLIFJavaValueContext_t;

typedef struct marpaESLIFJavaSymbolContext {
  genericStack_t                 _objectStack;
  genericStack_t                *objectStackp;
} marpaESLIFJavaSymbolContext_t;

typedef struct marpaESLIFJavaStringGeneratorContext { /* We use genericLogger to generate strings */
  char        *s;
  size_t       l;
  short        okb;
} marpaESLIFJavaStringGeneratorContext_t;

/* ------ */
/* Macros */
/* ------ */
static char _MARPAESLIF_JNI_CONTEXT;
#define MARPAESLIF_JNI_CONTEXT                    &_MARPAESLIF_JNI_CONTEXT
#define MARPAESLIF_JNI_VERSION                    JNI_VERSION_1_4 /* Because of NIO Support */
#define MARPAESLIF_FILENAMES                      "marpaESLIFJava.c"
#define MARPAESLIF_ESLIFEXCEPTION_CLASS           "org/parser/marpa/ESLIFException"
#define MARPAESLIF_ESLIF_CLASS                        "org/parser/marpa/ESLIF"
#define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS         "org/parser/marpa/ESLIFLoggerInterface"
#define MARPAESLIF_ESLIFGRAMMAR_CLASS                 "org/parser/marpa/ESLIFGrammar"
#define MARPAESLIF_ESLIFRECOGNIZER_CLASS              "org/parser/marpa/ESLIFRecognizer"
#define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS     "org/parser/marpa/ESLIFRecognizerInterface"
#define MARPAESLIF_ESLIFVALUE_CLASS                   "org/parser/marpa/ESLIFValue"
#define MARPAESLIF_ESLIFVALUEINTERFACE_CLASS          "org/parser/marpa/ESLIFValueInterface"
#define MARPAESLIF_ESLIFEVENTTYPE_CLASS               "org/parser/marpa/ESLIFEventType"
#define MARPAESLIF_ESLIFEVENT_CLASS                   "org/parser/marpa/ESLIFEvent"
#define MARPAESLIF_ESLIFLOGGERLEVEL_CLASS             "org/parser/marpa/ESLIFLoggerLevel"
#define MARPAESLIF_ESLIFGRAMMARPROPERTIES_CLASS       "org/parser/marpa/ESLIFGrammarProperties"
#define MARPAESLIF_ESLIFGRAMMARRULEPROPERTIES_CLASS   "org/parser/marpa/ESLIFGrammarRuleProperties"
#define MARPAESLIF_ESLIFGRAMMARSYMBOLPROPERTIES_CLASS "org/parser/marpa/ESLIFGrammarSymbolProperties"
#define MARPAESLIF_ESLIFRULEPROPERTYBITSET_CLASS      "org/parser/marpa/ESLIFRulePropertyBitSet"
#define MARPAESLIF_ESLIFSYMBOLPROPERTYBITSET_CLASS    "org/parser/marpa/ESLIFSymbolPropertyBitSet"
#define MARPAESLIF_ESLIFSYMBOLTYPE_CLASS              "org/parser/marpa/ESLIFSymbolType"
#define MARPAESLIF_ESLIFPROGRESS_CLASS                "org/parser/marpa/ESLIFProgress"
#define JAVA_LANG_OBJECT_CLASS                        "java/lang/Object"
#define JAVA_LANG_CLASS_CLASS                         "java/lang/Class"
#define JAVA_LANG_STRING_CLASS                        "java/lang/String"
#define JAVA_LANG_CHARACTER_CLASS                     "java/lang/Character"
#define JAVA_LANG_SHORT_CLASS                         "java/lang/Short"
#define JAVA_LANG_INTEGER_CLASS                       "java/lang/Integer"
#define JAVA_LANG_LONG_CLASS                          "java/lang/Long"
#define JAVA_LANG_FLOAT_CLASS                         "java/lang/Float"
#define JAVA_LANG_DOUBLE_CLASS                        "java/lang/Double"
#define JAVA_LANG_BOOLEAN_CLASS                       "java/lang/Boolean"
#define JAVA_UTIL_HASHMAP_CLASS                       "java/util/HashMap"
#define JAVA_MATH_BIGDECIMAL_CLASS                    "java/math/BigDecimal"
#define JAVA_MATH_BIGINTEGER_CLASS                    "java/math/BigInteger"
#define JAVA_LANG_BYTE_CLASS                          "java/lang/Byte"
#define JAVA_LANG_REFLECT_ARRAY_CLASS                 "java/lang/reflect/Array"
#define JAVA_UTIL_ITERATOR_CLASS                      "java/util/Iterator"
#define JAVA_UTIL_SET_CLASS                           "java/util/Set"
#define JAVA_UTIL_MAP_CLASS                           "java/util/Map"
#define MARPAESLIF_ESLIFSYMBOLEVENTBITSET_CLASS       "org/parser/marpa/ESLIFSymbolEventBitSet"
#define MARPAESLIF_ESLIFJSONENCODER_CLASS             "org/parser/marpa/ESLIFJSONEncoder"
#define MARPAESLIF_ESLIFJSONDECODER_CLASS             "org/parser/marpa/ESLIFJSONDecoder"
#define MARPAESLIF_ESLIFJSONDECODEROPTION_CLASS       "org/parser/marpa/ESLIFJSONDecoderOption"
#define MARPAESLIF_ESLIFREGEXCALLOUT_CLASS            "org/parser/marpa/ESLIFRegexCallout"
#define MARPAESLIF_ESLIFSYMBOL_CLASS                  "org/parser/marpa/ESLIFSymbol"

#define MARPAESLIF_ESLIFVALUEINTERFACE_SYMBOLACTION_SIGNATURE "(Ljava/lang/Object;)Ljava/lang/Object;"
#define MARPAESLIF_ESLIFVALUEINTERFACE_RULEACTION_SIGNATURE   "([Ljava/lang/Object;)Ljava/lang/Object;"
#define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_IFACTION_SIGNATURE "([B)Z"
#define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_EVENTACTION_SIGNATURE "([Lorg/parser/marpa/ESLIFEvent;)Z"
#define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_REGEXACTION_SIGNATURE "(Lorg/parser/marpa/ESLIFRegexCallout;)I"
#define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_GENERATORACTION_SIGNATURE "([Ljava/lang/Object;)Ljava/lang/String;"

#ifdef HAVE_INTPTR_T
#define PTR_TO_JLONG(p) ((jlong) ((intptr_t) (p)))
#else
/* Likely to raise a warning */
#define PTR_TO_JLONG(p) ((jlong) (p))
#endif

/* -------------------------------- */
/* Globals and accessors as macros */
/* -------------------------------- */
static JavaVM  *marpaESLIF_vmp;
static const char *marpaESLIFJava_UTF8s = "UTF-8";
static jstring marpaESLIF_UTF8p;

static marpaESLIFJavaClassCache_t marpaESLIFJavaClassCacheArrayp[] = {
  #define MARPAESLIF_ESLIFEXCEPTION_CLASSCACHE           marpaESLIFJavaClassCacheArrayp[0]
  #define MARPAESLIF_ESLIFEXCEPTION_CLASSP               marpaESLIFJavaClassCacheArrayp[0].classp
  {       MARPAESLIF_ESLIFEXCEPTION_CLASS,               NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIF_CLASSCACHE                    marpaESLIFJavaClassCacheArrayp[1]
  #define MARPAESLIF_ESLIF_CLASSP                        marpaESLIFJavaClassCacheArrayp[1].classp
  {       MARPAESLIF_ESLIF_CLASS,                        NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE     marpaESLIFJavaClassCacheArrayp[2]
  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSP         marpaESLIFJavaClassCacheArrayp[2].classp
  {       MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS,         NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFGRAMMAR_CLASSCACHE             marpaESLIFJavaClassCacheArrayp[3]
  #define MARPAESLIF_ESLIFGRAMMAR_CLASSP                 marpaESLIFJavaClassCacheArrayp[3].classp
  {       MARPAESLIF_ESLIFGRAMMAR_CLASS,                 NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZER_CLASSCACHE          marpaESLIFJavaClassCacheArrayp[4]
  #define MARPAESLIF_ESLIFRECOGNIZER_CLASSP              marpaESLIFJavaClassCacheArrayp[4].classp
  {       MARPAESLIF_ESLIFRECOGNIZER_CLASS,              NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASSCACHE marpaESLIFJavaClassCacheArrayp[5]
  #define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASSP     marpaESLIFJavaClassCacheArrayp[5].classp
  {       MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS,     NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUE_CLASSCACHE               marpaESLIFJavaClassCacheArrayp[6]
  #define MARPAESLIF_ESLIFVALUE_CLASSP                   marpaESLIFJavaClassCacheArrayp[6].classp
  {       MARPAESLIF_ESLIFVALUE_CLASS,                   NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUEINTERFACE_CLASSCACHE      marpaESLIFJavaClassCacheArrayp[7]
  #define MARPAESLIF_ESLIFVALUEINTERFACE_CLASSP          marpaESLIFJavaClassCacheArrayp[7].classp
  {       MARPAESLIF_ESLIFVALUEINTERFACE_CLASS,          NULL, 1 /* requiredb */ },

  #define JAVA_LANG_OBJECT_CLASSCACHE                    marpaESLIFJavaClassCacheArrayp[8]
  #define JAVA_LANG_OBJECT_CLASSP                        marpaESLIFJavaClassCacheArrayp[8].classp
  {       JAVA_LANG_OBJECT_CLASS,                        NULL, 1 /* requiredb */ },

  #define JAVA_LANG_CLASS_CLASSCACHE                     marpaESLIFJavaClassCacheArrayp[9]
  #define JAVA_LANG_CLASS_CLASSP                         marpaESLIFJavaClassCacheArrayp[9].classp
  {       JAVA_LANG_CLASS_CLASS,                         NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFEVENTTYPE_CLASSCACHE           marpaESLIFJavaClassCacheArrayp[10]
  #define MARPAESLIF_ESLIFEVENTTYPE_CLASSP               marpaESLIFJavaClassCacheArrayp[10].classp
  {       MARPAESLIF_ESLIFEVENTTYPE_CLASS,               NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFEVENT_CLASSCACHE               marpaESLIFJavaClassCacheArrayp[11]
  #define MARPAESLIF_ESLIFEVENT_CLASSP                   marpaESLIFJavaClassCacheArrayp[11].classp
  {       MARPAESLIF_ESLIFEVENT_CLASS,                   NULL, 1 /* requiredb */ },

  #define JAVA_LANG_STRING_CLASSCACHE                    marpaESLIFJavaClassCacheArrayp[12]
  #define JAVA_LANG_STRING_CLASSP                        marpaESLIFJavaClassCacheArrayp[12].classp
  {       JAVA_LANG_STRING_CLASS,                        NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFLOGGERLEVEL_CLASSCACHE         marpaESLIFJavaClassCacheArrayp[13]
  #define MARPAESLIF_ESLIFLOGGERLEVEL_CLASSP             marpaESLIFJavaClassCacheArrayp[13].classp
  {       MARPAESLIF_ESLIFLOGGERLEVEL_CLASS,             NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFGRAMMARPROPERTIES_CLASSCACHE   marpaESLIFJavaClassCacheArrayp[14]
  #define MARPAESLIF_ESLIFGRAMMARPROPERTIES_CLASSP       marpaESLIFJavaClassCacheArrayp[14].classp
  {       MARPAESLIF_ESLIFGRAMMARPROPERTIES_CLASS,       NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFGRAMMARRULEPROPERTIES_CLASSCACHE marpaESLIFJavaClassCacheArrayp[15]
  #define MARPAESLIF_ESLIFGRAMMARRULEPROPERTIES_CLASSP     marpaESLIFJavaClassCacheArrayp[15].classp
  {       MARPAESLIF_ESLIFGRAMMARRULEPROPERTIES_CLASS,     NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFGRAMMARSYMBOLPROPERTIES_CLASSCACHE marpaESLIFJavaClassCacheArrayp[16]
  #define MARPAESLIF_ESLIFGRAMMARSYMBOLPROPERTIES_CLASSP     marpaESLIFJavaClassCacheArrayp[16].classp
  {       MARPAESLIF_ESLIFGRAMMARSYMBOLPROPERTIES_CLASS,     NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRULEPROPERTYBITSET_CLASSCACHE  marpaESLIFJavaClassCacheArrayp[17]
  #define MARPAESLIF_ESLIFRULEPROPERTYBITSET_CLASSP      marpaESLIFJavaClassCacheArrayp[17].classp
  {       MARPAESLIF_ESLIFRULEPROPERTYBITSET_CLASS,      NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFSYMBOLPROPERTYBITSET_CLASSCACHE marpaESLIFJavaClassCacheArrayp[18]
  #define MARPAESLIF_ESLIFSYMBOLPROPERTYBITSET_CLASSP     marpaESLIFJavaClassCacheArrayp[18].classp
  {       MARPAESLIF_ESLIFSYMBOLPROPERTYBITSET_CLASS,     NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFSYMBOLTYPE_CLASSCACHE          marpaESLIFJavaClassCacheArrayp[19]
  #define MARPAESLIF_ESLIFSYMBOLTYPE_CLASSP              marpaESLIFJavaClassCacheArrayp[19].classp
  {       MARPAESLIF_ESLIFSYMBOLTYPE_CLASS,              NULL, 1 /* requiredb */ },

  #define JAVA_LANG_CHARACTER_CLASSCACHE                 marpaESLIFJavaClassCacheArrayp[20]
  #define JAVA_LANG_CHARACTER_CLASSP                     marpaESLIFJavaClassCacheArrayp[20].classp
  {       JAVA_LANG_CHARACTER_CLASS,                     NULL, 1 /* requiredb */ },

  #define JAVA_LANG_SHORT_CLASSCACHE                     marpaESLIFJavaClassCacheArrayp[21]
  #define JAVA_LANG_SHORT_CLASSP                         marpaESLIFJavaClassCacheArrayp[21].classp
  {       JAVA_LANG_SHORT_CLASS,                         NULL, 1 /* requiredb */ },

  #define JAVA_LANG_INTEGER_CLASSCACHE                   marpaESLIFJavaClassCacheArrayp[22]
  #define JAVA_LANG_INTEGER_CLASSP                       marpaESLIFJavaClassCacheArrayp[22].classp
  {       JAVA_LANG_INTEGER_CLASS,                       NULL, 1 /* requiredb */ },

  #define JAVA_LANG_LONG_CLASSCACHE                      marpaESLIFJavaClassCacheArrayp[23]
  #define JAVA_LANG_LONG_CLASSP                          marpaESLIFJavaClassCacheArrayp[23].classp
  {       JAVA_LANG_LONG_CLASS,                          NULL, 1 /* requiredb */ },

  #define JAVA_LANG_FLOAT_CLASSCACHE                     marpaESLIFJavaClassCacheArrayp[24]
  #define JAVA_LANG_FLOAT_CLASSP                         marpaESLIFJavaClassCacheArrayp[24].classp
  {       JAVA_LANG_FLOAT_CLASS,                         NULL, 1 /* requiredb */ },

  #define JAVA_LANG_DOUBLE_CLASSCACHE                    marpaESLIFJavaClassCacheArrayp[25]
  #define JAVA_LANG_DOUBLE_CLASSP                        marpaESLIFJavaClassCacheArrayp[25].classp
  {       JAVA_LANG_DOUBLE_CLASS,                        NULL, 1 /* requiredb */ },

  #define JAVA_LANG_BOOLEAN_CLASSCACHE                   marpaESLIFJavaClassCacheArrayp[26]
  #define JAVA_LANG_BOOLEAN_CLASSP                       marpaESLIFJavaClassCacheArrayp[26].classp
  {       JAVA_LANG_BOOLEAN_CLASS,                       NULL, 1 /* requiredb */ },

  #define JAVA_UTIL_HASHMAP_CLASSCACHE                   marpaESLIFJavaClassCacheArrayp[27]
  #define JAVA_UTIL_HASHMAP_CLASSP                       marpaESLIFJavaClassCacheArrayp[27].classp
  {       JAVA_UTIL_HASHMAP_CLASS,                       NULL, 1 /* requiredb */ },

  #define JAVA_MATH_BIGDECIMAL_CLASSCACHE                marpaESLIFJavaClassCacheArrayp[28]
  #define JAVA_MATH_BIGDECIMAL_CLASSP                    marpaESLIFJavaClassCacheArrayp[28].classp
  {       JAVA_MATH_BIGDECIMAL_CLASS,                    NULL, 1 /* requiredb */ },

  #define JAVA_MATH_BIGINTEGER_CLASSCACHE                marpaESLIFJavaClassCacheArrayp[29]
  #define JAVA_MATH_BIGINTEGER_CLASSP                    marpaESLIFJavaClassCacheArrayp[29].classp
  {       JAVA_MATH_BIGINTEGER_CLASS,                    NULL, 1 /* requiredb */ },

  #define JAVA_LANG_BYTE_CLASSCACHE                      marpaESLIFJavaClassCacheArrayp[30]
  #define JAVA_LANG_BYTE_CLASSP                          marpaESLIFJavaClassCacheArrayp[30].classp
  {       JAVA_LANG_BYTE_CLASS,                          NULL, 1 /* requiredb */ },

  #define JAVA_LANG_REFLECT_ARRAY_CLASSCACHE             marpaESLIFJavaClassCacheArrayp[31]
  #define JAVA_LANG_REFLECT_ARRAY_CLASSP                 marpaESLIFJavaClassCacheArrayp[31].classp
  {       JAVA_LANG_REFLECT_ARRAY_CLASS,                 NULL, 1 /* requiredb */ },

  #define JAVA_UTIL_ITERATOR_CLASSCACHE                  marpaESLIFJavaClassCacheArrayp[32]
  #define JAVA_UTIL_ITERATOR_CLASSP                      marpaESLIFJavaClassCacheArrayp[32].classp
  {       JAVA_UTIL_ITERATOR_CLASS,                      NULL, 1 /* requiredb */ },

  #define JAVA_UTIL_SET_CLASSCACHE                       marpaESLIFJavaClassCacheArrayp[33]
  #define JAVA_UTIL_SET_CLASSP                           marpaESLIFJavaClassCacheArrayp[33].classp
  {       JAVA_UTIL_SET_CLASS,                           NULL, 1 /* requiredb */ },

  #define JAVA_UTIL_MAP_CLASSCACHE                       marpaESLIFJavaClassCacheArrayp[34]
  #define JAVA_UTIL_MAP_CLASSP                           marpaESLIFJavaClassCacheArrayp[34].classp
  {       JAVA_UTIL_MAP_CLASS,                           NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFSYMBOLEVENTBITSET_CLASSCACHE   marpaESLIFJavaClassCacheArrayp[35]
  #define MARPAESLIF_ESLIFSYMBOLEVENTBITSET_CLASSP       marpaESLIFJavaClassCacheArrayp[35].classp
  {       MARPAESLIF_ESLIFSYMBOLEVENTBITSET_CLASS,       NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFJSONENCODER_CLASSCACHE         marpaESLIFJavaClassCacheArrayp[36]
  #define MARPAESLIF_ESLIFJSONENCODER_CLASSP             marpaESLIFJavaClassCacheArrayp[36].classp
  {       MARPAESLIF_ESLIFJSONENCODER_CLASS,             NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFJSONDECODER_CLASSCACHE         marpaESLIFJavaClassCacheArrayp[37]
  #define MARPAESLIF_ESLIFJSONDECODER_CLASSP             marpaESLIFJavaClassCacheArrayp[37].classp
  {       MARPAESLIF_ESLIFJSONDECODER_CLASS,             NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFJSONDECODEROPTION_CLASSCACHE   marpaESLIFJavaClassCacheArrayp[38]
  #define MARPAESLIF_ESLIFJSONDECODEROPTION_CLASSP       marpaESLIFJavaClassCacheArrayp[38].classp
  {       MARPAESLIF_ESLIFJSONDECODEROPTION_CLASS,       NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFREGEXCALLOUT_CLASSCACHE        marpaESLIFJavaClassCacheArrayp[39]
  #define MARPAESLIF_ESLIFREGEXCALLOUT_CLASSP            marpaESLIFJavaClassCacheArrayp[39].classp
  {       MARPAESLIF_ESLIFREGEXCALLOUT_CLASS,            NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFSYMBOL_CLASSCACHE              marpaESLIFJavaClassCacheArrayp[40]
  #define MARPAESLIF_ESLIFSYMBOL_CLASSP                  marpaESLIFJavaClassCacheArrayp[40].classp
  {       MARPAESLIF_ESLIFSYMBOL_CLASS,                  NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFPROGRESS_CLASSCACHE            marpaESLIFJavaClassCacheArrayp[41]
  #define MARPAESLIF_ESLIFPROGRESS_CLASSP                marpaESLIFJavaClassCacheArrayp[41].classp
  {       MARPAESLIF_ESLIFPROGRESS_CLASS,                NULL, 1 /* requiredb */ },

  { NULL }
};

static marpaESLIFJavaMethodCache_t marpaESLIFJavaMethodCacheArrayp[] = {
  #define MARPAESLIF_ESLIF_CLASS_getLoggerInterfacep_METHODP                        marpaESLIFJavaMethodCacheArrayp[0].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "getLoggerInterface",                 "()Lorg/parser/marpa/ESLIFLoggerInterface;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIF_CLASS_getMarpaESLIFp_METHODP                             marpaESLIFJavaMethodCacheArrayp[1].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "getMarpaESLIFp",                      "()Ljava/nio/ByteBuffer;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIF_CLASS_setMarpaESLIFp_METHODP                             marpaESLIFJavaMethodCacheArrayp[2].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "setMarpaESLIFp",                      "(Ljava/nio/ByteBuffer;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIF_CLASS_getGenericLoggerContextp_METHODP                   marpaESLIFJavaMethodCacheArrayp[3].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "getGenericLoggerContextp",            "()Ljava/nio/ByteBuffer;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIF_CLASS_setGenericLoggerContextp_METHODP                   marpaESLIFJavaMethodCacheArrayp[4].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "setGenericLoggerContextp",            "(Ljava/nio/ByteBuffer;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIF_CLASS_getGenericLoggerp_METHODP                          marpaESLIFJavaMethodCacheArrayp[5].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "getGenericLoggerp",                   "()Ljava/nio/ByteBuffer;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIF_CLASS_setGenericLoggerp_METHODP                          marpaESLIFJavaMethodCacheArrayp[6].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "setGenericLoggerp",                   "(Ljava/nio/ByteBuffer;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_trace_METHODP                       marpaESLIFJavaMethodCacheArrayp[7].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "trace",                "(Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_debug_METHODP                       marpaESLIFJavaMethodCacheArrayp[8].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "debug",                "(Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_info_METHODP                        marpaESLIFJavaMethodCacheArrayp[9].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "info",                 "(Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_notice_METHODP                      marpaESLIFJavaMethodCacheArrayp[10].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "notice",               "(Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_warning_METHODP                     marpaESLIFJavaMethodCacheArrayp[11].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "warning",              "(Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_error_METHODP                       marpaESLIFJavaMethodCacheArrayp[12].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "error",                "(Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_critical_METHODP                    marpaESLIFJavaMethodCacheArrayp[13].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "critical",             "(Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_alert_METHODP                       marpaESLIFJavaMethodCacheArrayp[14].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "alert",                "(Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_emergency_METHODP                   marpaESLIFJavaMethodCacheArrayp[15].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "emergency",            "(Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFGRAMMAR_CLASS_getLoggerInterfacep_METHODP                 marpaESLIFJavaMethodCacheArrayp[16].methodp
  {      &MARPAESLIF_ESLIFGRAMMAR_CLASSCACHE, "getLoggerInterface",                 "()Lorg/parser/marpa/ESLIFLoggerInterface;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFGRAMMAR_CLASS_getEslif_METHODP                            marpaESLIFJavaMethodCacheArrayp[17].methodp
  {      &MARPAESLIF_ESLIFGRAMMAR_CLASSCACHE, "getEslif",                           "()Lorg/parser/marpa/ESLIF;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFGRAMMAR_CLASS_getMarpaESLIFGrammarp_METHODP               marpaESLIFJavaMethodCacheArrayp[18].methodp
  {      &MARPAESLIF_ESLIFGRAMMAR_CLASSCACHE, "getMarpaESLIFGrammarp",              "()Ljava/nio/ByteBuffer;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFGRAMMAR_CLASS_setMarpaESLIFGrammarp_METHODP               marpaESLIFJavaMethodCacheArrayp[19].methodp
  {      &MARPAESLIF_ESLIFGRAMMAR_CLASSCACHE, "setMarpaESLIFGrammarp",              "(Ljava/nio/ByteBuffer;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZER_CLASS_getLoggerInterfacep_METHODP              marpaESLIFJavaMethodCacheArrayp[20].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZER_CLASSCACHE, "getLoggerInterface",              "()Lorg/parser/marpa/ESLIFLoggerInterface;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZER_CLASS_getEslifGrammar_METHODP                  marpaESLIFJavaMethodCacheArrayp[21].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZER_CLASSCACHE, "getEslifGrammar",                 "()Lorg/parser/marpa/ESLIFGrammar;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZER_CLASS_getMarpaESLIFRecognizerp_METHODP         marpaESLIFJavaMethodCacheArrayp[22].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZER_CLASSCACHE, "getMarpaESLIFRecognizerp",        "()Ljava/nio/ByteBuffer;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZER_CLASS_setMarpaESLIFRecognizerp_METHODP         marpaESLIFJavaMethodCacheArrayp[23].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZER_CLASSCACHE, "setMarpaESLIFRecognizerp",        "(Ljava/nio/ByteBuffer;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZER_CLASS_getEslifRecognizerInterface_METHODP      marpaESLIFJavaMethodCacheArrayp[24].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZER_CLASSCACHE, "getEslifRecognizerInterface",     "()Lorg/parser/marpa/ESLIFRecognizerInterface;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZER_CLASS_getMarpaESLIFRecognizerContextp_METHODP  marpaESLIFJavaMethodCacheArrayp[25].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZER_CLASSCACHE, "getMarpaESLIFRecognizerContextp", "()Ljava/nio/ByteBuffer;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZER_CLASS_setMarpaESLIFRecognizerContextp_METHODP  marpaESLIFJavaMethodCacheArrayp[26].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZER_CLASSCACHE, "setMarpaESLIFRecognizerContextp", "(Ljava/nio/ByteBuffer;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_read_METHODP                    marpaESLIFJavaMethodCacheArrayp[27].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASSCACHE, "read",                   "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isEof_METHODP                   marpaESLIFJavaMethodCacheArrayp[28].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASSCACHE, "isEof",                  "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isCharacterStream_METHODP       marpaESLIFJavaMethodCacheArrayp[29].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASSCACHE, "isCharacterStream",      "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_encoding_METHODP                marpaESLIFJavaMethodCacheArrayp[30].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASSCACHE, "encoding",               "()Ljava/lang/String;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_data_METHODP                    marpaESLIFJavaMethodCacheArrayp[31].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASSCACHE, "data",                   "()[B", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithDisableThreshold_METHODP  marpaESLIFJavaMethodCacheArrayp[32].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASSCACHE, "isWithDisableThreshold", "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithExhaustion_METHODP        marpaESLIFJavaMethodCacheArrayp[33].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASSCACHE, "isWithExhaustion",       "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithNewline_METHODP           marpaESLIFJavaMethodCacheArrayp[34].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASSCACHE, "isWithNewline",          "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithTrack_METHODP             marpaESLIFJavaMethodCacheArrayp[35].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASSCACHE, "isWithTrack",            "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUE_CLASS_getLoggerInterfacep_METHODP                   marpaESLIFJavaMethodCacheArrayp[36].methodp
  {      &MARPAESLIF_ESLIFVALUE_CLASSCACHE, "getLoggerInterface",                   "()Lorg/parser/marpa/ESLIFLoggerInterface;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUE_CLASS_getEslifRecognizer_METHODP                    marpaESLIFJavaMethodCacheArrayp[37].methodp
  {      &MARPAESLIF_ESLIFVALUE_CLASSCACHE, "getEslifRecognizer",                   "()Lorg/parser/marpa/ESLIFRecognizer;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUE_CLASS_getMarpaESLIFValuep_METHODP                   marpaESLIFJavaMethodCacheArrayp[38].methodp
  {      &MARPAESLIF_ESLIFVALUE_CLASSCACHE, "getMarpaESLIFValuep",                  "()Ljava/nio/ByteBuffer;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUE_CLASS_setMarpaESLIFValuep_METHODP                   marpaESLIFJavaMethodCacheArrayp[39].methodp
  {      &MARPAESLIF_ESLIFVALUE_CLASSCACHE, "setMarpaESLIFValuep",                  "(Ljava/nio/ByteBuffer;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUE_CLASS_getEslifValueInterface_METHODP                marpaESLIFJavaMethodCacheArrayp[40].methodp
  {      &MARPAESLIF_ESLIFVALUE_CLASSCACHE, "getEslifValueInterface",               "()Lorg/parser/marpa/ESLIFValueInterface;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUE_CLASS_getMarpaESLIFValueContextp_METHODP            marpaESLIFJavaMethodCacheArrayp[41].methodp
  {      &MARPAESLIF_ESLIFVALUE_CLASSCACHE, "getMarpaESLIFValueContextp",           "()Ljava/nio/ByteBuffer;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUE_CLASS_setMarpaESLIFValueContextp_METHODP            marpaESLIFJavaMethodCacheArrayp[42].methodp
  {      &MARPAESLIF_ESLIFVALUE_CLASSCACHE, "setMarpaESLIFValueContextp",           "(Ljava/nio/ByteBuffer;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_isWithHighRankOnly_METHODP           marpaESLIFJavaMethodCacheArrayp[43].methodp
  {      &MARPAESLIF_ESLIFVALUEINTERFACE_CLASSCACHE, "isWithHighRankOnly",          "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_isWithOrderByRank_METHODP            marpaESLIFJavaMethodCacheArrayp[44].methodp
  {      &MARPAESLIF_ESLIFVALUEINTERFACE_CLASSCACHE, "isWithOrderByRank",           "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_isWithAmbiguous_METHODP              marpaESLIFJavaMethodCacheArrayp[45].methodp
  {      &MARPAESLIF_ESLIFVALUEINTERFACE_CLASSCACHE, "isWithAmbiguous",             "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_isWithNull_METHODP                   marpaESLIFJavaMethodCacheArrayp[46].methodp
  {      &MARPAESLIF_ESLIFVALUEINTERFACE_CLASSCACHE, "isWithNull",                  "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_maxParses_METHODP                    marpaESLIFJavaMethodCacheArrayp[47].methodp
  {      &MARPAESLIF_ESLIFVALUEINTERFACE_CLASSCACHE, "maxParses",                   "()I", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_setResult_METHODP                    marpaESLIFJavaMethodCacheArrayp[48].methodp
  {      &MARPAESLIF_ESLIFVALUEINTERFACE_CLASSCACHE, "setResult",                   "(Ljava/lang/Object;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_getResult_METHODP                    marpaESLIFJavaMethodCacheArrayp[49].methodp
  {      &MARPAESLIF_ESLIFVALUEINTERFACE_CLASSCACHE, "getResult",                   "()Ljava/lang/Object;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_setSymbolName_METHODP                marpaESLIFJavaMethodCacheArrayp[50].methodp
  {      &MARPAESLIF_ESLIFVALUEINTERFACE_CLASSCACHE, "setSymbolName",               "(Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_setSymbolNumber_METHODP              marpaESLIFJavaMethodCacheArrayp[51].methodp
  {      &MARPAESLIF_ESLIFVALUEINTERFACE_CLASSCACHE, "setSymbolNumber",             "(I)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_setRuleName_METHODP                  marpaESLIFJavaMethodCacheArrayp[52].methodp
  {      &MARPAESLIF_ESLIFVALUEINTERFACE_CLASSCACHE, "setRuleName",                 "(Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_setRuleNumber_METHODP                marpaESLIFJavaMethodCacheArrayp[53].methodp
  {      &MARPAESLIF_ESLIFVALUEINTERFACE_CLASSCACHE, "setRuleNumber",               "(I)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_setGrammar_METHODP                   marpaESLIFJavaMethodCacheArrayp[54].methodp
  {      &MARPAESLIF_ESLIFVALUEINTERFACE_CLASSCACHE, "setGrammar",                  "(Lorg/parser/marpa/ESLIFGrammar;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_OBJECT_CLASS_getClass_METHODP                                   marpaESLIFJavaMethodCacheArrayp[55].methodp
  {      &JAVA_LANG_OBJECT_CLASSCACHE, "getClass",                                  "()Ljava/lang/Class;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_CLASS_CLASS_getName_METHODP                                     marpaESLIFJavaMethodCacheArrayp[56].methodp
  {      &JAVA_LANG_CLASS_CLASSCACHE, "getName",                                    "()Ljava/lang/String;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFEVENTTYPE_CLASS_get_METHODP                               marpaESLIFJavaMethodCacheArrayp[57].methodp
  {      &MARPAESLIF_ESLIFEVENTTYPE_CLASSCACHE, "get",                              "(I)Lorg/parser/marpa/ESLIFEventType;", 1 /* static */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFEVENTTYPE_CLASS_getCode_METHODP                           marpaESLIFJavaMethodCacheArrayp[58].methodp
  {      &MARPAESLIF_ESLIFEVENTTYPE_CLASSCACHE, "getCode",                          "()I", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFEVENT_CLASS_init_METHODP                                  marpaESLIFJavaMethodCacheArrayp[59].methodp
  {      &MARPAESLIF_ESLIFEVENT_CLASSCACHE, "<init>",                               "(Lorg/parser/marpa/ESLIFEventType;Ljava/lang/String;Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFLOGGERLEVEL_CLASS_getCode_METHODP                         marpaESLIFJavaMethodCacheArrayp[60].methodp
  {      &MARPAESLIF_ESLIFLOGGERLEVEL_CLASSCACHE, "getCode",                        "()I", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_STRING_CLASS_getBytes_String_METHODP                            marpaESLIFJavaMethodCacheArrayp[61].methodp
  {      &JAVA_LANG_STRING_CLASSCACHE, "getBytes",                                  "(Ljava/lang/String;)[B", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFGRAMMARPROPERTIES_CLASS_init_METHODP                      marpaESLIFJavaMethodCacheArrayp[62].methodp
  {      &MARPAESLIF_ESLIFGRAMMARPROPERTIES_CLASSCACHE, "<init>",                   "(IILjava/lang/String;ZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;II[I[ILjava/lang/String;Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFGRAMMARRULEPROPERTIES_CLASS_init_METHODP                  marpaESLIFJavaMethodCacheArrayp[63].methodp
  {      &MARPAESLIF_ESLIFGRAMMARRULEPROPERTIES_CLASSCACHE, "<init>",               "(ILjava/lang/String;Ljava/lang/String;II[I[ZILjava/lang/String;Ljava/lang/String;ZIZZZIIZ)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFGRAMMARSYMBOLPROPERTIES_CLASS_init_METHODP                marpaESLIFJavaMethodCacheArrayp[64].methodp
  {      &MARPAESLIF_ESLIFGRAMMARSYMBOLPROPERTIES_CLASSCACHE, "<init>",             "(Lorg/parser/marpa/ESLIFSymbolType;ZZZZZILjava/lang/String;Ljava/lang/String;ZLjava/lang/String;ZLjava/lang/String;ZLjava/lang/String;ZLjava/lang/String;ZLjava/lang/String;ZIILjava/lang/String;IILjava/lang/String;Ljava/lang/String;Z)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFSYMBOLTYPE_CLASS_get_METHODP                              marpaESLIFJavaMethodCacheArrayp[65].methodp
  {      &MARPAESLIF_ESLIFSYMBOLTYPE_CLASSCACHE, "get",                             "(I)Lorg/parser/marpa/ESLIFSymbolType;", 1 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIF_CLASS_getLoggerInterfaceByIndicep_METHODP                marpaESLIFJavaMethodCacheArrayp[66].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "getLoggerInterfaceByIndice",          "(I)Lorg/parser/marpa/ESLIFLoggerInterface;", 1 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_CHARACTER_CLASS_init_METHODP                                    marpaESLIFJavaMethodCacheArrayp[67].methodp
  {      &JAVA_LANG_CHARACTER_CLASSCACHE, "<init>",                                 "(C)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_CHARACTER_CLASS_valueOf_METHODP                                 marpaESLIFJavaMethodCacheArrayp[68].methodp
  {      &JAVA_LANG_CHARACTER_CLASSCACHE, "valueOf",                                "(C)Ljava/lang/Character;", 1 /* staticb */, NULL, 0 /* requiredb */ },

  #define JAVA_LANG_SHORT_CLASS_init_METHODP                                        marpaESLIFJavaMethodCacheArrayp[69].methodp
  {      &JAVA_LANG_SHORT_CLASSCACHE, "<init>",                                     "(S)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_SHORT_CLASS_valueOf_METHODP                                     marpaESLIFJavaMethodCacheArrayp[70].methodp
  {      &JAVA_LANG_SHORT_CLASSCACHE, "valueOf",                                    "(S)Ljava/lang/Short;", 1 /* staticb */, NULL, 0 /* requiredb */ },

  #define JAVA_LANG_INTEGER_CLASS_init_METHODP                                      marpaESLIFJavaMethodCacheArrayp[71].methodp
  {      &JAVA_LANG_INTEGER_CLASSCACHE, "<init>",                                   "(I)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_INTEGER_CLASS_valueOf_METHODP                                   marpaESLIFJavaMethodCacheArrayp[72].methodp
  {      &JAVA_LANG_INTEGER_CLASSCACHE, "valueOf",                                  "(I)Ljava/lang/Integer;", 1 /* staticb */, NULL, 0 /* requiredb */ },

  #define JAVA_LANG_LONG_CLASS_init_METHODP                                         marpaESLIFJavaMethodCacheArrayp[73].methodp
  {      &JAVA_LANG_LONG_CLASSCACHE, "<init>",                                      "(J)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_LONG_CLASS_valueOf_METHODP                                      marpaESLIFJavaMethodCacheArrayp[74].methodp
  {      &JAVA_LANG_LONG_CLASSCACHE, "valueOf",                                     "(J)Ljava/lang/Long;", 1 /* staticb */, NULL, 0 /* requiredb */ },

  #define JAVA_LANG_FLOAT_CLASS_init_METHODP                                        marpaESLIFJavaMethodCacheArrayp[75].methodp
  {      &JAVA_LANG_FLOAT_CLASSCACHE, "<init>",                                     "(F)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_FLOAT_CLASS_valueOf_METHODP                                     marpaESLIFJavaMethodCacheArrayp[76].methodp
  {      &JAVA_LANG_FLOAT_CLASSCACHE, "valueOf",                                    "(F)Ljava/lang/Float;", 1 /* staticb */, NULL, 0 /* requiredb */ },

  #define JAVA_LANG_DOUBLE_CLASS_init_METHODP                                       marpaESLIFJavaMethodCacheArrayp[77].methodp
  {      &JAVA_LANG_DOUBLE_CLASSCACHE, "<init>",                                    "(D)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_DOUBLE_CLASS_valueOf_METHODP                                    marpaESLIFJavaMethodCacheArrayp[78].methodp
  {      &JAVA_LANG_DOUBLE_CLASSCACHE, "valueOf",                                   "(D)Ljava/lang/Double;", 1 /* staticb */, NULL, 0 /* requiredb */ },

  #define JAVA_LANG_BOOLEAN_CLASS_init_METHODP                                      marpaESLIFJavaMethodCacheArrayp[79].methodp
  {      &JAVA_LANG_BOOLEAN_CLASSCACHE, "<init>",                                   "(Z)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_BOOLEAN_CLASS_valueOf_METHODP                                   marpaESLIFJavaMethodCacheArrayp[80].methodp
  {      &JAVA_LANG_BOOLEAN_CLASSCACHE, "valueOf",                                  "(Z)Ljava/lang/Boolean;", 1 /* staticb */, NULL, 0 /* requiredb */ },

  #define JAVA_LANG_STRING_CLASS_init_byteArray_METHODP                             marpaESLIFJavaMethodCacheArrayp[81].methodp
  {      &JAVA_LANG_STRING_CLASSCACHE, "<init>",                                    "([B)V", 0 /* staticb */, NULL, 0 /* requiredb */ },

  #define JAVA_LANG_STRING_CLASS_init_byteArray_String_METHODP                      marpaESLIFJavaMethodCacheArrayp[82].methodp
  {      &JAVA_LANG_STRING_CLASSCACHE, "<init>",                                    "([BLjava/lang/String;)V", 0 /* staticb */, NULL, 0 /* requiredb */ },

  #define JAVA_UTIL_HASHMAP_CLASS_init_METHODP                                      marpaESLIFJavaMethodCacheArrayp[83].methodp
  {      &JAVA_UTIL_HASHMAP_CLASSCACHE, "<init>",                                   "()V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_UTIL_HASHMAP_CLASS_put_METHODP                                       marpaESLIFJavaMethodCacheArrayp[84].methodp
  {      &JAVA_UTIL_HASHMAP_CLASSCACHE, "put",                                      "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_MATH_BIGDECIMAL_CLASS_String_init_METHODP                            marpaESLIFJavaMethodCacheArrayp[85].methodp
  {      &JAVA_MATH_BIGDECIMAL_CLASSCACHE, "<init>",                                "(Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_DOUBLE_CLASS_doubleValue_METHODP                                marpaESLIFJavaMethodCacheArrayp[86].methodp
  {      &JAVA_LANG_DOUBLE_CLASSCACHE, "doubleValue",                               "()D", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_CHARACTER_CLASS_charValue_METHODP                               marpaESLIFJavaMethodCacheArrayp[87].methodp
  {      &JAVA_LANG_CHARACTER_CLASSCACHE, "charValue",                              "()C", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_BYTE_CLASS_valueOf_METHODP                                      marpaESLIFJavaMethodCacheArrayp[88].methodp
  {      &JAVA_LANG_BYTE_CLASSCACHE, "valueOf",                                     "(B)Ljava/lang/Byte;", 1 /* staticb */, NULL, 0 /* requiredb */ },

  #define JAVA_LANG_BYTE_CLASS_init_METHODP                                         marpaESLIFJavaMethodCacheArrayp[89].methodp
  {      &JAVA_LANG_BYTE_CLASSCACHE, "<init>",                                      "(B)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_BOOLEAN_CLASS_booleanValue_METHODP                              marpaESLIFJavaMethodCacheArrayp[90].methodp
  {      &JAVA_LANG_BOOLEAN_CLASSCACHE, "booleanValue",                             "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_BYTE_CLASS_byteValue_METHODP                                    marpaESLIFJavaMethodCacheArrayp[91].methodp
  {      &JAVA_LANG_BYTE_CLASSCACHE, "byteValue",                                   "()B", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_CHARACTER_CLASS_toString_METHODP                                marpaESLIFJavaMethodCacheArrayp[92].methodp
  {      &JAVA_LANG_CHARACTER_CLASSCACHE, "toString",                               "()Ljava/lang/String;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_SHORT_CLASS_shortValue_METHODP                                  marpaESLIFJavaMethodCacheArrayp[93].methodp
  {      &JAVA_LANG_SHORT_CLASSCACHE, "shortValue",                                 "()S", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_CLASS_CLASS_equals_METHODP                                      marpaESLIFJavaMethodCacheArrayp[94].methodp
  {      &JAVA_LANG_CLASS_CLASSCACHE, "equals",                                     "(Ljava/lang/Object;)Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_MATH_BIGINTEGER_CLASS_String_init_METHODP                            marpaESLIFJavaMethodCacheArrayp[95].methodp
  {      &JAVA_MATH_BIGINTEGER_CLASSCACHE, "<init>",                                "(Ljava/lang/String;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_INTEGER_CLASS_intValue_METHODP                                  marpaESLIFJavaMethodCacheArrayp[96].methodp
  {      &JAVA_LANG_INTEGER_CLASSCACHE, "intValue",                                 "()I", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_LONG_CLASS_longValue_METHODP                                    marpaESLIFJavaMethodCacheArrayp[97].methodp
  {      &JAVA_LANG_LONG_CLASSCACHE, "longValue",                                   "()J", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_FLOAT_CLASS_floatValue_METHODP                                  marpaESLIFJavaMethodCacheArrayp[98].methodp
  {      &JAVA_LANG_FLOAT_CLASSCACHE, "floatValue",                                 "()F", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_CLASS_CLASS_isArray_METHODP                                     marpaESLIFJavaMethodCacheArrayp[99].methodp
  {      &JAVA_LANG_CLASS_CLASSCACHE, "isArray",                                    "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_CLASS_CLASS_getComponentType_METHODP                            marpaESLIFJavaMethodCacheArrayp[100].methodp
  {      &JAVA_LANG_CLASS_CLASSCACHE, "getComponentType",                           "()Ljava/lang/Class;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_CLASS_CLASS_isPrimitive_METHODP                                 marpaESLIFJavaMethodCacheArrayp[101].methodp
  {      &JAVA_LANG_CLASS_CLASSCACHE, "isPrimitive",                                "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_STRING_CLASS_init_METHODP                                       marpaESLIFJavaMethodCacheArrayp[102].methodp
  {      &JAVA_LANG_STRING_CLASSCACHE, "<init>",                                    "()V", 0 /* staticb */, NULL, 0 /* requiredb */ },

  #define JAVA_UTIL_SET_CLASS_iterator_METHODP                                      marpaESLIFJavaMethodCacheArrayp[103].methodp
  {      &JAVA_UTIL_SET_CLASSCACHE, "iterator",                                     "()Ljava/util/Iterator;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_UTIL_ITERATOR_CLASS_hasNext_METHODP                                  marpaESLIFJavaMethodCacheArrayp[104].methodp
  {      &JAVA_UTIL_ITERATOR_CLASSCACHE, "hasNext",                                 "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_UTIL_ITERATOR_CLASS_next_METHODP                                     marpaESLIFJavaMethodCacheArrayp[105].methodp
  {      &JAVA_UTIL_ITERATOR_CLASSCACHE, "next",                                    "()Ljava/lang/Object;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFJSONDECODEROPTION_CLASS_isDisallowDupkeys_METHODP         marpaESLIFJavaMethodCacheArrayp[106].methodp
  {      &MARPAESLIF_ESLIFJSONDECODEROPTION_CLASSCACHE, "isDisallowDupkeys",        "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFJSONDECODEROPTION_CLASS_getMaxDepth_METHODP               marpaESLIFJavaMethodCacheArrayp[107].methodp
  {      &MARPAESLIF_ESLIFJSONDECODEROPTION_CLASSCACHE, "getMaxDepth",              "()J", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFJSONDECODEROPTION_CLASS_isNoReplacementCharacter_METHODP  marpaESLIFJavaMethodCacheArrayp[108].methodp
  {      &MARPAESLIF_ESLIFJSONDECODEROPTION_CLASSCACHE, "isNoReplacementCharacter", "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFREGEXCALLOUT_CLASS_init_METHODP                           marpaESLIFJavaMethodCacheArrayp[109].methodp
  {      &MARPAESLIF_ESLIFREGEXCALLOUT_CLASSCACHE, "<init>",                        "(Ljava/lang/Integer;Ljava/lang/String;[BLjava/lang/String;JJ[JLjava/lang/Long;JJLjava/lang/String;II)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFSYMBOL_CLASS_getLoggerInterfacep_METHODP                  marpaESLIFJavaMethodCacheArrayp[110].methodp
  {      &MARPAESLIF_ESLIFSYMBOL_CLASSCACHE, "getLoggerInterface",                 "()Lorg/parser/marpa/ESLIFLoggerInterface;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFSYMBOL_CLASS_setMarpaESLIFSymbolp_METHODP                 marpaESLIFJavaMethodCacheArrayp[111].methodp
  {      &MARPAESLIF_ESLIFSYMBOL_CLASSCACHE, "setMarpaESLIFSymbolp",                "(Ljava/nio/ByteBuffer;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFSYMBOL_CLASS_getMarpaESLIFSymbolp_METHODP                 marpaESLIFJavaMethodCacheArrayp[112].methodp
  {      &MARPAESLIF_ESLIFSYMBOL_CLASSCACHE, "getMarpaESLIFSymbolp",                "()Ljava/nio/ByteBuffer;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFSYMBOL_CLASS_getEslif_METHODP                             marpaESLIFJavaMethodCacheArrayp[113].methodp
  {      &MARPAESLIF_ESLIFSYMBOL_CLASSCACHE, "getEslif",                            "()Lorg/parser/marpa/ESLIF;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFPROGRESS_CLASS_init_METHODP                               marpaESLIFJavaMethodCacheArrayp[114].methodp
  {      &MARPAESLIF_ESLIFPROGRESS_CLASSCACHE, "<init>",                            "(IIII)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFSYMBOL_CLASS_getMarpaESLIFSymbolContextp_METHODP          marpaESLIFJavaMethodCacheArrayp[115].methodp
  {      &MARPAESLIF_ESLIFSYMBOL_CLASSCACHE, "getMarpaESLIFSymbolContextp",         "()Ljava/nio/ByteBuffer;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFSYMBOL_CLASS_setMarpaESLIFSymbolContextp_METHODP          marpaESLIFJavaMethodCacheArrayp[116].methodp
  {      &MARPAESLIF_ESLIFSYMBOL_CLASSCACHE, "setMarpaESLIFSymbolContextp",         "(Ljava/nio/ByteBuffer;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_getEslifRecognizer_METHODP      marpaESLIFJavaMethodCacheArrayp[117].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASSCACHE, "getEslifRecognizer",     "()Lorg/parser/marpa/ESLIFRecognizer;", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_setEslifRecognizer_METHODP      marpaESLIFJavaMethodCacheArrayp[118].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASSCACHE, "setEslifRecognizer",     "(Lorg/parser/marpa/ESLIFRecognizer;)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZER_CLASS_isShallow_METHODP                        marpaESLIFJavaMethodCacheArrayp[119].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZER_CLASSCACHE, "isShallow",                       "()Z", 0 /* staticb */, NULL, 1 /* requiredb */ },

  #define MARPAESLIF_ESLIFRECOGNIZER_CLASS_setShallow_METHODP                       marpaESLIFJavaMethodCacheArrayp[120].methodp
  {      &MARPAESLIF_ESLIFRECOGNIZER_CLASSCACHE, "setShallow",                      "(Z)V", 0 /* staticb */, NULL, 1 /* requiredb */ },

  { NULL }
};

static marpaESLIFJavaFieldCache_t marpaESLIFJavaFieldCacheArrayp[] = {
  #define JAVA_LANG_BOOLEAN_CLASS_TYPE_FIELDP                                      marpaESLIFJavaFieldCacheArrayp[0].fieldp
  {      &JAVA_LANG_BOOLEAN_CLASSCACHE, "TYPE",                                    "Ljava/lang/Class;", 1 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_CHARACTER_CLASS_TYPE_FIELDP                                    marpaESLIFJavaFieldCacheArrayp[1].fieldp
  {      &JAVA_LANG_CHARACTER_CLASSCACHE, "TYPE",                                  "Ljava/lang/Class;", 1 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_BYTE_CLASS_TYPE_FIELDP                                         marpaESLIFJavaFieldCacheArrayp[2].fieldp
  {      &JAVA_LANG_BYTE_CLASSCACHE, "TYPE",                                       "Ljava/lang/Class;", 1 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_SHORT_CLASS_TYPE_FIELDP                                        marpaESLIFJavaFieldCacheArrayp[3].fieldp
  {      &JAVA_LANG_SHORT_CLASSCACHE, "TYPE",                                      "Ljava/lang/Class;", 1 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_INTEGER_CLASS_TYPE_FIELDP                                      marpaESLIFJavaFieldCacheArrayp[4].fieldp
  {      &JAVA_LANG_INTEGER_CLASSCACHE, "TYPE",                                    "Ljava/lang/Class;", 1 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_LONG_CLASS_TYPE_FIELDP                                         marpaESLIFJavaFieldCacheArrayp[5].fieldp
  {      &JAVA_LANG_LONG_CLASSCACHE, "TYPE",                                       "Ljava/lang/Class;", 1 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_FLOAT_CLASS_TYPE_FIELDP                                        marpaESLIFJavaFieldCacheArrayp[6].fieldp
  {      &JAVA_LANG_FLOAT_CLASSCACHE, "TYPE",                                      "Ljava/lang/Class;", 1 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_DOUBLE_CLASS_TYPE_FIELDP                                       marpaESLIFJavaFieldCacheArrayp[7].fieldp
  {      &JAVA_LANG_DOUBLE_CLASSCACHE, "TYPE",                                     "Ljava/lang/Class;", 1 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_FLOAT_CLASS_TYPE_POSITIVE_INFINITY                             marpaESLIFJavaFieldCacheArrayp[8].fieldp
  {      &JAVA_LANG_FLOAT_CLASSCACHE, "POSITIVE_INFINITY",                         "F", 1 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_FLOAT_CLASS_TYPE_NEGATIVE_INFINITY                             marpaESLIFJavaFieldCacheArrayp[9].fieldp
  {      &JAVA_LANG_FLOAT_CLASSCACHE, "NEGATIVE_INFINITY",                         "F", 1 /* staticb */, NULL, 1 /* requiredb */ },

  #define JAVA_LANG_FLOAT_CLASS_TYPE_NAN                                           marpaESLIFJavaFieldCacheArrayp[10].fieldp
  {      &JAVA_LANG_FLOAT_CLASSCACHE, "NaN",                                       "F", 1 /* staticb */, NULL, 1 /* requiredb */ },

  { NULL }
};

static jclass booleanTypeClassp = NULL;
static jclass characterTypeClassp = NULL;
static jclass byteTypeClassp = NULL;
static jclass shortTypeClassp = NULL;
static jclass integerTypeClassp = NULL;
static jclass longTypeClassp = NULL;
static jclass floatTypeClassp = NULL;
static jclass doubleTypeClassp = NULL;

static jfloat floatMaxValue;
static jfloat floatMinValue;
static jfloat floatPositiveInfinity;
static jfloat floatNegativeInfinity;
static jfloat floatNaN;

typedef struct fieldsValuesStorage {
  jclass    classp;
  jfieldID  fieldp;
  jclass    *primitiveClasspp;
} fieldsValuesStorage_t;

fieldsValuesStorage_t fieldsValues[] = {
  { NULL, NULL, &booleanTypeClassp },
  { NULL, NULL, &characterTypeClassp },
  { NULL, NULL, &byteTypeClassp },
  { NULL, NULL, &shortTypeClassp },
  { NULL, NULL, &integerTypeClassp },
  { NULL, NULL, &longTypeClassp },
  { NULL, NULL, &floatTypeClassp },
  { NULL, NULL, &doubleTypeClassp }
};

#define MARPAESLIFJAVA_PUSH_PTR(stackp, p) do {                         \
    GENERICSTACK_PUSH_PTR(stackp, p);                                   \
    if (GENERICSTACK_ERROR(stackp)) {                                   \
      RAISEEXCEPTIONF(envp, "%s push failure, %s", #stackp, strerror(errno)); \
    }                                                                   \
  } while (0)


#define MARPAESLIFJAVA_NEW_BYTE(envp, objectp, jbytevalue) do {         \
    if (JAVA_LANG_BYTE_CLASS_valueOf_METHODP != NULL) {                 \
      objectp = (*envp)->CallStaticObjectMethod(envp, JAVA_LANG_BYTE_CLASSP, JAVA_LANG_BYTE_CLASS_valueOf_METHODP, (jbyte) jbytevalue); \
    } else {                                                            \
      objectp = (*envp)->NewObject(envp, JAVA_LANG_BYTE_CLASSP, JAVA_LANG_BYTE_CLASS_init_METHODP, (jbyte) jbytevalue); \
    }                                                                   \
    if (objectp == NULL) {                                              \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define MARPAESLIFJAVA_NEW_SHORT(envp, objectp, jshortvalue) do {       \
    if (JAVA_LANG_SHORT_CLASS_valueOf_METHODP != NULL) {                \
      objectp = (*envp)->CallStaticObjectMethod(envp, JAVA_LANG_SHORT_CLASSP, JAVA_LANG_SHORT_CLASS_valueOf_METHODP, (jshort) jshortvalue); \
    } else {                                                            \
      objectp = (*envp)->NewObject(envp, JAVA_LANG_SHORT_CLASSP, JAVA_LANG_SHORT_CLASS_init_METHODP, (jshort) jshortvalue); \
    }                                                                   \
    if (objectp == NULL) {                                              \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define MARPAESLIFJAVA_NEW_INTEGER(envp, objectp, jintegervalue) do {   \
    if (JAVA_LANG_INTEGER_CLASS_valueOf_METHODP != NULL) {              \
      objectp = (*envp)->CallStaticObjectMethod(envp, JAVA_LANG_INTEGER_CLASSP, JAVA_LANG_INTEGER_CLASS_valueOf_METHODP, (jint) jintegervalue); \
    } else {                                                            \
      objectp = (*envp)->NewObject(envp, JAVA_LANG_INTEGER_CLASSP, JAVA_LANG_INTEGER_CLASS_init_METHODP, (jint) jintegervalue); \
    }                                                                   \
    if (objectp == NULL) {                                              \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define MARPAESLIFJAVA_NEW_LONG(envp, objectp, jlongvalue) do {         \
    if (JAVA_LANG_LONG_CLASS_valueOf_METHODP != NULL) {                 \
      objectp = (*envp)->CallStaticObjectMethod(envp, JAVA_LANG_LONG_CLASSP, JAVA_LANG_LONG_CLASS_valueOf_METHODP, (jlong) jlongvalue); \
    } else {                                                            \
      objectp = (*envp)->NewObject(envp, JAVA_LANG_LONG_CLASSP, JAVA_LANG_LONG_CLASS_init_METHODP, (jlong) jlongvalue); \
    }                                                                   \
    if (objectp == NULL) {                                              \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define MARPAESLIFJAVA_NEW_CHARACTER(envp, objectp, jcharvalue) do {    \
    if (JAVA_LANG_CHARACTER_CLASS_valueOf_METHODP != NULL) {            \
      objectp = (*envp)->CallStaticObjectMethod(envp, JAVA_LANG_CHARACTER_CLASSP, JAVA_LANG_CHARACTER_CLASS_valueOf_METHODP, (jchar) jcharvalue); \
    } else {                                                            \
      objectp = (*envp)->NewObject(envp, JAVA_LANG_CHARACTER_CLASSP, JAVA_LANG_CHARACTER_CLASS_init_METHODP, (jchar) jcharvalue); \
    }                                                                   \
    if (objectp == NULL) {                                              \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define MARPAESLIFJAVA_NEW_FLOAT(envp, objectp, jfloatvalue) do {       \
    if (JAVA_LANG_FLOAT_CLASS_valueOf_METHODP != NULL) {                \
      objectp = (*envp)->CallStaticObjectMethod(envp, JAVA_LANG_FLOAT_CLASSP, JAVA_LANG_FLOAT_CLASS_valueOf_METHODP, (jfloat) jfloatvalue); \
    } else {                                                            \
      objectp = (*envp)->NewObject(envp, JAVA_LANG_FLOAT_CLASSP, JAVA_LANG_FLOAT_CLASS_init_METHODP, (jfloat) jfloatvalue); \
    }                                                                   \
    if (objectp == NULL) {                                              \
      goto err;                                                         \
    }                                                                   \
  } while (0)


#define MARPAESLIFJAVA_NEW_DOUBLE(envp, objectp, jdoublevalue) do {     \
    if (JAVA_LANG_DOUBLE_CLASS_valueOf_METHODP != NULL) {               \
      objectp = (*envp)->CallStaticObjectMethod(envp, JAVA_LANG_DOUBLE_CLASSP, JAVA_LANG_DOUBLE_CLASS_valueOf_METHODP, (jdouble) jdoublevalue); \
    } else {                                                            \
      objectp = (*envp)->NewObject(envp, JAVA_LANG_DOUBLE_CLASSP, JAVA_LANG_DOUBLE_CLASS_init_METHODP, (jdouble) jdoublevalue); \
    }                                                                   \
    if (objectp == NULL) {                                              \
      goto err;                                                         \
    }                                                                   \
  } while (0)


#define MARPAESLIFJAVA_IMPORT_NUMBER_NOT_DECIMAL(envp, objectp, c_format, c_format_cast, c_value) do { \
    genericLogger_t              *_genericLoggerp          = NULL;      \
    jstring                       _numberp;                             \
    marpaESLIFJavaStringGeneratorContext_t  _stringGenerator = { NULL /* char *s */, 0 /* size_t l */, 0 /* short okb */}; \
    char                         *_p; /* To skip eventual spaces at the beginning of the string */ \
                                                                        \
    if (sizeof(c_format_cast) <= sizeof(jbyte)) {                       \
      /* fprintf(stderr, "==> %s: import Byte 0x%lx\n", funcs, (unsigned long) c_value); fflush(stdout); fflush(stderr); */  \
      MARPAESLIFJAVA_NEW_BYTE(envp, objectp, c_value);                  \
    } else if (sizeof(c_format_cast) <= sizeof(jshort)) {               \
      /* fprintf(stderr, "==> %s: import Short '%d'\n", funcs, (int) c_value); fflush(stdout); fflush(stderr); */  \
      MARPAESLIFJAVA_NEW_SHORT(envp, objectp, c_value);                 \
    } else if (sizeof(c_format_cast) <= sizeof(jint)) {                 \
      /* fprintf(stderr, "==> %s: import Integer '%d'\n", funcs, (int) c_value); fflush(stdout); fflush(stderr); */  \
      MARPAESLIFJAVA_NEW_INTEGER(envp, objectp, c_value);               \
    } else if (sizeof(c_format_cast) <= sizeof(jlong)) {                \
      /* fprintf(stderr, "==> %s: import Long '%ld'\n", funcs, (long) c_value); fflush(stdout); fflush(stderr); */  \
      MARPAESLIFJAVA_NEW_LONG(envp, objectp, c_value);                  \
    } else { /* Try with java.math.BigInteger */                        \
      /* We go through the string representation, hopefully containing all digits... */ \
      _genericLoggerp = GENERICLOGGER_CUSTOM(generateStringWithLoggerCallbackv, (void *) &_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE); \
      if (_genericLoggerp == NULL) {                                    \
        RAISEEXCEPTIONF(envp, "GENERICLOGGER_CUSTOM() returned NULL, %s", strerror(errno)); \
      }                                                                 \
      _stringGenerator.s   = NULL;                                      \
      _stringGenerator.l   = 0;                                         \
      _stringGenerator.okb = 0;                                         \
      GENERICLOGGER_TRACEF(_genericLoggerp, c_format, (c_format_cast) c_value); \
      if (! _stringGenerator.okb) {                                     \
        GENERICLOGGER_FREE(_genericLoggerp);                            \
        RAISEEXCEPTIONF(envp, "GENERICLOGGER_TRACEF with format string %s failed, %s", c_format, strerror(errno)); \
      }                                                                 \
      /* ASCII formatting, compatible with Java's modified UTF-8 */     \
      _p = _stringGenerator.s;                                          \
      while (*_p == ' ') {                                              \
        _p++;                                                           \
      }                                                                 \
      _numberp = (*envp)->NewStringUTF(envp, (const char *) _p); \
      if (_numberp == NULL) {                                           \
        /* We want OUR exception to be raised */                        \
        GENERICLOGGER_FREE(_genericLoggerp);                            \
        free(_stringGenerator.s);                                       \
        RAISEEXCEPTIONF(envp, "NewStringUTF(\"%s\") failure", _stringGenerator.s); \
      }                                                                 \
      /* fprintf(stderr, "==> %s: import BigInteger '%s'\n", funcs, (char *) _p); fflush(stdout); fflush(stderr); */  \
      objectp = (*envp)->NewObject(envp, JAVA_MATH_BIGINTEGER_CLASSP, JAVA_MATH_BIGINTEGER_CLASS_String_init_METHODP, _numberp); \
      if (objectp == NULL) {                                            \
        GENERICLOGGER_FREE(_genericLoggerp);                            \
        free(_stringGenerator.s);                                       \
        (*envp)->DeleteLocalRef(envp, _numberp);                        \
        RAISEEXCEPTION(envp, "NewObject failure");                      \
      }                                                                 \
      GENERICLOGGER_FREE(_genericLoggerp);                              \
      free(_stringGenerator.s);                                         \
      (*envp)->DeleteLocalRef(envp, _numberp);                          \
    }                                                                   \
  } while (0)

/* We have special things for +/-Infinity and NaN because Math.BigDecimal does NOT support */
/* these special values: only raw types do. We always project these special values to a jfloat */
/* that have the same representation in java than a jdouble when it is +/-Infinity or NaN */
#define MARPAESLIFJAVA_IMPORT_POSITIVE_INFINITY(marpaESLIFp, envp, objectp) do { \
    /* fprintf(stderr, "==> %s: import float positive infinity\n", funcs); fflush(stdout); fflush(stderr); */ \
    MARPAESLIFJAVA_NEW_FLOAT(envp, objectp, floatPositiveInfinity);     \
  } while (0);

#define MARPAESLIFJAVA_IMPORT_NEGATIVE_INFINITY(marpaESLIFp, envp, objectp) do { \
    /* fprintf(stderr, "==> %s: import float negative infinity\n", funcs); fflush(stdout); fflush(stderr); */ \
    MARPAESLIFJAVA_NEW_FLOAT(envp, objectp, floatNegativeInfinity);     \
  } while (0);

#define MARPAESLIFJAVA_IMPORT_NAN(marpaESLIFp, envp, objectp) do {      \
    /* fprintf(stderr, "==> %s: import float nan\n", funcs); fflush(stdout); fflush(stderr); */ \
    MARPAESLIFJAVA_NEW_FLOAT(envp, objectp, floatNaN);                  \
  } while (0);

#define MARPAESLIFJAVA_IMPORT_NUMBER_DECIMAL(marpaESLIFp, envp, objectp, c_format_cast, c_value, floattos) do { \
    jstring _numberp;                                                   \
    char *_s;                                                           \
                                                                        \
    if (sizeof(c_format_cast) <= sizeof(jfloat)) {                      \
      /* fprintf(stderr, "==> %s: import float '%f'\n", funcs, (double) c_value); fflush(stdout); fflush(stderr); */ \
      MARPAESLIFJAVA_NEW_FLOAT(envp, objectp, c_value);                 \
    } else if (sizeof(c_format_cast) <= sizeof(jdouble)) {              \
      /* fprintf(stderr, "==> %s: import double '%Lf'\n", funcs, (long double) c_value); fflush(stdout); fflush(stderr); */ \
      MARPAESLIFJAVA_NEW_DOUBLE(envp, objectp, c_value);                \
    } else { /* Try with java.math.BigDecimal */                        \
      /* We go through the string representation, hopefully containing all digits... */ \
      _s = floattos(marpaESLIFp, c_value);                              \
      if (_s == NULL) {                                                 \
        RAISEEXCEPTIONF(envp, "%s returned NULL", #floattos);           \
      }                                                                 \
      /* ASCII formatting, compatible with Java's modified UTF-8 */     \
      _numberp = (*envp)->NewStringUTF(envp, (const char *) _s);        \
      if (_numberp == NULL) {                                           \
        /* We want OUR exception to be raised */                        \
        free(_s);                                                       \
        RAISEEXCEPTIONF(envp, "NewStringUTF(\"%s\") failure", _stringGenerator.s); \
      }                                                                 \
      /* fprintf(stderr, "==> %s: import BigFloat '%s'\n", funcs, (char *) _s); fflush(stdout); fflush(stderr); */  \
      objectp = (*envp)->NewObject(envp, JAVA_MATH_BIGDECIMAL_CLASSP, JAVA_MATH_BIGDECIMAL_CLASS_String_init_METHODP, _numberp); \
      if (objectp == NULL) {                                            \
        free(_s);                                                       \
        (*envp)->DeleteLocalRef(envp, _numberp);                        \
        RAISEEXCEPTION(envp, "NewObject failure");                      \
      }                                                                 \
      free(_s);                                                         \
      (*envp)->DeleteLocalRef(envp, _numberp);                          \
    }                                                                   \
  } while (0)

#ifdef MARPAESLIF_HAVE_LONG_LONG

#  define MARPAESLIFJAVA_EXPORT_NUMBER_NOT_DECIMAL(envp, jni_type, jni_value, eslifb) do { \
    if (sizeof(jni_type) <= sizeof(char)) {                             \
      /* fprintf(stderr, "%s: export CHAR '%c'\n", funcs, (char) jni_value); fflush(stdout); fflush(stderr); */ \
      marpaESLIFValueResultp->type            = MARPAESLIF_VALUE_TYPE_CHAR; \
      marpaESLIFValueResultp->contextp        = MARPAESLIF_JNI_CONTEXT; \
      marpaESLIFValueResultp->representationp = NULL;                   \
      marpaESLIFValueResultp->u.c            = (char) jni_value;        \
      eslifb = 1;                                                       \
    } else if (sizeof(jni_type) <= sizeof(short)) {                     \
      /* fprintf(stderr, "%s: export SHORT '%d'\n", funcs, (int) jni_value); fflush(stdout); fflush(stderr); */ \
      marpaESLIFValueResultp->type            = MARPAESLIF_VALUE_TYPE_SHORT; \
      marpaESLIFValueResultp->contextp        = MARPAESLIF_JNI_CONTEXT; \
      marpaESLIFValueResultp->representationp = NULL;                   \
      marpaESLIFValueResultp->u.b            = (short) jni_value;       \
      eslifb = 1;                                                       \
    } else if (sizeof(jni_type) <= sizeof(int)) {                       \
      /* fprintf(stderr, "%s: export INT '%d'\n", funcs, (int) jni_value); fflush(stdout); fflush(stderr); */ \
      marpaESLIFValueResultp->type            = MARPAESLIF_VALUE_TYPE_INT; \
      marpaESLIFValueResultp->contextp        = MARPAESLIF_JNI_CONTEXT; \
      marpaESLIFValueResultp->representationp = NULL;                   \
      marpaESLIFValueResultp->u.i            = (int) jni_value;         \
      eslifb = 1;                                                       \
    } else if (sizeof(jni_type) <= sizeof(long)) {                      \
      /* fprintf(stderr, "%s: export LONG '%ld'\n", funcs, (long) jni_value); fflush(stdout); fflush(stderr); */ \
      marpaESLIFValueResultp->type            = MARPAESLIF_VALUE_TYPE_LONG; \
      marpaESLIFValueResultp->contextp        = MARPAESLIF_JNI_CONTEXT; \
      marpaESLIFValueResultp->representationp = NULL;                   \
      marpaESLIFValueResultp->u.l            = (long) jni_value;        \
      eslifb = 1;                                                       \
    } else if (sizeof(jni_type) <= sizeof(MARPAESLIF_LONG_LONG)) {      \
      /* fprintf(stderr, "%s: export LONG_LONG\n", funcs); fflush(stdout); fflush(stderr); */ \
      marpaESLIFValueResultp->type            = MARPAESLIF_VALUE_TYPE_LONG_LONG; \
      marpaESLIFValueResultp->contextp        = MARPAESLIF_JNI_CONTEXT; \
      marpaESLIFValueResultp->representationp = NULL;                   \
      marpaESLIFValueResultp->u.ll           = (MARPAESLIF_LONG_LONG) jni_value; \
      eslifb = 1;                                                       \
    }                                                                   \
  } while (0)

#else /* MARPAESLIF_HAVE_LONG_LONG */

#  define MARPAESLIFJAVA_EXPORT_NUMBER_NOT_DECIMAL(envp, jni_type, jni_value, eslifb) do { \
    if (sizeof(jni_type) <= sizeof(char)) {                               \
      /* fprintf(stderr, "%s: export CHAR '%c'\n", funcs, (char) jni_value); fflush(stdout); fflush(stderr); */ \
      marpaESLIFValueResultp->type            = MARPAESLIF_VALUE_TYPE_CHAR; \
      marpaESLIFValueResultp->contextp        = MARPAESLIF_JNI_CONTEXT; \
      marpaESLIFValueResultp->representationp = NULL;                   \
      marpaESLIFValueResultp->u.c            = (char) jni_value;        \
      eslifb = 1;                                                       \
    } else if (sizeof(jni_type) <= sizeof(short)) {                     \
      /* fprintf(stderr, "%s: export SHORT '%d'\n", funcs, (int) jni_value); fflush(stdout); fflush(stderr); */ \
      marpaESLIFValueResultp->type            = MARPAESLIF_VALUE_TYPE_SHORT; \
      marpaESLIFValueResultp->contextp        = MARPAESLIF_JNI_CONTEXT; \
      marpaESLIFValueResultp->representationp = NULL;                   \
      marpaESLIFValueResultp->u.b            = (short) jni_value;       \
      eslifb = 1;                                                       \
    } else if (sizeof(jni_type) <= sizeof(int)) {                       \
      /* fprintf(stderr, "%s: export INT '%d'\n", funcs, (int) jni_value); fflush(stdout); fflush(stderr); */ \
      marpaESLIFValueResultp->type            = MARPAESLIF_VALUE_TYPE_INT; \
      marpaESLIFValueResultp->contextp        = MARPAESLIF_JNI_CONTEXT; \
      marpaESLIFValueResultp->representationp = NULL;                   \
      marpaESLIFValueResultp->u.i            = (int) jni_value;         \
      eslifb = 1;                                                       \
    } else if (sizeof(jni_type) <= sizeof(long)) {                      \
      /* fprintf(stderr, "%s: export LONG '%ld'\n", funcs, (long) jni_value); fflush(stdout); fflush(stderr); */ \
      marpaESLIFValueResultp->type            = MARPAESLIF_VALUE_TYPE_LONG; \
      marpaESLIFValueResultp->contextp        = MARPAESLIF_JNI_CONTEXT; \
      marpaESLIFValueResultp->representationp = NULL;                   \
      marpaESLIFValueResultp->u.l            = (long) jni_value;        \
      eslifb = 1;                                                       \
    }                                                                   \
  } while (0)

#endif /* MARPAESLIF_HAVE_LONG_LONG */

#define MARPAESLIFJAVA_EXPORT_NUMBER_DECIMAL(envp, jni_type, jni_value, eslifb) do { \
    if (sizeof(jni_type) <= sizeof(float)) {                            \
      /* fprintf(stderr, "%s: export FLOAT '%f'\n", funcs, (double) jni_value); fflush(stdout); fflush(stderr); */ \
      marpaESLIFValueResultp->type            = MARPAESLIF_VALUE_TYPE_FLOAT; \
      marpaESLIFValueResultp->contextp        = MARPAESLIF_JNI_CONTEXT; \
      marpaESLIFValueResultp->representationp = NULL;                   \
      marpaESLIFValueResultp->u.f            = (float) jni_value;       \
      eslifb = 1;                                                       \
    } else if (sizeof(jni_type) <= sizeof(double)) {                    \
      /* fprintf(stderr, "%s: export FLOAT '%f'\n", funcs, (double) jni_value); fflush(stdout); fflush(stderr); */ \
      marpaESLIFValueResultp->type            = MARPAESLIF_VALUE_TYPE_DOUBLE; \
      marpaESLIFValueResultp->contextp        = MARPAESLIF_JNI_CONTEXT; \
      marpaESLIFValueResultp->representationp = NULL;                   \
      marpaESLIFValueResultp->u.d            = (double) jni_value;      \
      eslifb = 1;                                                       \
    } else if (sizeof(jni_type) <= sizeof(long double)) {               \
      /* fprintf(stderr, "%s: export LONG_DOUBLE '%Lf'\n", funcs, (long double) jni_value); fflush(stdout); fflush(stderr); */ \
      marpaESLIFValueResultp->type            = MARPAESLIF_VALUE_TYPE_LONG_DOUBLE; \
      marpaESLIFValueResultp->contextp        = MARPAESLIF_JNI_CONTEXT; \
      marpaESLIFValueResultp->representationp = NULL;                   \
      marpaESLIFValueResultp->u.ld            = (long double) jni_value; \
      eslifb = 1;                                                       \
    }                                                                   \
  } while (0)

/* -------------- */
/* Static methods */
/* -------------- */
static void  generateStringWithLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static void  genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static void  callLoggerInterfacev(jobject loggerInterfacep, genericLoggerLevel_t logLeveli, const char *msgs);
static void  readerCallbackDisposev(void *userDatavp, char *inputcp, size_t inputl, short eofb, short characterStreamb, char *encodings, size_t encodingl);
static short readerCallbackb(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);
static short ESLIF_contextb(JNIEnv *envp, jobject eslifp, genericLogger_t **genericLoggerpp, marpaESLIF_t **marpaESLIFpp);
static short ESLIFGrammar_contextb(JNIEnv *envp, jobject eslifGrammarp, marpaESLIF_t **marpaESLIFpp, marpaESLIFGrammar_t **marpaESLIFGrammarpp);
static short ESLIFSymbol_contextb(JNIEnv *envp, jobject eslifSymbolp, marpaESLIF_t **marpaESLIFpp, marpaESLIFSymbol_t **marpaESLIFSymbolpp, marpaESLIFJavaSymbolContext_t **marpaESLIFJavaSymbolContextpp);
static short ESLIFRecognizer_contextb(JNIEnv *envp, jobject eslifRecognizerp, marpaESLIFRecognizer_t **marpaESLIFRecognizerpp, marpaESLIFJavaRecognizerContext_t **marpaESLIFJavaRecognizerContextpp);
static short ESLIFValue_contextb(JNIEnv *envp, jobject eslifValuep, marpaESLIFValue_t **marpaESLIFValuepp, marpaESLIFJavaValueContext_t **marpaESLIFJavaValueContextpp);
static marpaESLIFValueRuleCallback_t   marpaESLIFValueRuleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static marpaESLIFValueSymbolCallback_t marpaESLIFValueSymbolActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValueResultSymbolp, char *actions);
static marpaESLIFRecognizerIfCallback_t marpaESLIFRecognizerIfActionResolver(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions);
static marpaESLIFRecognizerEventCallback_t marpaESLIFRecognizerEventActionResolver(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions);
static marpaESLIFRecognizerRegexCallback_t marpaESLIFRecognizerRegexActionResolver(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions);
static marpaESLIFRecognizerGeneratorCallback_t marpaESLIFRecognizerGeneratorActionResolver(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions);
static short marpaESLIFJavaValueContextInject(JNIEnv *envp, marpaESLIFValue_t *marpaESLIFValuep, jobject eslifValueInterfacep, marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp);
static short marpaESLIFValueRuleCallback(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short marpaESLIFJava_valueSymbolCallbackb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResultSymbolp, int resulti);
static short marpaESLIFJava_recognizerIfCallbackb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFValueResultSymbolp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static short marpaESLIFJava_recognizerEventCallbackb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFEvent_t *eventArrayp, size_t eventArrayl, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
static short marpaESLIFJava_recognizerRegexCallbackb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFCalloutBlockp, marpaESLIFValueResultInt_t *marpaESLIFValueResultOutp);
static short marpaESLIFJava_recognizerGeneratorCallbackb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp, marpaESLIFValueResultString_t *marpaESLIFValueResultOutp);
static void  marpaESLIFJava_genericFreeCallbackv(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static jmethodID marpaESLIFJava_valueActionResolveri(JNIEnv *envp, marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp, char *methods, char *signatures);
static jmethodID marpaESLIFJava_recognizerActionResolveri(JNIEnv *envp, marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp, char *methods, char *signatures);
static void marpaESLIFJava_valueContextFreev(JNIEnv *envp, marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp, short onStackb);
static void marpaESLIFJava_recognizerContextFreev(JNIEnv *envp, marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp, short onStackb);
static short marpaESLIFJava_valueContextInitb(JNIEnv *envp, jobject eslifValueInterfacep, jobject eslifGrammarp, marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp);
static short marpaESLIFJava_recognizerContextInitb(JNIEnv *envp, jobject eslifRecognizerInterfacep, marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp, short haveSymbolStackb);
static short marpaESLIFJava_symbolContextInitb(JNIEnv *envp, marpaESLIFJavaSymbolContext_t *marpaESLIFJavaSymbolContextp);
static void marpaESLIFJava_symbolContextFreev(JNIEnv *envp, marpaESLIFJavaSymbolContext_t *marpaESLIFJavaSymbolContextp);
static void marpaESLIFJava_representationCallbackDisposev(void *userDatavp, char *inputcp, size_t inputl, char *encodingasciis);
static short marpaESLIFJava_representationCallbackb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp, char **encodingasciisp, marpaESLIFRepresentationDispose_t *disposeCallbackpp, short *stringbp);
static jobject marpaESLIFJava_grammarPropertiesp(JNIEnv *envp, marpaESLIFGrammarProperty_t *grammarPropertyp);
static jobject marpaESLIFJava_rulePropertiesp(JNIEnv *envp, marpaESLIFRuleProperty_t *rulePropertyp);
static jobject marpaESLIFJava_symbolPropertiesp(JNIEnv *envp, marpaESLIFSymbolProperty_t *symbolPropertyp);
static short marpaESLIFJava_valueGetObjectp(JNIEnv *envp, marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp, marpaESLIFValue_t *marpaESLIFValuep, int stackindicei, marpaESLIFValueResult_t *marpaESLIFValueResultSymbolp, jobject *objectpp);
static short marpaESLIFJava_recognizerGetObjectp(JNIEnv *envp, marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFValueResultp, jobject *objectpp);
static short marpaESLIFJava_valueImportb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static short marpaESLIFJava_recognizerImportb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static short marpaESLIFJava_symbolImportb(marpaESLIFSymbol_t *marpaESLIFSymbolp, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static short marpaESLIFJava_importb(marpaESLIF_t *marpaESLIFp, genericStack_t *objectStackp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
static jstring marpaESLIFJava_marpaESLIFStringToJavap(JNIEnv *envp, marpaESLIFString_t *marpaESLIFStringp);
static jstring marpaESLIFJava_marpaESLIFASCIIToJavap(JNIEnv *envp, char *asciis);
static jstring marpaESLIFJava_marpaESLIFActionToJavap(JNIEnv *envp, marpaESLIFAction_t *actionp);
static short marpaESLIFJava_stack_setb(JNIEnv *envp, marpaESLIFValue_t *marpaESLIFValuep, short resulti, jobject objectp, marpaESLIFValueResult_t *marpaESLIFValueResultOutputp);
static short marpaESLIFJava_JSONDecodeNumberActionb(void *userDatavp, char *strings, size_t stringl, marpaESLIFValueResult_t *marpaESLIFValueResultp, short confidenceb);
static void  marpaESLIFJava_JSONDecodeFreeCallbackv(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);

/* --------------- */
/* Internal macros */
/* --------------- */

/* C.f. https://www.ibm.com/developerworks/aix/library/au-endianc/ */
static const int i_for_is_bigendian = 1;
short is_bigendian;
#define MARPAESLIF_IS_BIGENDIAN() ( (*(char*)&i_for_is_bigendian) == 0 )

/* Check if there is a pending exception - if yes, our raise will NOT raise a new one */
/* Since we asked for JNI_VERSION_1_4 per def ExceptionCheck() is available */
#define HAVEEXCEPTION(envp) (*envp)->ExceptionCheck(envp)

#define RAISEEXCEPTION(envp, msgs) do {                                 \
    if (! HAVEEXCEPTION(envp)) {                                        \
      if (MARPAESLIF_ESLIFEXCEPTION_CLASSP != NULL) {                   \
        genericLogger_t              *genericLoggerp = NULL;            \
        marpaESLIFJavaStringGeneratorContext_t  _stringGenerator;       \
                                                                        \
        _stringGenerator.s   = NULL;                                    \
        _stringGenerator.l   = 0;                                       \
        _stringGenerator.okb = 0;                                       \
                                                                        \
        genericLoggerp = GENERICLOGGER_CUSTOM(generateStringWithLoggerCallbackv, (void *) &_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE); \
        if (genericLoggerp != NULL) {                                   \
          GENERICLOGGER_TRACEF(genericLoggerp, "[In %s at %s:%d] %s", funcs, MARPAESLIF_FILENAMES, __LINE__, msgs); \
          if (_stringGenerator.okb) {                                   \
            (*envp)->ThrowNew(envp, MARPAESLIF_ESLIFEXCEPTION_CLASSP, _stringGenerator.s); \
            free(_stringGenerator.s);                                   \
          }                                                             \
          GENERICLOGGER_FREE(genericLoggerp);                           \
        }                                                               \
      }                                                                 \
    }                                                                   \
    goto err;                                                           \
} while (0)

#define RAISEEXCEPTIONF(envp, fmts, ...) do {                           \
    if (! HAVEEXCEPTION(envp)) {                                        \
      if (MARPAESLIF_ESLIFEXCEPTION_CLASSP != NULL) {                   \
        genericLogger_t              *genericLoggerp = NULL;            \
        marpaESLIFJavaStringGeneratorContext_t  _stringGenerator;       \
                                                                        \
        _stringGenerator.s   = NULL;                                    \
        _stringGenerator.l   = 0;                                       \
        _stringGenerator.okb = 0;                                       \
                                                                        \
        genericLoggerp = GENERICLOGGER_CUSTOM(generateStringWithLoggerCallbackv, (void *) &_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE); \
        if (genericLoggerp != NULL) {                                   \
          GENERICLOGGER_TRACEF(genericLoggerp, "[In %s at %s:%d] " fmts, funcs, MARPAESLIF_FILENAMES, __LINE__, __VA_ARGS__); \
          if (_stringGenerator.okb) {                                   \
            (*envp)->ThrowNew(envp, MARPAESLIF_ESLIFEXCEPTION_CLASSP, _stringGenerator.s); \
            free(_stringGenerator.s);                                   \
          }                                                             \
          GENERICLOGGER_FREE(genericLoggerp);                           \
        }                                                               \
      }                                                                 \
    }                                                                   \
    goto err;                                                           \
} while (0)

/* ---------------------------------------------------------------------------- */
/* Bytebuffer stuff - we always use a capaticy of 1 - we know what we are doing */
/* ---------------------------------------------------------------------------- */
#define BYTEBUFFER(what) byteBuffer##what##p
#define MARPAESLIF_PTR2BYTEBUFFER(what,ptr) do {                        \
    BYTEBUFFER(what) = (*envp)->NewDirectByteBuffer(envp, ptr, 1);      \
    if (BYTEBUFFER(what) == NULL) {                                     \
      RAISEEXCEPTION(envp, "NewDirectByteBuffer failure");              \
    }                                                                   \
  } while (0)

/*****************************************************************************/
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *JavaVMp, void* reservedp)
/*****************************************************************************/
{
  static const char            *funcs                      = "JNI_OnLoad";
  JNIEnv                       *envp                       = NULL;
  jclass                        classp                     = NULL;
  marpaESLIFJavaClassCache_t   *marpaESLIFJavaClassCachep  = marpaESLIFJavaClassCacheArrayp;
  marpaESLIFJavaMethodCache_t  *marpaESLIFJavaMethodCachep = marpaESLIFJavaMethodCacheArrayp;
  marpaESLIFJavaFieldCache_t   *marpaESLIFJavaFieldCachep  = marpaESLIFJavaFieldCacheArrayp;
  jstring                       stringp                    = NULL;
  jint                          rci;
  size_t                        i;

  /* ------------------------------------------------ */
  /* It is safe to store JavaVMp in a global variable */
  /* ------------------------------------------------ */
  marpaESLIF_vmp = JavaVMp;

  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    RAISEEXCEPTION(envp, "Failed to get environment");
  }

  /* Idem for marpaESLIF_UTF8p */
  stringp = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, "UTF-8");
  marpaESLIF_UTF8p = (jstring) (*envp)->NewGlobalRef(envp, (jobject) stringp);
  if (marpaESLIF_UTF8p == NULL) {
    /* We want OUR exception to be raised */
    RAISEEXCEPTION(envp, "NewGlobalRef failure");
  }

  /* --------------------------------------------------------------------------------------------------- */
  /* Get cache of classes - this is ok only if the result of FindClass is turned into a global reference */
  /* --------------------------------------------------------------------------------------------------- */
  while (marpaESLIFJavaClassCachep->classs != NULL) {
    if (classp != NULL) {
      (*envp)->DeleteLocalRef(envp, classp);
    }
    classp = (*envp)->FindClass(envp, marpaESLIFJavaClassCachep->classs);
    if (classp == NULL) {
      if (marpaESLIFJavaClassCachep->requiredb) {
        /* We want OUR exception to be raised */
        (*envp)->ExceptionClear(envp);
        RAISEEXCEPTIONF(envp, "Failed to find class \"%s\"", marpaESLIFJavaClassCachep->classs);
      } else {
        (*envp)->ExceptionClear(envp);
        /* Here, up to the program to know what it is doing */
      }
    } else {
      marpaESLIFJavaClassCachep->classp = (*envp)->NewGlobalRef(envp, classp);
      if (marpaESLIFJavaClassCachep->classp == NULL) {
        RAISEEXCEPTION(envp, "NewGlobalRef failure");
      }
      (*envp)->DeleteLocalRef(envp, classp);
      classp = NULL;
    }
    marpaESLIFJavaClassCachep++;
  }

  /* -------------------- */
  /* Get cache of methods */
  /* -------------------- */
  while (marpaESLIFJavaMethodCachep->marpaESLIFJavaClassCachep != NULL) {
    /* If class does not exist and method is required this is an error */
    if (marpaESLIFJavaMethodCachep->marpaESLIFJavaClassCachep->classp == NULL) {
      if (marpaESLIFJavaMethodCachep->requiredb) {
        RAISEEXCEPTIONF(envp,
                        "Failed to find %smethod \"%s\" with signature \"%s\" in class \"%s\" because the later was not found",
                        marpaESLIFJavaMethodCachep->staticb ? "static " : "",
                        marpaESLIFJavaMethodCachep->methods,
                        marpaESLIFJavaMethodCachep->signatures,
                        marpaESLIFJavaMethodCachep->marpaESLIFJavaClassCachep->classs);
      }
    } else {
      if (marpaESLIFJavaMethodCachep->staticb) {
        marpaESLIFJavaMethodCachep->methodp = (*envp)->GetStaticMethodID(envp,
                                                                     marpaESLIFJavaMethodCachep->marpaESLIFJavaClassCachep->classp,
                                                                     marpaESLIFJavaMethodCachep->methods,
                                                                     marpaESLIFJavaMethodCachep->signatures);
      } else {
        marpaESLIFJavaMethodCachep->methodp = (*envp)->GetMethodID(envp,
                                                               marpaESLIFJavaMethodCachep->marpaESLIFJavaClassCachep->classp,
                                                               marpaESLIFJavaMethodCachep->methods,
                                                               marpaESLIFJavaMethodCachep->signatures);
      }
      if (marpaESLIFJavaMethodCachep->methodp == NULL) {
        if (marpaESLIFJavaMethodCachep->requiredb) {
          /* We want OUR exception to be raised */
          (*envp)->ExceptionClear(envp);
          RAISEEXCEPTIONF(envp,
                          "Failed to find %smethod \"%s\" with signature \"%s\" in class \"%s\"",
                          marpaESLIFJavaMethodCachep->staticb ? "static " : "",
                          marpaESLIFJavaMethodCachep->methods,
                          marpaESLIFJavaMethodCachep->signatures,
                          marpaESLIFJavaMethodCachep->marpaESLIFJavaClassCachep->classs);
        } else {
          /* Here, up to the program to know what it is doing */
          (*envp)->ExceptionClear(envp);
        }
      }
    }
    marpaESLIFJavaMethodCachep++;
  }

  /* ------------------- */
  /* Get cache of fields */
  /* ------------------- */
  while (marpaESLIFJavaFieldCachep->marpaESLIFJavaClassCachep != NULL) {
    if (marpaESLIFJavaFieldCachep->marpaESLIFJavaClassCachep->classp == NULL) {
      /* We want OUR exception to be raised */
      (*envp)->ExceptionClear(envp);
      RAISEEXCEPTIONF(envp,
                      "Failed to find %sfield \"%s\" with signature \"%s\" in class \"%s\" because the later was not found",
                      marpaESLIFJavaMethodCachep->staticb ? "static " : "",
                      marpaESLIFJavaFieldCachep->fields,
                      marpaESLIFJavaFieldCachep->signatures,
                      marpaESLIFJavaFieldCachep->marpaESLIFJavaClassCachep->classs);
    } else {
      if (marpaESLIFJavaFieldCachep->staticb) {
        marpaESLIFJavaFieldCachep->fieldp = (*envp)->GetStaticFieldID(envp,
                                                                  marpaESLIFJavaFieldCachep->marpaESLIFJavaClassCachep->classp,
                                                                  marpaESLIFJavaFieldCachep->fields,
                                                                  marpaESLIFJavaFieldCachep->signatures);
      } else {
        marpaESLIFJavaFieldCachep->fieldp = (*envp)->GetFieldID(envp,
                                                            marpaESLIFJavaFieldCachep->marpaESLIFJavaClassCachep->classp,
                                                            marpaESLIFJavaFieldCachep->fields,
                                                            marpaESLIFJavaFieldCachep->signatures);
      }
      if (marpaESLIFJavaFieldCachep->fieldp == NULL) {
        if (marpaESLIFJavaFieldCachep->requiredb) {
          /* We want OUR exception to be raised */
          (*envp)->ExceptionClear(envp);
          RAISEEXCEPTIONF(envp,
                          "Failed to find %sfield \"%s\" with signature \"%s\" in class \"%s\"",
                          marpaESLIFJavaMethodCachep->staticb ? "static " : "",
                          marpaESLIFJavaFieldCachep->fields,
                          marpaESLIFJavaFieldCachep->signatures,
                          marpaESLIFJavaFieldCachep->marpaESLIFJavaClassCachep->classs);
        } else {          
          /* Here, up to the program to know what it is doing */
          (*envp)->ExceptionClear(envp);
        }
      }
    }
    marpaESLIFJavaFieldCachep++;
  }

  /* Get some static fields values */
  /* We have to fill the fieldsValues structure here for things that are not constant */
  fieldsValues[0].classp = JAVA_LANG_BOOLEAN_CLASSP;   fieldsValues[0].fieldp = JAVA_LANG_BOOLEAN_CLASS_TYPE_FIELDP;
  fieldsValues[1].classp = JAVA_LANG_CHARACTER_CLASSP; fieldsValues[1].fieldp = JAVA_LANG_CHARACTER_CLASS_TYPE_FIELDP;
  fieldsValues[2].classp = JAVA_LANG_BYTE_CLASSP;      fieldsValues[2].fieldp = JAVA_LANG_BYTE_CLASS_TYPE_FIELDP;
  fieldsValues[3].classp = JAVA_LANG_SHORT_CLASSP;     fieldsValues[3].fieldp = JAVA_LANG_SHORT_CLASS_TYPE_FIELDP;
  fieldsValues[4].classp = JAVA_LANG_INTEGER_CLASSP;   fieldsValues[4].fieldp = JAVA_LANG_INTEGER_CLASS_TYPE_FIELDP;
  fieldsValues[5].classp = JAVA_LANG_LONG_CLASSP;      fieldsValues[5].fieldp = JAVA_LANG_LONG_CLASS_TYPE_FIELDP;
  fieldsValues[6].classp = JAVA_LANG_FLOAT_CLASSP;     fieldsValues[6].fieldp = JAVA_LANG_FLOAT_CLASS_TYPE_FIELDP;
  fieldsValues[7].classp = JAVA_LANG_DOUBLE_CLASSP;    fieldsValues[7].fieldp = JAVA_LANG_DOUBLE_CLASS_TYPE_FIELDP;

  floatPositiveInfinity = (*envp)->GetStaticFloatField(envp, JAVA_LANG_FLOAT_CLASSP, JAVA_LANG_FLOAT_CLASS_TYPE_POSITIVE_INFINITY);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  floatNegativeInfinity = (*envp)->GetStaticFloatField(envp, JAVA_LANG_FLOAT_CLASSP, JAVA_LANG_FLOAT_CLASS_TYPE_NEGATIVE_INFINITY);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  floatNaN = (*envp)->GetStaticFloatField(envp, JAVA_LANG_FLOAT_CLASSP, JAVA_LANG_FLOAT_CLASS_TYPE_NAN);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  for (i = 0; i < sizeof(fieldsValues)/sizeof(fieldsValues[0]); i++) {
    classp = (*envp)->GetStaticObjectField(envp, fieldsValues[i].classp, fieldsValues[i].fieldp);
    if (classp == NULL) {
      RAISEEXCEPTION(envp, "GetStaticObjectField failure");
    }
    *(fieldsValues[i].primitiveClasspp) = (*envp)->NewGlobalRef(envp, classp);
    if (*(fieldsValues[i].primitiveClasspp) == NULL) {
      RAISEEXCEPTION(envp, "NewGlobalRef failure");
    }
    (*envp)->DeleteLocalRef(envp, classp);
  }

  /* Get endianness */
  is_bigendian = MARPAESLIF_IS_BIGENDIAN();

  rci = MARPAESLIF_JNI_VERSION;
  goto done;

 err:
  rci = JNI_ERR;

 done:
  if (envp != NULL) {
    if (classp != NULL) {
      (*envp)->DeleteLocalRef(envp, classp);
    }
    if (stringp != NULL) {
      (*envp)->DeleteLocalRef(envp, stringp);
    }
  }
  return rci;
}

/*****************************************************************************/
JNIEXPORT void JNICALL JNI_OnUnLoad(JavaVM *vmp, void* reservedp)
/*****************************************************************************/
{
  static const char          *funcs = "JNI_OnUnLoad";
  JNIEnv                     *envp;
  marpaESLIFJavaClassCache_t *marpaESLIFJavaClassCachep  = marpaESLIFJavaClassCacheArrayp;
  size_t                      i;

  /* ------------------------------------------------- */
  /* It was safe to store JavaVMp in a global variable */
  /* ------------------------------------------------- */
  if (((*vmp)->GetEnv(vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    RAISEEXCEPTION(envp, "Failed to get environment");
  }

  /* ---------------------------------------------- */
  /* Remove global references created by JNI_OnLoad */
  /* -----------------------------------------------*/
  while (marpaESLIFJavaClassCachep->classs != NULL) {
    if (marpaESLIFJavaClassCachep->classp != NULL) {
      (*envp)->DeleteGlobalRef(envp, marpaESLIFJavaClassCachep->classp);
    }
    marpaESLIFJavaClassCachep++;
  }
  if (marpaESLIF_UTF8p != NULL) {
    (*envp)->DeleteGlobalRef(envp, (jobject) marpaESLIF_UTF8p);
  }

  for (i = 0; i < sizeof(fieldsValues)/sizeof(fieldsValues[0]); i++) {
    if (*(fieldsValues[i].primitiveClasspp) != NULL) {
      (*envp)->DeleteGlobalRef(envp, *(fieldsValues[i].primitiveClasspp));
    }
  }

 err:
  return;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIF_jniNew(JNIEnv *envp, jobject eslifp, jint indice)
/*****************************************************************************/
{
  static const char                    *funcs = "Java_org_parser_marpa_ESLIF_jniNew";
  marpaESLIFJavaGenericLoggerContext_t *marpaESLIFJavaGenericLoggerContextp = NULL;
  genericLogger_t                      *genericLoggerp                      = NULL;
  marpaESLIF_t                         *marpaESLIFp                         = NULL;
  marpaESLIFOption_t                    marpaESLIFOption;
  jobject                               BYTEBUFFER(marpaESLIFJavaGenericLoggerContext);
  jobject                               BYTEBUFFER(genericLogger);
  jobject                               BYTEBUFFER(marpaESLIF);

  /* ------------------------------------- */
  /* Create and save genericLogger context */
  /* ------------------------------------- */
  marpaESLIFJavaGenericLoggerContextp = (marpaESLIFJavaGenericLoggerContext_t *) malloc(sizeof(marpaESLIFJavaGenericLoggerContext_t));
  if (marpaESLIFJavaGenericLoggerContextp == NULL) {
    RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
  }

  marpaESLIFJavaGenericLoggerContextp->indice = indice;

  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIFJavaGenericLoggerContext, marpaESLIFJavaGenericLoggerContextp);
 (*envp)->CallVoidMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_setGenericLoggerContextp_METHODP, BYTEBUFFER(marpaESLIFJavaGenericLoggerContext));
 if (HAVEEXCEPTION(envp)) {
   goto err;
 }

 /* ------------------------------ */
 /* Create and save generic logger */
 /* ------------------------------ */
  genericLoggerp = genericLogger_newp(genericLoggerCallbackv, (void *) marpaESLIFJavaGenericLoggerContextp, GENERICLOGGER_LOGLEVEL_TRACE);
  if (genericLoggerp == NULL) {
    RAISEEXCEPTIONF(envp, "genericLogger_newp failure, %s", strerror(errno));
  }
  MARPAESLIF_PTR2BYTEBUFFER(genericLogger, genericLoggerp);
  (*envp)->CallVoidMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_setGenericLoggerp_METHODP, BYTEBUFFER(genericLogger));
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  /* --------------------------- */
  /* Create and save marpaESLIFp */
  /* --------------------------- */
  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    RAISEEXCEPTIONF(envp, "marpaESLIF_newp failure, %s", strerror(errno));
  }
  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIF, marpaESLIFp);
  (*envp)->CallVoidMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_setMarpaESLIFp_METHODP, BYTEBUFFER(marpaESLIF));
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  goto done;
  
 err:
  /* Java_org_parser_marpa_ESLIF_jniFree(envp, eslifp); */

 done:
  return;
}

/*****************************************************************************/
JNIEXPORT jstring JNICALL Java_org_parser_marpa_ESLIF_jniVersion(JNIEnv *envp, jobject eslifp)
/*****************************************************************************/
{
  static const char *funcs = "Java_org_parser_marpa_ESLIF_jniVersion";
  marpaESLIF_t      *marpaESLIFp;
  char              *versions;

  if (ESLIF_contextb(envp, eslifp, NULL /* genericLoggerpp */, &marpaESLIFp)) {
    if (! marpaESLIF_versionb(marpaESLIFp, &versions)) {
      RAISEEXCEPTIONF(envp, "marpaESLIF_versionb failure, %s", strerror(errno));
    }

    return marpaESLIFJava_marpaESLIFASCIIToJavap(envp, versions);
  }

 err:
  return NULL;
}

/*****************************************************************************/
JNIEXPORT jint JNICALL Java_org_parser_marpa_ESLIF_jniVersionMajor(JNIEnv *envp, jobject eslifp)
/*****************************************************************************/
{
  static const char *funcs = "Java_org_parser_marpa_ESLIF_jniVersionMajor";
  marpaESLIF_t      *marpaESLIFp;
  int               majori;

  if (ESLIF_contextb(envp, eslifp, NULL /* genericLoggerpp */, &marpaESLIFp)) {
    if (! marpaESLIF_versionMajorb(marpaESLIFp, &majori)) {
      RAISEEXCEPTIONF(envp, "marpaESLIF_versionMajorb failure, %s", strerror(errno));
    }

    return (jint) majori;
  }

 err:
  return 0;
}

/*****************************************************************************/
JNIEXPORT jint JNICALL Java_org_parser_marpa_ESLIF_jniVersionMinor(JNIEnv *envp, jobject eslifp)
/*****************************************************************************/
{
  static const char *funcs = "Java_org_parser_marpa_ESLIF_jniVersionMinor";
  marpaESLIF_t      *marpaESLIFp;
  int               minori;

  if (ESLIF_contextb(envp, eslifp, NULL /* genericLoggerpp */, &marpaESLIFp)) {
    if (! marpaESLIF_versionMinorb(marpaESLIFp, &minori)) {
      RAISEEXCEPTIONF(envp, "marpaESLIF_versionMinorb failure, %s", strerror(errno));
    }

    return (jint) minori;
  }

 err:
  return 0;
}

/*****************************************************************************/
JNIEXPORT jint JNICALL Java_org_parser_marpa_ESLIF_jniVersionPatch(JNIEnv *envp, jobject eslifp)
/*****************************************************************************/
{
  static const char *funcs = "Java_org_parser_marpa_ESLIF_jniVersionPatch";
  marpaESLIF_t      *marpaESLIFp;
  int               patchi;

  if (ESLIF_contextb(envp, eslifp, NULL /* genericLoggerpp */, &marpaESLIFp)) {
    if (! marpaESLIF_versionPatchb(marpaESLIFp, &patchi)) {
      RAISEEXCEPTIONF(envp, "marpaESLIF_versionPatchb failure, %s", strerror(errno));
    }

    return (jint) patchi;
  }

 err:
  return 0;
}

/*****************************************************************************/
JNIEXPORT jstring JNICALL Java_org_parser_marpa_ESLIF_jniJavaVersion(JNIEnv *envp, jclass classp)
/*****************************************************************************/
{
  static const char *funcs = "Java_org_parser_marpa_ESLIF_jniJavaVersion";

  return marpaESLIFJava_marpaESLIFASCIIToJavap(envp, MARPAESLIFJAVA_VERSION);
}

/*****************************************************************************/
JNIEXPORT jint JNICALL Java_org_parser_marpa_ESLIF_jniJavaVersionMajor(JNIEnv *envp, jclass classp)
/*****************************************************************************/
{
  static const char *funcs = "Java_org_parser_marpa_ESLIF_jniJavaVersionMajor";

  return (jint) MARPAESLIFJAVA_VERSION_MAJOR;
}

/*****************************************************************************/
JNIEXPORT jint JNICALL Java_org_parser_marpa_ESLIF_jniJavaVersionMinor(JNIEnv *envp, jclass classp)
/*****************************************************************************/
{
  static const char *funcs = "Java_org_parser_marpa_ESLIF_jniJavaVersionMinor";

  return (jint) MARPAESLIFJAVA_VERSION_MINOR;
}

/*****************************************************************************/
JNIEXPORT jint JNICALL Java_org_parser_marpa_ESLIF_jniJavaVersionPatch(JNIEnv *envp, jclass classp)
/*****************************************************************************/
{
  static const char *funcs = "Java_org_parser_marpa_ESLIF_jniJavaVersionPatch";

  return (jint) MARPAESLIFJAVA_VERSION_PATCH;
}

/*
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIF_jniFree(JNIEnv *envp, jobject eslifp)
{
  marpaESLIFJavaGenericLoggerContext_t *marpaESLIFJavaGenericLoggerContextp;
  genericLogger_t        *genericLoggerp;
  marpaESLIF_t           *marpaESLIFp;

  if (! ESLIF_contextb(envp, eslifp, &genericLoggerp, &marpaESLIFJavaGenericLoggerContextp, &marpaESLIFp)) {
    goto done;
  }

  marpaESLIF_freev(marpaESLIFp);
  genericLogger_freev(&genericLoggerp);
  if (marpaESLIFJavaGenericLoggerContextp != NULL) {
    free(marpaESLIFJavaGenericLoggerContextp);
  }

  (*envp)->CallVoidMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_setGenericLoggerContextp_METHODP, NULL);
  (*envp)->CallVoidMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_setGenericLoggerp_METHODP, NULL);
  (*envp)->CallVoidMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_setMarpaESLIFp_METHODP, NULL);

 done:
  return;
}
*/

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFGrammar_jniNew(JNIEnv *envp, jobject eslifGrammarp, jbyteArray utf8byteArrayp)
/*****************************************************************************/
{
  static const char                *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniNew";
  jbyte                            *utf8bytep = NULL;
  marpaESLIFGrammar_t              *marpaESLIFGrammarp;
  marpaESLIF_t                     *marpaESLIFp;
  marpaESLIFGrammarOption_t         marpaESLIFGrammarOption;
  jobject                           BYTEBUFFER(marpaESLIFGrammar);
  jsize                             utf8lengthl;

  /* Always update genericLogger context */
  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, &marpaESLIFp, NULL /* marpaESLIFGrammarpp */)) {
    goto err;
  }

  /* Get information from the stack */
  if (utf8byteArrayp == NULL) {
    RAISEEXCEPTION(envp, "ByteArray argument is NULL");
  }
  utf8lengthl = (*envp)->GetArrayLength(envp, utf8byteArrayp);
  if (utf8lengthl <= 0) {
    RAISEEXCEPTION(envp, "ByteArray is empty");
  }
  utf8bytep = (jbyte *) malloc(utf8lengthl * sizeof(jbyte));
  if (utf8bytep == NULL) {
    RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
  }
  (*envp)->GetByteArrayRegion(envp, utf8byteArrayp, 0, utf8lengthl, utf8bytep);

  /* Create C object */
  marpaESLIFGrammarOption.bytep               = (void *) utf8bytep;
  marpaESLIFGrammarOption.bytel               = (size_t) utf8lengthl;
  marpaESLIFGrammarOption.encodings           = "UTF-8";
  marpaESLIFGrammarOption.encodingl           = strlen("UTF-8");

  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    RAISEEXCEPTIONF(envp, "marpaESLIFGrammar_newp failure, %s", strerror(errno));
  }

  /* Store the object */
  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIFGrammar, marpaESLIFGrammarp);  
  (*envp)->CallVoidMethod(envp, eslifGrammarp, MARPAESLIF_ESLIFGRAMMAR_CLASS_setMarpaESLIFGrammarp_METHODP, BYTEBUFFER(marpaESLIFGrammar));
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  goto done;
  
 err:
  Java_org_parser_marpa_ESLIFGrammar_jniFree(envp, eslifGrammarp);

 done:
  if (utf8bytep != NULL) {
    free(utf8bytep);
  }
  return;
}

/*****************************************************************************/
JNIEXPORT jint JNICALL Java_org_parser_marpa_ESLIFGrammar_jniNgrammar(JNIEnv *envp, jobject eslifGrammarp)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniNGrammar";
  int                  ngrammari;
  marpaESLIFGrammar_t *marpaESLIFGrammarp;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_ngrammarib failure");
  }

  goto done;

 err:
  ngrammari = -1;

 done:
  return (jint) ngrammari;
}

/*****************************************************************************/
JNIEXPORT jint JNICALL Java_org_parser_marpa_ESLIFGrammar_jniCurrentLevel(JNIEnv *envp, jobject eslifGrammarp)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniCurrentLevel";
  int                  leveli;
  marpaESLIFGrammar_t *marpaESLIFGrammarp;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_grammar_currentb(marpaESLIFGrammarp, &leveli, NULL)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_grammar_currentb failure");
  }

  goto done;

 err:
  leveli = -1;

 done:
  return (jint) leveli;
}

/*****************************************************************************/
JNIEXPORT jstring JNICALL Java_org_parser_marpa_ESLIFGrammar_jniCurrentDescription(JNIEnv *envp, jobject eslifGrammarp)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniCurrentDescription";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  marpaESLIFString_t  *descp;
  jstring              stringp;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_grammar_currentb(marpaESLIFGrammarp, NULL, &descp)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_grammar_currentb failure");
  }

  stringp = marpaESLIFJava_marpaESLIFStringToJavap(envp, descp);
  goto done;

 err:
  stringp = NULL;

 done:
  return stringp;
}

/*****************************************************************************/
JNIEXPORT jstring JNICALL Java_org_parser_marpa_ESLIFGrammar_jniDescriptionByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniDescriptionByLevel";
  int                  leveli = (jint) level;
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  marpaESLIFString_t  *descp;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_grammar_by_levelb(marpaESLIFGrammarp, leveli, NULL, NULL, &descp)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFGrammar_grammar_by_levelb failure for level %d", leveli);
  }

  return marpaESLIFJava_marpaESLIFStringToJavap(envp, descp);

 err:
  return NULL;
}

/*****************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniCurrentRuleIds(JNIEnv *envp, jobject eslifGrammarp)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniCurrentRuleIds";
  jintArray            intArray = NULL;
  jint                *intp = NULL;
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                 *ruleip;
  size_t               rulel;
  size_t               i;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_rulearray_currentb(marpaESLIFGrammarp, &ruleip, &rulel)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_rulearray_currentb failure");
  }
  if (rulel <= 0) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_rulearray_currentb returned no rule");
  }
  intp = (jint *) malloc(sizeof(jint) * rulel);
  if (intp == NULL) {
    RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
  }
  for (i = 0; i < rulel; i++) {
    intp[i] = (jint) ruleip[i];
  }

  intArray = (*envp)->NewIntArray(envp, (jsize) rulel);
  if (intArray == NULL) {
    RAISEEXCEPTION(envp, "NewIntArray failure");
  }

  /* If it fails the exception will be seen by the Java layer */
  (*envp)->SetIntArrayRegion(envp, intArray, 0, (jsize) rulel, intp);
  goto done;

 err:
  if (intArray != NULL) {
    (*envp)->DeleteLocalRef(envp, intArray);
    intArray = NULL;
  }

 done:
  if (intp != NULL) {
    free(intp);
  }
  return intArray;
}

/*****************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniCurrentSymbolIds(JNIEnv *envp, jobject eslifGrammarp)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniCurrentSymbolIds";
  jintArray            intArray = NULL;
  jint                *intp = NULL;
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                 *symbolip;
  size_t               symboll;
  size_t               i;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_symbolarray_currentb(marpaESLIFGrammarp, &symbolip, &symboll)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_symbolarray_currentb failure");
  }
  if (symboll <= 0) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_symbolarray_currentb returned no symbol");
  }
  intp = (jint *) malloc(sizeof(jint) * symboll);
  if (intp == NULL) {
    RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
  }
  for (i = 0; i < symboll; i++) {
    intp[i] = (jint) symbolip[i];
  }

  intArray = (*envp)->NewIntArray(envp, (jsize) symboll);
  if (intArray == NULL) {
    RAISEEXCEPTION(envp, "NewIntArray failure");
  }

  /* If it fails the exception will be seen by the Java layer */
  (*envp)->SetIntArrayRegion(envp, intArray, 0, (jsize) symboll, intp);
  goto done;

 err:
  if (intArray != NULL) {
    (*envp)->DeleteLocalRef(envp, intArray);
    intArray = NULL;
  }

 done:
  if (intp != NULL) {
    free(intp);
  }
  return intArray;
}

/*****************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleIdsByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniRuleIdsByLevel";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                  leveli = (int) level;
  jintArray            intArray;
  jint                *intp = NULL;
  int                 *ruleip;
  size_t               rulel;
  size_t               i;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_rulearray_by_levelb(marpaESLIFGrammarp, &ruleip, &rulel, leveli, NULL)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFGrammar_rulearray_by_levelb failure for level %d", leveli);
  }
  if (rulel <= 0) {
    RAISEEXCEPTIONF(envp, "marpaESLIFGrammar_rulearray_currentb returned no rule for level %d", leveli);
  }
  intp = (jint *) malloc(sizeof(jint) * rulel);
  if (intp == NULL) {
    RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
  }
  for (i = 0; i < rulel; i++) {
    intp[i] = (jint) ruleip[i];
  }

  intArray = (*envp)->NewIntArray(envp, (jsize) rulel);
  if (intArray == NULL) {
    RAISEEXCEPTION(envp, "NewIntArray failure");
  }

  /* If it fails the exception will be seen by the Java layer */
  (*envp)->SetIntArrayRegion(envp, intArray, 0, (jsize) rulel, intp);
  goto done;

 err:
  if (intArray != NULL) {
    (*envp)->DeleteLocalRef(envp, intArray);
    intArray = NULL;
  }

 done:
  if (intp != NULL) {
    free(intp);
  }
  return intArray;
}

/*****************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniSymbolIdsByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniSymbolIdsByLevel";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                  leveli = (int) level;
  jintArray            intArray;
  jint                *intp = NULL;
  int                 *symbolip;
  size_t               symboll;
  size_t               i;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_symbolarray_by_levelb(marpaESLIFGrammarp, &symbolip, &symboll, leveli, NULL)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFGrammar_symbolarray_by_levelb failure for level %d", leveli);
  }
  if (symboll <= 0) {
    RAISEEXCEPTIONF(envp, "marpaESLIFGrammar_symbolarray_currentb returned no symbol for level %d", leveli);
  }
  intp = (jint *) malloc(sizeof(jint) * symboll);
  if (intp == NULL) {
    RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
  }
  for (i = 0; i < symboll; i++) {
    intp[i] = (jint) symbolip[i];
  }

  intArray = (*envp)->NewIntArray(envp, (jsize) symboll);
  if (intArray == NULL) {
    RAISEEXCEPTION(envp, "NewIntArray failure");
  }

  /* If it fails the exception will be seen by the Java layer */
  (*envp)->SetIntArrayRegion(envp, intArray, 0, (jsize) symboll, intp);
  goto done;

 err:
  if (intArray != NULL) {
    (*envp)->DeleteLocalRef(envp, intArray);
    intArray = NULL;
  }

 done:
  if (intp != NULL) {
    free(intp);
  }
  return intArray;
}

/*****************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleDisplay(JNIEnv *envp, jobject eslifGrammarp, jint rule)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniRuleDisplay";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                  rulei = (int) rule;
  char                *ruledisplays;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_ruledisplayform_currentb(marpaESLIFGrammarp, rulei, &ruledisplays)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFGrammar_ruledisplayform_currentb failure for rule %d", rulei);
  }

  return marpaESLIFJava_marpaESLIFASCIIToJavap(envp, ruledisplays);

 err:
  return NULL;
}

/*****************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniSymbolDisplay(JNIEnv *envp, jobject eslifGrammarp, jint symbol)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniSymbolDisplay";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                  symboli = (int) symbol;
  char                *symboldisplays;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_symboldisplayform_currentb(marpaESLIFGrammarp, symboli, &symboldisplays)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFGrammar_symboldisplayform_currentb failure for symbol %d", symboli);
  }

  return marpaESLIFJava_marpaESLIFASCIIToJavap(envp, symboldisplays);

 err:
  return NULL;
}

/*****************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleShow(JNIEnv *envp, jobject eslifGrammarp, jint rule)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniRuleShow";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                  rulei = (int) rule;
  char                *ruleshows;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_ruleshowform_currentb(marpaESLIFGrammarp, rulei, &ruleshows)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFGrammar_ruleshowform_currentb failure for rule %d", rulei);
  }

  return marpaESLIFJava_marpaESLIFASCIIToJavap(envp, ruleshows);

 err:
  return NULL;
}

/*****************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleDisplayByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level, jint rule)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniRuleDisplayByLevel";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                  leveli = (int) level;
  int                  rulei = (int) rule;
  char                *ruledisplays;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_ruledisplayform_by_levelb(marpaESLIFGrammarp, rulei, &ruledisplays, leveli, NULL)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFGrammar_ruledisplayform_currentb failure for rule %d and level %d", rulei, leveli);
  }

  return marpaESLIFJava_marpaESLIFASCIIToJavap(envp, ruledisplays);

 err:
  return NULL;
}

/*****************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniSymbolDisplayByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level, jint symbol)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniSymbolDisplayByLevel";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                  leveli = (int) level;
  int                  symboli = (int) symbol;
  char                *symboldisplays;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_symboldisplayform_by_levelb(marpaESLIFGrammarp, symboli, &symboldisplays, leveli, NULL)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFGrammar_symboldisplayform_currentb failure for symbol %d and level %d", symboli, leveli);
  }

  return marpaESLIFJava_marpaESLIFASCIIToJavap(envp, symboldisplays);

 err:
  return NULL;
}

/*****************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleShowByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level, jint rule)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniRuleShowByLevel";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                  leveli = (int) level;
  int                  rulei = (int) rule;
  char                *ruleshows;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_ruleshowform_by_levelb(marpaESLIFGrammarp, rulei, &ruleshows, leveli, NULL)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFGrammar_ruleshowform_by_levelb failure for rule %d and level %d", rulei, leveli);
  }

  return marpaESLIFJava_marpaESLIFASCIIToJavap(envp, ruleshows);

 err:
  return NULL;
}

/*****************************************************************************/
JNIEXPORT jstring JNICALL Java_org_parser_marpa_ESLIFGrammar_jniShow(JNIEnv *envp, jobject eslifGrammarp)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniShow";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  char                *shows;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_grammarshowform_currentb(marpaESLIFGrammarp, &shows)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_grammarshowform_currentb failure");
  }

  return marpaESLIFJava_marpaESLIFASCIIToJavap(envp, shows);

 err:
  return NULL;
}

/*****************************************************************************/
JNIEXPORT jstring JNICALL Java_org_parser_marpa_ESLIFGrammar_jniShowByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniShowByLevel";
  marpaESLIFGrammar_t *marpaESLIFGrammarp;
  int                  leveli = (int) level;
  char                *shows;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammarp, &shows, leveli, NULL)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFGrammar_grammarshowform_by_levelb for level %d", leveli);
  }

  return marpaESLIFJava_marpaESLIFASCIIToJavap(envp, shows);

 err:
  return NULL;
}

/*****************************************************************************/
JNIEXPORT jboolean JNICALL Java_org_parser_marpa_ESLIFGrammar_jniParse(JNIEnv *envp, jobject eslifGrammarp, jobject eslifRecognizerInterfacep, jobject eslifValueInterfacep)
/*****************************************************************************/
{
  static const char                 *funcs                  = "Java_org_parser_marpa_ESLIFGrammar_jniParse";
  marpaESLIFValueResult_t           *marpaESLIFValueResultp = NULL;
  marpaESLIFGrammar_t               *marpaESLIFGrammarp;
  marpaESLIFRecognizerOption_t       marpaESLIFRecognizerOption;
  marpaESLIFValueOption_t            marpaESLIFValueOption;
  marpaESLIFJavaRecognizerContext_t  marpaESLIFJavaRecognizerContext;
  marpaESLIFJavaValueContext_t       marpaESLIFJavaValueContext;
  jobject                            objectp;
  jboolean                           rcb;

  if (! marpaESLIFJava_recognizerContextInitb(envp, eslifRecognizerInterfacep, &marpaESLIFJavaRecognizerContext, 0 /* haveSymbolStackb */)) {
    goto err;
  }

  marpaESLIFRecognizerOption.userDatavp                = &marpaESLIFJavaRecognizerContext;
  marpaESLIFRecognizerOption.readerCallbackp           = readerCallbackb;
  marpaESLIFRecognizerOption.disableThresholdb         = ((*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithDisableThreshold_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFRecognizerOption.exhaustedb                = ((*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithExhaustion_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFRecognizerOption.newlineb                  = ((*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithNewline_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFRecognizerOption.trackb                    = ((*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithTrack_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFRecognizerOption.bufsizl                   = 0; /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci           = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci               = 50; /* Recommended value */
  marpaESLIFRecognizerOption.ifActionResolverp         = marpaESLIFRecognizerIfActionResolver;
  marpaESLIFRecognizerOption.eventActionResolverp      = marpaESLIFRecognizerEventActionResolver;
  marpaESLIFRecognizerOption.regexActionResolverp      = marpaESLIFRecognizerRegexActionResolver;
  marpaESLIFRecognizerOption.generatorActionResolverp  = marpaESLIFRecognizerGeneratorActionResolver;
  marpaESLIFRecognizerOption.importerp                 = marpaESLIFJava_recognizerImportb;

  if (! marpaESLIFJava_valueContextInitb(envp, eslifValueInterfacep, eslifGrammarp, &marpaESLIFJavaValueContext)) {
    goto err;
  }
  
  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  marpaESLIFValueOption.userDatavp                     = &marpaESLIFJavaValueContext;
  marpaESLIFValueOption.ruleActionResolverp            = marpaESLIFValueRuleActionResolver;
  marpaESLIFValueOption.symbolActionResolverp          = marpaESLIFValueSymbolActionResolver;
  marpaESLIFValueOption.importerp                      = marpaESLIFJava_valueImportb;
  marpaESLIFValueOption.highRankOnlyb                  = ((*envp)->CallBooleanMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_isWithHighRankOnly_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFValueOption.orderByRankb                   = ((*envp)->CallBooleanMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_isWithOrderByRank_METHODP)  == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFValueOption.ambiguousb                     = ((*envp)->CallBooleanMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_isWithAmbiguous_METHODP)    == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFValueOption.nullb                          = ((*envp)->CallBooleanMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_isWithNull_METHODP)         == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFValueOption.maxParsesi                     = (*envp)->CallIntMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_maxParses_METHODP);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, NULL)) {
    goto err;
  }
  if (GENERICSTACK_USED(marpaESLIFJavaValueContext.objectStackp) != 1) {
    RAISEEXCEPTIONF(envp, "Internal value stack is %d instead of 1", GENERICSTACK_USED(marpaESLIFJavaValueContext.objectStackp));
  }
  objectp = (jobject) GENERICSTACK_POP_PTR(marpaESLIFJavaValueContext.objectStackp);

  (*envp)->CallVoidMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_setResult_METHODP, objectp);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  rcb = JNI_TRUE;
  goto done;

 err:
  rcb = JNI_FALSE;

 done:
  marpaESLIFJava_valueContextFreev(envp, &marpaESLIFJavaValueContext, 1 /* onStackb */);
  marpaESLIFJava_recognizerContextFreev(envp, &marpaESLIFJavaRecognizerContext, 1 /* onStackb */);
  return rcb;
}

/*****************************************************************************/
JNIEXPORT jobject JNICALL Java_org_parser_marpa_ESLIFGrammar_jniProperties(JNIEnv *envp, jobject eslifGrammarp)
/*****************************************************************************/
{
  static const char           *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniProperties";
  marpaESLIFGrammarProperty_t  grammarProperty;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  jobject                      propertiesp = NULL;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_grammarproperty_currentb(marpaESLIFGrammarp, &grammarProperty)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_grammarproperty_currentb failure");
  }

  propertiesp = marpaESLIFJava_grammarPropertiesp(envp, &grammarProperty);
  goto done;

 err:
  propertiesp = NULL;

 done:
  return propertiesp;
}

/*****************************************************************************/
JNIEXPORT jobject JNICALL Java_org_parser_marpa_ESLIFGrammar_jniPropertiesByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level)
/*****************************************************************************/
{
  static const char           *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniProperties";
  marpaESLIFGrammarProperty_t  grammarProperty;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  jobject                      propertiesp = NULL;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_grammarproperty_by_levelb(marpaESLIFGrammarp, &grammarProperty, (int) level, NULL /* descp */)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_grammarproperty_by_levelb failure");
  }

  propertiesp = marpaESLIFJava_grammarPropertiesp(envp, &grammarProperty);
  goto done;

 err:
  propertiesp = NULL;

 done:
  return propertiesp;
}

/*****************************************************************************/
JNIEXPORT jobject JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleProperties(JNIEnv *envp, jobject eslifGrammarp, jint rule)
/*****************************************************************************/
{
  static const char        *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniRuleProperties";
  marpaESLIFRuleProperty_t  ruleProperty;
  marpaESLIFGrammar_t      *marpaESLIFGrammarp;
  jobject                   propertiesp = NULL;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_ruleproperty_currentb(marpaESLIFGrammarp, (int) rule, &ruleProperty)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_ruleproperty_currentb failure");
  }

  propertiesp = marpaESLIFJava_rulePropertiesp(envp, &ruleProperty);
  goto done;

 err:
  propertiesp = NULL;

 done:
  return propertiesp;
}

/*****************************************************************************/
JNIEXPORT jobject JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRulePropertiesByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level, jint rule)
/*****************************************************************************/
{
  static const char        *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniRulePropertiesByLevel";
  marpaESLIFRuleProperty_t  ruleProperty;
  marpaESLIFGrammar_t      *marpaESLIFGrammarp;
  jobject                   propertiesp = NULL;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_ruleproperty_by_levelb(marpaESLIFGrammarp, (int) rule, &ruleProperty, (int) level, NULL /* descp */)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_ruleproperty_by_levelb failure");
  }

  propertiesp = marpaESLIFJava_rulePropertiesp(envp, &ruleProperty);
  goto done;

 err:
  propertiesp = NULL;

 done:
  return propertiesp;
}

/*****************************************************************************/
JNIEXPORT jobject JNICALL Java_org_parser_marpa_ESLIFGrammar_jniSymbolProperties(JNIEnv *envp, jobject eslifGrammarp, jint symbol)
/*****************************************************************************/
{
  static const char          *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniSymbolProperties";
  marpaESLIFSymbolProperty_t  symbolProperty;
  marpaESLIFGrammar_t        *marpaESLIFGrammarp;
  jobject                     propertiesp = NULL;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_symbolproperty_currentb(marpaESLIFGrammarp, (int) symbol, &symbolProperty)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_symbolproperty_currentb failure");
  }

  propertiesp = marpaESLIFJava_symbolPropertiesp(envp, &symbolProperty);
  goto done;

 err:
  propertiesp = NULL;

 done:
  return propertiesp;
}

/*****************************************************************************/
JNIEXPORT jobject JNICALL Java_org_parser_marpa_ESLIFGrammar_jniSymbolPropertiesByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level, jint symbol)
/*****************************************************************************/
{
  static const char          *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniSymbolPropertiesByLevel";
  marpaESLIFSymbolProperty_t  symbolProperty;
  marpaESLIFGrammar_t        *marpaESLIFGrammarp;
  jobject                     propertiesp = NULL;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    goto err;
  }

  if (! marpaESLIFGrammar_symbolproperty_by_levelb(marpaESLIFGrammarp, (int) symbol, &symbolProperty, (int) level, NULL /* descp */)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_symbolproperty_by_levelb failure");
  }

  propertiesp = marpaESLIFJava_symbolPropertiesp(envp, &symbolProperty);
  goto done;

 err:
  propertiesp = NULL;

 done:
  return propertiesp;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFGrammar_jniFree(JNIEnv *envp, jobject eslifGrammarp)
/*****************************************************************************/
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp;

  if (ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    marpaESLIFGrammar_freev(marpaESLIFGrammarp); /* This is NULL protected */
  }
}

/*****************************************************************************/
static void genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  marpaESLIFJavaGenericLoggerContext_t *marpaESLIFJavaGenericLoggerContextp = (marpaESLIFJavaGenericLoggerContext_t *) userDatavp;
  JNIEnv                               *envp;
  jobject                               loggerInterfacep;

  if (marpaESLIFJavaGenericLoggerContextp == NULL) {
    /* Impossible IMHO */
    return;
  }

  /* Logging callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    return;
  }

  loggerInterfacep = (*envp)->CallStaticObjectMethod(envp, MARPAESLIF_ESLIFEVENTTYPE_CLASSP, MARPAESLIF_ESLIF_CLASS_getLoggerInterfaceByIndicep_METHODP, marpaESLIFJavaGenericLoggerContextp->indice);
  if (loggerInterfacep == NULL) {
    return;
  }

  callLoggerInterfacev(loggerInterfacep, logLeveli, msgs);
}

/*****************************************************************************/
static void callLoggerInterfacev(jobject loggerInterfacep, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  JNIEnv     *envp;
  jmethodID   methodp;
  jstring     stringp;

  /* Logging callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    return;
  }

  switch (logLeveli) {
  case GENERICLOGGER_LOGLEVEL_TRACE:
    methodp = MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_trace_METHODP;
    break;
  case GENERICLOGGER_LOGLEVEL_DEBUG:
    methodp = MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_debug_METHODP;
    break;
  case GENERICLOGGER_LOGLEVEL_INFO:
    methodp = MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_info_METHODP;
    break;
  case GENERICLOGGER_LOGLEVEL_NOTICE:
    methodp = MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_notice_METHODP;
    break;
  case GENERICLOGGER_LOGLEVEL_WARNING:
    methodp = MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_warning_METHODP;
    break;
  case GENERICLOGGER_LOGLEVEL_ERROR:
    methodp = MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_error_METHODP;
    break;
  case GENERICLOGGER_LOGLEVEL_CRITICAL:
    methodp = MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_critical_METHODP;
    break;
  case GENERICLOGGER_LOGLEVEL_ALERT:
    methodp = MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_alert_METHODP;
    break;
  case GENERICLOGGER_LOGLEVEL_EMERGENCY:
    methodp = MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_emergency_METHODP;
    break;
  default:
    methodp = NULL;
    break;
  }

  if (methodp != NULL) {
    stringp = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, (char *) msgs);
    if (stringp != NULL) {
      (*envp)->CallVoidMethod(envp, loggerInterfacep, methodp, stringp);
      (*envp)->DeleteLocalRef(envp, stringp);
    }
  }
}

/*****************************************************************************/
static void generateStringWithLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  marpaESLIFJavaStringGeneratorContext_t *contextp = (marpaESLIFJavaStringGeneratorContext_t *) userDatavp;
  char                                    *tmps;

  if (contextp->s == NULL) {
    /* First time */
    contextp->l = strlen(msgs) + 1;
    contextp->s = strdup(msgs);
    if (contextp->s == NULL) {
      contextp->l = 0;
      /* contextp->okb = 0; */ /* We initialized it with a false value -; */
    } else {
      contextp->okb = 1;
    }
  } else if (contextp->okb) {
    /* Only if previous round was ok */
    contextp->l = strlen(contextp->s) + strlen(msgs) + 1;
    tmps = (char *) realloc(contextp->s, contextp->l);
    if (tmps != NULL) {
      strcat(tmps, msgs);
      contextp->s = tmps;
    } else {
      contextp->okb = 0;
      contextp->l = 0;
      free(contextp->s);
      contextp->s = NULL;
    }
  }
}

/*****************************************************************************/
static short ESLIF_contextb(JNIEnv *envp, jobject eslifp, genericLogger_t **genericLoggerpp, marpaESLIF_t **marpaESLIFpp)
/*****************************************************************************/
{
  static const char                    *funcs                               = "ESLIF_contextb";
  marpaESLIFJavaGenericLoggerContext_t *marpaESLIFJavaGenericLoggerContextp = NULL;
  marpaESLIF_t                         *marpaESLIFp                         = NULL;
  genericLogger_t                      *genericLoggerp                      = NULL;
  jobject                               BYTEBUFFER(marpaESLIF);
  jobject                               BYTEBUFFER(genericLogger);

  /* If caller is giving marpaESLIFpp we expect bytebuffer and more to not be NULL */
  BYTEBUFFER(marpaESLIF) = (*envp)->CallObjectMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_getMarpaESLIFp_METHODP);
  if (BYTEBUFFER(marpaESLIF) != NULL) {
    marpaESLIFp = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(marpaESLIF));
  }

  BYTEBUFFER(genericLogger) = (*envp)->CallObjectMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_getGenericLoggerp_METHODP);
  if (BYTEBUFFER(genericLogger) != NULL) {
    genericLoggerp = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(genericLogger));
  }

  if (marpaESLIFpp != NULL) {
    *marpaESLIFpp = marpaESLIFp;
  }
  if (genericLoggerpp != NULL) {
    *genericLoggerpp = genericLoggerp;
  }

  return 1;

 err:
  return 0;
}

/*****************************************************************************/
static short ESLIFGrammar_contextb(JNIEnv *envp, jobject eslifGrammarp, marpaESLIF_t **marpaESLIFpp, marpaESLIFGrammar_t **marpaESLIFGrammarpp)
/*****************************************************************************/
{
  static const char   *funcs              = "ESLIFGrammar_contextb";
  marpaESLIFGrammar_t *marpaESLIFGrammarp = NULL;;
  jobject              BYTEBUFFER(marpaESLIFGrammar);
  jobject              eslifp;
    
  BYTEBUFFER(marpaESLIFGrammar) = (*envp)->CallObjectMethod(envp, eslifGrammarp, MARPAESLIF_ESLIFGRAMMAR_CLASS_getMarpaESLIFGrammarp_METHODP);
  if (BYTEBUFFER(marpaESLIFGrammar) != NULL) {
    marpaESLIFGrammarp = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(marpaESLIFGrammar));
  }

  eslifp = (*envp)->CallObjectMethod(envp, eslifGrammarp, MARPAESLIF_ESLIFGRAMMAR_CLASS_getEslif_METHODP);
  if (eslifp == NULL) {
    /* Not allowed */
    RAISEEXCEPTION(envp, "eslifp is NULL");
  }

  if (! ESLIF_contextb(envp, eslifp, NULL, marpaESLIFpp)) {
    goto err;
  }

  if (marpaESLIFGrammarpp != NULL) {
    *marpaESLIFGrammarpp = marpaESLIFGrammarp;
  }

  return 1;

 err:
  return 0;
}

/*****************************************************************************/
static short ESLIFSymbol_contextb(JNIEnv *envp, jobject eslifSymbolp, marpaESLIF_t **marpaESLIFpp, marpaESLIFSymbol_t **marpaESLIFSymbolpp, marpaESLIFJavaSymbolContext_t **marpaESLIFJavaSymbolContextpp)
/*****************************************************************************/
{
  static const char             *funcs = "ESLIFSymbol_contextb";
  jobject                        BYTEBUFFER(marpaESLIFSymbol);
  jobject                        BYTEBUFFER(marpaESLIFJavaSymbolContext);
  jobject                        eslifp;
  marpaESLIFSymbol_t            *marpaESLIFSymbolp = NULL;
  marpaESLIFJavaSymbolContext_t *marpaESLIFJavaSymbolContextp = NULL;

  /* If caller is giving marpaESLIFSymbolpp we expect bytebuffer and more to not be NULL */
  BYTEBUFFER(marpaESLIFSymbol) = (*envp)->CallObjectMethod(envp, eslifSymbolp, MARPAESLIF_ESLIFSYMBOL_CLASS_getMarpaESLIFSymbolp_METHODP);
  if (BYTEBUFFER(marpaESLIFSymbol) != NULL) {
    marpaESLIFSymbolp = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(marpaESLIFSymbol));
  }

  BYTEBUFFER(marpaESLIFJavaSymbolContext) = (*envp)->CallObjectMethod(envp, eslifSymbolp, MARPAESLIF_ESLIFSYMBOL_CLASS_getMarpaESLIFSymbolContextp_METHODP);
  if (BYTEBUFFER(marpaESLIFJavaSymbolContext) != NULL) {
    marpaESLIFJavaSymbolContextp = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(marpaESLIFJavaSymbolContext));
  }

  eslifp = (*envp)->CallObjectMethod(envp, eslifSymbolp, MARPAESLIF_ESLIFSYMBOL_CLASS_getEslif_METHODP);
  if (eslifp == NULL) {
    /* Not allowed */
    RAISEEXCEPTION(envp, "eslifp is NULL");
  }
  if (! ESLIF_contextb(envp, eslifp, NULL, marpaESLIFpp)) {
    goto err;
  }

  if (marpaESLIFSymbolpp != NULL) {
    *marpaESLIFSymbolpp = marpaESLIFSymbolp;
  }
  if (marpaESLIFJavaSymbolContextpp != NULL) {
    *marpaESLIFJavaSymbolContextpp = marpaESLIFJavaSymbolContextp;
  }

  return 1;

 err:
  return 0;
}

/*****************************************************************************/
static short ESLIFRecognizer_contextb(JNIEnv *envp, jobject eslifRecognizerp, marpaESLIFRecognizer_t **marpaESLIFRecognizerpp, marpaESLIFJavaRecognizerContext_t **marpaESLIFJavaRecognizerContextpp)
/*****************************************************************************/
{
  static const char                 *funcs = "ESLIFRecognizer_contextb";
  jobject                            BYTEBUFFER(marpaESLIFRecognizer);
  jobject                            BYTEBUFFER(marpaESLIFJavaRecognizerContext);
  marpaESLIFRecognizer_t            *marpaESLIFRecognizerp = NULL;
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp = NULL;
    
  BYTEBUFFER(marpaESLIFRecognizer) = (*envp)->CallObjectMethod(envp, eslifRecognizerp, MARPAESLIF_ESLIFRECOGNIZER_CLASS_getMarpaESLIFRecognizerp_METHODP);
  if (BYTEBUFFER(marpaESLIFRecognizer) != NULL) {
    marpaESLIFRecognizerp = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(marpaESLIFRecognizer));
  }

  BYTEBUFFER(marpaESLIFJavaRecognizerContext) = (*envp)->CallObjectMethod(envp, eslifRecognizerp, MARPAESLIF_ESLIFRECOGNIZER_CLASS_getMarpaESLIFRecognizerContextp_METHODP);
  if (BYTEBUFFER(marpaESLIFJavaRecognizerContext) != NULL) {
    marpaESLIFJavaRecognizerContextp = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(marpaESLIFJavaRecognizerContext));
    marpaESLIFJavaRecognizerContextp->eslifRecognizerInterfacep = (*envp)->CallObjectMethod(envp, eslifRecognizerp, MARPAESLIF_ESLIFRECOGNIZER_CLASS_getEslifRecognizerInterface_METHODP);
  }

  if (marpaESLIFRecognizerpp != NULL) {
    *marpaESLIFRecognizerpp = marpaESLIFRecognizerp;
  }
  if (marpaESLIFJavaRecognizerContextpp != NULL) {
    *marpaESLIFJavaRecognizerContextpp = marpaESLIFJavaRecognizerContextp;
  }
  
  return 1;

 err:
  return 0;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniNew(JNIEnv *envp, jobject eslifRecognizerp, jobject eslifGrammarp)
/*****************************************************************************/
{
  static const char                     *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniNew";
  marpaESLIFRecognizer_t                *marpaESLIFRecognizerp  = NULL;
  marpaESLIFGrammar_t                   *marpaESLIFGrammarp;
  marpaESLIFRecognizerOption_t           marpaESLIFRecognizerOption;
  marpaESLIFJavaRecognizerContext_t     *marpaESLIFJavaRecognizerContextp;
  jobject                                eslifRecognizerInterfacep;
  jobject                                BYTEBUFFER(marpaESLIFRecognizer);
  jobject                                BYTEBUFFER(marpaESLIFJavaRecognizerContext);

  /* Always update genericLogger context */
  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, NULL /* marpaESLIFpp */, &marpaESLIFGrammarp)) {
    /* An exception was raised */
    goto err;
  }

  /* Get recognizer interface - class is protected to not accept the null argument */
  eslifRecognizerInterfacep = (*envp)->CallObjectMethod(envp, eslifRecognizerp, MARPAESLIF_ESLIFRECOGNIZER_CLASS_getEslifRecognizerInterface_METHODP);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  /* Recognizer context is a structure containing the current recognizer interface object instance - updated at every recognizer call */
  marpaESLIFJavaRecognizerContextp = (marpaESLIFJavaRecognizerContext_t *) malloc(sizeof(marpaESLIFJavaRecognizerContext_t));
  if (marpaESLIFJavaRecognizerContextp == NULL) {
    RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
  }

  if (! marpaESLIFJava_recognizerContextInitb(envp, eslifRecognizerInterfacep, marpaESLIFJavaRecognizerContextp, 1 /* haveSymbolStackb */)) {
    goto err;
  }

  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIFJavaRecognizerContext, marpaESLIFJavaRecognizerContextp);  
  (*envp)->CallVoidMethod(envp, eslifRecognizerp, MARPAESLIF_ESLIFRECOGNIZER_CLASS_setMarpaESLIFRecognizerContextp_METHODP, BYTEBUFFER(marpaESLIFJavaRecognizerContext));
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  /* Create C object */
  marpaESLIFRecognizerOption.userDatavp                = marpaESLIFJavaRecognizerContextp;
  marpaESLIFRecognizerOption.readerCallbackp           = readerCallbackb;
  marpaESLIFRecognizerOption.disableThresholdb         = ((*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithDisableThreshold_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFRecognizerOption.exhaustedb                = ((*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithExhaustion_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFRecognizerOption.newlineb                  = ((*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithNewline_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFRecognizerOption.trackb                    = ((*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithTrack_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFRecognizerOption.bufsizl                   = 0; /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci           = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci               = 50; /* Recommended value */
  marpaESLIFRecognizerOption.ifActionResolverp         = marpaESLIFRecognizerIfActionResolver;
  marpaESLIFRecognizerOption.eventActionResolverp      = marpaESLIFRecognizerEventActionResolver;
  marpaESLIFRecognizerOption.regexActionResolverp      = marpaESLIFRecognizerRegexActionResolver;
  marpaESLIFRecognizerOption.generatorActionResolverp  = marpaESLIFRecognizerGeneratorActionResolver;
  marpaESLIFRecognizerOption.importerp                 = marpaESLIFJava_recognizerImportb;

  marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
  if (marpaESLIFRecognizerp == NULL) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_newp failure");
  }

  /* Store the objects */
  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIFRecognizer, marpaESLIFRecognizerp);  
  (*envp)->CallVoidMethod(envp, eslifRecognizerp, MARPAESLIF_ESLIFRECOGNIZER_CLASS_setMarpaESLIFRecognizerp_METHODP, BYTEBUFFER(marpaESLIFRecognizer));
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  goto done;
  
 err:
  Java_org_parser_marpa_ESLIFRecognizer_jniFree(envp, eslifRecognizerp);

 done:
  return;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniShare(JNIEnv *envp, jobject eslifRecognizerp, jobject eslifRecognizerSharedp)
/*****************************************************************************/
{
  static const char *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniShare";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerSharedp;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! (*envp)->IsSameObject(envp, eslifRecognizerSharedp, NULL)) {
    if (! ESLIFRecognizer_contextb(envp, eslifRecognizerSharedp, &marpaESLIFRecognizerSharedp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
      goto err;
    }
  } else {
    marpaESLIFRecognizerSharedp = NULL;
  }

  if (! marpaESLIFRecognizer_shareb(marpaESLIFRecognizerp, marpaESLIFRecognizerSharedp)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_shareb failure");
  }

 err: /* err and done share the same code */
  return;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniUnshare(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniUnshare";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_shareb(marpaESLIFRecognizerp, NULL)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_shareb failure");
  }

 err: /* err and done share the same code */
  return;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniPeek(JNIEnv *envp, jobject eslifRecognizerp, jobject eslifRecognizerPeekedp)
/*****************************************************************************/
{
  static const char *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniPeek";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerPeekedp;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! (*envp)->IsSameObject(envp, eslifRecognizerPeekedp, NULL)) {
    if (! ESLIFRecognizer_contextb(envp, eslifRecognizerPeekedp, &marpaESLIFRecognizerPeekedp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
      goto err;
    }
  } else {
    marpaESLIFRecognizerPeekedp = NULL;
  }

  if (! marpaESLIFRecognizer_peekb(marpaESLIFRecognizerp, marpaESLIFRecognizerPeekedp)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_peekb failure");
  }

 err: /* err and done share the same code */
  return;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniUnpeek(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniUnpeek";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_peekb(marpaESLIFRecognizerp, NULL)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_peekb failure");
  }

 err: /* err and done share the same code */
  return;
}

/*****************************************************************************/
JNIEXPORT jboolean JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniScan(JNIEnv *envp, jobject eslifRecognizerp, jboolean initialEvents)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniScan";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  short                   initialEventsb = (initialEvents == JNI_TRUE);
  short                   rcb;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  rcb = marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, initialEventsb, NULL, NULL);

  return rcb ? JNI_TRUE : JNI_FALSE;

 err:
  return JNI_FALSE;
}

/*****************************************************************************/
JNIEXPORT jboolean JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniResume(JNIEnv *envp, jobject eslifRecognizerp, jint deltaLength)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniResume";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  /* Let's be conservative */
  if (deltaLength < 0) {
    RAISEEXCEPTION(envp, "Resume delta length cannot be negative");
  }
  
  if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, (size_t) deltaLength, NULL, NULL)) {
    goto err;
  }

  return JNI_TRUE;

 err:
  return JNI_FALSE;
}

/*****************************************************************************/
JNIEXPORT jboolean JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniAlternative(JNIEnv *envp, jobject eslifRecognizerp, jstring namep, jobject objectp, jint grammarLengthi)
/*****************************************************************************/
{
  static const char                 *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniAlternative";
  marpaESLIFRecognizer_t            *marpaESLIFRecognizerp;
  marpaESLIFAlternative_t            marpaESLIFAlternative;
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp;
  const char                        *names = NULL;
  jboolean                           isCopy;
  jboolean                           rcb;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, &marpaESLIFJavaRecognizerContextp)) {
    goto err;
  }

  /* Well, java have no notion of native unsigned value */
  if (grammarLengthi <= 0) {
    RAISEEXCEPTION(envp, "grammarLength cannot be <= 0");
  }
  
  if (namep != NULL) {
    names = (*envp)->GetStringUTFChars(envp, namep, &isCopy);
    if (names == NULL) {
      RAISEEXCEPTION(envp, "GetStringUTFChars failure");
    }
  }

  marpaESLIFAlternative.names          = (char *) names;
  marpaESLIFAlternative.grammarLengthl = (size_t) grammarLengthi;
  if (! marpaESLIFJava_stack_setb(envp, NULL /* marpaESLIFValuep */, -1 /* resulti */, objectp, &(marpaESLIFAlternative.value))) {
    goto err;
  }

  if (!  marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizerp, &marpaESLIFAlternative)) {
    goto err;
  }

  rcb = JNI_TRUE;
  goto done;

 err:
  rcb = JNI_FALSE;

 done:
  if (envp != NULL) {
    if ((namep != NULL) && (names != NULL)) {
      (*envp)->ReleaseStringUTFChars(envp, namep, names);
    }
  }

  return rcb;
}

/*****************************************************************************/
JNIEXPORT jboolean JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniAlternativeComplete(JNIEnv *envp, jobject eslifRecognizerp, jint lengthi)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniAlternativeComplete";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  /* Well, java have no notion of native unsigned value */
  if (lengthi < 0) {
    RAISEEXCEPTION(envp, "length cannot be < 0");
  }
  
  if (!  marpaESLIFRecognizer_alternative_completeb(marpaESLIFRecognizerp, (size_t) lengthi)) {
    goto err;
  }

  return JNI_TRUE;

 err:
  return JNI_FALSE;
}

/*****************************************************************************/
JNIEXPORT jboolean JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniAlternativeRead(JNIEnv *envp, jobject eslifRecognizerp, jstring namep, jobject objectp, jint lengthi, jint grammarLengthi)
/*****************************************************************************/
{
  static const char                 *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniAlternativeRead";
  marpaESLIFRecognizer_t            *marpaESLIFRecognizerp;
  marpaESLIFAlternative_t            marpaESLIFAlternative;
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp;
  const char                        *names = NULL;
  jboolean                           isCopy;
  jboolean                           rcb;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, &marpaESLIFJavaRecognizerContextp)) {
    goto err;
  }

  /* Well, java have no notion of native unsigned value */
  if (grammarLengthi <= 0) {
    RAISEEXCEPTION(envp, "grammarLength cannot be <= 0");
  }
  
  /* Well, java have no notion of native unsigned value */
  if (lengthi < 0) {
    RAISEEXCEPTION(envp, "length cannot be < 0");
  }
  
  if (namep != NULL) {
    names = (*envp)->GetStringUTFChars(envp, namep, &isCopy);
    if (names == NULL) {
      RAISEEXCEPTION(envp, "GetStringUTFChars failure");
    }
  }

  marpaESLIFAlternative.names          = (char *) names;
  marpaESLIFAlternative.grammarLengthl = (size_t) grammarLengthi;
  if (! marpaESLIFJava_stack_setb(envp, NULL /* marpaESLIFValuep */, -1 /* resulti */, objectp, &(marpaESLIFAlternative.value))) {
    goto err;
  }

  if (! marpaESLIFRecognizer_alternative_readb(marpaESLIFRecognizerp, &marpaESLIFAlternative, (size_t) lengthi)) {
    goto err;
  }

  rcb = JNI_TRUE;
  goto done;

 err:
  rcb = JNI_FALSE;

 done:
  if (envp != NULL) {
    if ((namep != NULL) && (names != NULL)) {
      (*envp)->ReleaseStringUTFChars(envp, namep, names);
    }
  }
  return rcb;
}

/*****************************************************************************/
JNIEXPORT jboolean JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniNameTry(JNIEnv *envp, jobject eslifRecognizerp, jstring namep)
/*****************************************************************************/
{
  static const char             *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniNameTry";
  marpaESLIFRecognizer_t        *marpaESLIFRecognizerp;
  const char                    *names = NULL;
  jboolean                       isCopy;
  short                          matchb;
  jboolean                       rcb;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (namep != NULL) {
    names = (*envp)->GetStringUTFChars(envp, namep, &isCopy);
    if (names == NULL) {
      RAISEEXCEPTION(envp, "GetStringUTFChars failure");
    }
  }

  if (! marpaESLIFRecognizer_name_tryb(marpaESLIFRecognizerp, (char *) names, &matchb)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFRecognizer_name_tryb failure", strerror(errno));
  }

  rcb = matchb ? JNI_TRUE : JNI_FALSE;
  goto done;

 err:
  rcb = JNI_FALSE;

 done:
  if (envp != NULL) {
    if ((namep != NULL) && (names != NULL)) {
      (*envp)->ReleaseStringUTFChars(envp, namep, names);
    }
  }

  return rcb;
}

/*****************************************************************************/
JNIEXPORT jboolean JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniDiscardTry(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char             *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniDiscardTry";
  marpaESLIFRecognizer_t        *marpaESLIFRecognizerp;
  short                          matchb;
  jboolean                       rcb;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_discard_tryb(marpaESLIFRecognizerp, &matchb)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_discard_tryb failure");
  }

  rcb = matchb ? JNI_TRUE : JNI_FALSE;
  goto done;

 err:
  rcb = JNI_FALSE;

 done:
  return rcb;
}

/*****************************************************************************/
JNIEXPORT jobjectArray JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniNameExpected(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniNameExpected";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  size_t                  nName;
  size_t                  i;
  char                  **namesArrayp;
  jobjectArray            objectArray = NULL;
  jstring                 string      = NULL;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_name_expectedb(marpaESLIFRecognizerp, &nName, &namesArrayp)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_name_expectedb failure");
  }

  if (nName > 0) {
    objectArray = (*envp)->NewObjectArray(envp, (jsize) nName, JAVA_LANG_STRING_CLASSP, NULL /* initialElement */);
    if (objectArray == NULL) {
      RAISEEXCEPTION(envp, "NewObjectArray failure");
    }
    for (i = 0; i < nName; i++) {
      string = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, namesArrayp[i]);
      (*envp)->SetObjectArrayElement(envp, objectArray, (jsize) i, string);
      if (HAVEEXCEPTION(envp)) {
        goto err;
      }
    }
  }

  goto done;

 err:
  if (envp != NULL) {
    if (objectArray != NULL) {
      (*envp)->DeleteLocalRef(envp, objectArray);
    }
    if (string != NULL) {
      (*envp)->DeleteLocalRef(envp, string);
    }
  }
  objectArray = NULL;

 done:
  return objectArray;
}

/*****************************************************************************/
JNIEXPORT jboolean JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniIsEof(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniIsEof";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  short                   eofb;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_isEofb(marpaESLIFRecognizerp, &eofb)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_isEofb failure");
  }

  return eofb ? JNI_TRUE : JNI_FALSE;

 err:
  return JNI_FALSE; /* Exception seen by the upper layer */
}

/*****************************************************************************/
JNIEXPORT jboolean JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniIsStartComplete(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniIsStartComplete";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  short                   completeb;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_isStartCompleteb(marpaESLIFRecognizerp, &completeb)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_isStartCompleteb failure");
  }

  return completeb ? JNI_TRUE : JNI_FALSE;

 err:
  return JNI_FALSE; /* Exception seen by the upper layer */
}

/*****************************************************************************/
JNIEXPORT jboolean JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniIsExhausted(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniIsExhausted";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  short                   exhaustedb;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_isExhaustedb(marpaESLIFRecognizerp, &exhaustedb)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_isExhaustedb failure");
  }

  return exhaustedb ? JNI_TRUE : JNI_FALSE;

 err:
  return JNI_FALSE; /* Exception seen by the upper layer */
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniSetExhaustedFlag(JNIEnv *envp, jobject eslifRecognizerp, jboolean flag)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniSetExhaustedFlag";
  short                   exhaustedb = (flag == JNI_TRUE) ? 1 : 0;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_set_exhausted_flagb(marpaESLIFRecognizerp, exhaustedb)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_set_exhausted_flagb failure");
  }

 err:
  return;
}

/*****************************************************************************/
JNIEXPORT jboolean JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniIsCanContinue(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniIsCanContinue";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  short                   canContinueb;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_isCanContinueb(marpaESLIFRecognizerp, &canContinueb)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_isCanContinueb failure");
  }

  return canContinueb ? JNI_TRUE : JNI_FALSE;

 err:
  return JNI_FALSE; /* Exception seen by the upper layer */
}

/*****************************************************************************/
JNIEXPORT jboolean JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniRead(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniRead";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (!  marpaESLIFRecognizer_readb(marpaESLIFRecognizerp, NULL, NULL)) {
    goto err;
  }

  return JNI_TRUE;

 err:
  return JNI_FALSE;
}

/*****************************************************************************/
JNIEXPORT jbyteArray JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniInput(JNIEnv *envp, jobject eslifRecognizerp, jint offset, jint length)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniInput";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  jbyteArray              byteArrayp = NULL;
  char                   *inputs;
  size_t                  inputl;
  char                   *realinputs;
  size_t                  realinputl;
  size_t                  deltal;
  char                   *maxinputs;
  jobject                 eslifLoggerInterfacep;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_inputb(marpaESLIFRecognizerp, &inputs, &inputl)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFRecognizer_inputb failure, %s", strerror(errno));
  }

  if ((inputs != NULL) && (inputl > 0)) {
    maxinputs = inputs + inputl - 1;
    /* Apply offset parameter */
    realinputs = inputs;
    if (offset < 0) {
      realinputs += inputl;
    }
    realinputs += offset;
    if ((realinputs < inputs) || (realinputs > maxinputs)) {
      /* Get eslifLoggerInterface instance */
      eslifLoggerInterfacep = (*envp)->CallObjectMethod(envp, eslifRecognizerp, MARPAESLIF_ESLIFRECOGNIZER_CLASS_getLoggerInterfacep_METHODP);
      if (eslifLoggerInterfacep != NULL) {
        callLoggerInterfacev(eslifLoggerInterfacep, GENERICLOGGER_LOGLEVEL_WARNING, "input() goes beyond either end of input buffer");
      }
      /* Default value is already NULL */
    } else {
      /* Adapt input length to the modified start offset */
      if (realinputs > inputs) {
        deltal = realinputs - inputs;
        inputl -= deltal;
      }
      /* Apply length parameter */
      if (length == 0) {
        realinputl = inputl; /* All bytes available */
      } else if (length > 0) {
        if (length < inputl) {
          realinputl = length; /* Remains more bytes than what the user want */
        } else {
          realinputl = inputl; /* Remains less bytes than what the user want */
        }
      } else {
        length = -length;
        if (length < inputl) {
          deltal = inputl - length; 
          realinputl = deltal; /* Skip length last bytes */
        } else {
          realinputl = 0; /* Skipping more bytes that what is available */
        }
      }

      byteArrayp = (*envp)->NewByteArray(envp, (jsize) realinputl);
      if (byteArrayp == NULL) {
        goto err;
      }
      (*envp)->SetByteArrayRegion(envp, byteArrayp, (jsize) 0, (jsize) realinputl, (jbyte *) realinputs);
      if (HAVEEXCEPTION(envp)) {
        goto err;
      }
    }
  }

  goto done;

 err:
  if (envp != NULL) {
    if (byteArrayp != NULL) {
      (*envp)->DeleteLocalRef(envp, byteArrayp);
    }
  }
  byteArrayp = NULL;

 done:
  return byteArrayp;
}

/*****************************************************************************/
JNIEXPORT jlong JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniInputLength(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniInputLength";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  size_t                  inputl;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (!  marpaESLIFRecognizer_inputb(marpaESLIFRecognizerp, NULL, &inputl)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFRecognizer_inputb failure, %s", strerror(errno));
  }

  goto done;

 err:

 done:
  return inputl;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniError(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniError";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_errorb(marpaESLIFRecognizerp)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_errorb failure");
  }

 err: /* err and done share the same code */
  return;
}

/*****************************************************************************/
JNIEXPORT jbyteArray JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniNameLastPause(JNIEnv *envp, jobject eslifRecognizerp, jstring namep)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniNameLastPause";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  jbyteArray              byteArrayp = NULL;
  const char             *names = NULL;
  jboolean                isCopy;
  char                   *pauses;
  size_t                  pausel;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (namep != NULL) {
    names = (*envp)->GetStringUTFChars(envp, namep, &isCopy);
    if (names == NULL) {
      RAISEEXCEPTION(envp, "GetStringUTFChars failure");
    }
  }

  if (!  marpaESLIFRecognizer_name_last_pauseb(marpaESLIFRecognizerp, (char *) names, &pauses, &pausel)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_name_last_pauseb failure");
  }

  if ((pauses != NULL) && (pausel > 0)) {
    byteArrayp = (*envp)->NewByteArray(envp, (jsize) pausel);
    if (byteArrayp == NULL) {
      goto err;
    }
    (*envp)->SetByteArrayRegion(envp, byteArrayp, (jsize) 0, (jsize) pausel, (jbyte *) pauses);
    if (HAVEEXCEPTION(envp)) {
      goto err;
    }
  }

  goto done;

 err:
  if (envp != NULL) {
    if (byteArrayp != NULL) {
      (*envp)->DeleteLocalRef(envp, byteArrayp);
    }
  }
  byteArrayp = NULL;

 done:
  if (envp != NULL) {
    if ((namep != NULL) && (names != NULL)) {
      (*envp)->ReleaseStringUTFChars(envp, namep, names);
    }
  }
  return byteArrayp;
}

/*****************************************************************************/
JNIEXPORT jbyteArray JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniNameLastTry(JNIEnv *envp, jobject eslifRecognizerp, jstring namep)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniNameLastTry";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  jbyteArray              byteArrayp = NULL;
  const char             *names = NULL;
  jboolean                isCopy;
  char                   *trys;
  size_t                  tryl;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (namep != NULL) {
    names = (*envp)->GetStringUTFChars(envp, namep, &isCopy);
    if (names == NULL) {
      RAISEEXCEPTION(envp, "GetStringUTFChars failure");
    }
  }

  if (!  marpaESLIFRecognizer_name_last_tryb(marpaESLIFRecognizerp, (char *) names, &trys, &tryl)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_name_last_tryb failure");
  }

  if ((trys != NULL) && (tryl > 0)) {
    byteArrayp = (*envp)->NewByteArray(envp, (jsize) tryl);
    if (byteArrayp == NULL) {
      goto err;
    }
    (*envp)->SetByteArrayRegion(envp, byteArrayp, (jsize) 0, (jsize) tryl, (jbyte *) trys);
    if (HAVEEXCEPTION(envp)) {
      goto err;
    }
  }

  goto done;

 err:
  if (envp != NULL) {
    if (byteArrayp != NULL) {
      (*envp)->DeleteLocalRef(envp, byteArrayp);
    }
  }
  byteArrayp = NULL;

 done:
  if (envp != NULL) {
    if ((namep != NULL) && (names != NULL)) {
      (*envp)->ReleaseStringUTFChars(envp, namep, names);
    }
  }
  return byteArrayp;
}

/*****************************************************************************/
JNIEXPORT jbyteArray JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniDiscardLastTry(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniDiscardLastTry";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  jbyteArray              byteArrayp = NULL;
  char                   *trys;
  size_t                  tryl;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (!  marpaESLIFRecognizer_discard_last_tryb(marpaESLIFRecognizerp, &trys, &tryl)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_discard_last_tryb failure");
  }

  if ((trys != NULL) && (tryl > 0)) {
    byteArrayp = (*envp)->NewByteArray(envp, (jsize) tryl);
    if (byteArrayp == NULL) {
      goto err;
    }
    (*envp)->SetByteArrayRegion(envp, byteArrayp, (jsize) 0, (jsize) tryl, (jbyte *) trys);
    if (HAVEEXCEPTION(envp)) {
      goto err;
    }
  }

  goto done;

 err:
  if (envp != NULL) {
    if (byteArrayp != NULL) {
      (*envp)->DeleteLocalRef(envp, byteArrayp);
    }
  }
  byteArrayp = NULL;

 done:
  return byteArrayp;
}

/*****************************************************************************/
JNIEXPORT jbyteArray JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniDiscardLast(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniDiscardLast";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  jbyteArray              byteArrayp = NULL;
  char                   *lasts;
  size_t                  lastl;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (!  marpaESLIFRecognizer_discard_lastb(marpaESLIFRecognizerp, &lasts, &lastl)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_discard_lastb failure");
  }

  if ((lasts != NULL) && (lastl > 0)) {
    byteArrayp = (*envp)->NewByteArray(envp, (jsize) lastl);
    if (byteArrayp == NULL) {
      goto err;
    }
    (*envp)->SetByteArrayRegion(envp, byteArrayp, (jsize) 0, (jsize) lastl, (jbyte *) lasts);
    if (HAVEEXCEPTION(envp)) {
      goto err;
    }
  }

  goto done;

 err:
  if (envp != NULL) {
    if (byteArrayp != NULL) {
      (*envp)->DeleteLocalRef(envp, byteArrayp);
    }
  }
  byteArrayp = NULL;

 done:
  return byteArrayp;
}

/*****************************************************************************/
JNIEXPORT jlong JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniLastCompletedOffset(JNIEnv *envp, jobject eslifRecognizerp, jstring namep)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniLastCompletedOffset";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  const char             *names = NULL;
  jboolean                isCopy;
  char                   *offsetp = NULL;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (namep != NULL) {
    names = (*envp)->GetStringUTFChars(envp, namep, &isCopy);
    if (names == NULL) {
      RAISEEXCEPTION(envp, "GetStringUTFChars failure");
    }
  }

  if (!  marpaESLIFRecognizer_last_completedb(marpaESLIFRecognizerp, (char *) names, &offsetp, NULL /* lengthlp */)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_last_completedb failure");
  }

  goto done;

 err:
  if (envp != NULL) {
    if ((namep != NULL) && (names != NULL)) {
      (*envp)->ReleaseStringUTFChars(envp, namep, names);
    }
  }

 done:
  return PTR_TO_JLONG(offsetp);
}

/*****************************************************************************/
JNIEXPORT jlong JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniLastCompletedLength(JNIEnv *envp, jobject eslifRecognizerp, jstring namep)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniLastCompletedLength";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  const char             *names = NULL;
  jboolean                isCopy;
  size_t                  lengthl;
  jlong                   rcl;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (namep != NULL) {
    names = (*envp)->GetStringUTFChars(envp, namep, &isCopy);
    if (names == NULL) {
      RAISEEXCEPTION(envp, "GetStringUTFChars failure");
    }
  }

  if (!  marpaESLIFRecognizer_last_completedb(marpaESLIFRecognizerp, (char *) names, NULL /* offsetpp */, &lengthl)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_last_completedb failure");
  }

  rcl = (jlong) lengthl;
  goto done;

 err:
  rcl = -1;

 done:
  if (envp != NULL) {
    if ((namep != NULL) && (names != NULL)) {
      (*envp)->ReleaseStringUTFChars(envp, namep, names);
    }
  }

  return rcl;
}

/*****************************************************************************/
JNIEXPORT jlong JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniLine(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniLine";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  size_t                  linel = 0;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (!  marpaESLIFRecognizer_locationb(marpaESLIFRecognizerp, &linel, NULL /* columnlp */)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_locationb failure");
  }

 err: /* err and done share the same code */
  return (jlong) linel;
}

/*****************************************************************************/
JNIEXPORT jlong JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniColumn(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniColumn";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  size_t                  columnl = 0;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (!  marpaESLIFRecognizer_locationb(marpaESLIFRecognizerp, NULL /* linelp */, &columnl)) {
    RAISEEXCEPTION(envp, "marpaESLIFRecognizer_locationb failure");
  }

 err: /* err and done share the same code */
  return (jlong) columnl;
}

/*****************************************************************************/
JNIEXPORT jobjectArray JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniEvent(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniEvent";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  size_t                  eventArrayl;
  marpaESLIFEvent_t      *eventArrayp;
  jobjectArray            objectArray = NULL;
  jobject                 object      = NULL;
  jobject                 eventType   = NULL;
  jstring                 symbol      = NULL;
  jstring                 event       = NULL;
  size_t                  i;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_eventb(marpaESLIFRecognizerp, &eventArrayl, &eventArrayp)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFRecognizer_eventb failure, %s", strerror(errno));
  }

  objectArray = (*envp)->NewObjectArray(envp, (jsize) eventArrayl, MARPAESLIF_ESLIFEVENT_CLASSP, NULL /* initialElement */);
  if (objectArray == NULL) {
    RAISEEXCEPTION(envp, "NewObjectArray failure");
  }

  for (i = 0; i < eventArrayl; i++) {
    object    = NULL;
    eventType = NULL;
    symbol    = NULL;
    event     = NULL;

    eventType = (*envp)->CallStaticObjectMethod(envp, MARPAESLIF_ESLIFEVENTTYPE_CLASSP, MARPAESLIF_ESLIFEVENTTYPE_CLASS_get_METHODP, eventArrayp[i].type);
    if (eventType == NULL) {
      RAISEEXCEPTION(envp, "CallStaticObjectMethod failure");
    }

    symbol = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, eventArrayp[i].symbols);
    event = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, eventArrayp[i].events);

    object = (*envp)->NewObject(envp, MARPAESLIF_ESLIFEVENT_CLASSP, MARPAESLIF_ESLIFEVENT_CLASS_init_METHODP, eventType, symbol, event);
    if (object == NULL) {
      RAISEEXCEPTION(envp, "NewObject failure");
    }
    /* They are all in "object" now */
    eventType = NULL;
    symbol    = NULL;
    event     = NULL;

    (*envp)->SetObjectArrayElement(envp, objectArray, (jsize) i, object);
    if (HAVEEXCEPTION(envp)) {
      goto err;
    }
  }

  goto done;

 err:
  if (envp != NULL) {
    if (objectArray != NULL) {
      (*envp)->DeleteLocalRef(envp, objectArray);
    }
    if (object != NULL) {
      (*envp)->DeleteLocalRef(envp, object);
    }
    if (eventType != NULL) {
      (*envp)->DeleteLocalRef(envp, eventType);
    }
    if (symbol != NULL) {
      (*envp)->DeleteLocalRef(envp, symbol);
    }
    if (event != NULL) {
      (*envp)->DeleteLocalRef(envp, event);
    }
  }
  objectArray = NULL;

 done:
  return objectArray;
}

/*****************************************************************************/
JNIEXPORT jobjectArray JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniProgress(JNIEnv *envp, jobject eslifRecognizerp, jint starti, jint endi)
/*****************************************************************************/
{
  static const char              *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniProgress";
  marpaESLIFRecognizer_t         *marpaESLIFRecognizerp;
  size_t                          progressl;
  marpaESLIFRecognizerProgress_t *progressp;
  jobjectArray                    objectArray = NULL;
  jobject                         object      = NULL;
  jint                            earleySetId;
  jint                            earleySetOrigId;
  jint                            rule;
  jint                            position;
  size_t                          i;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_progressb(marpaESLIFRecognizerp, (int) starti, (int) endi, &progressl, &progressp)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFRecognizer_progressb failure, %s", strerror(errno));
  }

  objectArray = (*envp)->NewObjectArray(envp, (jsize) progressl, MARPAESLIF_ESLIFPROGRESS_CLASSP, NULL /* initialElement */);
  if (objectArray == NULL) {
    RAISEEXCEPTION(envp, "NewObjectArray failure");
  }

  for (i = 0; i < progressl; i++) {
    earleySetId      = (jint) progressp[i].earleySetIdi;
    earleySetOrigId  = (jint) progressp[i].earleySetOrigIdi;
    rule             = (jint) progressp[i].rulei;
    position         = (jint) progressp[i].positioni;

    object = (*envp)->NewObject(envp, MARPAESLIF_ESLIFPROGRESS_CLASSP, MARPAESLIF_ESLIFPROGRESS_CLASS_init_METHODP, earleySetId, earleySetOrigId, rule, position);
    if (object == NULL) {
      RAISEEXCEPTION(envp, "NewObject failure");
    }

    (*envp)->SetObjectArrayElement(envp, objectArray, (jsize) i, object);
    if (HAVEEXCEPTION(envp)) {
      goto err;
    }
  }

  goto done;

 err:
  if (envp != NULL) {
    if (objectArray != NULL) {
      (*envp)->DeleteLocalRef(envp, objectArray);
    }
    if (object != NULL) {
      (*envp)->DeleteLocalRef(envp, object);
    }
  }
  objectArray = NULL;

 done:
  return objectArray;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniEventOnOff(JNIEnv *envp, jobject eslifRecognizerp, jstring symbolp, jobjectArray eventTypesp, jboolean onOff)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniEventOnOff";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  jboolean                isCopy;
  const char             *symbols    = (symbolp != NULL) ? (*envp)->GetStringUTFChars(envp, symbolp, &isCopy) : NULL;
  marpaESLIFEventType_t   eventSeti  = MARPAESLIF_EVENTTYPE_NONE;
  short                   onoffb     = (onOff == JNI_TRUE);
  jobject                 eventTypep = NULL;
  jsize                   eventTypesLengthl;
  jint                    codei;
  jsize                   i;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (eventTypesp != NULL) {
    eventTypesLengthl = (*envp)->GetArrayLength(envp, eventTypesp);
    if (HAVEEXCEPTION(envp)) {
      RAISEEXCEPTION(envp, "GetArrayLength failure");
    }
    for (i = 0; i < eventTypesLengthl; i++) {
      if (eventTypep != NULL) {
        (*envp)->DeleteLocalRef(envp, eventTypep);
      }
      eventTypep = (*envp)->GetObjectArrayElement(envp, eventTypesp, i);
      if (HAVEEXCEPTION(envp)) {
        RAISEEXCEPTION(envp, " GetObjectArrayElement failure");
      }
      if (eventTypep != NULL) {
        codei = (*envp)->CallIntMethod(envp, eventTypep, MARPAESLIF_ESLIFEVENTTYPE_CLASS_getCode_METHODP);
        if (HAVEEXCEPTION(envp)) {
          RAISEEXCEPTION(envp, "getCode failure");
        }
        switch (codei) {
        case MARPAESLIF_EVENTTYPE_NONE:
          break;
        case MARPAESLIF_EVENTTYPE_COMPLETED:
        case MARPAESLIF_EVENTTYPE_NULLED:
        case MARPAESLIF_EVENTTYPE_PREDICTED:
        case MARPAESLIF_EVENTTYPE_BEFORE:
        case MARPAESLIF_EVENTTYPE_AFTER:
        case MARPAESLIF_EVENTTYPE_EXHAUSTED:
        case MARPAESLIF_EVENTTYPE_DISCARD:
          eventSeti |= codei;
          break;
        default:
          RAISEEXCEPTIONF(envp, "Unknown code %d", (int) codei);
          break;
        }
      }
    }
  }

  if (! marpaESLIFRecognizer_event_onoffb(marpaESLIFRecognizerp, (char *) symbols, eventSeti, onoffb)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFRecognizer_event_onoffb failure, %s", strerror(errno));
  }

 err: /* err and done share the same code */
  if (envp != NULL) {
    if ((symbolp != NULL) && (symbols != NULL)) {
      (*envp)->ReleaseStringUTFChars(envp, symbolp, symbols);
    }
    if (eventTypep != NULL) {
      (*envp)->DeleteLocalRef(envp, eventTypep);
    }
  }

  return;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniHookDiscard(JNIEnv *envp, jobject eslifRecognizerp, jboolean onOff)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniHookDiscard";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  short                   discardOnoffb = (onOff == JNI_TRUE);

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_hook_discardb(marpaESLIFRecognizerp, discardOnoffb)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFRecognizer_hook_discardb failure, %s", strerror(errno));
  }

 err: /* err and done share the same code */

  return;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniHookDiscardSwitch(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniHookDiscardSwitch";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (! marpaESLIFRecognizer_hook_discard_switchb(marpaESLIFRecognizerp)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFRecognizer_hook_discard_switchb failure, %s", strerror(errno));
  }

 err: /* err and done share the same code */

  return;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniProgressLog(JNIEnv *envp, jobject eslifRecognizerp, jint starti, jint endi, jobject levelp)
/*****************************************************************************/
{
  static const char      *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniProgressLog";
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp;
  genericLoggerLevel_t    logleveli;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, NULL /* marpaESLIFJavaRecognizerContextpp */)) {
    goto err;
  }

  if (levelp != NULL) {
    logleveli = (*envp)->CallIntMethod(envp, levelp, MARPAESLIF_ESLIFLOGGERLEVEL_CLASS_getCode_METHODP);
    if (HAVEEXCEPTION(envp)) {
      RAISEEXCEPTION(envp, "getCode failure");
    }
    switch (logleveli) {
    case GENERICLOGGER_LOGLEVEL_TRACE:
    case GENERICLOGGER_LOGLEVEL_DEBUG:
    case GENERICLOGGER_LOGLEVEL_INFO:
    case GENERICLOGGER_LOGLEVEL_NOTICE:
    case GENERICLOGGER_LOGLEVEL_WARNING:
    case GENERICLOGGER_LOGLEVEL_ERROR:
    case GENERICLOGGER_LOGLEVEL_CRITICAL:
    case GENERICLOGGER_LOGLEVEL_ALERT:
    case GENERICLOGGER_LOGLEVEL_EMERGENCY:
      break;
    default:
      RAISEEXCEPTIONF(envp, "Unknown logger level %d", (int) logleveli);
      break;
    }
    if (! marpaESLIFRecognizer_progressLogb(marpaESLIFRecognizerp, (int) starti, (int) endi, logleveli)) {
      RAISEEXCEPTION(envp, "marpaESLIFRecognizer_progressLogb failure");
    }
  }

 err: /* err and done share the same code */
  return;
}

/*****************************************************************************/
JNIEXPORT jobject JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniSymbolTry(JNIEnv *envp, jobject eslifRecognizerp, jobject eslifSymbolp)
/*****************************************************************************/
{
  static const char                 *funcs = "Java_org_parser_marpa_ESLIFRecognizer_jniSymbolTry";
  marpaESLIFRecognizer_t            *marpaESLIFRecognizerp;
  marpaESLIFSymbol_t                *marpaESLIFSymbolp;
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp;
  short                              matchb;
  jobject                            objectp = NULL;

  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, &marpaESLIFJavaRecognizerContextp)) {
    goto err;
  }

  if (! ESLIFSymbol_contextb(envp, eslifSymbolp, NULL /* marpaESLIFpp */, &marpaESLIFSymbolp, NULL /* marpaESLIFJavaSymbolContextpp */)) {
    goto err;
  }

  if (!  marpaESLIFRecognizer_symbol_tryb(marpaESLIFRecognizerp, marpaESLIFSymbolp, &matchb)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFRecognizer_symbol_tryb failure, %s", strerror(errno));
  }

  if (matchb) {
    if (GENERICSTACK_USED(marpaESLIFJavaRecognizerContextp->objectStackp) != 1) {
      RAISEEXCEPTIONF(envp, "Internal value stack is %d instead of 1", GENERICSTACK_USED(marpaESLIFJavaRecognizerContextp->objectStackp));
    }
    objectp = (jobject) GENERICSTACK_POP_PTR(marpaESLIFJavaRecognizerContextp->objectStackp);
  }

  goto done;

 err:
 done:
  return objectp;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFRecognizer_jniFree(JNIEnv *envp, jobject eslifRecognizerp)
/*****************************************************************************/
{
  marpaESLIFRecognizer_t            *marpaESLIFRecognizerp;
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp;

  if (ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, &marpaESLIFJavaRecognizerContextp)) {
    marpaESLIFJava_recognizerContextFreev(envp, marpaESLIFJavaRecognizerContextp, 0 /* onStackb */);
    marpaESLIFRecognizer_freev(marpaESLIFRecognizerp); /* This is NULL protected */
  }
}

/*****************************************************************************/
static short ESLIFValue_contextb(JNIEnv *envp, jobject eslifValuep, marpaESLIFValue_t **marpaESLIFValuepp, marpaESLIFJavaValueContext_t **marpaESLIFJavaValueContextpp)
/*****************************************************************************/
{
  static const char            *funcs = "ESLIFValue_contextb";
  jobject                       BYTEBUFFER(marpaESLIFValue);
  jobject                       BYTEBUFFER(marpaESLIFJavaValueContext);
  marpaESLIFValue_t            *marpaESLIFValuep = NULL;
  marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp = NULL;
    
  BYTEBUFFER(marpaESLIFValue) = (*envp)->CallObjectMethod(envp, eslifValuep, MARPAESLIF_ESLIFVALUE_CLASS_getMarpaESLIFValuep_METHODP);
  if (BYTEBUFFER(marpaESLIFValue) != NULL) {
    marpaESLIFValuep = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(marpaESLIFValue));
  }

  BYTEBUFFER(marpaESLIFJavaValueContext) = (*envp)->CallObjectMethod(envp, eslifValuep, MARPAESLIF_ESLIFVALUE_CLASS_getMarpaESLIFValueContextp_METHODP);
  if (BYTEBUFFER(marpaESLIFJavaValueContext) != NULL) {
    marpaESLIFJavaValueContextp = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(marpaESLIFJavaValueContext));
    marpaESLIFJavaValueContextp->eslifValueInterfacep = (*envp)->CallObjectMethod(envp, eslifValuep, MARPAESLIF_ESLIFVALUE_CLASS_getEslifValueInterface_METHODP);
  }

  if (marpaESLIFValuepp != NULL) {
    *marpaESLIFValuepp = marpaESLIFValuep;
  }
  if (marpaESLIFJavaValueContextpp != NULL) {
    *marpaESLIFJavaValueContextpp = marpaESLIFJavaValueContextp;
  }

  return 1;

 err:
  return 0;
}

static void jdd () {
  fprintf(stderr, "OUPS\n");
  fflush(stderr);
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFValue_jniNew(JNIEnv *envp, jobject eslifValuep, jobject eslifRecognizerp)
/*****************************************************************************/
{
  static const char                    *funcs = "Java_org_parser_marpa_ESLIFValue_jniNew";
  marpaESLIFValue_t                    *marpaESLIFValuep  = NULL;
  marpaESLIFRecognizer_t               *marpaESLIFRecognizerp;
  marpaESLIFJavaRecognizerContext_t    *marpaESLIFJavaRecognizerContextp;
  marpaESLIFValueOption_t               marpaESLIFValueOption;
  marpaESLIFJavaValueContext_t         *marpaESLIFJavaValueContextp;
  jobject                               eslifValueInterfacep;
  jobject                               eslifGrammarp;
  jobject                               BYTEBUFFER(marpaESLIFValue);
  jobject                               BYTEBUFFER(marpaESLIFJavaValueContext);

  /* Always update genericLogger context */
  if (! ESLIFRecognizer_contextb(envp, eslifRecognizerp, &marpaESLIFRecognizerp, &marpaESLIFJavaRecognizerContextp)) {
    /* An exception was raised */
    goto err;
  }

  /* Get grammar instance */
  eslifGrammarp = (*envp)->CallObjectMethod(envp, eslifRecognizerp, MARPAESLIF_ESLIFRECOGNIZER_CLASS_getEslifGrammar_METHODP);
  if (eslifGrammarp == NULL) {
    RAISEEXCEPTION(envp, "eslifGrammarp is NULL");
  }

 /* Get value interface - class is protected to not accept the null argument */
  eslifValueInterfacep = (*envp)->CallObjectMethod(envp, eslifValuep, MARPAESLIF_ESLIFVALUE_CLASS_getEslifValueInterface_METHODP);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  /* Value context is a structure containing the current value interface object instance - updated at every recognizer call */
  marpaESLIFJavaValueContextp = (marpaESLIFJavaValueContext_t *) malloc(sizeof(marpaESLIFJavaValueContext_t));
  if (marpaESLIFJavaValueContextp == NULL) {
    RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
  }
  if (! marpaESLIFJava_valueContextInitb(envp, eslifValueInterfacep, eslifGrammarp, marpaESLIFJavaValueContextp)) {
    goto err;
  }
  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIFJavaValueContext, marpaESLIFJavaValueContextp);
  (*envp)->CallVoidMethod(envp, eslifValuep, MARPAESLIF_ESLIFVALUE_CLASS_setMarpaESLIFValueContextp_METHODP, BYTEBUFFER(marpaESLIFJavaValueContext));
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  /* Create C object */
  marpaESLIFValueOption.userDatavp            = marpaESLIFJavaValueContextp;
  marpaESLIFValueOption.ruleActionResolverp   = marpaESLIFValueRuleActionResolver;
  marpaESLIFValueOption.symbolActionResolverp = marpaESLIFValueSymbolActionResolver;
  marpaESLIFValueOption.importerp             = marpaESLIFJava_valueImportb;
  marpaESLIFValueOption.highRankOnlyb         = ((*envp)->CallBooleanMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_isWithHighRankOnly_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFValueOption.orderByRankb          = ((*envp)->CallBooleanMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_isWithOrderByRank_METHODP)  == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFValueOption.ambiguousb            = ((*envp)->CallBooleanMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_isWithAmbiguous_METHODP)    == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFValueOption.nullb                 = ((*envp)->CallBooleanMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_isWithNull_METHODP)         == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFValueOption.maxParsesi            = (*envp)->CallIntMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_maxParses_METHODP);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
  if (marpaESLIFValuep == NULL) {
    RAISEEXCEPTIONF(envp, "marpaESLIFValue_newp failure, %s", strerror(errno));
  }

  /* Store the object */
  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIFValue, marpaESLIFValuep);  
  (*envp)->CallVoidMethod(envp, eslifValuep, MARPAESLIF_ESLIFVALUE_CLASS_setMarpaESLIFValuep_METHODP, BYTEBUFFER(marpaESLIFValue));
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  goto done;
  
 err:
  Java_org_parser_marpa_ESLIFValue_jniFree(envp, eslifValuep);

 done:
  return;
}

/*****************************************************************************/
JNIEXPORT jboolean JNICALL Java_org_parser_marpa_ESLIFValue_jniValue(JNIEnv *envp, jobject eslifValuep)
/*****************************************************************************/
{
  static const char            *funcs = "Java_org_parser_marpa_ESLIFValue_jniValue";
  marpaESLIFValue_t            *marpaESLIFValuep;
  short                         valueb;
  jboolean                      rcb;
  marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp;
  jobject                       eslifValueInterfacep;
  jobject                       objectp;
  
  if (! ESLIFValue_contextb(envp, eslifValuep, &marpaESLIFValuep, &marpaESLIFJavaValueContextp)) {
    goto err;
  }

  eslifValueInterfacep = marpaESLIFJavaValueContextp->eslifValueInterfacep;

  valueb = marpaESLIFValue_valueb(marpaESLIFValuep);
  if (valueb < 0) {
    RAISEEXCEPTIONF(envp, "marpaESLIFValue_valueb failure, %s", strerror(errno));
  }

  if (valueb > 0) {
    if (GENERICSTACK_USED(marpaESLIFJavaValueContextp->objectStackp) != 1) {
      RAISEEXCEPTIONF(envp, "Internal value stack is %d instead of 1", GENERICSTACK_USED(marpaESLIFJavaValueContextp->objectStackp));
    }
    objectp = (jobject) GENERICSTACK_POP_PTR(marpaESLIFJavaValueContextp->objectStackp);
    (*envp)->CallVoidMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_setResult_METHODP, objectp);
    if (HAVEEXCEPTION(envp)) {
      goto err;
    }
    rcb = JNI_TRUE;
  } else {
    rcb = JNI_FALSE;
  }

goto done;

 err:
  /* Well, caller will see an exception at this stage */
  rcb = JNI_FALSE;

 done:
  return rcb;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFValue_jniFree(JNIEnv *envp, jobject eslifValuep)
/*****************************************************************************/
{
  marpaESLIFValue_t            *marpaESLIFValuep;
  marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp;

  if (ESLIFValue_contextb(envp, eslifValuep, &marpaESLIFValuep, &marpaESLIFJavaValueContextp)) {
    marpaESLIFJava_valueContextFreev(envp, marpaESLIFJavaValueContextp, 0 /* onStackb */);
    marpaESLIFValue_freev(marpaESLIFValuep); /* This is NULL protected */
  }
}

/*****************************************************************************/
static void readerCallbackDisposev(void *userDatavp, char *inputcp, size_t inputl, short eofb, short characterStreamb, char *encodings, size_t encodingl)
/*****************************************************************************/
{
  JNIEnv                        *envp;
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp;

  marpaESLIFJavaRecognizerContextp = (marpaESLIFJavaRecognizerContext_t *) userDatavp;

  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    return;
  }

  if (marpaESLIFJavaRecognizerContextp != NULL) {
    if ((marpaESLIFJavaRecognizerContextp->byteArrayp != NULL) && (inputcp != NULL)) {
      (*envp)->ReleaseByteArrayElements(envp, marpaESLIFJavaRecognizerContextp->byteArrayp, (jbyte *) inputcp, JNI_ABORT);
    }
    marpaESLIFJavaRecognizerContextp->byteArrayp = NULL;

    if ((marpaESLIFJavaRecognizerContextp->encodingp != NULL) && (encodings != NULL)) {
      (*envp)->ReleaseStringUTFChars(envp, marpaESLIFJavaRecognizerContextp->encodingp, (const char *) encodings);
    }
    marpaESLIFJavaRecognizerContextp->encodingp = NULL;
  }
}

/*****************************************************************************/
static short readerCallbackb(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp)
/*****************************************************************************/
{
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp;
  jobject                            eslifRecognizerInterfacep;
  JNIEnv                            *envp = NULL;
  jbyteArray                         byteArrayp = NULL;
  jbyte                             *datap = NULL;
  jstring                            encodingp = NULL;
  const char                        *encodings = NULL;
  jboolean                           isCopy;
  jboolean                           readb;
  short                              rcb;

  marpaESLIFJavaRecognizerContextp = (marpaESLIFJavaRecognizerContext_t *) userDatavp;
  eslifRecognizerInterfacep   = marpaESLIFJavaRecognizerContextp->eslifRecognizerInterfacep;

  /* Reader callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  /* Call the read interface */
  readb = (*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_read_METHODP);
  if (HAVEEXCEPTION(envp) || (readb != JNI_TRUE)) {
    goto err;
  }

  byteArrayp = (*envp)->CallObjectMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_data_METHODP);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  datap      = (byteArrayp != NULL) ? (*envp)->GetByteArrayElements(envp, byteArrayp, &isCopy) : NULL;
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  encodingp  = (*envp)->CallObjectMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_encoding_METHODP);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  encodings  = (encodingp != NULL) ? (*envp)->GetStringUTFChars(envp, encodingp, &isCopy) : NULL;  /* => The famous UTF-8 hardcoded below */
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  *inputcpp             = (char *) datap;
  *inputlp              = (size_t) (byteArrayp != NULL) ? (*envp)->GetArrayLength(envp, byteArrayp) : 0;
  *eofbp                = ((*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isEof_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  *characterStreambp    = ((*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isCharacterStream_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  *encodingsp           = (char *) encodings;
  *encodinglp           = (size_t) (encodings != NULL ? strlen(encodings) : 0);
  *disposeCallbackpp    = readerCallbackDisposev;

  marpaESLIFJavaRecognizerContextp->byteArrayp = byteArrayp;
  marpaESLIFJavaRecognizerContextp->encodingp  = encodingp;

  rcb = 1;
  goto done;

 err:
  if (envp != NULL) {
    if ((byteArrayp != NULL) && (datap != NULL)) {
      (*envp)->ReleaseByteArrayElements(envp, byteArrayp, datap, JNI_ABORT);
    }
    if ((encodingp != NULL) && (encodings != NULL)) {
      (*envp)->ReleaseStringUTFChars(envp, encodingp, encodings);
    }
  }

  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static marpaESLIFValueRuleCallback_t marpaESLIFValueRuleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/*****************************************************************************/
{
  JNIEnv                       *envp;
  marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp = (marpaESLIFJavaValueContext_t *) userDatavp;
  jmethodID                     methodp;

  /* Resolver callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  /* Make sure rule action method is resolved */
  methodp = marpaESLIFJava_valueActionResolveri(envp, marpaESLIFJavaValueContextp, actions, MARPAESLIF_ESLIFVALUEINTERFACE_RULEACTION_SIGNATURE);
  if (methodp == NULL) {
    goto err;
  }

  marpaESLIFJavaValueContextp->actions = actions;
  marpaESLIFJavaValueContextp->methodp = methodp;

  return marpaESLIFValueRuleCallback;

 err:
  return NULL;
}

/*****************************************************************************/
static marpaESLIFValueSymbolCallback_t marpaESLIFValueSymbolActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValueResultSymbolp, char *actions)
/*****************************************************************************/
{
  JNIEnv                       *envp;
  marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp = (marpaESLIFJavaValueContext_t *) userDatavp;
  jmethodID                     methodp;

  /* Resolver callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  /* Make sure symbol action method is resolved */
  methodp = marpaESLIFJava_valueActionResolveri(envp, marpaESLIFJavaValueContextp, actions, MARPAESLIF_ESLIFVALUEINTERFACE_SYMBOLACTION_SIGNATURE);
  if (methodp == NULL) {
    goto err;
  }

  marpaESLIFJavaValueContextp->actions = actions;
  marpaESLIFJavaValueContextp->methodp = methodp;

  return marpaESLIFJava_valueSymbolCallbackb;

 err:
  return NULL;
}

/*****************************************************************************/
static marpaESLIFRecognizerIfCallback_t marpaESLIFRecognizerIfActionResolver(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions)
/*****************************************************************************/
{
  JNIEnv                            *envp;
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp = (marpaESLIFJavaRecognizerContext_t *) userDatavp;
  jmethodID                          methodp;

  /* Resolver callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  /* Make sure symbol action method is resolved */
  methodp = marpaESLIFJava_recognizerActionResolveri(envp, marpaESLIFJavaRecognizerContextp, actions, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_IFACTION_SIGNATURE);
  if (methodp == NULL) {
    goto err;
  }

  marpaESLIFJavaRecognizerContextp->actions = actions;
  marpaESLIFJavaRecognizerContextp->methodp = methodp;

  return marpaESLIFJava_recognizerIfCallbackb;

 err:
  return NULL;
}

/*****************************************************************************/
static marpaESLIFRecognizerEventCallback_t marpaESLIFRecognizerEventActionResolver(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions)
/*****************************************************************************/
{
  JNIEnv                            *envp;
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp = (marpaESLIFJavaRecognizerContext_t *) userDatavp;
  jmethodID                          methodp;

  /* Resolver callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  /* Make sure event action method is resolved */
  methodp = marpaESLIFJava_recognizerActionResolveri(envp, marpaESLIFJavaRecognizerContextp, actions, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_EVENTACTION_SIGNATURE);
  if (methodp == NULL) {
    goto err;
  }

  marpaESLIFJavaRecognizerContextp->actions = actions;
  marpaESLIFJavaRecognizerContextp->methodp = methodp;

  return marpaESLIFJava_recognizerEventCallbackb;

 err:
  return NULL;
}

/*****************************************************************************/
static marpaESLIFRecognizerRegexCallback_t marpaESLIFRecognizerRegexActionResolver(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions)
/*****************************************************************************/
{
  JNIEnv                            *envp;
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp = (marpaESLIFJavaRecognizerContext_t *) userDatavp;
  jmethodID                          methodp;

  /* Resolver callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  /* Make sure event action method is resolved */
  methodp = marpaESLIFJava_recognizerActionResolveri(envp, marpaESLIFJavaRecognizerContextp, actions, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_REGEXACTION_SIGNATURE);
  if (methodp == NULL) {
    goto err;
  }

  marpaESLIFJavaRecognizerContextp->actions = actions;
  marpaESLIFJavaRecognizerContextp->methodp = methodp;

  return marpaESLIFJava_recognizerRegexCallbackb;

 err:
  return NULL;
}

/*****************************************************************************/
static marpaESLIFRecognizerGeneratorCallback_t marpaESLIFRecognizerGeneratorActionResolver(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions)
/*****************************************************************************/
{
  JNIEnv                            *envp;
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp = (marpaESLIFJavaRecognizerContext_t *) userDatavp;
  jmethodID                          methodp;

  /* Resolver callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  /* Make sure generator action method is resolved */
  methodp = marpaESLIFJava_recognizerActionResolveri(envp, marpaESLIFJavaRecognizerContextp, actions, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_GENERATORACTION_SIGNATURE);
  if (methodp == NULL) {
    goto err;
  }

  marpaESLIFJavaRecognizerContextp->actions = actions;
  marpaESLIFJavaRecognizerContextp->methodp = methodp;

  return marpaESLIFJava_recognizerGeneratorCallbackb;

 err:
  return NULL;
}

/*****************************************************************************/
static short marpaESLIFJavaValueContextInject(JNIEnv *envp, marpaESLIFValue_t *marpaESLIFValuep, jobject eslifValueInterfacep, marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFJavaValueContextInject";
  short              rcb;
  char              *symbols;
  int                symboli;
  char              *rules;
  int                rulei;
  jstring            symbolp = NULL;
  jstring            rulep = NULL;

  /* Get value context */
  if (! marpaESLIFValue_contextb(marpaESLIFValuep, &symbols, &symboli, &rules, &rulei)) {
    RAISEEXCEPTION(envp, "marpaESLIFValue_contextb");
  }

  /* Symbol name */
  if (symbols != NULL) {
    symbolp = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, symbols);
  }

  (*envp)->CallVoidMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_setSymbolName_METHODP, symbolp /* Can be NULL */);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  /* Symbol number */
  (*envp)->CallVoidMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_setSymbolNumber_METHODP, symboli);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  /* Rule name */
  if (rules != NULL) {
    rulep = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, rules);
  }

  (*envp)->CallVoidMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_setRuleName_METHODP, rulep /* Can be NULL */);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  /* Rule number */
  (*envp)->CallVoidMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_setRuleNumber_METHODP, rulei);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  /* Grammar instance - stored as a global reference in  marpaESLIFJavaValueContextp */
  (*envp)->CallVoidMethod(envp, eslifValueInterfacep, MARPAESLIF_ESLIFVALUEINTERFACE_CLASS_setGrammar_METHODP, marpaESLIFJavaValueContextp->eslifGrammarp);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (symbolp != NULL) {
    (*envp)->DeleteLocalRef(envp, symbolp);
  }
  if (rulep != NULL) {
    (*envp)->DeleteLocalRef(envp, rulep);
  }
  return rcb;
}

/*****************************************************************************/
static short marpaESLIFValueRuleCallback(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/*****************************************************************************/
{
  static const char             *funcs                       = "marpaESLIFValueRuleCallback";
  marpaESLIFJavaValueContext_t  *marpaESLIFJavaValueContextp = (marpaESLIFJavaValueContext_t *) userDatavp;
  jobjectArray                   list;
  JNIEnv                        *envp;
  jobject                        actionResultp;
  short                          rcb;
  int                            i;
  jint                           capacityi;
  jobject                        objectp;

  /* Rule callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  /* Prototype of a rule action is: public Object action(List<Object> list) */
  if (! nullableb) {
    /* This is a "variable local references" method - make sure we will have enough room in local frame */
    capacityi = (argni - arg0i + 1) /* worst case where everything is a grammar terminal */ + 1 /* list itself */ + 1 /* action result */;
    if ((*envp)->EnsureLocalCapacity(envp, capacityi) != 0) {
      RAISEEXCEPTION(envp, "EnsureLocalCapacity failure");
    }
  
    /* Make the list */
    list = (*envp)->NewObjectArray(envp, argni - arg0i + 1, JAVA_LANG_OBJECT_CLASSP, NULL /* initialElement */);
    if (list == NULL) {
      RAISEEXCEPTION(envp, "NewObjectArray failure");
    }

    for (i = arg0i; i <= argni; i++) {
      if (! marpaESLIFJava_valueGetObjectp(envp, marpaESLIFJavaValueContextp, marpaESLIFValuep, i, NULL /* marpaESLIFValueResultSymbol */, &objectp)) {
	goto err;
      }
      (*envp)->SetObjectArrayElement(envp, list, i - arg0i, objectp);
    }
  }

  /* Inject the context */
  if (! marpaESLIFJavaValueContextInject(envp, marpaESLIFValuep, marpaESLIFJavaValueContextp->eslifValueInterfacep, marpaESLIFJavaValueContextp)) {
    goto err;
  }
  /* Call the rule action */
  actionResultp = (*envp)->CallObjectMethod(envp, marpaESLIFJavaValueContextp->eslifValueInterfacep, marpaESLIFJavaValueContextp->methodp, list);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  if (! marpaESLIFJava_stack_setb(envp, marpaESLIFValuep, resulti, actionResultp, NULL)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short marpaESLIFJava_valueSymbolCallbackb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResultSymbolp, int resulti)
/*****************************************************************************/
{
  /* Almost exactly like marpaESLIFValueRuleCallback except that we construct a list of one element containing a jbyteArray that we do ourself */
  static const char             *funcs                       = "marpaESLIFJava_valueSymbolCallbackb";
  marpaESLIFJavaValueContext_t  *marpaESLIFJavaValueContextp = (marpaESLIFJavaValueContext_t *) userDatavp;
  JNIEnv                        *envp;
  jobject                        actionResultp;
  short                          rcb;
  jobject                        objectp;

  /* Symbol callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  if (! marpaESLIFJava_valueGetObjectp(envp, marpaESLIFJavaValueContextp, marpaESLIFValuep, -1 /* stackindicei */, marpaESLIFValueResultSymbolp, &objectp)) {
    goto err;
  }

  /* Inject the context */
  if (! marpaESLIFJavaValueContextInject(envp, marpaESLIFValuep, marpaESLIFJavaValueContextp->eslifValueInterfacep, marpaESLIFJavaValueContextp)) {
    goto err;
  }

  /* Call the symbol action */
  actionResultp = (*envp)->CallObjectMethod(envp, marpaESLIFJavaValueContextp->eslifValueInterfacep, marpaESLIFJavaValueContextp->methodp, objectp);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  if (! marpaESLIFJava_stack_setb(envp, marpaESLIFValuep, resulti, actionResultp, NULL)) {
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short marpaESLIFJava_recognizerIfCallbackb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFValueResultSymbolp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp)
/*****************************************************************************/
{
  /* Almost exactly like marpaESLIFValueSymbolCallback except that we construct a list of one element containing a jbyteArray that we do ourself */
  static const char                 *funcs                            = "marpaESLIFJava_recognizerIfCallbackb";
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp = (marpaESLIFJavaRecognizerContext_t *) userDatavp;
  JNIEnv                            *envp;
  jboolean                           boolean;
  short                              rcb;
  jbyteArray                         byteArrayp;

  /* If callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  byteArrayp = (*envp)->NewByteArray(envp, (jsize) marpaESLIFValueResultSymbolp->u.a.sizel);
  if (byteArrayp == NULL) {
    goto err;
  }
  if (marpaESLIFValueResultSymbolp->u.a.sizel > 0) {
    /* Don't know if it is legal to call SetByteArrayRegion with a zero size -; */
    (*envp)->SetByteArrayRegion(envp, byteArrayp, 0, (jsize) marpaESLIFValueResultSymbolp->u.a.sizel, (jbyte *) marpaESLIFValueResultSymbolp->u.a.p);
    if (HAVEEXCEPTION(envp)) {
      goto err;
    }
  }

  /* Call the if action */
  boolean = (*envp)->CallBooleanMethod(envp, marpaESLIFJavaRecognizerContextp->eslifRecognizerInterfacep, marpaESLIFJavaRecognizerContextp->methodp, byteArrayp);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  *marpaESLIFValueResultBoolp = (boolean == JNI_FALSE) ? MARPAESLIFVALUERESULTBOOL_FALSE : MARPAESLIFVALUERESULTBOOL_TRUE;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short marpaESLIFJava_recognizerEventCallbackb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFEvent_t *eventArrayp, size_t eventArrayl, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp)
/*****************************************************************************/
{
  /* Almost exactly like marpaESLIFJava_recognizerIfCallbackb */
  static const char                 *funcs                            = "marpaESLIFJava_recognizerEventCallbackb";
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp = (marpaESLIFJavaRecognizerContext_t *) userDatavp;
  jobjectArray                       objectArray                      = NULL;
  jobject                            object                           = NULL;
  jobject                            eventType                        = NULL;
  jstring                            symbol                           = NULL;
  jstring                            event                            = NULL;
  size_t                             i;
  JNIEnv                            *envp;
  jboolean                           boolean;
  short                              rcb;

  /* Event callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  objectArray = (*envp)->NewObjectArray(envp, (jsize) eventArrayl, MARPAESLIF_ESLIFEVENT_CLASSP, NULL /* initialElement */);
  if (objectArray == NULL) {
    RAISEEXCEPTION(envp, "NewObjectArray failure");
  }

  for (i = 0; i < eventArrayl; i++) {
    object    = NULL;
    eventType = NULL;
    symbol    = NULL;
    event     = NULL;

    eventType = (*envp)->CallStaticObjectMethod(envp, MARPAESLIF_ESLIFEVENTTYPE_CLASSP, MARPAESLIF_ESLIFEVENTTYPE_CLASS_get_METHODP, eventArrayp[i].type);
    if (eventType == NULL) {
      RAISEEXCEPTION(envp, "CallStaticObjectMethod failure");
    }

    symbol = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, eventArrayp[i].symbols);
    event = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, eventArrayp[i].events);

    object = (*envp)->NewObject(envp, MARPAESLIF_ESLIFEVENT_CLASSP, MARPAESLIF_ESLIFEVENT_CLASS_init_METHODP, eventType, symbol, event);
    if (object == NULL) {
      RAISEEXCEPTION(envp, "NewObject failure");
    }
    /* They are all in "object" now */
    eventType = NULL;
    symbol    = NULL;
    event     = NULL;

    (*envp)->SetObjectArrayElement(envp, objectArray, (jsize) i, object);
    if (HAVEEXCEPTION(envp)) {
      goto err;
    }
  }

  /* Call the event action */
  boolean = (*envp)->CallBooleanMethod(envp, marpaESLIFJavaRecognizerContextp->eslifRecognizerInterfacep, marpaESLIFJavaRecognizerContextp->methodp, objectArray);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  *marpaESLIFValueResultBoolp = (boolean == JNI_FALSE) ? MARPAESLIFVALUERESULTBOOL_FALSE : MARPAESLIFVALUERESULTBOOL_TRUE;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (envp != NULL) {
    if (objectArray != NULL) {
      (*envp)->DeleteLocalRef(envp, objectArray);
    }
    if (object != NULL) {
      (*envp)->DeleteLocalRef(envp, object);
    }
    if (eventType != NULL) {
      (*envp)->DeleteLocalRef(envp, eventType);
    }
    if (symbol != NULL) {
      (*envp)->DeleteLocalRef(envp, symbol);
    }
    if (event != NULL) {
      (*envp)->DeleteLocalRef(envp, event);
    }
  }

  return rcb;
}

/*****************************************************************************/
static short marpaESLIFJava_recognizerRegexCallbackb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFCalloutBlockp, marpaESLIFValueResultInt_t *marpaESLIFValueResultOutp)
/*****************************************************************************/
{
  static const char                         *funcs                            = "marpaESLIFJava_recognizerRegexCallbackb";
  marpaESLIFJavaRecognizerContext_t         *marpaESLIFJavaRecognizerContextp = (marpaESLIFJavaRecognizerContext_t *) userDatavp;
  jobject                                    regexEslifCalloutp               = NULL;
  jobject                                    calloutNumberp                   = NULL;
  jstring                                    calloutStringp                   = NULL;
  jbyteArray                                 subjectp                         = NULL;
  jstring                                    patternp                         = NULL;
  jlong                                      captureTop;
  jlong                                      captureLast;
  jlongArray                                 offsetVectorp                    = NULL;
  jlong                                     *offsetVectorArrayp               = NULL;
  jobject                                    markp                            = NULL;
  jlong                                      startMatch;
  jlong                                      currentPosition;
  jstring                                    nextItemp                        = NULL;
  jint                                       grammarLevel;
  jint                                       symbolId;
  JNIEnv                                    *envp;
  marpaESLIFString_t                         marpaESLIFString;
  jint                                       integer;
  short                                      rcb;
  size_t                                     l;

  /* Regex callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  /* By definition, marpaESLIFCalloutBlockp is of type TABLE */
  if (marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_CALLOUT_NUMBER].value.type == MARPAESLIF_VALUE_TYPE_LONG) {
    MARPAESLIFJAVA_NEW_LONG(envp, calloutNumberp, marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_CALLOUT_NUMBER].value.u.l);
  }

  if (marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_CALLOUT_STRING].value.type == MARPAESLIF_VALUE_TYPE_STRING) {
    marpaESLIFString.bytep          = (char *) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_CALLOUT_STRING].value.u.s.p;
    marpaESLIFString.bytel          = marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_CALLOUT_STRING].value.u.s.sizel;
    marpaESLIFString.encodingasciis = marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_CALLOUT_STRING].value.u.s.encodingasciis;
    marpaESLIFString.asciis         = NULL;
    calloutStringp = marpaESLIFJava_marpaESLIFStringToJavap(envp, &marpaESLIFString);
  }

  if (marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_SUBJECT].value.type == MARPAESLIF_VALUE_TYPE_ARRAY) {
    subjectp = (*envp)->NewByteArray(envp, (jsize) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_SUBJECT].value.u.a.sizel);
    if (subjectp == NULL) {
      goto err;
    }
    if (marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_SUBJECT].value.u.a.sizel > 0) {
      (*envp)->SetByteArrayRegion(envp, subjectp, (jsize) 0, (jsize) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_SUBJECT].value.u.a.sizel, (jbyte *) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_SUBJECT].value.u.a.p);
      if (HAVEEXCEPTION(envp)) {
        goto err;
      }
    }
  }

  if (marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_PATTERN].value.type == MARPAESLIF_VALUE_TYPE_STRING) {
    marpaESLIFString.bytep          = (char *) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_PATTERN].value.u.s.p;
    marpaESLIFString.bytel          = marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_PATTERN].value.u.s.sizel;
    marpaESLIFString.encodingasciis = marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_PATTERN].value.u.s.encodingasciis;
    marpaESLIFString.asciis         = NULL;
    patternp = marpaESLIFJava_marpaESLIFStringToJavap(envp, &marpaESLIFString);
  }

  captureTop = (jlong) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_CAPTURE_TOP].value.u.l;
  captureLast = (jlong) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_CAPTURE_LAST].value.u.l;

  offsetVectorp = (*envp)->NewLongArray(envp, (jsize) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_OFFSET_VECTOR].value.u.r.sizel);
  if (offsetVectorp == NULL) {
    goto err;
  }
  if (marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_OFFSET_VECTOR].value.u.r.sizel > 0) {
    offsetVectorArrayp = (jlong *) malloc(sizeof(jlong) * marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_OFFSET_VECTOR].value.u.r.sizel);
    if (offsetVectorArrayp == NULL) {
      RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
    }
    for (l = 0; l < marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_OFFSET_VECTOR].value.u.r.sizel; l++) {
      offsetVectorArrayp[l] = (long) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_OFFSET_VECTOR].value.u.r.p[l].u.l;
    }
    (*envp)->SetLongArrayRegion(envp, offsetVectorp, 0, (jsize) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_OFFSET_VECTOR].value.u.r.sizel, offsetVectorArrayp);
    if (HAVEEXCEPTION(envp)) {
      goto err;
    }
  }
  
  if (marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_MARK].value.type == MARPAESLIF_VALUE_TYPE_STRING) {
    marpaESLIFString.bytep          = (char *) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_MARK].value.u.s.p;
    marpaESLIFString.bytel          = marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_MARK].value.u.s.sizel;
    marpaESLIFString.encodingasciis = marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_MARK].value.u.s.encodingasciis;
    marpaESLIFString.asciis         = NULL;
    markp = marpaESLIFJava_marpaESLIFStringToJavap(envp, &marpaESLIFString);
  }

  startMatch = (jlong) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_START_MATCH].value.u.l;
  currentPosition = (jlong) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_CURRENT_POSITION].value.u.l;
  grammarLevel = (jint) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_GRAMMAR_LEVEL].value.u.i;
  symbolId = (jint) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_SYMBOL_ID].value.u.i;

  if (marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_NEXT_ITEM].value.type == MARPAESLIF_VALUE_TYPE_STRING) {
    marpaESLIFString.bytep          = (char *) marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_NEXT_ITEM].value.u.s.p;
    marpaESLIFString.bytel          = marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_NEXT_ITEM].value.u.s.sizel;
    marpaESLIFString.encodingasciis = marpaESLIFCalloutBlockp->u.t.p[MARPAESLIFCALLOUTBLOCK_NEXT_ITEM].value.u.s.encodingasciis;
    marpaESLIFString.asciis         = NULL;
    nextItemp = marpaESLIFJava_marpaESLIFStringToJavap(envp, &marpaESLIFString);
  }

  regexEslifCalloutp = (*envp)->NewObject(envp,
                                          MARPAESLIF_ESLIFREGEXCALLOUT_CLASSP,
                                          MARPAESLIF_ESLIFREGEXCALLOUT_CLASS_init_METHODP,
                                          calloutNumberp,
                                          calloutStringp,
                                          subjectp,
                                          patternp,
                                          captureTop,
                                          captureLast,
                                          offsetVectorp,
                                          markp,
                                          startMatch,
                                          currentPosition,
                                          nextItemp,
                                          grammarLevel,
                                          symbolId);
  if (regexEslifCalloutp == NULL) {
    goto err;
  }

  /* All objects are now in regexEslifCalloutp */
  calloutNumberp     = NULL;
  calloutStringp     = NULL;
  subjectp           = NULL;
  patternp           = NULL;
  offsetVectorp      = NULL;
  markp              = NULL;
  nextItemp          = NULL;

  /* Call the regex action */
  integer = (*envp)->CallIntMethod(envp, marpaESLIFJavaRecognizerContextp->eslifRecognizerInterfacep, marpaESLIFJavaRecognizerContextp->methodp, regexEslifCalloutp);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  *marpaESLIFValueResultOutp = (int) integer;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (offsetVectorArrayp != NULL) {
    free(offsetVectorArrayp);
  }

  if (envp != NULL) {
    if (calloutNumberp != NULL) {
      (*envp)->DeleteLocalRef(envp, calloutNumberp);
    }
    if (calloutStringp != NULL) {
      (*envp)->DeleteLocalRef(envp, calloutStringp);
    }
    if (subjectp != NULL) {
      (*envp)->DeleteLocalRef(envp, subjectp);
    }
    if (patternp != NULL) {
      (*envp)->DeleteLocalRef(envp, patternp);
    }
    if (offsetVectorp != NULL) {
      (*envp)->DeleteLocalRef(envp, offsetVectorp);
    }
    if (markp != NULL) {
      (*envp)->DeleteLocalRef(envp, markp);
    }
    if (nextItemp != NULL) {
      (*envp)->DeleteLocalRef(envp, nextItemp);
    }
    if (regexEslifCalloutp != NULL) {
      (*envp)->DeleteLocalRef(envp, regexEslifCalloutp);
    }
  }

  return rcb;
}

/*****************************************************************************/
static short marpaESLIFJava_recognizerGeneratorCallbackb(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp, marpaESLIFValueResultString_t *marpaESLIFValueResultOutp)
/*****************************************************************************/
{
  static const char                 *funcs                            = "marpaESLIFJava_recognizerGeneratorCallbackb";
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp = (marpaESLIFJavaRecognizerContext_t *) userDatavp;
  jobjectArray                       list;
  JNIEnv                            *envp;
  jobject                            objectArrayp;
  jstring                            stringp;
  marpaESLIFValueResult_t            marpaESLIFValueResultTmp;
  marpaESLIFRepresentationDispose_t  disposeCallbackp;
  jobject                            actionResultp;
  short                              stringb;
  short                              rcb;

  /* Generator callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  /* By definition, contextp is of type ROW - so calling for its import will naturally create a jobjectArray */
  if (! marpaESLIFJava_recognizerGetObjectp(envp, marpaESLIFJavaRecognizerContextp, marpaESLIFRecognizerp, contextp, &objectArrayp)) {
    goto err;
  }

  /* Call the generator action */
  actionResultp = (jstring) (*envp)->CallObjectMethod(envp, marpaESLIFJavaRecognizerContextp->eslifRecognizerInterfacep, marpaESLIFJavaRecognizerContextp->methodp, objectArrayp);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  /* Fake a PTR and make that an UTF-8 string */
  marpaESLIFValueResultTmp.type                 = MARPAESLIF_VALUE_TYPE_PTR;
  marpaESLIFValueResultTmp.contextp             = MARPAESLIF_JNI_CONTEXT;
  marpaESLIFValueResultTmp.u.p.p                = (void *) actionResultp;
  marpaESLIFValueResultTmp.u.p.freeUserDatavp   = NULL;
  marpaESLIFValueResultTmp.u.p.freeCallbackp    = NULL;
  marpaESLIFValueResultTmp.u.p.shallowb         = 1;
  if (! marpaESLIFJava_representationCallbackb(NULL /* marpaESLIFJavaValueContextp */,
                                               &marpaESLIFValueResultTmp,
                                               (char **) &(marpaESLIFValueResultOutp->p),
                                               &(marpaESLIFValueResultOutp->sizel),
                                               &(marpaESLIFValueResultOutp->encodingasciis),
                                               &disposeCallbackp,
                                               &stringb)) {
    goto err;
  }
  marpaESLIFValueResultOutp->shallowb       = 0;
  marpaESLIFValueResultOutp->freeUserDatavp = NULL; /* We will resolve JNIEnv using jvm that is a safe static */
  marpaESLIFValueResultOutp->freeCallbackp  = marpaESLIFJava_genericFreeCallbackv;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static void marpaESLIFJava_genericFreeCallbackv(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFJava_genericFreeCallbackv";
  JNIEnv            *envp;

  /* Free callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    return;
  }
  switch (marpaESLIFValueResultp->type) {
  case MARPAESLIF_VALUE_TYPE_PTR:
    /* This is a global reference */
    if (marpaESLIFValueResultp->u.p.p != NULL) {
      (*envp)->DeleteGlobalRef(envp, (jobject) marpaESLIFValueResultp->u.p.p);
    }
    break;
  case MARPAESLIF_VALUE_TYPE_ARRAY:
    if (marpaESLIFValueResultp->u.a.p != NULL) {
      free(marpaESLIFValueResultp->u.a.p);
    }
    break;
  case MARPAESLIF_VALUE_TYPE_STRING:
    if (marpaESLIFValueResultp->u.s.p != NULL) {
      /* string may refer to the constant empty string */
      free(marpaESLIFValueResultp->u.s.p);
    }
    /* encoding may refer to the constant UTF8s */
    if ((marpaESLIFValueResultp->u.s.encodingasciis != NULL) && (marpaESLIFValueResultp->u.s.encodingasciis != marpaESLIFJava_UTF8s)) {
      free(marpaESLIFValueResultp->u.s.encodingasciis);
    }
    break;
  case MARPAESLIF_VALUE_TYPE_ROW:
    if (marpaESLIFValueResultp->u.r.p != NULL) {
      free(marpaESLIFValueResultp->u.r.p);
    }
    break;
  case MARPAESLIF_VALUE_TYPE_TABLE:
    if (marpaESLIFValueResultp->u.t.p != NULL) {
      free(marpaESLIFValueResultp->u.t.p);
    }
    break;
  default:
    break;
  }
}

/*****************************************************************************/
static jmethodID marpaESLIFJava_valueActionResolveri(JNIEnv *envp, marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp, char *methods, char *signatures)
/*****************************************************************************/
{
  /* We do not used an internal hash, believing it is not worth the overhead */
  static const char           *funcs             = "marpaESLIFJava_valueActionResolveri";
  marpaESLIFJavaMethodCache_t *methodCachep      = marpaESLIFJavaValueContextp->methodCachep;
  size_t                       methodCacheSizel  = marpaESLIFJavaValueContextp->methodCacheSizel;
  marpaESLIFJavaMethodCache_t *localMethodCachep;
  jmethodID                    methodp;
  size_t                       i;
  

  for (i = 0, localMethodCachep = methodCachep /* May be NULL */; i < methodCacheSizel; i++, localMethodCachep++) {
    if ((strcmp(localMethodCachep->methods, methods) == 0) && (strcmp(localMethodCachep->signatures, signatures) == 0)) {
      return localMethodCachep->methodp;
    }
  }

  /* If we are here, this mean that method with given signature was not resolved */
  if (methodCacheSizel <= 0) {
    methodCachep = (marpaESLIFJavaMethodCache_t *) malloc(sizeof(marpaESLIFJavaMethodCache_t));
    if (methodCachep == NULL) {
      RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
    }
  } else {
    methodCachep = (marpaESLIFJavaMethodCache_t *) realloc(methodCachep, sizeof(marpaESLIFJavaMethodCache_t) * (methodCacheSizel + 1));
    if (methodCachep == NULL) {
      RAISEEXCEPTIONF(envp, "realloc failure, %s", strerror(errno));
    }
  }

  marpaESLIFJavaValueContextp->methodCachep     = methodCachep;
  marpaESLIFJavaValueContextp->methodCacheSizel++;

  localMethodCachep                            = &(methodCachep[methodCacheSizel]);
  localMethodCachep->marpaESLIFJavaClassCachep = &(marpaESLIFJavaValueContextp->classCache);
  localMethodCachep->methods                   = NULL;
  localMethodCachep->signatures                = NULL;
  localMethodCachep->methodp                   = NULL;

  localMethodCachep->methods = strdup(methods);
  if (localMethodCachep->methods == NULL) {
    RAISEEXCEPTIONF(envp, "strdup failure, %s", strerror(errno));
  }

  localMethodCachep->signatures = strdup(signatures);
  if (localMethodCachep->signatures == NULL) {
    RAISEEXCEPTIONF(envp, "strdup failure, %s", strerror(errno));
  }

  methodp = (*envp)->GetMethodID(envp,
                                 marpaESLIFJavaValueContextp->classCache.classp,
                                 methods,
                                 signatures);
  if (methodp == NULL) {
    /* We want OUR exception to be shown */
    (*envp)->ExceptionClear(envp);
    RAISEEXCEPTIONF(envp,
                    "Failed to find method \"%s\" with signature \"%s\" in class \"%s\"",
                    methods,
                    signatures,
                    marpaESLIFJavaValueContextp->classCache.classs);
  }

  localMethodCachep->methodp = methodp;
  return methodp;

 err:
  return NULL;
}

/*****************************************************************************/
static jmethodID marpaESLIFJava_recognizerActionResolveri(JNIEnv *envp, marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp, char *methods, char *signatures)
/*****************************************************************************/
{
  /* We do not used an internal hash, believing it is not worth the overhead */
  static const char           *funcs             = "marpaESLIFJava_recognizerActionResolveri";
  marpaESLIFJavaMethodCache_t *methodCachep      = marpaESLIFJavaRecognizerContextp->methodCachep;
  size_t                       methodCacheSizel  = marpaESLIFJavaRecognizerContextp->methodCacheSizel;
  marpaESLIFJavaMethodCache_t *localMethodCachep;
  jmethodID                    methodp;
  size_t                       i;

  for (i = 0, localMethodCachep = methodCachep /* May be NULL */; i < methodCacheSizel; i++, localMethodCachep++) {
    if ((strcmp(localMethodCachep->methods, methods) == 0) && (strcmp(localMethodCachep->signatures, signatures) == 0)) {
      return localMethodCachep->methodp;
    }
  }

  /* If we are here, this mean that method with given signature was not resolved */
  if (methodCacheSizel <= 0) {
    methodCachep = (marpaESLIFJavaMethodCache_t *) malloc(sizeof(marpaESLIFJavaMethodCache_t));
    if (methodCachep == NULL) {
      RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
    }
  } else {
    methodCachep = (marpaESLIFJavaMethodCache_t *) realloc(methodCachep, sizeof(marpaESLIFJavaMethodCache_t) * (methodCacheSizel + 1));
    if (methodCachep == NULL) {
      RAISEEXCEPTIONF(envp, "realloc failure, %s", strerror(errno));
    }
  }

  marpaESLIFJavaRecognizerContextp->methodCachep     = methodCachep;
  marpaESLIFJavaRecognizerContextp->methodCacheSizel++;

  localMethodCachep                            = &(methodCachep[methodCacheSizel]);
  localMethodCachep->marpaESLIFJavaClassCachep = &(marpaESLIFJavaRecognizerContextp->classCache);
  localMethodCachep->methods                   = NULL;
  localMethodCachep->signatures                = NULL;
  localMethodCachep->methodp                   = NULL;

  localMethodCachep->methods = strdup(methods);
  if (localMethodCachep->methods == NULL) {
    RAISEEXCEPTIONF(envp, "strdup failure, %s", strerror(errno));
  }

  localMethodCachep->signatures = strdup(signatures);
  if (localMethodCachep->signatures == NULL) {
    RAISEEXCEPTIONF(envp, "strdup failure, %s", strerror(errno));
  }

  methodp = (*envp)->GetMethodID(envp,
                                 marpaESLIFJavaRecognizerContextp->classCache.classp,
                                 methods,
                                 signatures);
  if (methodp == NULL) {
    /* We want OUR exception to be shown */
    (*envp)->ExceptionClear(envp);
    RAISEEXCEPTIONF(envp,
                    "Failed to find method \"%s\" with signature \"%s\" in class \"%s\"",
                    methods,
                    signatures,
                    marpaESLIFJavaRecognizerContextp->classCache.classs);
  }

  localMethodCachep->methodp = methodp;
  return methodp;

 err:
  return NULL;
}

/*****************************************************************************/
static void marpaESLIFJava_valueContextFreev(JNIEnv *envp, marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp, short onStackb)
/*****************************************************************************/
{
  marpaESLIFJavaMethodCache_t *methodCachep;
  size_t                       methodCacheSizel;
  marpaESLIFJavaMethodCache_t *localMethodCachep;
  int                          i;
  jobject                      objectp;

  if (marpaESLIFJavaValueContextp != NULL) {

    if (marpaESLIFJavaValueContextp->objectStackp != NULL) {
      /* It is important to delete references in the reverse order of their creation */
      while (GENERICSTACK_USED(marpaESLIFJavaValueContextp->objectStackp) > 0) {
        /* Last indice ... */
        i = GENERICSTACK_USED(marpaESLIFJavaValueContextp->objectStackp) - 1;
        /* ... is cleared ... */
        if (GENERICSTACK_IS_PTR(marpaESLIFJavaValueContextp->objectStackp, i)) {
          objectp = (jobject) GENERICSTACK_GET_PTR(marpaESLIFJavaValueContextp->objectStackp, i);
          if (envp != NULL) {
            (*envp)->DeleteGlobalRef(envp, objectp);
          }
        }
        /* ... and becomes current used size */
        GENERICSTACK_USED(marpaESLIFJavaValueContextp->objectStackp) = i;
      }
      GENERICSTACK_RESET(marpaESLIFJavaValueContextp->objectStackp);
    }

    if (marpaESLIFJavaValueContextp->classCache.classp != NULL) {
      if (envp != NULL) {
        (*envp)->DeleteGlobalRef(envp, marpaESLIFJavaValueContextp->classCache.classp);
      }
    }
    if (marpaESLIFJavaValueContextp->classCache.classs != NULL) {
      free(marpaESLIFJavaValueContextp->classCache.classs);
    }
    if (marpaESLIFJavaValueContextp->methodCachep != NULL) {
      methodCachep      = marpaESLIFJavaValueContextp->methodCachep;
      methodCacheSizel  = marpaESLIFJavaValueContextp->methodCacheSizel;

      for (i = 0, localMethodCachep = methodCachep; i < (int) methodCacheSizel; i++, localMethodCachep++) {
        if (localMethodCachep->methods != NULL) {
          free(localMethodCachep->methods);
        }
        if (localMethodCachep->signatures != NULL) {
          free(localMethodCachep->signatures);
        }
      }
      free(marpaESLIFJavaValueContextp->methodCachep);
    }
    if (marpaESLIFJavaValueContextp->eslifGrammarp != NULL) {
      (*envp)->DeleteGlobalRef(envp, marpaESLIFJavaValueContextp->eslifGrammarp);
    }
    if (! onStackb) {
      free(marpaESLIFJavaValueContextp);
    }
  }
}

/*****************************************************************************/
static void marpaESLIFJava_recognizerContextFreev(JNIEnv *envp, marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp, short onStackb)
/*****************************************************************************/
{
  marpaESLIFJavaMethodCache_t *methodCachep;
  size_t                       methodCacheSizel;
  marpaESLIFJavaMethodCache_t *localMethodCachep;
  int                          i;
  jobject                      objectp;
  genericStack_t              *symbolStackp;

  if (marpaESLIFJavaRecognizerContextp != NULL) {

    if (marpaESLIFJavaRecognizerContextp->objectStackp != NULL) {
      /* It is important to delete references in the reverse order of their creation */
      while (GENERICSTACK_USED(marpaESLIFJavaRecognizerContextp->objectStackp) > 0) {
        /* Last indice ... */
        i = GENERICSTACK_USED(marpaESLIFJavaRecognizerContextp->objectStackp) - 1;
        /* ... is cleared ... */
        if (GENERICSTACK_IS_PTR(marpaESLIFJavaRecognizerContextp->objectStackp, i)) {
          objectp = (jobject) GENERICSTACK_GET_PTR(marpaESLIFJavaRecognizerContextp->objectStackp, i);
          if (envp != NULL) {
            (*envp)->DeleteGlobalRef(envp, objectp);
          }
        }
        /* ... and becomes current used size */
        GENERICSTACK_USED(marpaESLIFJavaRecognizerContextp->objectStackp) = i;
      }
      GENERICSTACK_RESET(marpaESLIFJavaRecognizerContextp->objectStackp);
    }

    symbolStackp = marpaESLIFJavaRecognizerContextp->symbolStackp;
    if (symbolStackp != NULL) {
      /* It is important to delete global references in the reverse order of their creation */
      while (GENERICSTACK_USED(symbolStackp) > 0) {
        i = GENERICSTACK_USED(symbolStackp) - 1;
        if (GENERICSTACK_IS_PTR(symbolStackp, i)) {
          objectp = (jobject) GENERICSTACK_POP_PTR(symbolStackp);
          if (objectp != NULL) {
            (*envp)->DeleteGlobalRef(envp, objectp);
          }
        } else {
          GENERICSTACK_USED(symbolStackp)--;
        }
      }
      GENERICSTACK_RESET(marpaESLIFJavaRecognizerContextp->symbolStackp);
    }
    if (marpaESLIFJavaRecognizerContextp->classCache.classp != NULL) {
      if (envp != NULL) {
        (*envp)->DeleteGlobalRef(envp, marpaESLIFJavaRecognizerContextp->classCache.classp);
      }
    }
    if (marpaESLIFJavaRecognizerContextp->classCache.classs != NULL) {
      free(marpaESLIFJavaRecognizerContextp->classCache.classs);
    }
    if (marpaESLIFJavaRecognizerContextp->methodCachep != NULL) {
      methodCachep      = marpaESLIFJavaRecognizerContextp->methodCachep;
      methodCacheSizel  = marpaESLIFJavaRecognizerContextp->methodCacheSizel;

      for (i = 0, localMethodCachep = methodCachep; i < (int) methodCacheSizel; i++, localMethodCachep++) {
        if (localMethodCachep->methods != NULL) {
          free(localMethodCachep->methods);
        }
        if (localMethodCachep->signatures != NULL) {
          free(localMethodCachep->signatures);
        }
      }
      free(marpaESLIFJavaRecognizerContextp->methodCachep);
    }
    if (! onStackb) {
      free(marpaESLIFJavaRecognizerContextp);
    }
  }
}

/*****************************************************************************/
static short marpaESLIFJava_valueContextInitb(JNIEnv *envp, jobject eslifValueInterfacep, jobject eslifGrammarp, marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp)
/*****************************************************************************/
{
  static const char *funcs   = "marpaESLIFJava_valueContextInitb";
  jclass             classp  = NULL;
  jstring            stringp = NULL;
  const char        *classs  = NULL;
  short              rcb;
  jboolean           isCopy;

  marpaESLIFJavaValueContextp->eslifValueInterfacep          = eslifValueInterfacep;
  marpaESLIFJavaValueContextp->eslifGrammarp                 = NULL;
  marpaESLIFJavaValueContextp->classCache.classs             = NULL;
  marpaESLIFJavaValueContextp->classCache.classp             = NULL;
  marpaESLIFJavaValueContextp->methodCachep                  = NULL;
  marpaESLIFJavaValueContextp->methodCacheSizel              = 0;
  marpaESLIFJavaValueContextp->methodp                       = 0;
  marpaESLIFJavaValueContextp->actions                       = NULL;
  marpaESLIFJavaValueContextp->objectStackp                  = &(marpaESLIFJavaValueContextp->_objectStack);

  GENERICSTACK_INIT(marpaESLIFJavaValueContextp->objectStackp);
  if (GENERICSTACK_ERROR(marpaESLIFJavaValueContextp->objectStackp)) {
    marpaESLIFJavaValueContextp->objectStackp = NULL;
    RAISEEXCEPTIONF(envp, "GENERICSTACK_INIT failure, %s", strerror(errno));
  }

  /* We want the grammar instance to stay alive until valuation is done */
  marpaESLIFJavaValueContextp->eslifGrammarp = (*envp)->NewGlobalRef(envp, eslifGrammarp);
  if (marpaESLIFJavaValueContextp->eslifGrammarp == NULL) {
    RAISEEXCEPTION(envp, "NewGlobalRef failure");
  }

  if (eslifValueInterfacep != NULL) {
    /* For run-time resolving of actions we need current jclass */
    classp = (*envp)->GetObjectClass(envp, eslifValueInterfacep);
    if (classp == NULL) {
      /* An exception was (must have been) raised */
      RAISEEXCEPTION(envp, "GetObjectClass failure");
    }

    stringp = (*envp)->CallObjectMethod(envp, classp, JAVA_LANG_CLASS_CLASS_getName_METHODP);
    if (stringp == NULL) {
      /* An exception was (must have been) raised */
      RAISEEXCEPTION(envp, "CallObjectMethod failure");
    }

    classs = (*envp)->GetStringUTFChars(envp, stringp, &isCopy);
    if (classs == NULL) {
      /* An exception was (must have been) raised */
      RAISEEXCEPTION(envp, "GetStringUTFChars failure");
    }
    marpaESLIFJavaValueContextp->classCache.classs = strdup(classs);
    if (marpaESLIFJavaValueContextp->classCache.classs == NULL) {
      RAISEEXCEPTIONF(envp, "strdup failure, %s", strerror(errno));
    }

    marpaESLIFJavaValueContextp->classCache.classp = (*envp)->NewGlobalRef(envp, classp);
    if (marpaESLIFJavaValueContextp->classCache.classp == NULL) {
      /* An exception was (must have been) raised */
      RAISEEXCEPTION(envp, "NewGlobalRef failure");
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (envp != NULL) {
    if (stringp != NULL) {
      if (classs != NULL) {
        (*envp)->ReleaseStringUTFChars(envp, stringp, classs);
      } else {
        (*envp)->DeleteLocalRef(envp, stringp);
      }
    }
    if (classp != NULL) {
      (*envp)->DeleteLocalRef(envp, classp);
    }
  }

  return rcb;
}

/*****************************************************************************/
static short marpaESLIFJava_recognizerContextInitb(JNIEnv *envp, jobject eslifRecognizerInterfacep, marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp, short haveSymbolStackb)
/*****************************************************************************/
{
  static const char *funcs   = "marpaESLIFJava_recognizerContextInitb";
  jclass             classp  = NULL;
  jstring            stringp = NULL;
  const char        *classs  = NULL;
  short              rcb;
  jboolean           isCopy;

  marpaESLIFJavaRecognizerContextp->eslifRecognizerInterfacep = eslifRecognizerInterfacep;
  marpaESLIFJavaRecognizerContextp->byteArrayp                = NULL;
  marpaESLIFJavaRecognizerContextp->encodingp                 = NULL;
  marpaESLIFJavaRecognizerContextp->symbolStackp              = haveSymbolStackb ? &(marpaESLIFJavaRecognizerContextp->_symbolStackp) : NULL;
  marpaESLIFJavaRecognizerContextp->classCache.classs         = NULL;
  marpaESLIFJavaRecognizerContextp->classCache.classp         = NULL;
  marpaESLIFJavaRecognizerContextp->methodCachep              = NULL;
  marpaESLIFJavaRecognizerContextp->methodCacheSizel          = 0;
  marpaESLIFJavaRecognizerContextp->actions                   = NULL;
  marpaESLIFJavaRecognizerContextp->methodp                   = 0;
  marpaESLIFJavaRecognizerContextp->objectStackp              = &(marpaESLIFJavaRecognizerContextp->_objectStack);

  GENERICSTACK_INIT(marpaESLIFJavaRecognizerContextp->objectStackp);
  if (GENERICSTACK_ERROR(marpaESLIFJavaRecognizerContextp->objectStackp)) {
    marpaESLIFJavaRecognizerContextp->objectStackp = NULL;
    RAISEEXCEPTIONF(envp, "GENERICSTACK_INIT failure, %s", strerror(errno));
  }

  if (haveSymbolStackb) {
    /* A call to jniParse do not need this stack */
    GENERICSTACK_INIT(marpaESLIFJavaRecognizerContextp->symbolStackp);
    if (GENERICSTACK_ERROR(marpaESLIFJavaRecognizerContextp->symbolStackp)) {
      marpaESLIFJavaRecognizerContextp->symbolStackp = NULL;
      RAISEEXCEPTIONF(envp, "GENERICSTACK_INIT failure, %s", strerror(errno));
    }
  }

  /* For run-time resolving of actions we need current jclass */
  classp = (*envp)->GetObjectClass(envp, eslifRecognizerInterfacep);
  if (classp == NULL) {
    /* An exception was (must have been) raised */
    RAISEEXCEPTION(envp, "GetObjectClass failure");
  }

  stringp = (*envp)->CallObjectMethod(envp, classp, JAVA_LANG_CLASS_CLASS_getName_METHODP);
  if (stringp == NULL) {
    /* An exception was (must have been) raised */
    RAISEEXCEPTION(envp, "CallObjectMethod failure");
  }

  classs = (*envp)->GetStringUTFChars(envp, stringp, &isCopy);
  if (classs == NULL) {
    /* An exception was (must have been) raised */
    RAISEEXCEPTION(envp, "GetStringUTFChars failure");
  }
  marpaESLIFJavaRecognizerContextp->classCache.classs = strdup(classs);
  if (marpaESLIFJavaRecognizerContextp->classCache.classs == NULL) {
    RAISEEXCEPTIONF(envp, "strdup failure, %s", strerror(errno));
  }

  marpaESLIFJavaRecognizerContextp->classCache.classp = (*envp)->NewGlobalRef(envp, classp);
  if (marpaESLIFJavaRecognizerContextp->classCache.classp == NULL) {
    /* An exception was (must have been) raised */
    RAISEEXCEPTION(envp, "NewGlobalRef failure");
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (envp != NULL) {
    if (stringp != NULL) {
      if (classs != NULL) {
        (*envp)->ReleaseStringUTFChars(envp, stringp, classs);
      } else {
        (*envp)->DeleteLocalRef(envp, stringp);
      }
    }
    if (classp != NULL) {
      (*envp)->DeleteLocalRef(envp, classp);
    }
  }

  return rcb;
}

/*****************************************************************************/
static short marpaESLIFJava_symbolContextInitb(JNIEnv *envp, marpaESLIFJavaSymbolContext_t *marpaESLIFJavaSymbolContextp)
/*****************************************************************************/
{
  static const char *funcs   = "marpaESLIFJava_symbolContextInitb";
  short              rcb;

  marpaESLIFJavaSymbolContextp->objectStackp = &(marpaESLIFJavaSymbolContextp->_objectStack);

  GENERICSTACK_INIT(marpaESLIFJavaSymbolContextp->objectStackp);
  if (GENERICSTACK_ERROR(marpaESLIFJavaSymbolContextp->objectStackp)) {
    marpaESLIFJavaSymbolContextp->objectStackp = NULL;
    RAISEEXCEPTIONF(envp, "GENERICSTACK_INIT failure, %s", strerror(errno));
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static void marpaESLIFJava_symbolContextFreev(JNIEnv *envp, marpaESLIFJavaSymbolContext_t *marpaESLIFJavaSymbolContextp)
/*****************************************************************************/
{
  int                          i;
  jobject                      objectp;

  if (marpaESLIFJavaSymbolContextp != NULL) {

    if (marpaESLIFJavaSymbolContextp->objectStackp != NULL) {
      /* It is important to delete references in the reverse order of their creation */
      while (GENERICSTACK_USED(marpaESLIFJavaSymbolContextp->objectStackp) > 0) {
        /* Last indice ... */
        i = GENERICSTACK_USED(marpaESLIFJavaSymbolContextp->objectStackp) - 1;
        /* ... is cleared ... */
        if (GENERICSTACK_IS_PTR(marpaESLIFJavaSymbolContextp->objectStackp, i)) {
          objectp = (jobject) GENERICSTACK_GET_PTR(marpaESLIFJavaSymbolContextp->objectStackp, i);
          if (envp != NULL) {
            (*envp)->DeleteGlobalRef(envp, objectp);
          }
        }
        /* ... and becomes current used size */
        GENERICSTACK_USED(marpaESLIFJavaSymbolContextp->objectStackp) = i;
      }
      GENERICSTACK_RESET(marpaESLIFJavaSymbolContextp->objectStackp);
    }

    free(marpaESLIFJavaSymbolContextp);
  }
}

/*****************************************************************************/
static void marpaESLIFJava_representationCallbackDisposev(void *userDatavp, char *inputcp, size_t inputl, char *encodingasciis)
/*****************************************************************************/
{
  if (inputcp != NULL) {
    free(inputcp);
  }
}

/*****************************************************************************/
static short marpaESLIFJava_representationCallbackb(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp, char **encodingasciisp, marpaESLIFRepresentationDispose_t *disposeCallbackpp, short *stringbp)
/*****************************************************************************/
{
  static const char            *funcs = "marpaESLIFJava_representationCallbackb";
  JNIEnv                       *envp;
  marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp = (marpaESLIFJavaValueContext_t *) userDatavp;
  jstring                       stringp = NULL;
  jclass                        classp = NULL;
  jobject                       objectp;
  jmethodID                     methodp;
  jsize                         UTF16NbCodeUnit;
  size_t                        UTF8NbByte;
  jbyteArray                    UTF8ByteArray = NULL;
  jbyte                        *UTF8Bytes = NULL;
  char                         *utf8s = NULL;
  size_t                        utf8l = 0;
  short                         rcb;
  jboolean                      isCopy;

  /* Representation callack is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  /* We always push a PTR */
  if (marpaESLIFValueResultp->type != MARPAESLIF_VALUE_TYPE_PTR) {
    RAISEEXCEPTIONF(envp, "User-defined value type is not MARPAESLIF_VALUE_TYPE_PTR but %d", marpaESLIFValueResultp->type);
  }
  objectp = (jobject) marpaESLIFValueResultp->u.p.p;

  /* We need to resolve the "toString" method - that every object have in Java */
  classp = (*envp)->GetObjectClass(envp, objectp);
  if (classp == NULL) {
    /* We want OUR exception to be raised */
    (*envp)->ExceptionClear(envp);
    RAISEEXCEPTION(envp, "Failed to find object class");
  }
  methodp = (*envp)->GetMethodID(envp, classp, "toString", "()Ljava/lang/String;");
  if (methodp == NULL) {
    /* We want OUR exception to be raised */
    (*envp)->ExceptionClear(envp);
    RAISEEXCEPTION(envp, "Failed to find toString method");
  }

  /* Call the toString method */
  stringp = (*envp)->CallObjectMethod(envp, objectp, methodp);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  /* It is not illegal to have a NULL stringp */
  if (stringp != NULL) {
    /* The UTF-16 string is always injected as an UTF-8 thingy as per the documentation */
    UTF16NbCodeUnit = (*envp)->GetStringLength(envp, stringp);
    /* If zero character, we do nothing - nothing will be appended by default */
    if (UTF16NbCodeUnit > 0) {
      /* Convert to UTF-8: a string in java is ALWAYS in native charset */
      UTF8ByteArray = (jbyteArray) (*envp)->CallObjectMethod(envp, stringp, JAVA_LANG_STRING_CLASS_getBytes_String_METHODP, marpaESLIF_UTF8p);
      if (UTF8ByteArray == NULL) {
        RAISEEXCEPTIONF(envp, "String.getBytes(..., \"%s\")", "UTF-8");
      }
      UTF8NbByte = (*envp)->GetArrayLength(envp, UTF8ByteArray);
      if (UTF8NbByte <= 0) {
        RAISEEXCEPTIONF(envp, "UTF-8 string length is %ld bytes long while UTF-16 string length is %ld code units ?", (unsigned long) UTF8NbByte, (unsigned long) UTF16NbCodeUnit);
      }
      UTF8Bytes = (*envp)->GetByteArrayElements(envp, UTF8ByteArray, &isCopy);

      utf8s = (char *) malloc((size_t) (UTF8NbByte + 1)); /* Hiden NUL byte */
      if (utf8s == NULL) {
        RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
      }
      memcpy(utf8s, UTF8Bytes, UTF8NbByte);
      utf8l  = (size_t) UTF8NbByte;
      utf8s[utf8l] = '\0';
    } else {
      utf8s = (char *) malloc(1); /* Hiden NUL byte */
      if (utf8s == NULL) {
        RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
      }
      utf8s[0] = '\0';
      utf8l  = 0;
    }
  } else {
    utf8s = (char *) malloc(1); /* Hiden NUL byte */
    if (utf8s == NULL) {
      RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
    }
    utf8s[0] = '\0';
    utf8l  = 0;
  }

  /* In any case, encoding must be set */
  *inputcpp          = utf8s;
  *inputlp           = utf8l;
  *encodingasciisp   = (char *) marpaESLIFJava_UTF8s;
  *disposeCallbackpp = marpaESLIFJava_representationCallbackDisposev;

  /* We overwrite the string context only when we know we derive from Math::BigDecimal */
  if ((*envp)->IsInstanceOf(envp, objectp, JAVA_MATH_BIGDECIMAL_CLASSP) == JNI_TRUE) {
    *stringbp = 0;
  }

  rcb = 1;
  goto done;
  
 err:
  if (utf8s != NULL) {
    free(utf8s);
  }
  rcb = 0;

 done:
  if (classp != NULL) {
    (*envp)->DeleteLocalRef(envp, classp);
  }
  if (UTF8ByteArray != NULL) {
    if (UTF8Bytes != NULL) {
      (*envp)->ReleaseByteArrayElements(envp, UTF8ByteArray, UTF8Bytes, JNI_ABORT);
    }
    (*envp)->DeleteLocalRef(envp, UTF8ByteArray);
  }

  return rcb;
}

/*****************************************************************************/
static jobject marpaESLIFJava_grammarPropertiesp(JNIEnv *envp, marpaESLIFGrammarProperty_t *grammarPropertyp)
/*****************************************************************************/
{
  static const char           *funcs = "marpaESLIFJava_grammarPropertiesp";
  jobject                      propertiesp = NULL;
  jint                         level;
  jint                         maxLevel;
  jstring                      description;
  jboolean                     latm;
  jstring                      defaultSymbolAction;
  jstring                      defaultRuleAction;
  jstring                      defaultEventAction;
  jstring                      defaultRegexAction;
  jint                         startId;
  jint                         discardId;
  jintArray                    symbolIds;
  jintArray                    ruleIds;
  jint                        *symbolIdsIntp = NULL;
  jint                        *ruleIdsIntp = NULL;
  jstring                      defaultEncoding;
  jstring                      fallbackEncoding;
  size_t                       i;

  level               = (jint) grammarPropertyp->leveli;
  maxLevel            = (jint) grammarPropertyp->maxLeveli;
  description         = marpaESLIFJava_marpaESLIFStringToJavap(envp, grammarPropertyp->descp);
  latm                = grammarPropertyp->latmb ? JNI_TRUE : JNI_FALSE;
  defaultSymbolAction = marpaESLIFJava_marpaESLIFActionToJavap(envp, grammarPropertyp->defaultSymbolActionp);
  defaultRuleAction   = marpaESLIFJava_marpaESLIFActionToJavap(envp, grammarPropertyp->defaultRuleActionp);
  defaultEventAction  = marpaESLIFJava_marpaESLIFActionToJavap(envp, grammarPropertyp->defaultEventActionp);
  defaultRegexAction  = marpaESLIFJava_marpaESLIFActionToJavap(envp, grammarPropertyp->defaultRegexActionp);
  startId             = (jint) grammarPropertyp->starti;
  discardId           = (jint) grammarPropertyp->discardi;
  defaultEncoding     = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, grammarPropertyp->defaultEncodings);
  fallbackEncoding    = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, grammarPropertyp->fallbackEncodings);

  if (grammarPropertyp->nsymboll <= 0) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_grammarproperty_currentb returned no symbol");
  }
  symbolIdsIntp = (jint *) malloc(sizeof(jint) * grammarPropertyp->nsymboll);
  if (symbolIdsIntp == NULL) {
    RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
  }
  for (i = 0; i < grammarPropertyp->nsymboll; i++) {
    symbolIdsIntp[i] = (jint) grammarPropertyp->symbolip[i];
  }

  symbolIds = (*envp)->NewIntArray(envp, (jsize) grammarPropertyp->nsymboll);
  if (symbolIds == NULL) {
    RAISEEXCEPTION(envp, "NewIntArray failure");
  }

  (*envp)->SetIntArrayRegion(envp, symbolIds, 0, (jsize) grammarPropertyp->nsymboll, symbolIdsIntp);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  if (grammarPropertyp->nrulel <= 0) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_grammarproperty_currentb returned no rule");
  }
  ruleIdsIntp = (jint *) malloc(sizeof(jint) * grammarPropertyp->nrulel);
  if (ruleIdsIntp == NULL) {
    RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
  }
  for (i = 0; i < grammarPropertyp->nrulel; i++) {
    ruleIdsIntp[i] = (jint) grammarPropertyp->ruleip[i];
  }

  ruleIds = (*envp)->NewIntArray(envp, (jsize) grammarPropertyp->nrulel);
  if (ruleIds == NULL) {
    RAISEEXCEPTION(envp, "NewIntArray failure");
  }

  (*envp)->SetIntArrayRegion(envp, ruleIds, 0, (jsize) grammarPropertyp->nrulel, ruleIdsIntp);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  propertiesp = (*envp)->NewObject(envp,
                                   MARPAESLIF_ESLIFGRAMMARPROPERTIES_CLASSP,
                                   MARPAESLIF_ESLIFGRAMMARPROPERTIES_CLASS_init_METHODP,
                                   level,
                                   maxLevel,
                                   description,
                                   latm,
                                   defaultSymbolAction,
                                   defaultRuleAction,
                                   defaultEventAction,
                                   defaultRegexAction,
                                   startId,
                                   discardId,
                                   symbolIds,
                                   ruleIds,
                                   defaultEncoding,
                                   fallbackEncoding
                                   );

 err: /* err and done share the same code */
  /* Java will immediately see the exception if there is one */
  if (symbolIdsIntp != NULL) {
    free(symbolIdsIntp);
  }
  if (ruleIdsIntp != NULL) {
    free(ruleIdsIntp);
  }
  return propertiesp;
}

/*****************************************************************************/
static jobject marpaESLIFJava_rulePropertiesp(JNIEnv *envp, marpaESLIFRuleProperty_t *rulePropertyp)
/*****************************************************************************/
{
  static const char           *funcs             = "marpaESLIFJava_rulePropertiesp";
  jobject                      propertiesp       = NULL;
  jintArray                    rhsIds            = NULL;
  jbooleanArray                skipIndices       = NULL;
  jint                        *rhsIdsIntp        = NULL;
  jboolean                    *skipIndicesShortp = NULL;
  jint                         id;
  jstring                      description;
  jstring                      show;
  jint                         lhsId;
  jint                         separatorId;
  jint                         exceptionId;
  jstring                      action;
  jstring                      discardEvent;
  jboolean                     discardEventInitialState;
  jint                         rank;
  jboolean                     nullRanksHigh;
  jboolean                     sequence;
  jboolean                     proper;
  jint                         minimum;
  jint                         propertyBitSet;
  jboolean                     hideseparator;
  size_t                       i;

  id          = (jint) rulePropertyp->idi;
  description = marpaESLIFJava_marpaESLIFStringToJavap(envp, rulePropertyp->descp);
  show        = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, rulePropertyp->asciishows);
  lhsId       = (jint) rulePropertyp->lhsi;
  separatorId = (jint) rulePropertyp->separatori;

  if (rulePropertyp->nrhsl > 0) {
    rhsIdsIntp = (jint *) malloc(sizeof(jint) * rulePropertyp->nrhsl);
    if (rhsIdsIntp == NULL) {
      RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
    }
    for (i = 0; i < rulePropertyp->nrhsl; i++) {
      rhsIdsIntp[i] = (jint) rulePropertyp->rhsip[i];
    }
    rhsIds = (*envp)->NewIntArray(envp, (jsize) rulePropertyp->nrhsl);
    if (rhsIds == NULL) {
      RAISEEXCEPTION(envp, "NewIntArray failure");
    }
    (*envp)->SetIntArrayRegion(envp, rhsIds, 0, (jsize) rulePropertyp->nrhsl, rhsIdsIntp);
    if (HAVEEXCEPTION(envp)) {
      goto err;
    }
  }

  if (rulePropertyp->skipbp != NULL) {
    skipIndicesShortp = (jboolean *) malloc(sizeof(jboolean) * rulePropertyp->nrhsl);
    if (skipIndicesShortp == NULL) {
      RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
    }
    for (i = 0; i < rulePropertyp->nrhsl; i++) {
      skipIndicesShortp[i] = rulePropertyp->skipbp[i] ? JNI_TRUE : JNI_FALSE;
    }
    skipIndices = (*envp)->NewBooleanArray(envp, (jsize) rulePropertyp->nrhsl);
    if (skipIndices == NULL) {
      RAISEEXCEPTION(envp, "NewBooleanArray failure");
    }
    (*envp)->SetBooleanArrayRegion(envp, skipIndices, 0, (jsize) rulePropertyp->nrhsl, skipIndicesShortp);
    if (HAVEEXCEPTION(envp)) {
      goto err;
    }
  }

  exceptionId = (jint) rulePropertyp->exceptioni;
  action      = marpaESLIFJava_marpaESLIFActionToJavap(envp, rulePropertyp->actionp);

  if (rulePropertyp->discardEvents == NULL) {
    discardEvent = NULL;
  } else {
    discardEvent = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, rulePropertyp->discardEvents);
  }

  discardEventInitialState = rulePropertyp->discardEventb ? JNI_TRUE : JNI_FALSE;
  rank                     = (jint) rulePropertyp->ranki;
  nullRanksHigh            = rulePropertyp->nullRanksHighb ? JNI_TRUE : JNI_FALSE;
  sequence                 = rulePropertyp->sequenceb ? JNI_TRUE : JNI_FALSE;
  proper                   = rulePropertyp->properb ? JNI_TRUE : JNI_FALSE;
  minimum                  = (jint) rulePropertyp->minimumi;
  propertyBitSet           = (jint) rulePropertyp->propertyBitSet;
  hideseparator            = rulePropertyp->hideseparatorb ? JNI_TRUE : JNI_FALSE;

  propertiesp = (*envp)->NewObject(envp,
                                   MARPAESLIF_ESLIFGRAMMARRULEPROPERTIES_CLASSP,
                                   MARPAESLIF_ESLIFGRAMMARRULEPROPERTIES_CLASS_init_METHODP,
                                   id,
                                   description,
                                   show,
                                   lhsId,
                                   separatorId,
                                   rhsIds,
                                   skipIndices,
                                   exceptionId,
                                   action,
                                   discardEvent,
                                   discardEventInitialState,
                                   rank,
                                   nullRanksHigh,
                                   sequence,
                                   proper,
                                   minimum,
                                   propertyBitSet,
                                   hideseparator
                                   );

 err: /* err and done share the same code */
  /* Java will immediately see the exception if there is one */
  if (rhsIdsIntp != NULL) {
    free(rhsIdsIntp);
  }
  if (skipIndicesShortp != NULL) {
    free(skipIndicesShortp);
  }
  return propertiesp;
}

/*****************************************************************************/
static jobject marpaESLIFJava_symbolPropertiesp(JNIEnv *envp, marpaESLIFSymbolProperty_t *symbolPropertyp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFJava_symbolPropertiesp";
  jobject            propertiesp = NULL;
  jobject            type;
  jboolean           start;
  jboolean           discard;
  jboolean           discardRhs;
  jboolean           lhs;
  jboolean           top;
  jint               id;
  jstring            description;
  jstring            eventBefore;
  jboolean           eventBeforeInitialState;
  jstring            eventAfter;
  jboolean           eventAfterInitialState;
  jstring            eventPredicted;
  jboolean           eventPredictedInitialState;
  jstring            eventNulled;
  jboolean           eventNulledInitialState;
  jstring            eventCompleted;
  jboolean           eventCompletedInitialState;
  jstring            discardEvent;
  jboolean           discardEventInitialState;
  jint               lookupResolvedLeveli;
  jint               priority;
  jboolean           verbose;
  jstring            nullableAction;
  jint               propertyBitSet;
  jint               eventBitSet;
  jstring            symbolAction;
  jstring            ifAction;

  type = (*envp)->CallStaticObjectMethod(envp, MARPAESLIF_ESLIFSYMBOLTYPE_CLASSP, MARPAESLIF_ESLIFSYMBOLTYPE_CLASS_get_METHODP, symbolPropertyp->type);
  if (type == NULL) {
    RAISEEXCEPTION(envp, "CallStaticObjectMethod failure");
  }


  start                      = symbolPropertyp->startb ? JNI_TRUE : JNI_FALSE;
  discard                    = symbolPropertyp->discardb ? JNI_TRUE : JNI_FALSE;
  discardRhs                 = symbolPropertyp->discardRhsb ? JNI_TRUE : JNI_FALSE;
  lhs                        = symbolPropertyp->lhsb ? JNI_TRUE : JNI_FALSE;
  top                        = symbolPropertyp->topb ? JNI_TRUE : JNI_FALSE;
  id                         = (jint) symbolPropertyp->idi;
  description                = marpaESLIFJava_marpaESLIFStringToJavap(envp, symbolPropertyp->descp);
  eventBefore                = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, symbolPropertyp->eventBefores);
  eventBeforeInitialState    = symbolPropertyp->eventBeforeb ? JNI_TRUE : JNI_FALSE;
  eventAfter                 = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, symbolPropertyp->eventAfters);
  eventAfterInitialState     = symbolPropertyp->eventAfterb ? JNI_TRUE : JNI_FALSE;
  eventPredicted             = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, symbolPropertyp->eventPredicteds);
  eventPredictedInitialState = symbolPropertyp->eventPredictedb ? JNI_TRUE : JNI_FALSE;
  eventNulled                = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, symbolPropertyp->eventNulleds);
  eventNulledInitialState    = symbolPropertyp->eventNulledb ? JNI_TRUE : JNI_FALSE;
  eventCompleted             = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, symbolPropertyp->eventCompleteds);
  eventCompletedInitialState = symbolPropertyp->eventCompletedb ? JNI_TRUE : JNI_FALSE;
  discardEvent               = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, symbolPropertyp->discardEvents);
  discardEventInitialState   = symbolPropertyp->discardEventb ? JNI_TRUE : JNI_FALSE;
  lookupResolvedLeveli       = (jint) symbolPropertyp->lookupResolvedLeveli;
  priority                   = (jint) symbolPropertyp->priorityi;
  verbose                    = symbolPropertyp->verboseb ? JNI_TRUE : JNI_FALSE;
  nullableAction             = marpaESLIFJava_marpaESLIFActionToJavap(envp, symbolPropertyp->nullableActionp);
  propertyBitSet             = (jint) symbolPropertyp->propertyBitSet;
  eventBitSet                = (jint) symbolPropertyp->eventBitSet;
  symbolAction               = marpaESLIFJava_marpaESLIFActionToJavap(envp, symbolPropertyp->symbolActionp);
  ifAction                   = marpaESLIFJava_marpaESLIFActionToJavap(envp, symbolPropertyp->ifActionp);

  propertiesp = (*envp)->NewObject(envp,
                                   MARPAESLIF_ESLIFGRAMMARSYMBOLPROPERTIES_CLASSP,
                                   MARPAESLIF_ESLIFGRAMMARSYMBOLPROPERTIES_CLASS_init_METHODP,
                                   type,
                                   start,
                                   discard,
                                   discardRhs,
                                   lhs,
                                   top,
                                   id,
                                   description,
                                   eventBefore,
                                   eventBeforeInitialState,
                                   eventAfter,
                                   eventAfterInitialState,
                                   eventPredicted,
                                   eventPredictedInitialState,
                                   eventNulled,
                                   eventNulledInitialState,
                                   eventCompleted,
                                   eventCompletedInitialState,
                                   discardEvent,
                                   discardEventInitialState,
                                   lookupResolvedLeveli,
                                   priority,
                                   nullableAction,
                                   propertyBitSet,
                                   eventBitSet,
                                   symbolAction,
                                   ifAction,
                                   verbose
                                   );

 err: /* err and done share the same code */
  return propertiesp;
}

/*****************************************************************************/
static short marpaESLIFJava_valueGetObjectp(JNIEnv *envp, marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp, marpaESLIFValue_t *marpaESLIFValuep, int stackindicei, marpaESLIFValueResult_t *marpaESLIFValueResultSymbolp, jobject *objectpp)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFJava_valueGetObjectp";
  marpaESLIFValueResult_t *marpaESLIFValueResultp;
  jobject                  objectp;
  short                    rcb;

  if (marpaESLIFValueResultSymbolp != NULL) {
    marpaESLIFValueResultp = marpaESLIFValueResultSymbolp;
  } else {
    marpaESLIFValueResultp = marpaESLIFValue_stack_getp(marpaESLIFValuep, stackindicei);
    if (marpaESLIFValueResultp == NULL) {
      RAISEEXCEPTIONF(envp, "marpaESLIFValueResultp is NULL at stack indice %d", stackindicei);
    }
  }

  if (! marpaESLIFValue_importb(marpaESLIFValuep, marpaESLIFValueResultp)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFValue_importb failure, %s", strerror(errno));
  }

  if (GENERICSTACK_USED(marpaESLIFJavaValueContextp->objectStackp) != 1) {
    RAISEEXCEPTIONF(envp, "Internal value stack is %d instead of 1", GENERICSTACK_USED(marpaESLIFJavaValueContextp->objectStackp));
  }

  objectp = (jobject) GENERICSTACK_POP_PTR(marpaESLIFJavaValueContextp->objectStackp);
  if (objectpp != NULL) {
    *objectpp = objectp;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short marpaESLIFJava_recognizerGetObjectp(JNIEnv *envp, marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFValueResultp, jobject *objectpp)
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFJava_recognizerGetObjectp";
  jobject                  objectp;
  short                    rcb;

  if (! marpaESLIFRecognizer_importb(marpaESLIFRecognizerp, marpaESLIFValueResultp)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFRecognizer_importb failure, %s", strerror(errno));
  }

  if (GENERICSTACK_USED(marpaESLIFJavaRecognizerContextp->objectStackp) != 1) {
    RAISEEXCEPTIONF(envp, "Internal value stack is %d instead of 1", GENERICSTACK_USED(marpaESLIFJavaRecognizerContextp->objectStackp));
  }

  objectp = (jobject) GENERICSTACK_POP_PTR(marpaESLIFJavaRecognizerContextp->objectStackp);
  if (objectpp != NULL) {
    *objectpp = objectp;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static short marpaESLIFJava_valueImportb(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  marpaESLIFJavaValueContext_t *marpaESLIFJavaValueContextp = (marpaESLIFJavaValueContext_t *) userDatavp;
  marpaESLIF_t                 *marpaESLIFp                 = marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep)));

  return marpaESLIFJava_importb(marpaESLIFp, marpaESLIFJavaValueContextp->objectStackp, marpaESLIFValueResultp);
}

/*****************************************************************************/
static short marpaESLIFJava_recognizerImportb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp = (marpaESLIFJavaRecognizerContext_t *) userDatavp;
  marpaESLIF_t                      *marpaESLIFp                      = marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFRecognizerp));

  return marpaESLIFJava_importb(marpaESLIFp, marpaESLIFJavaRecognizerContextp->objectStackp, marpaESLIFValueResultp);
}

/*****************************************************************************/
static short marpaESLIFJava_symbolImportb(marpaESLIFSymbol_t *marpaESLIFSymbolp, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
/* No need of userDatavp here */
/*****************************************************************************/
{
  marpaESLIFJavaSymbolContext_t *marpaESLIFJavaSymbolContextp = (marpaESLIFJavaSymbolContext_t *) userDatavp;
  marpaESLIF_t *marpaESLIFp = marpaESLIFSymbol_eslifp(marpaESLIFSymbolp);

  return marpaESLIFJava_importb(marpaESLIFp, marpaESLIFJavaSymbolContextp->objectStackp, marpaESLIFValueResultp);
}

/*****************************************************************************/
static short marpaESLIFJava_importb(marpaESLIF_t *marpaESLIFp, genericStack_t *objectStackp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  static const char *funcs      = "marpaESLIFJava_importb";
  jbyteArray         byteArrayp = NULL;
  jstring            encodingp  = NULL;
  short              rcb;
  jobject            objectp;
  jobjectArray       objectArrayp;
  jboolean           jb;
  size_t             i;
  size_t             j;
  jobject            objectHashMapp;
  jobject            keyp;
  jobject            valuep;
  JNIEnv            *envp;

  /* Import callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    RAISEEXCEPTION(envp, "Failed to get environment");
  }

  /*
    marpaESLIF Type                    C type    C nb_bits      Java Type
    
    MARPAESLIF_VALUE_TYPE_UNDEF
    MARPAESLIF_VALUE_TYPE_CHAR         char      CHAR_BIT       java.lang.Byte or java.lang.Short or java.lang.Integer or java.lang.Long or java.math.BigInteger
    MARPAESLIF_VALUE_TYPE_SHORT        short     >= 16          java.lang.Byte or java.lang.Short or java.lang.Integer or java.lang.Long or java.math.BigInteger
    MARPAESLIF_VALUE_TYPE_INT          int       >= 16          java.lang.Byte or java.lang.Short or java.lang.Integer or java.lang.Long or java.math.BigInteger
    MARPAESLIF_VALUE_TYPE_LONG         long      >= 32          java.lang.Byte or java.lang.Short or java.lang.Integer or java.lang.Long or java.math.BigInteger
    MARPAESLIF_VALUE_TYPE_FLOAT        float     depends        java.lang.Float (includes +/-Inf and Nan) or java.lang.Double or java.math.BigDecimal
    MARPAESLIF_VALUE_TYPE_DOUBLE       double    depends        java.lang.Float or java.lang.Double or java.math.BigDecimal
    MARPAESLIF_VALUE_TYPE_PTR          char*                    java.lang.Object or java.lang.Long
    MARPAESLIF_VALUE_TYPE_ARRAY                                 byte[]
    MARPAESLIF_VALUE_TYPE_BOOL                                  java.lang.Bool
    MARPAESLIF_VALUE_TYPE_STRING                                java.lang.String
    MARPAESLIF_VALUE_TYPE_ROW                                   object[]
    MARPAESLIF_VALUE_TYPE_TABLE                                 java.util.HashMap
    MARPAESLIF_VALUE_TYPE_LONG_DOUBLE                           java.lang.Float or java.lang.Double or java.math.BigDecimal
    MARPAESLIF_VALUE_TYPE_LONG_LONG    long long >= 64          java.lang.Byte or java.lang.Short or java.lang.Integer or java.lang.Long or java.math.BigInteger
    
  */

  switch (marpaESLIFValueResultp->type) {
  case MARPAESLIF_VALUE_TYPE_UNDEF:
    /* fprintf(stderr, "==> %s: import null\n", funcs); fflush(stdout); fflush(stderr); */
    objectp = NULL;
    GENERICSTACK_PUSH_PTR(objectStackp, objectp);
    if (GENERICSTACK_ERROR(objectStackp)) {
      RAISEEXCEPTIONF(envp, "objectStackp push failure, %s", strerror(errno));
    }
    break;
  case MARPAESLIF_VALUE_TYPE_CHAR:
    /* fprintf(stderr, "==> %s: import CHAR 0x%lx\n", funcs, (unsigned long) marpaESLIFValueResultp->u.c); fflush(stdout); fflush(stderr); */
    MARPAESLIFJAVA_IMPORT_NUMBER_NOT_DECIMAL(envp, objectp, "%c", char, marpaESLIFValueResultp->u.c);
    MARPAESLIFJAVA_PUSH_PTR(objectStackp, objectp);
    break;
  case MARPAESLIF_VALUE_TYPE_SHORT:
    /* fprintf(stderr, "==> %s: import SHORT\n", funcs); fflush(stdout); fflush(stderr); */
    MARPAESLIFJAVA_IMPORT_NUMBER_NOT_DECIMAL(envp, objectp, "%d", int, marpaESLIFValueResultp->u.b);
    MARPAESLIFJAVA_PUSH_PTR(objectStackp, objectp);
    break;
  case MARPAESLIF_VALUE_TYPE_INT:
    /* fprintf(stderr, "==> %s: import INT\n", funcs); fflush(stdout); fflush(stderr); */
    MARPAESLIFJAVA_IMPORT_NUMBER_NOT_DECIMAL(envp, objectp, "%d", int, marpaESLIFValueResultp->u.i);
    MARPAESLIFJAVA_PUSH_PTR(objectStackp, objectp);
    break;
  case MARPAESLIF_VALUE_TYPE_LONG:
    /* fprintf(stderr, "==> %s: import LONG\n", funcs); fflush(stdout); fflush(stderr); */
    MARPAESLIFJAVA_IMPORT_NUMBER_NOT_DECIMAL(envp, objectp, "%ld", long, marpaESLIFValueResultp->u.l);
    MARPAESLIFJAVA_PUSH_PTR(objectStackp, objectp);
    break;
  case MARPAESLIF_VALUE_TYPE_FLOAT:
    /* If value is +/-Infinity or NaN this will be handle natively via jfloat */
    /* fprintf(stderr, "==> %s: MARPAESLIF_VALUE_TYPE_FLOAT: %f\n", funcs, (double) marpaESLIFValueResultp->u.f); fflush(stdout); fflush(stderr); */
    if (marpaESLIFValueResult_isinfb(marpaESLIFp, marpaESLIFValueResultp)) {
      if (marpaESLIFValueResultp->u.f >= 0) {
        MARPAESLIFJAVA_IMPORT_POSITIVE_INFINITY(marpaESLIFp, envp, objectp);
      } else {
        MARPAESLIFJAVA_IMPORT_NEGATIVE_INFINITY(marpaESLIFp, envp, objectp);
      }
    } else if (marpaESLIFValueResult_isnanb(marpaESLIFp, marpaESLIFValueResultp)) {
      MARPAESLIFJAVA_IMPORT_NAN(marpaESLIFp, envp, objectp);
    } else {
      MARPAESLIFJAVA_IMPORT_NUMBER_DECIMAL(marpaESLIFp, envp, objectp, double, marpaESLIFValueResultp->u.f, marpaESLIF_ftos);
    }
    MARPAESLIFJAVA_PUSH_PTR(objectStackp, objectp);
    break;
  case MARPAESLIF_VALUE_TYPE_DOUBLE:
    /* If value is +/-Infinity or NaN this will be handle natively via jdouble */
    /* fprintf(stderr, "==> %s: MARPAESLIF_VALUE_TYPE_DOUBLE: %f\n", funcs, (double) marpaESLIFValueResultp->u.d); fflush(stdout); fflush(stderr); */
    if (marpaESLIFValueResult_isinfb(marpaESLIFp, marpaESLIFValueResultp)) {
      if (marpaESLIFValueResultp->u.d >= 0) {
        MARPAESLIFJAVA_IMPORT_POSITIVE_INFINITY(marpaESLIFp, envp, objectp);
      } else {
        MARPAESLIFJAVA_IMPORT_NEGATIVE_INFINITY(marpaESLIFp, envp, objectp);
      }
    } else if (marpaESLIFValueResult_isnanb(marpaESLIFp, marpaESLIFValueResultp)) {
      MARPAESLIFJAVA_IMPORT_NAN(marpaESLIFp, envp, objectp);
    } else {
      MARPAESLIFJAVA_IMPORT_NUMBER_DECIMAL(marpaESLIFp, envp, objectp, double, marpaESLIFValueResultp->u.d, marpaESLIF_dtos);
    }
    MARPAESLIFJAVA_PUSH_PTR(objectStackp, objectp);
    break;
  case MARPAESLIF_VALUE_TYPE_PTR:
    /* fprintf(stderr, "==> %s: import PTR\n", funcs); fflush(stdout); fflush(stderr); */
    if (marpaESLIFValueResultp->contextp == MARPAESLIF_JNI_CONTEXT) {
      /* This is an object that we pushed */
      /* fprintf(stderr, "==> %s: import Object '%p'\n", funcs, (void *) marpaESLIFValueResultp->u.p.p); fflush(stdout); fflush(stderr); */
      objectp = (*envp)->NewLocalRef(envp, (jobject) marpaESLIFValueResultp->u.p.p);
    } else {
      /* This is a pointer coming from another source - we express that as a jlong */
      /* fprintf(stderr, "==> %s: import ptr as Long '%Ld'\n", funcs, (long) marpaESLIFValueResultp->u.p.p); fflush(stdout); fflush(stderr); */
      if (JAVA_LANG_LONG_CLASS_valueOf_METHODP != NULL) {
        objectp = (*envp)->CallStaticObjectMethod(envp, JAVA_LANG_LONG_CLASSP, JAVA_LANG_LONG_CLASS_valueOf_METHODP, PTR_TO_JLONG(marpaESLIFValueResultp->u.p.p));
      } else {
        objectp = (*envp)->NewObject(envp, JAVA_LANG_LONG_CLASSP, JAVA_LANG_LONG_CLASS_init_METHODP, PTR_TO_JLONG(marpaESLIFValueResultp->u.p.p));
      }
    }
    if (objectp == NULL) {
      /* An exception was raised by the JVM */
      goto err;
    }
    MARPAESLIFJAVA_PUSH_PTR(objectStackp, objectp);
    break;
  case MARPAESLIF_VALUE_TYPE_ARRAY:
    /* fprintf(stderr, "==> %s: import ARRAY\n", funcs); fflush(stdout); fflush(stderr); */
    byteArrayp = (*envp)->NewByteArray(envp, (jsize) marpaESLIFValueResultp->u.a.sizel);
    if (byteArrayp == NULL) {
      goto err;
    }
    if (marpaESLIFValueResultp->u.a.sizel > 0) {
      /* Don't know if it is legal to call SetByteArrayRegion with a zero size -; */
      (*envp)->SetByteArrayRegion(envp, byteArrayp, 0, (jsize) marpaESLIFValueResultp->u.a.sizel, (jbyte *) marpaESLIFValueResultp->u.a.p);
      if (HAVEEXCEPTION(envp)) {
        goto err;
      }
    }
    objectp = (jobject) byteArrayp;
    MARPAESLIFJAVA_PUSH_PTR(objectStackp, objectp);
    byteArrayp = NULL; /* byteArrayp is in marpaESLIFJavaValueContextp->objectp */
    break;
  case MARPAESLIF_VALUE_TYPE_BOOL:
    /* fprintf(stderr, "==> %s: import BOOL\n", funcs); fflush(stdout); fflush(stderr); */
    jb = (marpaESLIFValueResultp->u.y == MARPAESLIFVALUERESULTBOOL_FALSE) ? JNI_FALSE : JNI_TRUE;
    if (JAVA_LANG_BOOLEAN_CLASS_valueOf_METHODP != NULL) {
      objectp = (*envp)->CallStaticObjectMethod(envp, JAVA_LANG_BOOLEAN_CLASSP, JAVA_LANG_BOOLEAN_CLASS_valueOf_METHODP, jb);
    } else {
      objectp = (*envp)->NewObject(envp, JAVA_LANG_BOOLEAN_CLASSP, JAVA_LANG_BOOLEAN_CLASS_init_METHODP, jb);
    }
    if (objectp == NULL) {
      /* An exception was raised by the JVM */
      goto err;
    }
    MARPAESLIFJAVA_PUSH_PTR(objectStackp, objectp);
    break;
  case MARPAESLIF_VALUE_TYPE_STRING:
    /* fprintf(stderr, "==> %s: import STRING\n", funcs); fflush(stdout); fflush(stderr); */
    encodingp = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, marpaESLIFValueResultp->u.s.encodingasciis);
    if (encodingp == NULL) {
      /* We want OUR exception to be raised */
      RAISEEXCEPTION(envp, "marpaESLIFJava_marpaESLIFASCIIToJavap failure");
    }
    byteArrayp = (*envp)->NewByteArray(envp, (jsize) marpaESLIFValueResultp->u.s.sizel);
    if (byteArrayp == NULL) {
      goto err;
    }
    if ((marpaESLIFValueResultp->u.s.p != NULL) && (marpaESLIFValueResultp->u.s.sizel > 0)) {
      (*envp)->SetByteArrayRegion(envp, byteArrayp, (jsize) 0, (jsize) marpaESLIFValueResultp->u.s.sizel, (jbyte *) marpaESLIFValueResultp->u.s.p);
      if (HAVEEXCEPTION(envp)) {
        goto err;
      }
    }
    objectp = (*envp)->NewObject(envp, JAVA_LANG_STRING_CLASSP, JAVA_LANG_STRING_CLASS_init_byteArray_String_METHODP, byteArrayp, encodingp);
    if (objectp == NULL) {
      goto err;
    }
    MARPAESLIFJAVA_PUSH_PTR(objectStackp, objectp);
    break;
  case MARPAESLIF_VALUE_TYPE_ROW:
    /* fprintf(stderr, "==> %s: import ROW\n", funcs); fflush(stdout); fflush(stderr); */
    /* We received elements in order: first, second, etc..., we pushed that in valueStack, so pop will say last, beforelast, etc..., second, first */
    objectArrayp = (*envp)->NewObjectArray(envp, (jsize) marpaESLIFValueResultp->u.r.sizel, JAVA_LANG_OBJECT_CLASSP, NULL /* initialElement */);
    if (objectArrayp == NULL) {
      RAISEEXCEPTION(envp, "NewObjectArray failure");
    }
    for (i = 0, j = marpaESLIFValueResultp->u.r.sizel - 1; i < marpaESLIFValueResultp->u.r.sizel; i++, j--) {
      objectp = (jobject) GENERICSTACK_POP_PTR(objectStackp);
      (*envp)->SetObjectArrayElement(envp, objectArrayp, (jsize) j, objectp);
      if (HAVEEXCEPTION(envp)) {
        goto err;
      }
    }
    MARPAESLIFJAVA_PUSH_PTR(objectStackp, objectArrayp);
    break;
  case MARPAESLIF_VALUE_TYPE_TABLE:
    /* fprintf(stderr, "==> %s: import TABLE\n", funcs); fflush(stdout); fflush(stderr); */
    objectHashMapp = (*envp)->NewObject(envp, JAVA_UTIL_HASHMAP_CLASSP, JAVA_UTIL_HASHMAP_CLASS_init_METHODP);
    if (objectHashMapp == NULL) {
      RAISEEXCEPTION(envp, "NewObject failure");
    }
    /* We received elements in order: firstkey, firstvalue, secondkey, secondvalue, etc..., we pushed that in valueStack, so pop will say lastvalue, lastkey, ..., firstvalue, firstkey */
    for (i = 0; i < marpaESLIFValueResultp->u.t.sizel; i++) {
      valuep = (jobject) GENERICSTACK_POP_PTR(objectStackp);
      keyp = (jobject) GENERICSTACK_POP_PTR(objectStackp);
      (*envp)->CallObjectMethod(envp, objectHashMapp, JAVA_UTIL_HASHMAP_CLASS_put_METHODP, keyp, valuep);
      if (HAVEEXCEPTION(envp)) {
        goto err;
      }
    }
    MARPAESLIFJAVA_PUSH_PTR(objectStackp, objectHashMapp);
    break;
  case MARPAESLIF_VALUE_TYPE_LONG_DOUBLE:
    if (marpaESLIFValueResult_isinfb(marpaESLIFp, marpaESLIFValueResultp)) {
      if (marpaESLIFValueResultp->u.ld >= 0) {
        MARPAESLIFJAVA_IMPORT_POSITIVE_INFINITY(marpaESLIFp, envp, objectp);
      } else {
        MARPAESLIFJAVA_IMPORT_NEGATIVE_INFINITY(marpaESLIFp, envp, objectp);
      }
    } else if (marpaESLIFValueResult_isnanb(marpaESLIFp, marpaESLIFValueResultp)) {
      MARPAESLIFJAVA_IMPORT_NAN(marpaESLIFp, envp, objectp);
    } else {
      MARPAESLIFJAVA_IMPORT_NUMBER_DECIMAL(marpaESLIFp, envp, objectp, long double, marpaESLIFValueResultp->u.ld, marpaESLIF_ldtos);
    }
    MARPAESLIFJAVA_PUSH_PTR(objectStackp, objectp);
    break;
#ifdef MARPAESLIF_HAVE_LONG_LONG
  case MARPAESLIF_VALUE_TYPE_LONG_LONG:
    /* fprintf(stderr, "==> %s: import LONG_LONG\n", funcs); fflush(stdout); fflush(stderr); */
    MARPAESLIFJAVA_IMPORT_NUMBER_NOT_DECIMAL(envp, objectp, MARPAESLIF_LONG_LONG_FMT, MARPAESLIF_LONG_LONG, marpaESLIFValueResultp->u.ll);
    MARPAESLIFJAVA_PUSH_PTR(objectStackp, objectp);
    break;
#endif
  default:
    break;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (envp != NULL) {
    if (byteArrayp != NULL) {
      (*envp)->DeleteLocalRef(envp, byteArrayp);
    }
    if (encodingp != NULL) {
      (*envp)->DeleteLocalRef(envp, encodingp);
    }
  }
  return rcb;
}

/*****************************************************************************/
static jstring marpaESLIFJava_marpaESLIFStringToJavap(JNIEnv *envp, marpaESLIFString_t *marpaESLIFStringp)
/*****************************************************************************/
{
  static const char *funcs          = "marpaESLIFJava_marpaESLIFStringToJavap";
  jstring            stringp        = NULL;
  jstring            encodingasciip = NULL;
  jbyteArray         byteArrayp     = NULL;

  if (marpaESLIFStringp != NULL) {
    if (marpaESLIFStringp->bytep != NULL) {
      /* Note: ok if bytel is zero, then it is an empty string */
      byteArrayp = (*envp)->NewByteArray(envp, (jsize) marpaESLIFStringp->bytel);
      if (byteArrayp == NULL) {
        goto err;
      }
      if (marpaESLIFStringp->bytel > 0) {
        (*envp)->SetByteArrayRegion(envp, byteArrayp, (jsize) 0, (jsize) marpaESLIFStringp->bytel, (jbyte *) marpaESLIFStringp->bytep);
        if (HAVEEXCEPTION(envp)) {
          goto err;
        }
      }
      if (marpaESLIFStringp->encodingasciis != NULL) {
        /* As per IANA this is in ASCII, thus compatible with Java's modified UTF-8 */
        encodingasciip = (*envp)->NewStringUTF(envp, (const char *) marpaESLIFStringp->encodingasciis);
        if (encodingasciip == NULL) {
          /* We want OUR exception to be raised */
          RAISEEXCEPTIONF(envp, "NewStringUTF(\"%s\") failure", marpaESLIFStringp->encodingasciis);
        }
        stringp = (*envp)->NewObject(envp, JAVA_LANG_STRING_CLASSP, JAVA_LANG_STRING_CLASS_init_byteArray_String_METHODP, byteArrayp, encodingasciip);
      } else {
        stringp = (*envp)->NewObject(envp, JAVA_LANG_STRING_CLASSP, JAVA_LANG_STRING_CLASS_init_byteArray_METHODP, byteArrayp);
      }
      if (stringp == NULL) {
        RAISEEXCEPTION(envp, "NewObject failure");
      }
    }
  }

  goto done;

 err:
  if (envp != NULL) {
    if (byteArrayp != NULL) {
      (*envp)->DeleteLocalRef(envp, byteArrayp);
    }
    if (encodingasciip != NULL) {
      (*envp)->DeleteLocalRef(envp, encodingasciip);
    }
    if (stringp != NULL) {
      (*envp)->DeleteLocalRef(envp, stringp);
    }
  }
  stringp = NULL;

 done:
  return stringp;
}

/*****************************************************************************/
static jstring marpaESLIFJava_marpaESLIFASCIIToJavap(JNIEnv *envp, char *asciis)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFJava_marpaESLIFASCIIToJavap";
  jstring            stringp;

  if (asciis != NULL) {
    /* Per definition an ASCII string is compatible with modified UTF-8 */
    stringp = (*envp)->NewStringUTF(envp, asciis);
    if (stringp == NULL) {
      RAISEEXCEPTION(envp, "NewStringUTF(\"UTF-8\") failure");
    }
  } else {
    stringp = NULL;
  }

  goto done;

 err:
  stringp = NULL;

 done:
  return stringp;
}

/*****************************************************************************/
static jstring marpaESLIFJava_marpaESLIFActionToJavap(JNIEnv *envp, marpaESLIFAction_t *actionp)
/*****************************************************************************/
{
  static const char *funcs = "marpaESLIFJava_marpaESLIFActionToJavap";
  jstring            stringp;

  if (actionp == NULL) {
    stringp = NULL;
  } else {
    switch (actionp->type) {
    case MARPAESLIF_ACTION_TYPE_NAME:
      stringp = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, actionp->u.names);
      break;
    case MARPAESLIF_ACTION_TYPE_STRING:
      stringp = marpaESLIFJava_marpaESLIFStringToJavap(envp, actionp->u.stringp);
      break;
    case MARPAESLIF_ACTION_TYPE_LUA:
      stringp = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, actionp->u.luas);
      break;
    case MARPAESLIF_ACTION_TYPE_LUA_FUNCTION:
      /* ESLIF generates "return function..." */
      stringp = marpaESLIFJava_marpaESLIFASCIIToJavap(envp, actionp->u.luaFunction.luas);
      break;
    default:
      RAISEEXCEPTIONF(envp, "Unsuported action type %d", actionp->type);
    }
  }

  goto done;

 err:
  stringp = NULL;

 done:
  return stringp;
}

/*****************************************************************************/
static short marpaESLIFJava_stack_setb(JNIEnv *envp, marpaESLIFValue_t *marpaESLIFValuep, short resulti, jobject objectp, marpaESLIFValueResult_t *marpaESLIFValueResultOutputp)
/*****************************************************************************/
/* Take care: IF resulti is >= 0, then marpaESLIFValuep must be != NULL      */
/*****************************************************************************/
{
  static const char       *funcs = "marpaESLIFJava_stack_setb";
  marpaESLIF_t            *marpaESLIFp = marpaESLIFGrammar_eslifp(marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep)));
  genericStack_t           marpaESLIFValueResultStack;
  genericStack_t          *marpaESLIFValueResultStackp = NULL;
  short                    rcb;
  genericStack_t           objectStack;
  genericStack_t          *objectStackp = NULL;
  marpaESLIFValueResult_t  marpaESLIFValueResult;
  marpaESLIFValueResult_t  marpaESLIFValueResultTmp;
  marpaESLIFValueResult_t *marpaESLIFValueResultp;
  jclass                   classp;
  jclass                   componentclassp;
  jbyte                    java_byte;
  jshort                   java_short;
  jint                     java_int;
  jlong                    java_long;
  jboolean                 java_boolean;
  jfloat                   java_float;
  jdouble                  java_double;
  short                    eslifb;
  jobject                  globalObjectp;
  jbyte                   *bytep;
  jsize                    bytel;
  jsize                    arrayl;
  jsize                    i;
  jobject                  componentObjectp;
  jobject                  entrySetp;
  jobject                  iteratorp;
  jboolean                 hasNextb;
  jobject                  nextp;
  jobject                  keyp;
  jobject                  valuep;
  jint                     sizei;
  jmethodID                sizeMethodp;
  jmethodID                getMethodp;
  jmethodID                entrySetMethodp;
  jmethodID                getKeyMethodp;
  jmethodID                getValueMethodp;
  jclass                   nextclassp;
  marpaESLIFRepresentationDispose_t disposeCallbackp;
  short                    stringb;

  /*
    Java Type                        marpaESLIFType

    N/A                              N/A
    java.lang.Byte                   CHAR or SHORT or INT or LONG or LONG_LONG or opaque
    java.lang.Short                  CHAR or SHORT or INT or LONG or LONG_LONG or opaque
    java.lang.Integer                CHAR or SHORT or INT or LONG or LONG_LONG or opaque
    java.lang.Long                   CHAR or SHORT or INT or LONG or LONG_LONG or opaque
    java.lang.Float                  FLOAT or DOUBLE or LONG_DOUBLE or opaque
    java.lang.Double                 FLOAT or DOUBLE or LONG_DOUBLE or opaque
    byte[]                           ARRAY
    java.lang.Boolean                BOOL
    java.lang.Character              STRING
    java.lang.String                 STRING
    object[]                         ROW
    java.util.HashMap                TABLE
    
  */

  /* We maintain in parallel a marpaESLIFValueResult and a jobject stacks */
  marpaESLIFValueResultStackp = &marpaESLIFValueResultStack;
  GENERICSTACK_INIT(marpaESLIFValueResultStackp);
  if (GENERICSTACK_ERROR(marpaESLIFValueResultStackp)) {
    marpaESLIFValueResultStackp = NULL;
    RAISEEXCEPTIONF(envp, "marpaESLIFValueResultStackp init failure, %s", strerror(errno));
  }

  objectStackp = &objectStack;
  GENERICSTACK_INIT(objectStackp);
  if (GENERICSTACK_ERROR(objectStackp)) {
    objectStackp = NULL;
    RAISEEXCEPTIONF(envp, "objectStackp init failure, %s", strerror(errno));
  }

  GENERICSTACK_PUSH_PTR(marpaESLIFValueResultStackp, &marpaESLIFValueResult);
  if (GENERICSTACK_ERROR(marpaESLIFValueResultStackp)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFValueResultStackp push failure, %s", strerror(errno));
  }

  GENERICSTACK_PUSH_PTR(objectStackp, objectp);
  if (GENERICSTACK_ERROR(objectStackp)) {
    RAISEEXCEPTIONF(envp, "objectStackp push failure, %s", strerror(errno));
  }

  while (GENERICSTACK_USED(marpaESLIFValueResultStackp) > 0) {
    marpaESLIFValueResultp = (marpaESLIFValueResult_t *) GENERICSTACK_POP_PTR(marpaESLIFValueResultStackp);
    objectp = (jobject) GENERICSTACK_POP_PTR(objectStackp);

    eslifb = 0;

    if (objectp == NULL) {
      /* fprintf(stderr, "==> %s: export UNDEF\n", funcs); fflush(stdout); fflush(stderr); */
      marpaESLIFValueResultp->type            = MARPAESLIF_VALUE_TYPE_UNDEF;
      marpaESLIFValueResultp->contextp        = MARPAESLIF_JNI_CONTEXT;
      marpaESLIFValueResultp->representationp = NULL;
      eslifb = 1;
    } else {
      /* Get object class */
      classp = (*envp)->GetObjectClass(envp, objectp);
      if (classp == NULL) {
        /* An exception was (must have been) raised */
        RAISEEXCEPTION(envp, "GetObjectClass failure");
      }
      /* Because of auto-boxing, direct type is always an Object (we require the Object type in the output of all actions) */
      if ((*envp)->CallBooleanMethod(envp, classp, JAVA_LANG_CLASS_CLASS_equals_METHODP, JAVA_LANG_BYTE_CLASSP) == JNI_TRUE) {
        java_byte = (*envp)->CallByteMethod(envp, objectp, JAVA_LANG_BYTE_CLASS_byteValue_METHODP);
        if (HAVEEXCEPTION(envp)) {
          goto err;
        }
        MARPAESLIFJAVA_EXPORT_NUMBER_NOT_DECIMAL(envp, jbyte, java_byte, eslifb);
      } else if ((*envp)->CallBooleanMethod(envp, classp, JAVA_LANG_CLASS_CLASS_equals_METHODP, JAVA_LANG_SHORT_CLASSP) == JNI_TRUE) {
        java_short = (*envp)->CallShortMethod(envp, objectp, JAVA_LANG_SHORT_CLASS_shortValue_METHODP);
        if (HAVEEXCEPTION(envp)) {
          goto err;
        }
        MARPAESLIFJAVA_EXPORT_NUMBER_NOT_DECIMAL(envp, jshort, java_short, eslifb);
      } else if ((*envp)->CallBooleanMethod(envp, classp, JAVA_LANG_CLASS_CLASS_equals_METHODP, JAVA_LANG_INTEGER_CLASSP) == JNI_TRUE) {
        java_int = (*envp)->CallIntMethod(envp, objectp, JAVA_LANG_INTEGER_CLASS_intValue_METHODP);
        if (HAVEEXCEPTION(envp)) {
          goto err;
        }
        MARPAESLIFJAVA_EXPORT_NUMBER_NOT_DECIMAL(envp, jint, java_int, eslifb);
      } else if ((*envp)->CallBooleanMethod(envp, classp, JAVA_LANG_CLASS_CLASS_equals_METHODP, JAVA_LANG_LONG_CLASSP) == JNI_TRUE) {
        java_long = (*envp)->CallLongMethod(envp, objectp, JAVA_LANG_LONG_CLASS_longValue_METHODP);
        if (HAVEEXCEPTION(envp)) {
          goto err;
        }
        MARPAESLIFJAVA_EXPORT_NUMBER_NOT_DECIMAL(envp, jlong, java_long, eslifb);
      } else if ((*envp)->CallBooleanMethod(envp, classp, JAVA_LANG_CLASS_CLASS_equals_METHODP, JAVA_LANG_FLOAT_CLASSP) == JNI_TRUE) {
        java_float = (*envp)->CallFloatMethod(envp, objectp, JAVA_LANG_FLOAT_CLASS_floatValue_METHODP);
        if (HAVEEXCEPTION(envp)) {
          goto err;
        }
        MARPAESLIFJAVA_EXPORT_NUMBER_DECIMAL(envp, jfloat, java_float, eslifb);
      } else if ((*envp)->CallBooleanMethod(envp, classp, JAVA_LANG_CLASS_CLASS_equals_METHODP, JAVA_LANG_DOUBLE_CLASSP) == JNI_TRUE) {
        java_double = (*envp)->CallDoubleMethod(envp, objectp, JAVA_LANG_DOUBLE_CLASS_doubleValue_METHODP);
        if (HAVEEXCEPTION(envp)) {
          goto err;
        }
        MARPAESLIFJAVA_EXPORT_NUMBER_DECIMAL(envp, jdouble, java_double, eslifb);
      } else if ((*envp)->CallBooleanMethod(envp, classp, JAVA_LANG_CLASS_CLASS_equals_METHODP, JAVA_LANG_BOOLEAN_CLASSP) == JNI_TRUE) {
        java_boolean = (*envp)->CallBooleanMethod(envp, objectp, JAVA_LANG_BOOLEAN_CLASS_booleanValue_METHODP);
        if (HAVEEXCEPTION(envp)) {
          goto err;
        }

        /* fprintf(stderr, "==> %s: export BOOL '%d'\n", funcs, (int) java_boolean); fflush(stdout); fflush(stderr); */
        marpaESLIFValueResultp->type            = MARPAESLIF_VALUE_TYPE_BOOL;
        marpaESLIFValueResultp->contextp        = MARPAESLIF_JNI_CONTEXT;
        marpaESLIFValueResultp->representationp = NULL;
        marpaESLIFValueResultp->u.y             = (java_boolean == JNI_FALSE) ? MARPAESLIFVALUERESULTBOOL_FALSE : MARPAESLIFVALUERESULTBOOL_TRUE;
        eslifb = 1;

      } else if (((*envp)->CallBooleanMethod(envp, classp, JAVA_LANG_CLASS_CLASS_equals_METHODP, JAVA_LANG_CHARACTER_CLASSP) == JNI_TRUE) ||
                 ((*envp)->CallBooleanMethod(envp, classp, JAVA_LANG_CLASS_CLASS_equals_METHODP, JAVA_LANG_STRING_CLASSP) == JNI_TRUE)) {
        /* Fake a PTR and make that an UTF-8 string */
        marpaESLIFValueResultTmp.type                 = MARPAESLIF_VALUE_TYPE_PTR;
        marpaESLIFValueResultTmp.contextp             = MARPAESLIF_JNI_CONTEXT;
        marpaESLIFValueResultTmp.u.p.p                = (void *) objectp;
        marpaESLIFValueResultTmp.u.p.freeUserDatavp   = NULL;
        marpaESLIFValueResultTmp.u.p.freeCallbackp    = NULL;
        marpaESLIFValueResultTmp.u.p.shallowb         = 1;
        if (! marpaESLIFJava_representationCallbackb(NULL /* marpaESLIFJavaValueContextp */,
                                                     &marpaESLIFValueResultTmp,
                                                     (char **) &(marpaESLIFValueResultp->u.s.p),
                                                     &(marpaESLIFValueResultp->u.s.sizel),
                                                     &(marpaESLIFValueResultp->u.s.encodingasciis),
                                                     &disposeCallbackp,
                                                     &stringb)) {
          goto err;
        }
	/* Note that disposeCallbackp value is ignored because we push the result in marpaESLIFValueResultp that has */
	/* a freeCallbackp that is equivalent to our internal dispose callback for representation */

        /* fprintf(stderr, "==> %s: export STRING '%s' encoding '%s'\n", funcs, marpaESLIFValueResultp->u.s.p, marpaESLIFValueResultp->u.s.encodingasciis); fflush(stdout); fflush(stderr); */
        marpaESLIFValueResultp->type               = MARPAESLIF_VALUE_TYPE_STRING;
        marpaESLIFValueResultp->contextp           = MARPAESLIF_JNI_CONTEXT;
        marpaESLIFValueResultp->representationp    = NULL;
        marpaESLIFValueResultp->u.s.shallowb       = 0;
        marpaESLIFValueResultp->u.s.freeUserDatavp = NULL; /* We will resolve JNIEnv using jvm that is a safe static */
        marpaESLIFValueResultp->u.s.freeCallbackp  = marpaESLIFJava_genericFreeCallbackv;
        eslifb = 1;

      } else if ((*envp)->CallBooleanMethod(envp, classp, JAVA_LANG_CLASS_CLASS_isArray_METHODP) == JNI_TRUE) {
        /* Get component class: this CAN be a true primitive and not an object */
        componentclassp = (*envp)->CallObjectMethod(envp, classp, JAVA_LANG_CLASS_CLASS_getComponentType_METHODP);
        if (componentclassp == NULL) {
          /* An exception was (must have been) raised */
          RAISEEXCEPTION(envp, "getComponentType failure");
        }
        if (((*envp)->CallBooleanMethod(envp, componentclassp, JAVA_LANG_CLASS_CLASS_equals_METHODP, JAVA_LANG_BYTE_CLASSP) == JNI_TRUE) ||
            ((*envp)->CallBooleanMethod(envp, componentclassp, JAVA_LANG_CLASS_CLASS_equals_METHODP, byteTypeClassp) == JNI_TRUE)) {
          /* This is a byte[] array */
          /* fprintf(stderr, "==> %s: export ARRAY\n", funcs); fflush(stdout); fflush(stderr); */
          marpaESLIFValueResultp->type               = MARPAESLIF_VALUE_TYPE_ARRAY;
          marpaESLIFValueResultp->contextp           = MARPAESLIF_JNI_CONTEXT;
          marpaESLIFValueResultp->representationp    = NULL;

          bytel = (*envp)->GetArrayLength(envp, objectp);

          marpaESLIFValueResultp->u.a.sizel    = (size_t) bytel;
          if (bytel > 0) {
            if (CHAR_BIT == 8) { /* a char is the same as jbyte */
              marpaESLIFValueResultp->u.a.p = (char *) malloc(bytel);
              if (marpaESLIFValueResultp->u.a.p == NULL) {
                RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
              }
              (*envp)->GetByteArrayRegion(envp, objectp, 0, bytel, marpaESLIFValueResultp->u.a.p);
            } else { /* a char is not 8 bits on this system */
              bytep = (jbyte *) malloc(bytel * sizeof(jbyte));
              if (bytep == NULL) {
                RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
              }
              (*envp)->GetByteArrayRegion(envp, objectp, 0, bytel, bytep);
              marpaESLIFValueResultp->u.a.p = (char *) malloc(bytel);
              for (i = 0; i < bytel; i++) {
                marpaESLIFValueResultp->u.a.p[i] = (char) bytep[i];
              }
              free(bytep);
            }
            marpaESLIFValueResultp->u.a.shallowb       = 0;
            marpaESLIFValueResultp->u.a.freeUserDatavp = NULL; /* We will resolve JNIEnv using jvm that is a safe static */
            marpaESLIFValueResultp->u.a.freeCallbackp  = marpaESLIFJava_genericFreeCallbackv;
          } else {
            marpaESLIFValueResultp->u.a.p = NULL;
            marpaESLIFValueResultp->u.a.shallowb       = 1;
            marpaESLIFValueResultp->u.a.freeUserDatavp = NULL; /* We will resolve JNIEnv using jvm that is a safe static */
            marpaESLIFValueResultp->u.a.freeCallbackp  = NULL;
          }
          eslifb = 1;
        } else { /* if ((*envp)->CallBooleanMethod(envp, componentclassp, JAVA_LANG_CLASS_CLASS_equals_METHODP, JAVA_LANG_OBJECT_CLASSP) == JNI_TRUE) { */
          /* fprintf(stderr, "==> %s: export ROW\n", funcs); fflush(stdout); fflush(stderr); */
          arrayl = (*envp)->GetArrayLength(envp, objectp);
          /* This is an object[] array */
          marpaESLIFValueResultp->type               = MARPAESLIF_VALUE_TYPE_ROW;
          marpaESLIFValueResultp->contextp           = MARPAESLIF_JNI_CONTEXT;
          marpaESLIFValueResultp->representationp    = NULL;
          marpaESLIFValueResultp->u.r.sizel          = (size_t) arrayl;
          marpaESLIFValueResultp->u.r.shallowb       = 0;
          marpaESLIFValueResultp->u.r.freeUserDatavp = NULL; /* We will resolve JNIEnv using jvm that is a safe static */
          marpaESLIFValueResultp->u.r.freeCallbackp  = marpaESLIFJava_genericFreeCallbackv;

          if (arrayl > 0) {
            marpaESLIFValueResultp->u.r.p = (marpaESLIFValueResult_t *) malloc(arrayl * sizeof(marpaESLIFValueResult_t));
            if (marpaESLIFValueResultp->u.r.p == NULL) {
              RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
            }
            for (i = 0; i < arrayl; i++) {
              componentObjectp = (*envp)->GetObjectArrayElement(envp, (jobjectArray) objectp, i);
              if (HAVEEXCEPTION(envp)) {
                RAISEEXCEPTION(envp, " GetObjectArrayElement failure");
              }
              GENERICSTACK_PUSH_PTR(objectStackp, componentObjectp);
              if (GENERICSTACK_ERROR(objectStackp)) {
                RAISEEXCEPTIONF(envp, "objectStackp push failure, %s", strerror(errno));
              }
              GENERICSTACK_PUSH_PTR(marpaESLIFValueResultStackp, (void *) &(marpaESLIFValueResultp->u.r.p[i]));
              if (GENERICSTACK_ERROR(marpaESLIFValueResultStackp)) {
                RAISEEXCEPTIONF(envp, "marpaESLIFValueResultStackp push failure, %s", strerror(errno));
              }
            }
          } else {
            marpaESLIFValueResultp->u.r.p = NULL;
          }
          eslifb = 1;
        }
      } else if ((*envp)->IsInstanceOf(envp, objectp, JAVA_UTIL_MAP_CLASSP) == JNI_TRUE) {

        /* fprintf(stderr, "==> %s: export TABLE\n", funcs); fflush(stdout); fflush(stderr); */

	sizeMethodp = (*envp)->GetMethodID(envp, classp, "size", "()I");
	if (sizeMethodp == NULL) {
          /* We want OUR exception to be raised */
          (*envp)->ExceptionClear(envp);
          RAISEEXCEPTION(envp, "Failed to find method \"size\" with signature \"()I\" in class being an instance of java/util/Map");
	}

	getMethodp = (*envp)->GetMethodID(envp, classp, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
	if (getMethodp == NULL) {
          /* We want OUR exception to be raised */
          (*envp)->ExceptionClear(envp);
          RAISEEXCEPTION(envp, "Failed to find method \"get\" with signature \"(Ljava/lang/Object;)Ljava/lang/Object;\" in class being an instance of java/util/Map");
	}

	entrySetMethodp = (*envp)->GetMethodID(envp, classp, "entrySet", "()Ljava/util/Set;");
	if (entrySetMethodp == NULL) {
          /* We want OUR exception to be raised */
          (*envp)->ExceptionClear(envp);
          RAISEEXCEPTION(envp, "Failed to find method \"entrySet\" with signature \"()Ljava/util/Set;\" in class being an instance of java/util/Map");
	}

        sizei = (*envp)->CallIntMethod(envp, objectp, sizeMethodp);
        /* fprintf(stderr, "==> %s: export TABLE: size=%ld\n", funcs, (unsigned long) sizei); fflush(stdout); fflush(stderr); */

        marpaESLIFValueResultp->type               = MARPAESLIF_VALUE_TYPE_TABLE;
        marpaESLIFValueResultp->contextp           = MARPAESLIF_JNI_CONTEXT;
        marpaESLIFValueResultp->representationp    = NULL;
        marpaESLIFValueResultp->u.t.sizel          = (size_t) sizei;
        marpaESLIFValueResultp->u.t.shallowb       = 0;
        marpaESLIFValueResultp->u.t.freeUserDatavp = NULL; /* We will resolve JNIEnv using jvm that is a safe static */
        marpaESLIFValueResultp->u.t.freeCallbackp  = marpaESLIFJava_genericFreeCallbackv;

        if (marpaESLIFValueResultp->u.t.sizel > 0) {
          marpaESLIFValueResultp->u.t.p = (marpaESLIFValueResultPair_t *) malloc(sizei * sizeof(marpaESLIFValueResultPair_t));
          if (marpaESLIFValueResultp->u.t.p == NULL) {
            RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
          }

          /* fprintf(stderr, "==> %s: export TABLE: entrySet\n", funcs); fflush(stdout); fflush(stderr); */
          entrySetp = (*envp)->CallObjectMethod(envp, objectp, entrySetMethodp);
          if (entrySetp == NULL) {
            /* An exception was (must have been) raised */
            RAISEEXCEPTION(envp, "entrySet failure");
          }

          /* fprintf(stderr, "==> %s: export TABLE: iterator\n", funcs); fflush(stdout); fflush(stderr); */
          iteratorp = (*envp)->CallObjectMethod(envp, entrySetp, JAVA_UTIL_SET_CLASS_iterator_METHODP);
          if (iteratorp == NULL) {
            /* An exception was (must have been) raised */
            RAISEEXCEPTION(envp, "iterator failure");
          }

          i = 0;
          while (1) {
            /* fprintf(stderr, "==> %s: export TABLE: iterator: hasNext\n", funcs); fflush(stdout); fflush(stderr); */
            hasNextb = (*envp)->CallBooleanMethod(envp, iteratorp, JAVA_UTIL_ITERATOR_CLASS_hasNext_METHODP);
            if (HAVEEXCEPTION(envp)) {
              goto err;
            }
            if (hasNextb != JNI_TRUE) {
              break;
            }

            /* fprintf(stderr, "==> %s: export TABLE: iterator: next\n", funcs); fflush(stdout); fflush(stderr); */
            nextp = (*envp)->CallObjectMethod(envp, iteratorp, JAVA_UTIL_ITERATOR_CLASS_next_METHODP);
            if (nextp == NULL) {
              goto err;
            }

	    nextclassp = (*envp)->GetObjectClass(envp, nextp);
	    if (nextclassp == NULL) {
	      /* An exception was (must have been) raised */
	      RAISEEXCEPTION(envp, "GetObjectClass failure");
	    }

	    getKeyMethodp = (*envp)->GetMethodID(envp, nextclassp, "getKey", "()Ljava/lang/Object;");
	    if (getKeyMethodp == NULL) {
	      /* We want OUR exception to be raised */
	      (*envp)->ExceptionClear(envp);
	      RAISEEXCEPTION(envp, "Failed to find method \"getKey\" with signature \"()Ljava/lang/Object;\"");
	    }

            /* fprintf(stderr, "==> %s: export TABLE: iterator: next: getKey\n", funcs); fflush(stdout); fflush(stderr); */
	    keyp = (*envp)->CallObjectMethod(envp, nextp, getKeyMethodp);
            if (HAVEEXCEPTION(envp)) {
              goto err;
            }
            GENERICSTACK_PUSH_PTR(objectStackp, keyp);
            if (GENERICSTACK_ERROR(objectStackp)) {
              RAISEEXCEPTIONF(envp, "objectStackp push failure, %s", strerror(errno));
            }
            GENERICSTACK_PUSH_PTR(marpaESLIFValueResultStackp, (void *) &(marpaESLIFValueResultp->u.t.p[i].key));
            if (GENERICSTACK_ERROR(marpaESLIFValueResultStackp)) {
              RAISEEXCEPTIONF(envp, "marpaESLIFValueResultStackp push failure, %s", strerror(errno));
            }

	    getValueMethodp = (*envp)->GetMethodID(envp, nextclassp, "getValue", "()Ljava/lang/Object;");
	    if (getValueMethodp == NULL) {
	      /* We want OUR exception to be raised */
	      (*envp)->ExceptionClear(envp);
	      RAISEEXCEPTION(envp, "Failed to find method \"getValue\" with signature \"()Ljava/lang/Object;\"");
	    }

            /* fprintf(stderr, "==> %s: export TABLE: iterator: next: getValue\n", funcs); fflush(stdout); fflush(stderr); */
	    valuep = (*envp)->CallObjectMethod(envp, nextp, getValueMethodp);
            if (HAVEEXCEPTION(envp)) {
              goto err;
            }
            GENERICSTACK_PUSH_PTR(objectStackp, valuep);
            if (GENERICSTACK_ERROR(objectStackp)) {
              RAISEEXCEPTIONF(envp, "objectStackp push failure, %s", strerror(errno));
            }
            GENERICSTACK_PUSH_PTR(marpaESLIFValueResultStackp, (void *) &(marpaESLIFValueResultp->u.t.p[i].value));
            if (GENERICSTACK_ERROR(marpaESLIFValueResultStackp)) {
              RAISEEXCEPTIONF(envp, "marpaESLIFValueResultStackp push failure, %s", strerror(errno));
            }

            i++;
          }
        } else {
          marpaESLIFValueResultp->u.t.p = NULL;
        }
	eslifb = 1;
      }
    }

    if (! eslifb) {
      /* fprintf(stderr, "==> %s: export PTR\n", funcs); fflush(stdout); fflush(stderr); */
      /* Make that a global reference and send a PTR */
      globalObjectp = (*envp)->NewGlobalRef(envp, objectp);
      if (globalObjectp == NULL) {
        goto err;
      }
      marpaESLIFValueResultp->type               = MARPAESLIF_VALUE_TYPE_PTR;
      marpaESLIFValueResultp->contextp           = MARPAESLIF_JNI_CONTEXT;
      marpaESLIFValueResultp->representationp    = marpaESLIFJava_representationCallbackb;
      marpaESLIFValueResultp->u.p.p              = (void *) globalObjectp;
      marpaESLIFValueResultp->u.p.shallowb       = 0;
      marpaESLIFValueResultp->u.p.freeUserDatavp = NULL; /* We will resolve JNIEnv using jvm that is a safe static */
      marpaESLIFValueResultp->u.p.freeCallbackp  = marpaESLIFJava_genericFreeCallbackv;
    }
  }

  if (resulti >= 0) {
    if (! marpaESLIFValue_stack_setb(marpaESLIFValuep, resulti, &marpaESLIFValueResult)) {
      RAISEEXCEPTIONF(envp, "marpaESLIFValue_stack_setb failure, %s", strerror(errno));
    }
  }
  if (marpaESLIFValueResultOutputp != NULL) {
    *marpaESLIFValueResultOutputp = marpaESLIFValueResult;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  GENERICSTACK_RESET(objectStackp);
  GENERICSTACK_RESET(marpaESLIFValueResultStackp);
  return rcb;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFJSONEncoder_jniNew(JNIEnv *envp, jobject eslifJSONEncoderp, jboolean strict)
/*****************************************************************************/
{
  static const char                *funcs = "Java_org_parser_marpa_ESLIFJSONEncoder_jniNew";
  marpaESLIFGrammar_t              *marpaESLIFJSONEncoderp;
  marpaESLIF_t                     *marpaESLIFp;
  jobject                           BYTEBUFFER(marpaESLIFJSONEncoder);

  /* Always update genericLogger context */
  if (! ESLIFGrammar_contextb(envp, eslifJSONEncoderp, &marpaESLIFp, NULL /* marpaESLIFGrammarpp */)) {
    goto err;
  }

  /* Create C object */
  marpaESLIFJSONEncoderp = marpaESLIFJSON_encode_newp(marpaESLIFp, (strict == JNI_TRUE) ? 1 : 0);
  if (marpaESLIFJSONEncoderp == NULL) {
    RAISEEXCEPTIONF(envp, "marpaESLIFJSON_encode_newp failure, %s", strerror(errno));
  }

  /* Store the object */
  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIFJSONEncoder, marpaESLIFJSONEncoderp);
  (*envp)->CallVoidMethod(envp, eslifJSONEncoderp, MARPAESLIF_ESLIFGRAMMAR_CLASS_setMarpaESLIFGrammarp_METHODP, BYTEBUFFER(marpaESLIFJSONEncoder));
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  goto done;

 err:
  Java_org_parser_marpa_ESLIFJSONEncoder_jniFree(envp, eslifJSONEncoderp);

 done:
  return;
}

/*****************************************************************************/
JNIEXPORT jstring JNICALL Java_org_parser_marpa_ESLIFJSONEncoder_jniEncode(JNIEnv *envp, jobject eslifJSONEncoderp, jobject objectp)
/*****************************************************************************/
{
  static const char           *funcs = "Java_org_parser_marpa_ESLIFJSONEncoder_jniEncode";
  jstring                      rcp;
  marpaESLIFGrammar_t         *marpaESLIFJSONEncoderp;
  marpaESLIFValueResult_t      marpaESLIFValueResult;
  marpaESLIFValueOption_t      marpaESLIFValueOption;
  marpaESLIFJavaValueContext_t marpaESLIFJavaValueContext;

  if (! ESLIFGrammar_contextb(envp, eslifJSONEncoderp, NULL /* marpaESLIFpp */, &marpaESLIFJSONEncoderp)) {
    goto err;
  }

  /* Value interface is unmanaged: ESLIF does all the job */
  if (! marpaESLIFJava_valueContextInitb(envp, NULL /* eslifValueInterfacep */, eslifJSONEncoderp /* eslifGrammarp */, &marpaESLIFJavaValueContext)) {
    goto err;
  }

  marpaESLIFValueOption.userDatavp = &marpaESLIFJavaValueContext;
  marpaESLIFValueOption.importerp  = marpaESLIFJava_valueImportb;

  /* Create a marpaESLIFValueResult from objectp */
  if (! marpaESLIFJava_stack_setb(envp, NULL /* marpaESLIFValuep */, -1 /* resulti */, objectp, &marpaESLIFValueResult)) {
    goto err;
  }

  if (! marpaESLIFJSON_encodeb(marpaESLIFJSONEncoderp, &marpaESLIFValueResult, &marpaESLIFValueOption)) {
    marpaESLIFJava_valueContextFreev(envp, &marpaESLIFJavaValueContext, 1 /* onStackb */);
    RAISEEXCEPTION(envp, "marpaESLIFJSON_encodeb failure");
  }
  if (GENERICSTACK_USED(marpaESLIFJavaValueContext.objectStackp) != 1) {
    RAISEEXCEPTIONF(envp, "Internal value stack is %d instead of 1", GENERICSTACK_USED(marpaESLIFJavaValueContext.objectStackp));
  }

  rcp = (jstring) GENERICSTACK_POP_PTR(marpaESLIFJavaValueContext.objectStackp); /* It is a string by definition */
  goto done;

 err:
  rcp = NULL;

 done:
  return rcp;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFJSONEncoder_jniFree(JNIEnv *envp, jobject eslifJSONencoderp)
/*****************************************************************************/
{
  marpaESLIFGrammar_t *marpaESLIFJSONEncoderp;

  if (ESLIFGrammar_contextb(envp, eslifJSONencoderp, NULL /* marpaESLIFpp */, &marpaESLIFJSONEncoderp)) {
    marpaESLIFGrammar_freev(marpaESLIFJSONEncoderp); /* This is NULL protected */
  }
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFJSONDecoder_jniNew(JNIEnv *envp, jobject eslifJSONDecoderp, jboolean strict)
/*****************************************************************************/
{
  static const char                *funcs = "Java_org_parser_marpa_ESLIFJSONDecoder_jniNew";
  marpaESLIFGrammar_t              *marpaESLIFJSONDecoderp;
  marpaESLIF_t                     *marpaESLIFp;
  jobject                           BYTEBUFFER(marpaESLIFJSONDecoder);

  /* Always update genericLogger context */
  if (! ESLIFGrammar_contextb(envp, eslifJSONDecoderp, &marpaESLIFp, NULL /* marpaESLIFGrammarpp */)) {
    goto err;
  }

  /* Create C object */
  marpaESLIFJSONDecoderp = marpaESLIFJSON_decode_newp(marpaESLIFp, (strict == JNI_TRUE) ? 1 : 0);
  if (marpaESLIFJSONDecoderp == NULL) {
    RAISEEXCEPTIONF(envp, "marpaESLIFJSON_decode_newp failure, %s", strerror(errno));
  }

  /* Store the object */
  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIFJSONDecoder, marpaESLIFJSONDecoderp);
  (*envp)->CallVoidMethod(envp, eslifJSONDecoderp, MARPAESLIF_ESLIFGRAMMAR_CLASS_setMarpaESLIFGrammarp_METHODP, BYTEBUFFER(marpaESLIFJSONDecoder));
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  goto done;

 err:
  Java_org_parser_marpa_ESLIFJSONDecoder_jniFree(envp, eslifJSONDecoderp);

 done:
  return;
}

/*****************************************************************************/
JNIEXPORT jobject JNICALL Java_org_parser_marpa_ESLIFJSONDecoder_jniDecode(JNIEnv *envp, jobject eslifJSONDecoderp, jobject eslifRecognizerInterfacep, jobject eslifDecodeOptionp)
/*****************************************************************************/
{
  static const char                *funcs = "Java_org_parser_marpa_ESLIFJSONDecoder_jniDecode";
  jobject                           rcp;
  marpaESLIFGrammar_t              *marpaESLIFJSONDecoderp;
  marpaESLIFValueOption_t           marpaESLIFValueOption;
  marpaESLIFJSONDecodeOption_t      marpaESLIFJSONDecodeOption;
  marpaESLIFJavaRecognizerContext_t marpaESLIFJavaRecognizerContext;
  marpaESLIFRecognizerOption_t      marpaESLIFRecognizerOption;
  marpaESLIFJavaValueContext_t      marpaESLIFJavaValueContext;

  if (! marpaESLIFJava_recognizerContextInitb(envp, eslifRecognizerInterfacep, &marpaESLIFJavaRecognizerContext, 0 /* haveSymbolStackb */)) {
    goto err;
  }

  marpaESLIFJSONDecodeOption.disallowDupkeysb = ((*envp)->CallBooleanMethod(envp, eslifDecodeOptionp, MARPAESLIF_ESLIFJSONDECODEROPTION_CLASS_isDisallowDupkeys_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFJSONDecodeOption.maxDepthl = (size_t) (*envp)->CallLongMethod(envp, eslifDecodeOptionp, MARPAESLIF_ESLIFJSONDECODEROPTION_CLASS_getMaxDepth_METHODP);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFJSONDecodeOption.noReplacementCharacterb = ((*envp)->CallBooleanMethod(envp, eslifDecodeOptionp, MARPAESLIF_ESLIFJSONDECODEROPTION_CLASS_isNoReplacementCharacter_METHODP) == JNI_TRUE);
  marpaESLIFJSONDecodeOption.positiveInfinityActionp = NULL;         /* Because java always supports +Infinity in float */
  marpaESLIFJSONDecodeOption.negativeInfinityActionp = NULL;         /* Because java always supports -Infinity in float */
  marpaESLIFJSONDecodeOption.positiveNanActionp      = NULL;         /* Because java always supports NaN in float */
  marpaESLIFJSONDecodeOption.negativeNanActionp      = NULL;         /* Because java always supports NaN in float */
  marpaESLIFJSONDecodeOption.numberActionp           = marpaESLIFJava_JSONDecodeNumberActionb;

  marpaESLIFRecognizerOption.userDatavp                = &marpaESLIFJavaRecognizerContext;
  marpaESLIFRecognizerOption.readerCallbackp           = readerCallbackb;
  marpaESLIFRecognizerOption.disableThresholdb         = ((*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithDisableThreshold_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFRecognizerOption.exhaustedb                = ((*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithExhaustion_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFRecognizerOption.newlineb                  = ((*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithNewline_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFRecognizerOption.trackb                    = ((*envp)->CallBooleanMethod(envp, eslifRecognizerInterfacep, MARPAESLIF_ESLIFRECOGNIZERINTERFACE_CLASS_isWithTrack_METHODP) == JNI_TRUE);
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }
  marpaESLIFRecognizerOption.bufsizl                   = 0; /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci           = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci               = 50; /* Recommended value */
  marpaESLIFRecognizerOption.ifActionResolverp         = marpaESLIFRecognizerIfActionResolver;
  marpaESLIFRecognizerOption.eventActionResolverp      = marpaESLIFRecognizerEventActionResolver;
  marpaESLIFRecognizerOption.regexActionResolverp      = marpaESLIFRecognizerRegexActionResolver;
  marpaESLIFRecognizerOption.generatorActionResolverp  = marpaESLIFRecognizerGeneratorActionResolver;
  marpaESLIFRecognizerOption.importerp                 = marpaESLIFJava_recognizerImportb;

  /* Value interface is unmanaged: ESLIF does all the job */
  if (! marpaESLIFJava_valueContextInitb(envp, NULL /* eslifValueInterfacep */, eslifJSONDecoderp /* eslifGrammarp */, &marpaESLIFJavaValueContext)) {
    goto err;
  }

  marpaESLIFValueOption.userDatavp = &marpaESLIFJavaValueContext;
  marpaESLIFValueOption.importerp  = marpaESLIFJava_valueImportb;

  if (! ESLIFGrammar_contextb(envp, eslifJSONDecoderp, NULL /* marpaESLIFpp */, &marpaESLIFJSONDecoderp)) {
    goto err;
  }

  if (! marpaESLIFJSON_decodeb(marpaESLIFJSONDecoderp, &marpaESLIFJSONDecodeOption, &marpaESLIFRecognizerOption, &marpaESLIFValueOption)) {
    RAISEEXCEPTION(envp, "marpaESLIFJSON_decodeb failure");
  }

  if (GENERICSTACK_USED(marpaESLIFJavaValueContext.objectStackp) != 1) {
    RAISEEXCEPTIONF(envp, "Internal value stack is %d instead of 1", GENERICSTACK_USED(marpaESLIFJavaValueContext.objectStackp));
  }

  rcp = (jstring) GENERICSTACK_POP_PTR(marpaESLIFJavaValueContext.objectStackp); /* It is a string by definition */
  goto done;

 err:
  rcp = NULL;

 done:
  return rcp;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFJSONDecoder_jniFree(JNIEnv *envp, jobject eslifJSONdecoderp)
/*****************************************************************************/
{
  marpaESLIFGrammar_t *marpaESLIFJSONDecoderp;

  if (ESLIFGrammar_contextb(envp, eslifJSONdecoderp, NULL /* marpaESLIFpp */, &marpaESLIFJSONDecoderp)) {
    marpaESLIFGrammar_freev(marpaESLIFJSONDecoderp); /* This is NULL protected */
  }
}

/*****************************************************************************/
static short marpaESLIFJava_JSONDecodeNumberActionb(void *userDatavp, char *strings, size_t stringl, marpaESLIFValueResult_t *marpaESLIFValueResultp, short confidenceb)
/*****************************************************************************/
{
  /* We always use Math::BigFloat->new(strings) */
  static const char                 *funcs = "marpaESLIFJava_JSONDecodeNumberAction";
  marpaESLIFJavaRecognizerContext_t *marpaESLIFJavaRecognizerContextp = (marpaESLIFJavaRecognizerContext_t *) userDatavp;
  jstring                            numberp;
  jobject                            objectp;
  short                              rcb;
  JNIEnv                            *envp;

  /* Number callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    RAISEEXCEPTION(envp, "Failed to get environment");
  }

  if (confidenceb) {
    return 1;
  }

  numberp = (*envp)->NewStringUTF(envp, (const char *) strings);
  if (numberp == NULL) {
    /* We want OUR exception to be raised */
    RAISEEXCEPTIONF(envp, "NewStringUTF(\"%s\") failure", strings);
  }

  objectp = (*envp)->NewObject(envp, JAVA_MATH_BIGDECIMAL_CLASSP, JAVA_MATH_BIGDECIMAL_CLASS_String_init_METHODP, numberp);
  if (objectp == NULL) {
    (*envp)->DeleteLocalRef(envp, numberp);
    RAISEEXCEPTION(envp, "NewObject failure");
  }
  (*envp)->DeleteLocalRef(envp, numberp);

  /* Note that we do NOT need a global reference: this is a C callback within the same and single JNI call */

  marpaESLIFValueResultp->type               = MARPAESLIF_VALUE_TYPE_PTR;
  marpaESLIFValueResultp->contextp           = MARPAESLIF_JNI_CONTEXT;
  marpaESLIFValueResultp->representationp    = marpaESLIFJava_representationCallbackb;
  marpaESLIFValueResultp->u.p.p              = (void *) objectp;
  marpaESLIFValueResultp->u.p.shallowb       = 0;
  marpaESLIFValueResultp->u.p.freeUserDatavp = NULL;
  marpaESLIFValueResultp->u.p.freeCallbackp  = marpaESLIFJava_JSONDecodeFreeCallbackv;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static void marpaESLIFJava_JSONDecodeFreeCallbackv(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp)
/*****************************************************************************/
{
  /* Same as marpaESLIFJava_genericFreeCallbackv but we KNOW that we work on a local reference */
  /* because this is a C callback within one single JNI call. */
  static const char *funcs = "marpaESLIFJava_JSONDecodeFreeCallbackv";
  JNIEnv            *envp;

  /* Free callback is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    return;
  }
  switch (marpaESLIFValueResultp->type) {
  case MARPAESLIF_VALUE_TYPE_PTR:
    /* This is a local reference */
    if (marpaESLIFValueResultp->u.p.p != NULL) {
      (*envp)->DeleteLocalRef(envp, (jobject) marpaESLIFValueResultp->u.p.p);
    }
    break;
  default:
    break;
  }
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFSymbol_jniNew(JNIEnv *envp, jobject eslifSymbolp, jstring typep, jbyteArray utf8byteArrayp, jstring modifiersp)
/*****************************************************************************/
{
  static const char             *funcs = "Java_org_parser_marpa_ESLIFSymbol_jniNew";
  jbyte                         *utf8bytep = NULL;
  marpaESLIFSymbol_t            *marpaESLIFSymbolp;
  marpaESLIF_t                  *marpaESLIFp;
  marpaESLIFString_t             marpaESLIFString;
  jobject                        BYTEBUFFER(marpaESLIFSymbol);
  jobject                        BYTEBUFFER(marpaESLIFJavaSymbolContext);
  jsize                          utf8lengthl;
  const char                    *types;
  const char                    *modifiers = NULL;
  jboolean                       isCopy;
  marpaESLIFSymbolOption_t       marpaESLIFSymbolOption;
  marpaESLIFJavaSymbolContext_t *marpaESLIFJavaSymbolContextp;

  if (! ESLIFSymbol_contextb(envp, eslifSymbolp, &marpaESLIFp, NULL /* marpaESLIFSymbolpp */, NULL /* marpaESLIFJavaSymbolContextpp */)) {
    goto err;
  }

  marpaESLIFJavaSymbolContextp = (marpaESLIFJavaSymbolContext_t *) malloc(sizeof(marpaESLIFJavaSymbolContext_t));
  if (marpaESLIFJavaSymbolContextp == NULL) {
    RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
  }
  if (! marpaESLIFJava_symbolContextInitb(envp, marpaESLIFJavaSymbolContextp)) {
    goto err;
  }
  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIFJavaSymbolContext, marpaESLIFJavaSymbolContextp);  
  (*envp)->CallVoidMethod(envp, eslifSymbolp, MARPAESLIF_ESLIFSYMBOL_CLASS_setMarpaESLIFSymbolContextp_METHODP, BYTEBUFFER(marpaESLIFJavaSymbolContext));
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  /* Get information from the stack */
  if (utf8byteArrayp == NULL) {
    RAISEEXCEPTION(envp, "ByteArray argument is NULL");
  }
  utf8lengthl = (*envp)->GetArrayLength(envp, utf8byteArrayp);
  if (utf8lengthl <= 0) {
    RAISEEXCEPTION(envp, "ByteArray is empty");
  }
  utf8bytep = (jbyte *) malloc(utf8lengthl * sizeof(jbyte));
  if (utf8bytep == NULL) {
    RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
  }
  (*envp)->GetByteArrayRegion(envp, utf8byteArrayp, 0, utf8lengthl, utf8bytep);

  types = (*envp)->GetStringUTFChars(envp, typep, &isCopy);
  if (types == NULL) {
    RAISEEXCEPTION(envp, "GetStringUTFChars failure");
  }

  if (modifiersp != NULL) {
    modifiers = (*envp)->GetStringUTFChars(envp, modifiersp, &isCopy);
    if (modifiers == NULL) {
      RAISEEXCEPTION(envp, "GetStringUTFChars failure");
    }
  }

  /* Create C object */
  marpaESLIFString.bytep          = utf8bytep;
  marpaESLIFString.bytel          = utf8lengthl;
  marpaESLIFString.encodingasciis = "UTF-8";
  marpaESLIFString.asciis         = NULL;

  marpaESLIFSymbolOption.userDatavp = marpaESLIFJavaSymbolContextp;
  marpaESLIFSymbolOption.importerp  = marpaESLIFJava_symbolImportb;

  marpaESLIFSymbolp = (strcmp(types, "regex") == 0) ? marpaESLIFSymbol_regex_newp(marpaESLIFp, &marpaESLIFString, (char *) modifiers, &marpaESLIFSymbolOption) : marpaESLIFSymbol_string_newp(marpaESLIFp, &marpaESLIFString, (char *) modifiers, &marpaESLIFSymbolOption);
  if (marpaESLIFSymbolp == NULL) {
    if (strcmp(types, "regex") == 0) {
      RAISEEXCEPTIONF(envp, "marpaESLIFSymbol_regex_newp failure, %s", strerror(errno));
    } else {
      RAISEEXCEPTIONF(envp, "marpaESLIFSymbol_string_newp failure, %s", strerror(errno));
    }
  }

  /* Store the object */
  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIFSymbol, marpaESLIFSymbolp);  
  (*envp)->CallVoidMethod(envp, eslifSymbolp, MARPAESLIF_ESLIFSYMBOL_CLASS_setMarpaESLIFSymbolp_METHODP, BYTEBUFFER(marpaESLIFSymbol));
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  goto done;
  
 err:
  Java_org_parser_marpa_ESLIFSymbol_jniFree(envp, eslifSymbolp);

 done:
  if (utf8bytep != NULL) {
    free(utf8bytep);
  }
  if ((typep != NULL) && (types != NULL)) {
    (*envp)->ReleaseStringUTFChars(envp, typep, types);
  }
  if ((modifiersp != NULL) && (modifiers != NULL)) {
    (*envp)->ReleaseStringUTFChars(envp, modifiersp, modifiers);
  }
  return;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFSymbol_jniMetaNew(JNIEnv *envp, jobject eslifSymbolp, jobject eslifGrammarp, jstring symbolp)
/*****************************************************************************/
{
  static const char             *funcs = "Java_org_parser_marpa_ESLIFSymbol_jniMetaNew";
  marpaESLIFSymbol_t            *marpaESLIFSymbolp;
  marpaESLIF_t                  *marpaESLIFp;
  marpaESLIFGrammar_t           *marpaESLIFGrammarp;
  jobject                        BYTEBUFFER(marpaESLIFSymbol);
  jobject                        BYTEBUFFER(marpaESLIFJavaSymbolContext);
  const char                    *symbols;
  jboolean                       isCopy;
  marpaESLIFSymbolOption_t       marpaESLIFSymbolOption;
  marpaESLIFJavaSymbolContext_t *marpaESLIFJavaSymbolContextp;

  if (! ESLIFGrammar_contextb(envp, eslifGrammarp, &marpaESLIFp, &marpaESLIFGrammarp)) {
    goto err;
  }

  marpaESLIFJavaSymbolContextp = (marpaESLIFJavaSymbolContext_t *) malloc(sizeof(marpaESLIFJavaSymbolContext_t));
  if (marpaESLIFJavaSymbolContextp == NULL) {
    RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
  }
  if (! marpaESLIFJava_symbolContextInitb(envp, marpaESLIFJavaSymbolContextp)) {
    goto err;
  }
  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIFJavaSymbolContext, marpaESLIFJavaSymbolContextp);
  (*envp)->CallVoidMethod(envp, eslifSymbolp, MARPAESLIF_ESLIFSYMBOL_CLASS_setMarpaESLIFSymbolContextp_METHODP, BYTEBUFFER(marpaESLIFJavaSymbolContext));
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  symbols = (*envp)->GetStringUTFChars(envp, symbolp, &isCopy);
  if (symbols == NULL) {
    RAISEEXCEPTION(envp, "GetStringUTFChars failure");
  }

  marpaESLIFSymbolOption.userDatavp = marpaESLIFJavaSymbolContextp;
  marpaESLIFSymbolOption.importerp  = marpaESLIFJava_symbolImportb;

  marpaESLIFSymbolp = marpaESLIFSymbol_meta_newp(marpaESLIFp, marpaESLIFGrammarp, (char *) symbols, &marpaESLIFSymbolOption);
  if (marpaESLIFSymbolp == NULL) {
    RAISEEXCEPTIONF(envp, "marpaESLIFSymbol_meta_newp failure, %s", strerror(errno));
  }

  /* Store the object */
  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIFSymbol, marpaESLIFSymbolp);
  (*envp)->CallVoidMethod(envp, eslifSymbolp, MARPAESLIF_ESLIFSYMBOL_CLASS_setMarpaESLIFSymbolp_METHODP, BYTEBUFFER(marpaESLIFSymbol));
  if (HAVEEXCEPTION(envp)) {
    goto err;
  }

  goto done;

 err:
  Java_org_parser_marpa_ESLIFSymbol_jniFree(envp, eslifSymbolp);

 done:
  if ((symbolp != NULL) && (symbols != NULL)) {
    (*envp)->ReleaseStringUTFChars(envp, symbolp, symbols);
  }
  return;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFSymbol_jniFree(JNIEnv *envp, jobject eslifSymbolp)
/*****************************************************************************/
{
  marpaESLIFSymbol_t            *marpaESLIFSymbolp;
  marpaESLIFJavaSymbolContext_t *marpaESLIFJavaSymbolContextp;

  if (ESLIFSymbol_contextb(envp, eslifSymbolp, NULL /* marpaESLIFpp */, &marpaESLIFSymbolp, &marpaESLIFJavaSymbolContextp)) {
    marpaESLIFJava_symbolContextFreev(envp, marpaESLIFJavaSymbolContextp);
    marpaESLIFSymbol_freev(marpaESLIFSymbolp); /* This is NULL protected */
  }
}

/*****************************************************************************/
JNIEXPORT jbyteArray JNICALL Java_org_parser_marpa_ESLIFSymbol_jniTest(JNIEnv *envp, jobject eslifSymbolp, jbyteArray jinputp)
/*****************************************************************************/
{
  static const char             *funcs      = "Java_org_parser_marpa_ESLIFSymbol_jniTest";
  jbyte                         *inputp     = NULL;
  jbyteArray                     byteArrayp = NULL;
  marpaESLIFSymbol_t            *marpaESLIFSymbolp;
  jsize                          inputl;
  short                          matchb;
  marpaESLIFJavaSymbolContext_t *marpaESLIFJavaSymbolContextp;

  if (! ESLIFSymbol_contextb(envp, eslifSymbolp, NULL /* marpaESLIFpp */, &marpaESLIFSymbolp, &marpaESLIFJavaSymbolContextp)) {
    goto err;
  }

  if (jinputp == NULL) {
    RAISEEXCEPTION(envp, "ByteArray argument is NULL");
  }
  inputl = (*envp)->GetArrayLength(envp, jinputp);
  if (inputl > 0) {
    inputp = (jbyte *) malloc(inputl * sizeof(jbyte));
    if (inputp == NULL) {
      RAISEEXCEPTIONF(envp, "malloc failure, %s", strerror(errno));
    }
    (*envp)->GetByteArrayRegion(envp, jinputp, 0, inputl, inputp);
  }

  if (! marpaESLIFSymbol_tryb(marpaESLIFSymbolp, (char *) inputp, (size_t) inputl, &matchb)) {
    RAISEEXCEPTIONF(envp, "marpaESLIFSymbol_tryb failure, %s", strerror(errno));
  }

  if (matchb) {
    if (GENERICSTACK_USED(marpaESLIFJavaSymbolContextp->objectStackp) != 1) {
      RAISEEXCEPTIONF(envp, "Internal value stack is %d instead of 1", GENERICSTACK_USED(marpaESLIFJavaSymbolContextp->objectStackp));
    }
    byteArrayp = (jbyteArray) GENERICSTACK_POP_PTR(marpaESLIFJavaSymbolContextp->objectStackp);
  }

  goto done;

 err:
  if (inputp != NULL) {
    free(inputp);
  }

 done:
  return byteArrayp;
}

