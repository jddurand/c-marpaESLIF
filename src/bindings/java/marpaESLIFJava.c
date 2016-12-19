/* Java JNI bindings for marpaESLIF                          */
#include <jni.h>
#include <marpaESLIF.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <genericLogger.h>

/* ---------------- */
/* Exported methods */
/* ---------------- */
JNIEXPORT jint    JNICALL JNI_OnLoad(JavaVM *vmp, void* reservedp);
JNIEXPORT void    JNICALL JNI_OnUnLoad(JavaVM *vmp, void* reservedp);

JNIEXPORT void    JNICALL Java_org_parser_marpa_ESLIF_jniNew    (JNIEnv *envp, jobject eslifp);
JNIEXPORT jstring JNICALL Java_org_parser_marpa_ESLIF_jniVersion(JNIEnv *envp, jobject eslifp);
JNIEXPORT void    JNICALL Java_org_parser_marpa_ESLIF_jniFree   (JNIEnv *envp, jobject eslifp);

JNIEXPORT void      JNICALL Java_org_parser_marpa_ESLIFGrammar_jniNew               (JNIEnv *envp, jobject eslifGrammarp, jbyteArray utf8byteArrayp);
JNIEXPORT jint      JNICALL Java_org_parser_marpa_ESLIFGrammar_jniNgrammar          (JNIEnv *envp, jobject eslifGrammarp);
JNIEXPORT jint      JNICALL Java_org_parser_marpa_ESLIFGrammar_jniCurrentLevel      (JNIEnv *envp, jobject eslifGrammarp);
JNIEXPORT jstring   JNICALL Java_org_parser_marpa_ESLIFGrammar_jniCurrentDescription(JNIEnv *envp, jobject eslifGrammarp);
JNIEXPORT jstring   JNICALL Java_org_parser_marpa_ESLIFGrammar_jniDescriptionByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level);
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniCurrentRuleIds    (JNIEnv *envp, jobject eslifGrammarp);
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleIdsByLevel    (JNIEnv *envp, jobject eslifGrammarp, jint level);
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleDisplay       (JNIEnv *envp, jobject eslifGrammarp, jint rule);
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleShow          (JNIEnv *envp, jobject eslifGrammarp, jint rule);
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleDisplayByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level, jint rule);
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleShowByLevel   (JNIEnv *envp, jobject eslifGrammarp, jint level, jint rule);
JNIEXPORT jstring   JNICALL Java_org_parser_marpa_ESLIFGrammar_jniShow              (JNIEnv *envp, jobject eslifGrammarp);
JNIEXPORT jstring   JNICALL Java_org_parser_marpa_ESLIFGrammar_jniShowByLevel       (JNIEnv *envp, jobject eslifGrammarp, jint level);
JNIEXPORT void      JNICALL Java_org_parser_marpa_ESLIFGrammar_jniFree              (JNIEnv *envp, jobject eslifGrammarp);

/* ---------- */
/* Structures */
/* ---------- */
typedef struct marpaESLIFGenericLoggerContext {
  jobject   objectp;              /* Current object - this can change at every call */
  jmethodID getLoggerInterfacep;
} marpaESLIFGenericLoggerContext_t;

typedef struct marpaESLIFClassCache {
  char     *classs;
  jclass    classp;
} marpaESLIFClassCache_t;

typedef struct marpaESLIFMethodCache {
  marpaESLIFClassCache_t *marpaESLIFClassCachep;
  char                   *methods;
  char                   *signatures;
  jmethodID              methodp;
} marpaESLIFMethodCache_t;

typedef struct marpaESLIF_stringGenerator { /* We use genericLogger to generate strings */
  char        *s;
  size_t       l;
  short        okb;
} marpaESLIF_stringGenerator_t;

/* ------ */
/* Macros */
/* ------ */
#define MARPAESLIF_JNI_VERSION                JNI_VERSION_1_4 /* Because of NIO Support */
#define MARPAESLIF_FILENAMES                  "marpaESLIFJava.c"
#define MARPAESLIF_EXCEPTION_CLASS            "java/lang/Exception"
#define MARPAESLIF_ESLIF_CLASS                "org/parser/marpa/ESLIF"
#define MARPAESLIF_ESLIFGRAMMAR_CLASS         "org/parser/marpa/ESLIFGrammar"
#define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS "org/parser/marpa/ESLIFLoggerInterface"

/* -------------------------------- */
/* Globals and accessors as macros */
/* -------------------------------- */
static JavaVM *marpaESLIF_vmp;

static marpaESLIFClassCache_t marpaESLIFClassCacheArrayp[] = {
  #define MARPAESLIF_EXCEPTION_CLASSCACHE             marpaESLIFClassCacheArrayp[0]
  #define MARPAESLIF_EXCEPTION_CLASSP                 marpaESLIFClassCacheArrayp[0].classp
  {       MARPAESLIF_EXCEPTION_CLASS,                 NULL },

  #define MARPAESLIF_ESLIF_CLASSCACHE                 marpaESLIFClassCacheArrayp[1]
  #define MARPAESLIF_ESLIF_CLASSP                     marpaESLIFClassCacheArrayp[1].classp
  {       MARPAESLIF_ESLIF_CLASS,                     NULL },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE  marpaESLIFClassCacheArrayp[2]
  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSP      marpaESLIFClassCacheArrayp[2].classp
  {       MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS,      NULL },

  #define MARPAESLIF_ESLIFGRAMMAR_CLASSCACHE          marpaESLIFClassCacheArrayp[3]
  #define MARPAESLIF_ESLIFGRAMMAR_CLASSP              marpaESLIFClassCacheArrayp[3].classp
  {       MARPAESLIF_ESLIFGRAMMAR_CLASS,              NULL },

  { NULL }
};

