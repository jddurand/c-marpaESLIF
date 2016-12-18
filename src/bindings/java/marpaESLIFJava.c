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
JNIEXPORT void    JNICALL Java_org_parser_marpa_ESLIF_newp(JNIEnv *envp, jobject objectp);
JNIEXPORT void    JNICALL Java_org_parser_marpa_ESLIF_freev(JNIEnv *envp, jobject objectp);

/* ---------- */
/* Structures */
/* ---------- */
typedef struct marpaESLIFGenericLoggerContext {
  jobject   objectp;             /* Current object - this can change at every call */
  jmethodID getLoggerInterfacep; /* getLoggerInterface method ID of current object */
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

  { NULL }
};


/* -------------- */
/* Static methods */
/* -------------- */
static void   generateStringWithLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static void   genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);

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
      }                                                                 \
    }                                                                   \
} while (0)

/* ---------------------------------------------------------------------------- */
/* Bytebuffer stuff - we always use a capaticy of 1 - we know what we are doing */
/* ---------------------------------------------------------------------------- */
#define BYTEBUFFER(what) byteBuffer##what##p
#define CREATE_BYTEBUFFER(what,ptr) do {                               \
    BYTEBUFFER(what) = (*envp)->NewDirectByteBuffer(envp, ptr, 1);      \
    if (BYTEBUFFER(what) == NULL) {                                     \
      RAISEEXCEPTION(envp, "NewDirectByteBuffer failure at %s:%d", MARPAESLIF_FILENAMES, __LINE__); \
      goto err;                                                         \
    }                                                                   \
  } while (0)