static marpaESLIFMethodCache_t marpaESLIFMethodCacheArrayp[] = {
  #define MARPAESLIF_ESLIF_CLASS_getLoggerInterfacep_METHODP              marpaESLIFMethodCacheArrayp[0].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "getLoggerInterface",       "()Lorg/parser/marpa/ESLIFLoggerInterface;", NULL },

  #define MARPAESLIF_ESLIF_CLASS_getMarpaESLIFp_METHODP                   marpaESLIFMethodCacheArrayp[1].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "getMarpaESLIFp",            "()Ljava/nio/ByteBuffer;", NULL },

  #define MARPAESLIF_ESLIF_CLASS_setMarpaESLIFp_METHODP                   marpaESLIFMethodCacheArrayp[2].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "setMarpaESLIFp",            "(Ljava/nio/ByteBuffer;)V", NULL },

  #define MARPAESLIF_ESLIF_CLASS_getGenericLoggerContextp_METHODP         marpaESLIFMethodCacheArrayp[3].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "getGenericLoggerContextp",  "()Ljava/nio/ByteBuffer;", NULL },

  #define MARPAESLIF_ESLIF_CLASS_setGenericLoggerContextp_METHODP         marpaESLIFMethodCacheArrayp[4].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "setGenericLoggerContextp",  "(Ljava/nio/ByteBuffer;)V", NULL },

  #define MARPAESLIF_ESLIF_CLASS_getGenericLoggerp_METHODP                marpaESLIFMethodCacheArrayp[5].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "getGenericLoggerp",         "()Ljava/nio/ByteBuffer;", NULL },

  #define MARPAESLIF_ESLIF_CLASS_setGenericLoggerp_METHODP                marpaESLIFMethodCacheArrayp[6].methodp
  {      &MARPAESLIF_ESLIF_CLASSCACHE,       "setGenericLoggerp",         "(Ljava/nio/ByteBuffer;)V", NULL },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_trace_METHODP             marpaESLIFMethodCacheArrayp[7].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "trace",      "(Ljava/lang/String;)V", NULL },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_debug_METHODP             marpaESLIFMethodCacheArrayp[8].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "debug",      "(Ljava/lang/String;)V", NULL },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_info_METHODP              marpaESLIFMethodCacheArrayp[9].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "info",       "(Ljava/lang/String;)V", NULL },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_notice_METHODP            marpaESLIFMethodCacheArrayp[10].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "notice",     "(Ljava/lang/String;)V", NULL },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_warning_METHODP           marpaESLIFMethodCacheArrayp[11].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "warning",    "(Ljava/lang/String;)V", NULL },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_error_METHODP             marpaESLIFMethodCacheArrayp[12].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "error",      "(Ljava/lang/String;)V", NULL },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_critical_METHODP          marpaESLIFMethodCacheArrayp[13].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "critical",   "(Ljava/lang/String;)V", NULL },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_alert_METHODP             marpaESLIFMethodCacheArrayp[14].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "alert",      "(Ljava/lang/String;)V", NULL },

  #define MARPAESLIF_ESLIFLOGGERINTERFACE_CLASS_emergency_METHODP         marpaESLIFMethodCacheArrayp[15].methodp
  {      &MARPAESLIF_ESLIFLOGGERINTERFACE_CLASSCACHE,       "emergency",  "(Ljava/lang/String;)V", NULL },

  #define MARPAESLIF_ESLIFGRAMMAR_CLASS_getLoggerInterfacep_METHODP       marpaESLIFMethodCacheArrayp[16].methodp
  {      &MARPAESLIF_ESLIFGRAMMAR_CLASSCACHE, "getLoggerInterface",       "()Lorg/parser/marpa/ESLIFLoggerInterface;", NULL },

  #define MARPAESLIF_ESLIFGRAMMAR_CLASS_getEslif_METHODP                  marpaESLIFMethodCacheArrayp[17].methodp
  {      &MARPAESLIF_ESLIFGRAMMAR_CLASSCACHE, "getEslif",                 "()Lorg/parser/marpa/ESLIF;", NULL },

  #define MARPAESLIF_ESLIFGRAMMAR_CLASS_getMarpaGrammarp_METHODP          marpaESLIFMethodCacheArrayp[18].methodp
  {      &MARPAESLIF_ESLIFGRAMMAR_CLASSCACHE, "getMarpaGrammarp",         "()Ljava/nio/ByteBuffer;", NULL },

  #define MARPAESLIF_ESLIFGRAMMAR_CLASS_setMarpaGrammarp_METHODP          marpaESLIFMethodCacheArrayp[19].methodp
  {      &MARPAESLIF_ESLIFGRAMMAR_CLASSCACHE, "setMarpaGrammarp",         "(Ljava/nio/ByteBuffer;)V", NULL },

  { NULL }
};


/* -------------- */
/* Static methods */
/* -------------- */
static void          generateStringWithLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static void          genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static marpaESLIF_t *ESLIFGrammar_updateContextAndGetMarpaeslifp(JNIEnv *envp, jobject eslifGrammarp, jobject objectp, jmethodID getLoggerInterfacep, genericLogger_t **genericLoggerpp);
static marpaESLIF_t *ESLIF_updateContextAndGetMarpaeslifp(JNIEnv *envp, jobject eslifp, jobject objectp, jmethodID getLoggerInterfacep, genericLogger_t **genericLoggerpp);

/* --------------- */
/* Internal macros */
/* --------------- */

/* Raise exception - I use a macro because I did not want to include stdarg in this file - it is used as a static method */
#define RAISEEXCEPTION(envp, fmts, ...) do {                            \
    if (MARPAESLIF_EXCEPTION_CLASSP != NULL) {                          \
      genericLogger_t              *genericLoggerp = NULL;              \
      marpaESLIF_stringGenerator_t  marpaESLIF_stringGenerator;         \
                                                                        \
      marpaESLIF_stringGenerator.s   = NULL;                            \
      marpaESLIF_stringGenerator.l   = 0;                               \
      marpaESLIF_stringGenerator.okb = 0;                               \
                                                                        \
      genericLoggerp = GENERICLOGGER_CUSTOM(generateStringWithLoggerCallbackv, (void *) &marpaESLIF_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE); \
      if (genericLoggerp != NULL) {                                     \
        GENERICLOGGER_TRACEF(genericLoggerp, fmts, __VA_ARGS__);        \
        if (marpaESLIF_stringGenerator.okb) {                           \
          (*envp)->ThrowNew(envp, MARPAESLIF_EXCEPTION_CLASSP, marpaESLIF_stringGenerator.s); \
        }                                                               \
        free(marpaESLIF_stringGenerator.s);                             \
        GENERICLOGGER_FREE(genericLoggerp);                             \
      } else {                                                          \
        (*envp)->ThrowNew(envp, MARPAESLIF_EXCEPTION_CLASSP, "Exception (no description)"); \
      }                                                                 \
      goto err;                                                         \
    }                                                                   \
} while (0)

/* ---------------------------------------------------------------------------- */
/* Bytebuffer stuff - we always use a capaticy of 1 - we know what we are doing */
/* ---------------------------------------------------------------------------- */
#define BYTEBUFFER(what) byteBuffer##what##p
#define MARPAESLIF_PTR2BYTEBUFFER(what,ptr) do {                        \
    BYTEBUFFER(what) = (*envp)->NewDirectByteBuffer(envp, ptr, 1);      \
    if (BYTEBUFFER(what) == NULL) {                                     \
      RAISEEXCEPTION(envp, "NewDirectByteBuffer failure at %s:%d", MARPAESLIF_FILENAMES, __LINE__); \
    }                                                                   \
  } while (0)