/*****************************************************************************/
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *JavaVMp, void* reservedp)
/*****************************************************************************/
{
  JNIEnv                  *envp;
  marpaESLIFClassCache_t  *marpaESLIFClassCachep  = marpaESLIFClassCacheArrayp;
  marpaESLIFMethodCache_t *marpaESLIFMethodCachep = marpaESLIFMethodCacheArrayp;

  /* ------------------------------------------------ */
  /* It is safe to store JavaVMp in a global variable */
  /* ------------------------------------------------ */
  marpaESLIF_vmp = JavaVMp;

  if (((*marpaESLIF_vmp)->GetEnv(marpaESLIF_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    RAISEEXCEPTION(envp, "Failed to get environment at %s:%d", MARPAESLIF_FILENAMES, __LINE__);
    goto err;
  }

  /* -------------------- */
  /* Get cache of classes */
  /* -------------------- */
  while (marpaESLIFClassCachep->classs != NULL) {
    marpaESLIFClassCachep->classp = (*envp)->FindClass(envp,
                                                       marpaESLIFClassCachep->classs);
    if (marpaESLIFClassCachep->classp == NULL) {
      RAISEEXCEPTION(envp, "Failed to find class \"%s\" at %s:%d", marpaESLIFClassCachep->classs, MARPAESLIF_FILENAMES, __LINE__);
      goto err;
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
      goto err;
    }
    marpaESLIFMethodCachep++;
  }

  return MARPAESLIF_JNI_VERSION;

 err:
  return -1;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIF_newp(JNIEnv *envp, jobject objectp)
/*****************************************************************************/
{
  marpaESLIFGenericLoggerContext_t *marpaESLIFGenericLoggerContextp = NULL;
  genericLogger_t                  *genericLoggerp                  = NULL;
  marpaESLIF_t                     *marpaESLIFp                     = NULL;
  marpaESLIFOption_t                marpaESLIFOption;
  jobject                           BYTEBUFFER(marpaESLIFGenericLoggerContext);
  jobject                           BYTEBUFFER(genericLogger);
  jobject                           BYTEBUFFER(marpaESLIF);

  marpaESLIFGenericLoggerContextp = (marpaESLIFGenericLoggerContext_t *) malloc(sizeof(marpaESLIFGenericLoggerContext_t));
  if (marpaESLIFGenericLoggerContextp == NULL) {
    RAISEEXCEPTION(envp, "malloc failure exception at %s:%d, %s", MARPAESLIF_FILENAMES, __LINE__, strerror(errno));
    goto err;
  }

  marpaESLIFGenericLoggerContextp->objectp             = objectp;
  marpaESLIFGenericLoggerContextp->getLoggerInterfacep = MARPAESLIF_ESLIF_CLASS_getLoggerInterfacep_METHODP;

  genericLoggerp = genericLogger_newp(genericLoggerCallbackv, (void *) marpaESLIFGenericLoggerContextp, GENERICLOGGER_LOGLEVEL_TRACE);
  if (genericLoggerp == NULL) {
    RAISEEXCEPTION(envp, "genericLogger_newp exception at %s:%d, %s", MARPAESLIF_FILENAMES, __LINE__, strerror(errno));
    goto err;
  }

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    RAISEEXCEPTION(envp, "marpaESLIF_newp exception at %s:%d, %s", MARPAESLIF_FILENAMES, __LINE__, strerror(errno));
    goto err;
  }

  CREATE_BYTEBUFFER(marpaESLIFGenericLoggerContext, marpaESLIFGenericLoggerContextp);
  CREATE_BYTEBUFFER(genericLogger, genericLoggerp);
  CREATE_BYTEBUFFER(marpaESLIF, marpaESLIFp);
  
  (*envp)->CallVoidMethod(envp, objectp, MARPAESLIF_ESLIF_CLASS_setGenericLoggerContextp_METHODP, BYTEBUFFER(marpaESLIFGenericLoggerContext));
  (*envp)->CallVoidMethod(envp, objectp, MARPAESLIF_ESLIF_CLASS_setMarpaESLIFp_METHODP,           BYTEBUFFER(marpaESLIF));
  (*envp)->CallVoidMethod(envp, objectp, MARPAESLIF_ESLIF_CLASS_setGenericLoggerp_METHODP,        BYTEBUFFER(genericLogger));
  goto done;
  
 err:
  Java_org_parser_marpa_ESLIF_freev(envp, objectp);

 done:
  return;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_parser_marpa_ESLIF_freev(JNIEnv *envp, jobject objectp)
/*****************************************************************************/
{
  marpaESLIFGenericLoggerContext_t *marpaESLIFGenericLoggerContextp = NULL;
  genericLogger_t                  *genericLoggerp                  = NULL;
  marpaESLIF_t                     *marpaESLIFp                     = NULL;
  jobject                           BYTEBUFFER(marpaESLIFGenericLoggerContext);
  jobject                           BYTEBUFFER(genericLogger);
  jobject                           BYTEBUFFER(marpaESLIF);

  BYTEBUFFER(marpaESLIFGenericLoggerContext) = (*envp)->CallObjectMethod(envp, objectp, MARPAESLIF_ESLIF_CLASS_getGenericLoggerContextp_METHODP);
  BYTEBUFFER(genericLogger)                  = (*envp)->CallObjectMethod(envp, objectp, MARPAESLIF_ESLIF_CLASS_getGenericLoggerp_METHODP);
  BYTEBUFFER(marpaESLIF)                     = (*envp)->CallObjectMethod(envp, objectp, MARPAESLIF_ESLIF_CLASS_getMarpaESLIFp_METHODP);

  if (BYTEBUFFER(marpaESLIFGenericLoggerContext) != NULL) {
    marpaESLIFGenericLoggerContextp = (*envp)->GetDirectBufferAddress(envp, BYTEBUFFER(marpaESLIFGenericLoggerContext));
    if (marpaESLIFGenericLoggerContextp != NULL) {
      marpaESLIFGenericLoggerContextp->objectp            = objectp;
      marpaESLIFGenericLoggerContextp->getLoggerInterfacep = MARPAESLIF_ESLIF_CLASS_getLoggerInterfacep_METHODP;
    }
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

  (*envp)->CallVoidMethod(envp, objectp, MARPAESLIF_ESLIF_CLASS_setGenericLoggerContextp_METHODP, NULL);
  (*envp)->CallVoidMethod(envp, objectp, MARPAESLIF_ESLIF_CLASS_setGenericLoggerp_METHODP, NULL);
  (*envp)->CallVoidMethod(envp, objectp, MARPAESLIF_ESLIF_CLASS_setMarpaESLIFp_METHODP, NULL);  
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