/*****************************************************************************/
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *JavaVMp, void* reservedp)
/*****************************************************************************/
{
  JNIEnv                  *envp;
  jclass                   classp;
  marpaESLIFClassCache_t  *marpaESLIFClassCachep  = marpaESLIFClassCacheArrayp;
  marpaESLIFMethodCache_t *marpaESLIFMethodCachep = marpaESLIFMethodCacheArrayp;

  /* ------------------------------------------------ */
  /* It is safe to store JavaVMp in a global variable */
  /* ------------------------------------------------ */
  marpaESLIF_vmp = JavaVMp;

  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    RAISEEXCEPTION(envp, "Failed to get environment at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }

  /* --------------------------------------------------------------------------------------------------- */
  /* Get cache of classes - this is ok only if the result of FindClass is turned into a global reference */
  /* --------------------------------------------------------------------------------------------------- */
  while (marpaESLIFClassCachep->classs != NULL) {
    classp = (*envp)->FindClass(envp, marpaESLIFClassCachep->classs);
    if (classp == NULL) {
      RAISEEXCEPTION(envp, "Failed to find class \"%s\" at %s:%d", marpaESLIFClassCachep->classs, MARPAESLIF_FILENAMES, __LINE__);
    }
    /* classp is a local reference and will vanish itself */
    marpaESLIFClassCachep->classp = (*envp)->NewGlobalRef(envp, classp);
    if (marpaESLIFClassCachep->classp == NULL) {
      RAISEEXCEPTION(envp, "NewGlobalRef failure at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
    }
    marpaESLIFClassCachep++;
  }

  /* -------------------- */
  /* Get cache of methods */
  /* -------------------- */
  while (marpaESLIFMethodCachep->marpaESLIFClassCachep != NULL) {
    marpaESLIFMethodCachep->methodp = (*envp)->GetMethodID(envp,
                                                           marpaESLIFMethodCachep->marpaESLIFClassCachep->classp,
                                                           marpaESLIFMethodCachep->methods,
                                                           marpaESLIFMethodCachep->signatures);
    if (marpaESLIFMethodCachep->methodp == NULL) {
      RAISEEXCEPTION(envp,
                     "Failed to find method \"%s\" with signature \"%s\" in class \"%s\" at %s:%d",
                     marpaESLIFMethodCachep->methods,
                     marpaESLIFMethodCachep->signatures,
                     marpaESLIFMethodCachep->marpaESLIFClassCachep->classs,
                     MARPAESLIF_FILENAMES,
                     __LINE__);
    }
    marpaESLIFMethodCachep++;
  }

  return MARPAESLIF_JNI_VERSION;

 err:
  return JNI_ERR;
}

/*****************************************************************************/
JNIEXPORT void JNICALL JNI_OnUnLoad(JavaVM *vmp, void* reservedp)
/*****************************************************************************/
{
  JNIEnv                 *envp;
  marpaESLIFClassCache_t *marpaESLIFClassCachep  = marpaESLIFClassCacheArrayp;

  /* ------------------------------------------------ */
  /* It is safe to store JavaVMp in a global variable */
  /* ------------------------------------------------ */
  if (((*vmp)->GetEnv(vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    RAISEEXCEPTION(envp, "Failed to get environment at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }

  /* ---------------------------------------------- */
  /* Remove global references created by JNI_OnLoad
  /* ----------------------------------g------------ -*/
  while (marpaESLIFClassCachep->classs != NULL) {
    if (marpaESLIFClassCachep->classp != NULL) {
      (*envp)->DeleteGlobalRef(envp, marpaESLIFClassCachep->classp);
    }
    marpaESLIFClassCachep++;
  }

 err:
  return;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIF_jniNew(JNIEnv *envp, jobject eslifp)
/*****************************************************************************/
{
  static const char                *funcs = "Java_org_parser_marpa_ESLIF_jniNew";
  marpaESLIFGenericLoggerContext_t *marpaESLIFGenericLoggerContextp = NULL;
  genericLogger_t                  *genericLoggerp                  = NULL;
  marpaESLIF_t                     *marpaESLIFp                     = NULL;
  marpaESLIFOption_t                marpaESLIFOption;
  jobject                           BYTEBUFFER(marpaESLIFGenericLoggerContext);
  jobject                           BYTEBUFFER(genericLogger);
  jobject                           BYTEBUFFER(marpaESLIF);

  /* ------------------------------------- */
  /* Create and save genericLogger context */
  /* ------------------------------------- */
  marpaESLIFGenericLoggerContextp = (marpaESLIFGenericLoggerContext_t *) malloc(sizeof(marpaESLIFGenericLoggerContext_t));
  if (marpaESLIFGenericLoggerContextp == NULL) {
    RAISEEXCEPTION(envp, "malloc failure at %s:%d, %s", MARPAESLIF_FILENAMES, __LINE__, strerror(errno));
  }

  marpaESLIFGenericLoggerContextp->objectp             = eslifp;
  marpaESLIFGenericLoggerContextp->getLoggerInterfacep = MARPAESLIF_ESLIF_CLASS_getLoggerInterfacep_METHODP;

  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIFGenericLoggerContext, marpaESLIFGenericLoggerContextp);
 (*envp)->CallVoidMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_setGenericLoggerContextp_METHODP, BYTEBUFFER(marpaESLIFGenericLoggerContext));

 /* ------------------------------ */
 /* Create and save generic logger */
 /* ------------------------------ */
  genericLoggerp = genericLogger_newp(genericLoggerCallbackv, (void *) marpaESLIFGenericLoggerContextp, GENERICLOGGER_LOGLEVEL_TRACE);
  if (genericLoggerp == NULL) {
    RAISEEXCEPTION(envp, "genericLogger_newp failure at %s:%d, %s", MARPAESLIF_FILENAMES, __LINE__, strerror(errno));
  }
  MARPAESLIF_PTR2BYTEBUFFER(genericLogger, genericLoggerp);
  (*envp)->CallVoidMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_setGenericLoggerp_METHODP, BYTEBUFFER(genericLogger));

  /* Now we can also log */
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  /* --------------------------- */
  /* Create and save marpaESLIFp */
  /* --------------------------- */
  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    RAISEEXCEPTION(envp, "marpaESLIF_newp failure at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }
  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIF, marpaESLIFp);
  (*envp)->CallVoidMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_setMarpaESLIFp_METHODP, BYTEBUFFER(marpaESLIF));

  goto done;
  
 err:
  Java_org_parser_marpa_ESLIF_jniFree(envp, eslifp);

 done:
  return;
}

/*****************************************************************************/
JNIEXPORT jstring JNICALL Java_org_parser_marpa_ESLIF_jniVersion(JNIEnv *envp, jobject eslifp)
/*****************************************************************************/
{
  static const char *funcs = "Java_org_parser_marpa_ESLIF_jniVersion";
  genericLogger_t   *genericLoggerp = NULL;
  jstring            versionp;

  /* Always update genericLogger context */
  if (ESLIF_updateContextAndGetMarpaeslifp(envp, eslifp, eslifp, MARPAESLIF_ESLIF_CLASS_getLoggerInterfacep_METHODP, &genericLoggerp) == NULL) {
    /* An exception was raised */
    goto err;
  }
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  versionp = (*envp)->NewStringUTF(envp, marpaESLIF_versions());
  goto done;

 err:
  versionp = NULL;

 done:
  return versionp;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIF_jniFree(JNIEnv *envp, jobject eslifp)
/*****************************************************************************/
{
  static const char *funcs = "Java_org_parser_marpa_ESLIF_jniFree";
  marpaESLIFGenericLoggerContext_t *marpaESLIFGenericLoggerContextp = NULL;
  genericLogger_t                  *genericLoggerp                  = NULL;
  marpaESLIF_t                     *marpaESLIFp                     = NULL;
  jobject                           BYTEBUFFER(marpaESLIFGenericLoggerContext);
  jobject                           BYTEBUFFER(genericLogger);
  jobject                           BYTEBUFFER(marpaESLIF);

  /* Always update genericLogger context */
  if (ESLIF_updateContextAndGetMarpaeslifp(envp, eslifp, eslifp, MARPAESLIF_ESLIF_CLASS_getLoggerInterfacep_METHODP, &genericLoggerp) == NULL) {
    /* An exception was raised */
    goto done;
  }
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  BYTEBUFFER(marpaESLIFGenericLoggerContext) = (*envp)->CallObjectMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_getGenericLoggerContextp_METHODP);
  BYTEBUFFER(genericLogger)                  = (*envp)->CallObjectMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_getGenericLoggerp_METHODP);
  BYTEBUFFER(marpaESLIF)                     = (*envp)->CallObjectMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_getMarpaESLIFp_METHODP);

  if (BYTEBUFFER(marpaESLIFGenericLoggerContext) != NULL) {
    marpaESLIFGenericLoggerContextp = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(marpaESLIFGenericLoggerContext));
  }
  if (BYTEBUFFER(genericLogger) != NULL) {
    genericLoggerp = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(genericLogger));
  }
  if (BYTEBUFFER(marpaESLIF) != NULL) {
    marpaESLIFp = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(marpaESLIF));
  }


  /* Now we can free */
  marpaESLIF_freev(marpaESLIFp); /* This is NULL protected */
  genericLogger_freev(&genericLoggerp); /* Ditto */
  if (marpaESLIFGenericLoggerContextp != NULL) {
    free(marpaESLIFGenericLoggerContextp);
  }

  (*envp)->CallVoidMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_setGenericLoggerContextp_METHODP, NULL);
  (*envp)->CallVoidMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_setGenericLoggerp_METHODP, NULL);
  (*envp)->CallVoidMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_setMarpaESLIFp_METHODP, NULL);

 done:
  return;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFGrammar_jniNew(JNIEnv *envp, jobject eslifGrammarp, jbyteArray utf8byteArrayp)
/*****************************************************************************/
{
  static const char                *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniNew";
  marpaESLIFGrammar_t              *marpaESLIFGrammarp  = NULL;
  marpaESLIF_t                     *marpaESLIFp;
  marpaESLIFGrammarOption_t         marpaESLIFGrammarOption;
  jobject                           BYTEBUFFER(marpaESLIFGrammar);
  jbyte                            *utf8bytep = NULL;
  jsize                             utf8lengthl = 0;
  jboolean                          isCopyb = JNI_FALSE;
  genericLogger_t                  *genericLoggerp;

  /* Always update genericLogger context */
  marpaESLIFp = ESLIFGrammar_updateContextAndGetMarpaeslifp(envp, eslifGrammarp, eslifGrammarp, MARPAESLIF_ESLIFGRAMMAR_CLASS_getLoggerInterfacep_METHODP, &genericLoggerp);
  if (marpaESLIFp == NULL) {
    /* An exception was raised */
    goto err;
  }
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  /* Get information from the stack */
  if (utf8byteArrayp == NULL) {
    RAISEEXCEPTION(envp, "utf8byteArrayp is NULL at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }
  utf8bytep = (*envp)->GetByteArrayElements(envp, utf8byteArrayp, &isCopyb);
  if (utf8bytep == NULL) {
    RAISEEXCEPTION(envp, "utf8bytep is NULL at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }
  utf8lengthl = (*envp)->GetArrayLength(envp, utf8byteArrayp);
  if (utf8lengthl <= 0) {
    RAISEEXCEPTION(envp, "utf8lengthl is <= 0 at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }

  /* Create C object */
  marpaESLIFGrammarOption.bytep               = (void *) utf8bytep;
  marpaESLIFGrammarOption.bytel               = (size_t) utf8lengthl;
  marpaESLIFGrammarOption.encodings           = "UTF-8";
  marpaESLIFGrammarOption.encodingl           = strlen("UTF-8");
  marpaESLIFGrammarOption.encodingOfEncodings = "ASCII";

  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammarp failure at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }

  /* Store the object */
  MARPAESLIF_PTR2BYTEBUFFER(marpaESLIFGrammar, marpaESLIFGrammarp);  
  (*envp)->CallVoidMethod(envp, eslifGrammarp, MARPAESLIF_ESLIFGRAMMAR_CLASS_setMarpaGrammarp_METHODP, BYTEBUFFER(marpaESLIFGrammar));
  goto done;
  
 err:
  Java_org_parser_marpa_ESLIFGrammar_jniFree(envp, eslifGrammarp);

 done:
  if (utf8bytep != NULL) {
    (*envp)->ReleaseByteArrayElements(envp, utf8byteArrayp, utf8bytep, JNI_ABORT); /* We certainly did not want to modify anything -; */
  }
  return;
}

#define MARPAESLIF_GRAMMAR_COMMON_START(envp, eslifGrammarp, marpaESLIFGrammarp, genericLoggerpp) do { \
    jobject              BYTEBUFFER(marpaESLIFGrammar);                 \
    genericLogger_t     *genericLoggerp = NULL;                         \
                                                                        \
    if (ESLIFGrammar_updateContextAndGetMarpaeslifp(envp, eslifGrammarp, eslifGrammarp, MARPAESLIF_ESLIFGRAMMAR_CLASS_getLoggerInterfacep_METHODP, genericLoggerpp) == NULL) { \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    BYTEBUFFER(marpaESLIFGrammar) = (*envp)->CallObjectMethod(envp, eslifGrammarp, MARPAESLIF_ESLIFGRAMMAR_CLASS_getMarpaGrammarp_METHODP); \
    if (BYTEBUFFER(marpaESLIFGrammar) == NULL) {                        \
      RAISEEXCEPTION(envp, "marpaESLIFGrammar bytebuffer is NULL at %s:%d", MARPAESLIF_FILENAMES, __LINE__); \
    }                                                                   \
    marpaESLIFGrammarp = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(marpaESLIFGrammar)); \
    if (marpaESLIFGrammarp == NULL) {                                   \
      RAISEEXCEPTION(envp, "marpaESLIFGrammarp is NULL at %s:%d", MARPAESLIF_FILENAMES, __LINE__); \
    }                                                                   \
  } while (0)

/*****************************************************************************/
JNIEXPORT jint JNICALL Java_org_parser_marpa_ESLIFGrammar_jniNgrammar(JNIEnv *envp, jobject eslifGrammarp)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniNgrammar";
  marpaESLIFGrammar_t *marpaESLIFGrammarp  = NULL;
  int                  ngrammari = 0;
  genericLogger_t     *genericLoggerp = NULL;

  MARPAESLIF_GRAMMAR_COMMON_START(envp, eslifGrammarp, marpaESLIFGrammarp, &genericLoggerp);
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  if (! marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_ngrammarib failure at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
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
  marpaESLIFGrammar_t *marpaESLIFGrammarp  = NULL;
  int                  leveli = 0;
  genericLogger_t     *genericLoggerp = NULL;

  MARPAESLIF_GRAMMAR_COMMON_START(envp, eslifGrammarp, marpaESLIFGrammarp, &genericLoggerp);
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  if (! marpaESLIFGrammar_grammar_currentb(marpaESLIFGrammarp, &leveli, NULL)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_grammar_currentb failure at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
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
  marpaESLIFGrammar_t *marpaESLIFGrammarp  = NULL;
  marpaESLIFString_t  *descp;
  jstring              stringp;
  genericLogger_t     *genericLoggerp = NULL;

  MARPAESLIF_GRAMMAR_COMMON_START(envp, eslifGrammarp, marpaESLIFGrammarp, &genericLoggerp);
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  if (! marpaESLIFGrammar_grammar_currentb(marpaESLIFGrammarp, NULL, &descp)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_grammar_currentb failure at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }

  /* We enforced UTF-8 when parsing the grammar, so description is also in UTF-8 */
  stringp = (*envp)->NewStringUTF(envp, descp->bytep);
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
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniDescription";
  marpaESLIFGrammar_t *marpaESLIFGrammarp  = NULL;
  marpaESLIFString_t  *descp;
  int                  leveli = (jint) level;
  jstring              stringp;
  genericLogger_t     *genericLoggerp = NULL;

  MARPAESLIF_GRAMMAR_COMMON_START(envp, eslifGrammarp, marpaESLIFGrammarp, &genericLoggerp);
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  if (! marpaESLIFGrammar_grammar_by_levelb(marpaESLIFGrammarp, leveli, NULL, NULL, &descp)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_grammar_by_levelb failure for level %d at %s:%d", leveli, MARPAESLIF_FILENAMES, __LINE__);
  }

  /* We enforced UTF-8 when parsing the grammar, so description is also in UTF-8 */
  stringp = (*envp)->NewStringUTF(envp, descp->bytep);
  goto done;

 err:
  stringp = NULL;

 done:
  return stringp;
}

/*****************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniCurrentRuleIds(JNIEnv *envp, jobject eslifGrammarp)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniCurrentRuleIds";
  marpaESLIFGrammar_t *marpaESLIFGrammarp  = NULL;
  jintArray            intArray = NULL;
  jint                *intp = NULL;
  int                 *ruleip;
  size_t               rulel;
  int                  i;
  genericLogger_t     *genericLoggerp = NULL;

  MARPAESLIF_GRAMMAR_COMMON_START(envp, eslifGrammarp, marpaESLIFGrammarp, &genericLoggerp);
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  if (! marpaESLIFGrammar_rulearray_currentb(marpaESLIFGrammarp, &ruleip, &rulel)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_rulearray_currentb failure at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }
  if (rulel <= 0) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_rulearray_currentb returned rulel <= 0 at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }
  intp = (jint *) malloc(sizeof(jint) * rulel);
  if (intp == NULL) {
    RAISEEXCEPTION(envp, "malloc failure at %s:%d; %s", MARPAESLIF_FILENAMES, __LINE__, strerror(errno));
  }
  for (i = 0; i < rulel; i++) {
    intp[i] = (jint) ruleip[i];
  }

  intArray = (*envp)->NewIntArray(envp, (jsize) rulel);
  if (intArray == NULL) {
    RAISEEXCEPTION(envp, "NewIntArray failure at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }
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
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleIdsByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniRuleIdsByLevel";
  marpaESLIFGrammar_t *marpaESLIFGrammarp  = NULL;
  int                  leveli = (int) level;
  jintArray            intArray;
  jint                *intp = NULL;
  int                 *ruleip;
  size_t               rulel;
  int                  i;
  genericLogger_t     *genericLoggerp = NULL;

  MARPAESLIF_GRAMMAR_COMMON_START(envp, eslifGrammarp, marpaESLIFGrammarp, &genericLoggerp);
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  if (! marpaESLIFGrammar_rulearray_by_levelb(marpaESLIFGrammarp, &ruleip, &rulel, leveli, NULL)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_rulearray_by_levelb failure for level %d at %s:%d", leveli, MARPAESLIF_FILENAMES, __LINE__);
  }
  if (rulel <= 0) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_rulearray_currentb returned rulel <= 0 at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }
  intp = (jint *) malloc(sizeof(jint) * rulel);
  if (intp == NULL) {
    RAISEEXCEPTION(envp, "malloc failure at %s:%d; %s", MARPAESLIF_FILENAMES, __LINE__, strerror(errno));
  }
  for (i = 0; i < rulel; i++) {
    intp[i] = (jint) ruleip[i];
  }

  intArray = (*envp)->NewIntArray(envp, (jsize) rulel);
  if (intArray == NULL) {
    RAISEEXCEPTION(envp, "NewIntArray failure at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }
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
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleDisplay(JNIEnv *envp, jobject eslifGrammarp, jint rule)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniRuleDisplay";
  marpaESLIFGrammar_t *marpaESLIFGrammarp  = NULL;
  int                  rulei = (int) rule;
  char                *ruledisplays;
  genericLogger_t     *genericLoggerp = NULL;
  jstring              stringp;

  MARPAESLIF_GRAMMAR_COMMON_START(envp, eslifGrammarp, marpaESLIFGrammarp, &genericLoggerp);
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  if (! marpaESLIFGrammar_ruledisplayform_currentb(marpaESLIFGrammarp, rulei, &ruledisplays)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_ruledisplayform_currentb failure for rule %d at %s:%d", rulei, MARPAESLIF_FILENAMES, __LINE__);
  }

  stringp = (*envp)->NewStringUTF(envp, ruledisplays);
  goto done;

 err:
  stringp = NULL;

 done:
  return stringp;
}

/*****************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleShow(JNIEnv *envp, jobject eslifGrammarp, jint rule)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniRuleShow";
  marpaESLIFGrammar_t *marpaESLIFGrammarp  = NULL;
  int                  rulei = (int) rule;
  char                *ruleshows;
  genericLogger_t     *genericLoggerp = NULL;
  jstring              stringp;

  MARPAESLIF_GRAMMAR_COMMON_START(envp, eslifGrammarp, marpaESLIFGrammarp, &genericLoggerp);
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  if (! marpaESLIFGrammar_ruleshowform_currentb(marpaESLIFGrammarp, rulei, &ruleshows)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_ruleshowform_currentb failure for rule %d at %s:%d", rulei, MARPAESLIF_FILENAMES, __LINE__);
  }

  stringp = (*envp)->NewStringUTF(envp, ruleshows);
  goto done;

 err:
  stringp = NULL;

 done:
  return stringp;
}

/*****************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleDisplayByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level, jint rule)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniRuleDisplayByLevel";
  marpaESLIFGrammar_t *marpaESLIFGrammarp  = NULL;
  int                  leveli = (int) level;
  int                  rulei = (int) rule;
  char                *ruledisplays;
  genericLogger_t     *genericLoggerp = NULL;
  jstring              stringp;

  MARPAESLIF_GRAMMAR_COMMON_START(envp, eslifGrammarp, marpaESLIFGrammarp, &genericLoggerp);
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  if (! marpaESLIFGrammar_ruledisplayform_by_levelb(marpaESLIFGrammarp, rulei, &ruledisplays, leveli, NULL)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_ruledisplayform_currentb failure for rule %d and level %d at %s:%d", rulei, leveli, MARPAESLIF_FILENAMES, __LINE__);
  }

  stringp = (*envp)->NewStringUTF(envp, ruledisplays);
  goto done;

 err:
  stringp = NULL;

 done:
  return stringp;
}

/*****************************************************************************/
JNIEXPORT jintArray JNICALL Java_org_parser_marpa_ESLIFGrammar_jniRuleShowByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level, jint rule)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniRuleShowByLevel";
  marpaESLIFGrammar_t *marpaESLIFGrammarp  = NULL;
  int                  leveli = (int) level;
  int                  rulei = (int) rule;
  char                *ruleshows;
  genericLogger_t     *genericLoggerp = NULL;
  jstring              stringp;

  MARPAESLIF_GRAMMAR_COMMON_START(envp, eslifGrammarp, marpaESLIFGrammarp, &genericLoggerp);
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  if (! marpaESLIFGrammar_ruleshowform_by_levelb(marpaESLIFGrammarp, rulei, &ruleshows, leveli, NULL)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_ruleshowform_by_levelb failure for rule %d and level %d at %s:%d", rulei, leveli, MARPAESLIF_FILENAMES, __LINE__);
  }

  stringp = (*envp)->NewStringUTF(envp, ruleshows);
  goto done;

 err:
  stringp = NULL;

 done:
  return stringp;
}

/*****************************************************************************/
JNIEXPORT jstring JNICALL Java_org_parser_marpa_ESLIFGrammar_jniShow(JNIEnv *envp, jobject eslifGrammarp)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniShow";
  marpaESLIFGrammar_t *marpaESLIFGrammarp  = NULL;
  char                *shows;
  jstring              stringp;
  genericLogger_t     *genericLoggerp = NULL;

  MARPAESLIF_GRAMMAR_COMMON_START(envp, eslifGrammarp, marpaESLIFGrammarp, &genericLoggerp);
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  if (! marpaESLIFGrammar_grammarshowform_currentb(marpaESLIFGrammarp, &shows)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_grammarshowform_currentb failure at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }

  stringp = (*envp)->NewStringUTF(envp, shows);
  goto done;

 err:
  stringp = NULL;

 done:
  return stringp;
}

/*****************************************************************************/
JNIEXPORT jstring JNICALL Java_org_parser_marpa_ESLIFGrammar_jniShowByLevel(JNIEnv *envp, jobject eslifGrammarp, jint level)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniShowByLevel";
  marpaESLIFGrammar_t *marpaESLIFGrammarp  = NULL;
  int                  leveli = (int) level;
  char                *shows;
  jstring              stringp;
  genericLogger_t     *genericLoggerp = NULL;

  MARPAESLIF_GRAMMAR_COMMON_START(envp, eslifGrammarp, marpaESLIFGrammarp, &genericLoggerp);
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  if (! marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammarp, &shows, leveli, NULL)) {
    RAISEEXCEPTION(envp, "marpaESLIFGrammar_grammarshowform_by_levelb for level at %s:%d", leveli, MARPAESLIF_FILENAMES, __LINE__);
  }

  stringp = (*envp)->NewStringUTF(envp, shows);
  goto done;

 err:
  stringp = NULL;

 done:
  return stringp;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIFGrammar_jniFree(JNIEnv *envp, jobject eslifGrammarp)
/*****************************************************************************/
{
  static const char   *funcs = "Java_org_parser_marpa_ESLIFGrammar_jniFree";
  marpaESLIFGrammar_t *marpaESLIFGrammarp  = NULL;
  genericLogger_t     *genericLoggerp = NULL;

  MARPAESLIF_GRAMMAR_COMMON_START(envp, eslifGrammarp, marpaESLIFGrammarp, &genericLoggerp);
#ifndef MARPAESLIF_NTRACE
  GENERICLOGGER_TRACEF(genericLoggerp, "In %s", funcs);
#endif

  marpaESLIFGrammar_freev(marpaESLIFGrammarp); /* This is NULL protected */
  goto done;
 err:
 done:
  return;
}

/*****************************************************************************/
static void genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  marpaESLIFGenericLoggerContext_t *marpaESLIFGenericLoggerContextp = (marpaESLIFGenericLoggerContext_t *) userDatavp;
  JNIEnv                           *envp;
  jobject                           loggerInterfacep;
  jmethodID                         methodp;
  jstring                           stringp;

  if (marpaESLIFGenericLoggerContextp == NULL) {
    /* Impossible IMHO */
    return;
  }
  if ((marpaESLIFGenericLoggerContextp->objectp == NULL) || (marpaESLIFGenericLoggerContextp->getLoggerInterfacep == NULL)) {
    /* Ditto */
    return;
  }

  /* Logging callack is never running in another thread - no need to attach */
  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    return;
  }

  loggerInterfacep = (*envp)->CallObjectMethod(envp, marpaESLIFGenericLoggerContextp->objectp, marpaESLIFGenericLoggerContextp->getLoggerInterfacep);
  if (loggerInterfacep == NULL) {
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

  if (methodp == NULL) {
    return;
  }

  /* marpaESLIF is never logging with characters outside of 7-bits ASCII */
  stringp = (*envp)->NewStringUTF(envp, msgs);
  if (stringp == NULL) {
    return;
  }

  (*envp)->CallVoidMethod(envp, loggerInterfacep, methodp, stringp);
  (*envp)->DeleteLocalRef(envp, stringp);
}

/*****************************************************************************/
static void generateStringWithLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  marpaESLIF_stringGenerator_t *contextp = (marpaESLIF_stringGenerator_t *) userDatavp;
  char                         *tmps;

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
static marpaESLIF_t *ESLIF_updateContextAndGetMarpaeslifp(JNIEnv *envp, jobject eslifp, jobject objectp, jmethodID getLoggerInterfacep, genericLogger_t **genericLoggerpp)
/*****************************************************************************/
{
  marpaESLIFGenericLoggerContext_t *marpaESLIFGenericLoggerContextp = NULL;
  marpaESLIF_t                     *marpaESLIFp;
  genericLogger_t                  *genericLoggerp;
  jobject                           BYTEBUFFER(marpaESLIFGenericLoggerContext);
  jobject                           BYTEBUFFER(marpaESLIF);
  jobject                           BYTEBUFFER(genericLogger);
  short                             rcb;

  BYTEBUFFER(marpaESLIFGenericLoggerContext) = (*envp)->CallObjectMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_getGenericLoggerContextp_METHODP);
  BYTEBUFFER(marpaESLIF)                     = (*envp)->CallObjectMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_getMarpaESLIFp_METHODP);
  BYTEBUFFER(genericLogger)                  = (*envp)->CallObjectMethod(envp, eslifp, MARPAESLIF_ESLIF_CLASS_getGenericLoggerp_METHODP);

  if (BYTEBUFFER(marpaESLIFGenericLoggerContext) == NULL) { RAISEEXCEPTION(envp, "marpaESLIFGenericLoggerContext bytebuffer is NULL at %s:%d", MARPAESLIF_FILENAMES, __LINE__); goto err; }
  if (BYTEBUFFER(marpaESLIF)                     == NULL) { RAISEEXCEPTION(envp, "eslif bytebuffer is NULL at %s:%d", MARPAESLIF_FILENAMES, __LINE__); goto err; }
  if (BYTEBUFFER(genericLogger)                  == NULL) { RAISEEXCEPTION(envp, "genericLogger bytebuffer is NULL at %s:%d", MARPAESLIF_FILENAMES, __LINE__); goto err; }

  marpaESLIFGenericLoggerContextp = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(marpaESLIFGenericLoggerContext));
  marpaESLIFp                     = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(marpaESLIF));
  genericLoggerp                  = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(genericLogger));

  if (marpaESLIFGenericLoggerContextp == NULL) { RAISEEXCEPTION(envp, "marpaESLIFGenericLoggerContextp is NULL at %s:%d", MARPAESLIF_FILENAMES, __LINE__); goto err; }
  if (marpaESLIFp                     == NULL) { RAISEEXCEPTION(envp, "marpaESLIFp is NULL at %s:%d", MARPAESLIF_FILENAMES, __LINE__); goto err; }
  if (genericLoggerp                  == NULL) { RAISEEXCEPTION(envp, "genericLoggerp is NULL at %s:%d", MARPAESLIF_FILENAMES, __LINE__); goto err; }

  marpaESLIFGenericLoggerContextp->objectp             = objectp;
  marpaESLIFGenericLoggerContextp->getLoggerInterfacep = getLoggerInterfacep;

  if (genericLoggerpp != NULL) {
    *genericLoggerpp = genericLoggerp;
  }

  goto done;

 err:
  marpaESLIFp = NULL;

 done:
  return marpaESLIFp; 
}

/*****************************************************************************/
static marpaESLIF_t *ESLIFGrammar_updateContextAndGetMarpaeslifp(JNIEnv *envp, jobject eslifGrammarp, jobject objectp, jmethodID getLoggerInterfacep, genericLogger_t **genericLoggerpp)
/*****************************************************************************/
{
  jobject       eslifp;
  marpaESLIF_t *marpaESLIFp;

  /* Get information from caller object (ESLIF) */
  eslifp = (*envp)->CallObjectMethod(envp, eslifGrammarp, MARPAESLIF_ESLIFGRAMMAR_CLASS_getEslif_METHODP);
  if (eslifp == NULL) {
    RAISEEXCEPTION(envp, "eslifp is NULL at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
  }

  marpaESLIFp = ESLIF_updateContextAndGetMarpaeslifp(envp, eslifp, objectp, getLoggerInterfacep, genericLoggerpp);
  goto done;

 err:
  marpaESLIFp = NULL;

 done:
  return marpaESLIFp;
}
