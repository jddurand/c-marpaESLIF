/* Java JNI bindings for marpaESLIF                          */
#include <jni.h>
#include <marpaESLIF.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <genericLogger.h>
#include <tconv.h>

/* ---------------- */
/* Exported methods */
/* ---------------- */
JNIEXPORT jint    JNICALL JNI_OnLoad(JavaVM *vmp, void* reservedp);
JNIEXPORT jobject JNICALL Java_org_marpa_ESLIF_newp(JNIEnv *envp, jobject ESLIF_objectp);

/* ----------------------------------- */
/* Global variables, constants, macros */
/* ----------------------------------- */
const int i_for_is_bigendian = 1;
#define IS_BIGENDIAN() ( (*(char*)&i_for_is_bigendian) == 0 ) /* C.f. http://www.ibm.com/developerworks/aix/library/au-endianc/ */


#define MARPAESLIF_JNI_VERSION JNI_VERSION_1_4 /* Because of LOGGER_CLASS */
#define FILENAMES "marpaESLIF.c"

/* It is safe to store this value via JNI_OnLoad */
static JavaVM *g_vmp;

/* Cache of class pointers */
#define EXCEPTION_CLASS   "java/lang/Exception"
#define ESLIF_CLASS       "org/marpa/ESLIF"
#define ESLIFOPTION_CLASS "org/marpa/ESLIFOption"
#define LOGGER_CLASS      "java/util/logging/Logger"

typedef struct genericLoggerContext {
  jobject Logger_objectp;   /* This can change at every call */
} genericLoggerContext_t;

typedef struct marpaESLIFClassCache {
  char     *classs;
  jclass    classp;
} marpaESLIFClassCache_t;

static marpaESLIFClassCache_t marpaESLIFClassCacheArrayp[] = {
  { EXCEPTION_CLASS,   NULL }, /* 0 */
  { ESLIF_CLASS,       NULL }, /* 1 */
  { ESLIFOPTION_CLASS, NULL }, /* 2 */
  { LOGGER_CLASS,      NULL }, /* 3 */
  { NULL }
};

#define EXCEPTION_CLASSCACHE   marpaESLIFClassCacheArrayp[0]
#define ESLIF_CLASSCACHE       marpaESLIFClassCacheArrayp[1]
#define ESLIFOPTION_CLASSCACHE marpaESLIFClassCacheArrayp[2]
#define LOGGER_CLASSCACHE      marpaESLIFClassCacheArrayp[3]

#define EXCEPTION_CLASSP       marpaESLIFClassCacheArrayp[0].classp
#define ESLIF_CLASSP           marpaESLIFClassCacheArrayp[1].classp
#define ESLIFOPTION_CLASSP     marpaESLIFClassCacheArrayp[2].classp
#define LOGGER_CLASSP          marpaESLIFClassCacheArrayp[3].classp
  
/* Cache of method pointers */
typedef struct marpaESLIFMethodCache {
  marpaESLIFClassCache_t *classCachep;
  char                   *methods;
  char                   *signatures;
  jmethodID              methodp;
} marpaESLIFMethodCache_t;

static marpaESLIFMethodCache_t marpaESLIFMethodCacheArrayp[] = {
  { &ESLIF_CLASSCACHE,       "get_ESLIFOption",           "()Lorg/marpa/ESLIFOption;",    NULL }, /*  0 */
  { &ESLIF_CLASSCACHE,       "get_marpaESLIFp",           "()Ljava/nio/ByteBuffer;",      NULL }, /*  1 */
  { &ESLIF_CLASSCACHE,       "set_marpaESLIFp",           "(Ljava/nio/ByteBuffer;)V",     NULL }, /*  2 */
  { &ESLIFOPTION_CLASSCACHE, "get_genericLoggerp",        "()Ljava/nio/ByteBuffer;",      NULL }, /*  3 */
  { &ESLIFOPTION_CLASSCACHE, "set_genericLoggerp",        "(Ljava/nio/ByteBuffer;)V",     NULL }, /*  4 */
  { &ESLIFOPTION_CLASSCACHE, "get_genericLoggerContextp", "()Ljava/nio/ByteBuffer;",      NULL }, /*  5 */
  { &ESLIFOPTION_CLASSCACHE, "set_genericLoggerContextp", "(Ljava/nio/ByteBuffer;)V",     NULL }, /*  6 */
  { &ESLIFOPTION_CLASSCACHE, "get_logger",                "()Ljava/util/logging/Logger;", NULL }, /*  7 */
  { &LOGGER_CLASSCACHE,      "severe",                    "(Ljava/lang/String;)V",        NULL }, /*  8 */
  { &LOGGER_CLASSCACHE,      "warning",                   "(Ljava/lang/String;)V",        NULL }, /*  9 */
  { &LOGGER_CLASSCACHE,      "info",                      "(Ljava/lang/String;)V",        NULL }, /* 10 */
  { &LOGGER_CLASSCACHE,      "fine",                      "(Ljava/lang/String;)V",        NULL }, /* 11 */
  { &LOGGER_CLASSCACHE,      "finer",                     "(Ljava/lang/String;)V",        NULL }, /* 12 */
  { &LOGGER_CLASSCACHE,      "finest",                    "(Ljava/lang/String;)V",        NULL }, /* 13 */
  { NULL }
};

#define ESLIF_CLASS_get_ESLIFOption_METHODP                 marpaESLIFMethodCacheArrayp[ 0].methodp
#define ESLIF_CLASS_get_marpaESLIFp_METHODP                 marpaESLIFMethodCacheArrayp[ 1].methodp
#define ESLIF_CLASS_set_marpaESLIFp_METHODP                 marpaESLIFMethodCacheArrayp[ 2].methodp
#define ESLIFOPTION_CLASS_get_genericLoggerp_METHODP        marpaESLIFMethodCacheArrayp[ 3].methodp
#define ESLIFOPTION_CLASS_set_genericLoggerp_METHODP        marpaESLIFMethodCacheArrayp[ 4].methodp
#define ESLIFOPTION_CLASS_get_genericLoggerContextp_METHODP marpaESLIFMethodCacheArrayp[ 5].methodp
#define ESLIFOPTION_CLASS_set_genericLoggerContextp_METHODP marpaESLIFMethodCacheArrayp[ 6].methodp
#define ESLIFOPTION_CLASS_get_logger_METHODP                marpaESLIFMethodCacheArrayp[ 7].methodp
#define LOGGER_CLASS_severe_METHODP                         marpaESLIFMethodCacheArrayp[ 8].methodp
#define LOGGER_CLASS_warning_METHODP                        marpaESLIFMethodCacheArrayp[ 9].methodp
#define LOGGER_CLASS_info_METHODP                           marpaESLIFMethodCacheArrayp[10].methodp
#define LOGGER_CLASS_fine_METHODP                           marpaESLIFMethodCacheArrayp[11].methodp
#define LOGGER_CLASS_finer_METHODP                          marpaESLIFMethodCacheArrayp[12].methodp
#define LOGGER_CLASS_finest_METHODP                         marpaESLIFMethodCacheArrayp[13].methodp

/* We use genericLogger to generate strings -; */
typedef struct marpaESLIF_stringGenerator {
  char         *s;
  size_t        l;
  short         okb;
} marpaESLIF_stringGenerator_t;

/* Raise exception - I use a macro because I did not want to include stdarg in this file -; */
#define RAISEEXCEPTION(envp, fmts, ...) do {                            \
    if (EXCEPTION_CLASSP != NULL) {                                     \
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
          (*envp)->ThrowNew(envp, EXCEPTION_CLASSP, marpaESLIF_stringGenerator.s); \
        }                                                               \
        free(marpaESLIF_stringGenerator.s);                             \
        GENERICLOGGER_FREE(genericLoggerp);                             \
      }                                                                 \
    }                                                                   \
} while (0)

/* -------------- */
/* Static methods */
/* -------------- */
static void   generateStringWithLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static void   genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static void   updategenericLoggerContextv(JNIEnv *envp, jobject ESLIF_objectp);
static jchar *stringToUTF16(JNIEnv *envp, char *srcs, size_t *lenlp);

/*****************************************************************************/
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *JavaVMp, void* reservedp)
/*****************************************************************************/
{
  JNIEnv                  *envp;
  marpaESLIFClassCache_t  *marpaESLIFClassCachep  = marpaESLIFClassCacheArrayp;
  marpaESLIFMethodCache_t *marpaESLIFMethodCachep = marpaESLIFMethodCacheArrayp;

  /* It is safe to store JavaVMp in a global variable */
  g_vmp = JavaVMp;

  if (((*g_vmp)->GetEnv(g_vmp, (void **) &envp, MARPAESLIF_JNI_VERSION) != JNI_OK) || (envp == NULL)) {
    RAISEEXCEPTION(envp, "Failed to get environment at %s:%d", FILENAMES, __LINE__);
    goto err;
  }

  /* Get cache of classes */
  while (marpaESLIFClassCachep->classs != NULL) {
    marpaESLIFClassCachep->classp = (*envp)->FindClass(envp,
                                                       marpaESLIFClassCachep->classs);
    if (marpaESLIFClassCachep->classp == NULL) {
      RAISEEXCEPTION(envp, "Failed to find class \"%s\" at %s:%d", marpaESLIFClassCachep->classs, FILENAMES, __LINE__);
      goto err;
    }
    marpaESLIFClassCachep++;
  }

  /* Get cache of methods */
  while (marpaESLIFMethodCachep->classCachep != NULL) {
    marpaESLIFMethodCachep->methodp = (*envp)->GetMethodID(envp,
                                                           marpaESLIFMethodCachep->classCachep->classp,
                                                           marpaESLIFMethodCachep->methods,
                                                           marpaESLIFMethodCachep->signatures);
    if (marpaESLIFMethodCachep->methodp == NULL) {
      RAISEEXCEPTION(envp,
                     "Failed to find method \"%s\" with signature \"%s\" in class \"%s\" at %s:%d",
                     marpaESLIFMethodCachep->methods,
                     marpaESLIFMethodCachep->signatures,
                     marpaESLIFMethodCachep->classCachep->classs,
                     FILENAMES,
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
JNIEXPORT void JNICALL Java_org_marpa_ESLIF_jniInit(JNIEnv *envp, jobject ESLIF_objectp)
/*****************************************************************************/
{
  jobject                 ESLIFOption_objectp      = NULL;
  jobject                 Logger_objectp           = NULL;
  jobject                 directByteBuffer_objectp = NULL;
  genericLoggerContext_t *genericLoggerContextp    = NULL;

  marpaESLIF_t           *marpaESLIFp       = NULL;
  genericLogger_t        *genericLoggerp    = NULL;
  marpaESLIFOption_t      marpaESLIFOption;

  /* ------------------------- */
  /* Generic logger management */
  /* ------------------------- */
  /* If there is a logger, it is referenced by Java by ESLIFOption, itself referenced by ESLIF
     so we do not have to create a global reference ourself */
  ESLIFOption_objectp = (*envp)->CallObjectMethod(envp, ESLIF_objectp, ESLIF_CLASS_get_ESLIFOption_METHODP);
  if (ESLIFOption_objectp != NULL) {
    Logger_objectp = (*envp)->CallObjectMethod(envp, ESLIFOption_objectp, ESLIFOPTION_CLASS_get_logger_METHODP);
    if (Logger_objectp != NULL) {

      /* Create and save generic logger context address - the content of context is updated at every JNI call */
      genericLoggerContextp = (genericLoggerContext_t *) malloc(sizeof(genericLoggerContext_t));
      if (genericLoggerContextp == NULL) {
        RAISEEXCEPTION(envp, "malloc failure at %s:%d, %s", FILENAMES, __LINE__, strerror(errno));
        goto err;
      }
      genericLoggerContextp->Logger_objectp = NULL;
        
      directByteBuffer_objectp = (*envp)->NewDirectByteBuffer(envp, genericLoggerContextp, 1); /* Number of bytes is not relevant here */
      if (directByteBuffer_objectp == NULL) {
        RAISEEXCEPTION(envp, "NewDirectByteBuffer failure at %s:%d", FILENAMES, __LINE__);
        goto err;
      }
      (*envp)->CallVoidMethod(envp, ESLIFOption_objectp, ESLIFOPTION_CLASS_set_genericLoggerContextp_METHODP, directByteBuffer_objectp);
    
      /* Create and save generic logger address */
      genericLoggerp = genericLogger_newp(genericLoggerCallbackv, (void *) genericLoggerContextp, GENERICLOGGER_LOGLEVEL_TRACE);
      if (genericLoggerp == NULL) {
        RAISEEXCEPTION(envp, "genericLogger_newp failure at %s:%d, %s", FILENAMES, __LINE__, strerror(errno));
        goto err;
      }
      directByteBuffer_objectp = (*envp)->NewDirectByteBuffer(envp, genericLoggerp, 1); /* Same remark */
      if (directByteBuffer_objectp == NULL) {
        RAISEEXCEPTION(envp, "NewDirectByteBuffer failure at %s:%d", FILENAMES, __LINE__);
        goto err;
      }
      (*envp)->CallVoidMethod(envp, ESLIFOption_objectp, ESLIFOPTION_CLASS_set_genericLoggerp_METHODP, directByteBuffer_objectp);
    }
  }

  /* From now, at every invocation, we will update Logger_objectp in generic logger context */
  /* This is not necessary to redo this here, but let's be coherent and do it -; */
  updategenericLoggerContextv(envp, ESLIF_objectp);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  directByteBuffer_objectp = (*envp)->NewDirectByteBuffer(envp, marpaESLIFp, sizeof(marpaESLIF_t *)); /* Again, size does not really matter here */
  if (directByteBuffer_objectp == NULL) {
    RAISEEXCEPTION(envp, "NewDirectByteBuffer failure at %s:%d", FILENAMES, __LINE__);
    goto err;
  }
  (*envp)->CallVoidMethod(envp, ESLIF_objectp, ESLIF_CLASS_set_marpaESLIFp_METHODP, directByteBuffer_objectp);
    
  goto done;

 err:
  if (genericLoggerContextp != NULL) {
    free(genericLoggerContextp);
  }
  genericLogger_freev(&genericLoggerp); /* This is NULL protected */
  marpaESLIF_freev(marpaESLIFp); /* Ditto */

 done:
  return;
}

/*****************************************************************************/
JNIEXPORT void JNICALL Java_org_marpa_ESLIF_jniRelease(JNIEnv *envp, jobject ESLIF_objectp)
/*****************************************************************************/
{
  jobject                 directByteBuffer_objectp = NULL;
  jobject                 ESLIFOption_objectp      = NULL;
  jobject                 Logger_objectp           = NULL;
  genericLogger_t        *genericLoggerp           = NULL;
  genericLoggerContext_t *genericLoggerContextp    = NULL;
  marpaESLIF_t           *marpaESLIFp;

  updategenericLoggerContextv(envp, ESLIF_objectp);

  /* Get generic logger pointer and context pointer */
  ESLIFOption_objectp = (*envp)->CallObjectMethod(envp, ESLIF_objectp, ESLIF_CLASS_get_ESLIFOption_METHODP);
  if (ESLIFOption_objectp != NULL) {
    Logger_objectp = (*envp)->CallObjectMethod(envp, ESLIFOption_objectp, ESLIFOPTION_CLASS_get_logger_METHODP);
    if (Logger_objectp != NULL) {
      directByteBuffer_objectp = (*envp)->CallObjectMethod(envp, ESLIFOption_objectp, ESLIFOPTION_CLASS_get_genericLoggerp_METHODP);
      if (directByteBuffer_objectp != NULL) {
        genericLoggerp = (genericLogger_t *) (*envp)->GetDirectBufferAddress(envp, directByteBuffer_objectp);
      }
      directByteBuffer_objectp = (*envp)->CallObjectMethod(envp, ESLIFOption_objectp, ESLIFOPTION_CLASS_get_genericLoggerContextp_METHODP);
      if (directByteBuffer_objectp != NULL) {
        genericLoggerContextp = (genericLoggerContext_t *) (*envp)->GetDirectBufferAddress(envp, directByteBuffer_objectp);
      }
    }
  }

  /* Get marpaESLIF pointer */
  directByteBuffer_objectp = (*envp)->CallObjectMethod(envp, ESLIF_objectp, ESLIF_CLASS_get_marpaESLIFp_METHODP);
  if (directByteBuffer_objectp != NULL) {
    marpaESLIFp = (marpaESLIF_t *) (*envp)->GetDirectBufferAddress(envp, directByteBuffer_objectp);
    if (marpaESLIFp != NULL) {
      if (genericLoggerp != NULL) {
        GENERICLOGGER_DEBUGF(genericLoggerp, "Freeing marpaESLIFp at %p", marpaESLIFp);
      }
      marpaESLIF_freev(marpaESLIFp); /* marpaESLIF_freev is protected against NULL */
      (*envp)->CallVoidMethod(envp, ESLIF_objectp, ESLIF_CLASS_set_marpaESLIFp_METHODP, NULL);
    }
    if (genericLoggerp != NULL) {
      GENERICLOGGER_DEBUGF(genericLoggerp, "Freeing genericLoggerp at %p", genericLoggerp);
      GENERICLOGGER_FREE(genericLoggerp);
      (*envp)->CallVoidMethod(envp, ESLIFOption_objectp, ESLIFOPTION_CLASS_set_genericLoggerp_METHODP, NULL);
    }
    /* Hmm.... This one cannot be logged */
    if (genericLoggerContextp != NULL) {
      free(genericLoggerContextp);
      (*envp)->CallVoidMethod(envp, ESLIFOption_objectp, ESLIFOPTION_CLASS_set_genericLoggerContextp_METHODP, NULL);
    }
  }
}

/*****************************************************************************/
static void genericLoggerCallbackv(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  genericLoggerContext_t *genericLoggerContextp = (genericLoggerContext_t *) userDatavp;
  jobject                 Logger_objectp;
  JNIEnv                 *envp;
  jstring                 stringp;
  jchar                  *utf16p;
  size_t                  lenl;
  jsize                   utf16l;

  if (genericLoggerContextp == NULL) {
    return;
  }
  Logger_objectp = genericLoggerContextp->Logger_objectp;
  if (Logger_objectp == NULL) {
    return;
  } 
  
  /* Logging callack is never running in another thread - no need to attach */
  if (((*g_vmp)->GetEnv(g_vmp, (void **) &envp, JNI_VERSION_1_1) != JNI_OK) || (envp == NULL)) {
    return;
  }

  /* I do not think marpaESLIF ever log something else but ASCII characters, and if this is not the case... Tant pis */
  stringp = (*envp)->NewStringUTF(envp, msgs);
  if (stringp == NULL) {
    return;
  }

  utf16p = stringToUTF16(envp, (char *) msgs, &lenl);
  if ((utf16p == NULL) || (lenl < 2)) {
    return;
  }
  /* As far as I know marpaESLIF never log outside of BMP -; */
  utf16l = lenl / 2;
  stringp = (*envp)->NewString(envp, utf16p, utf16l);
  if (stringp == NULL) {
    return;
  }
    
  switch (logLeveli) {
  case GENERICLOGGER_LOGLEVEL_TRACE:
    (*envp)->CallVoidMethod(envp, Logger_objectp, LOGGER_CLASS_finest_METHODP, stringp);
    break;
  case GENERICLOGGER_LOGLEVEL_DEBUG:
    (*envp)->CallVoidMethod(envp, Logger_objectp, LOGGER_CLASS_finer_METHODP, stringp);
    break;
  case GENERICLOGGER_LOGLEVEL_INFO:
    (*envp)->CallVoidMethod(envp, Logger_objectp, LOGGER_CLASS_info_METHODP, stringp);
    break;
  case GENERICLOGGER_LOGLEVEL_NOTICE:
    (*envp)->CallVoidMethod(envp, Logger_objectp, LOGGER_CLASS_fine_METHODP, stringp);
    break;
  case GENERICLOGGER_LOGLEVEL_WARNING:
    (*envp)->CallVoidMethod(envp, Logger_objectp, LOGGER_CLASS_warning_METHODP, stringp);
    break;
  case GENERICLOGGER_LOGLEVEL_ERROR:
  case GENERICLOGGER_LOGLEVEL_CRITICAL:
  case GENERICLOGGER_LOGLEVEL_ALERT:
  case GENERICLOGGER_LOGLEVEL_EMERGENCY:
    (*envp)->CallVoidMethod(envp, Logger_objectp, LOGGER_CLASS_severe_METHODP, stringp);
    break;
  default:
    break;
  }
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
static void  updategenericLoggerContextv(JNIEnv *envp, jobject ESLIF_objectp)
/*****************************************************************************/
{
  jobject                 ESLIFOption_objectp      = NULL;
  jobject                 directByteBuffer_objectp = NULL;
  genericLoggerContext_t *genericLoggerContextp    = NULL;

  ESLIFOption_objectp = (*envp)->CallObjectMethod(envp, ESLIF_objectp, ESLIF_CLASS_get_ESLIFOption_METHODP);
  if (ESLIFOption_objectp != NULL) {
    directByteBuffer_objectp = (*envp)->CallObjectMethod(envp, ESLIFOption_objectp, ESLIFOPTION_CLASS_get_genericLoggerContextp_METHODP);
    if (directByteBuffer_objectp != NULL) {
      genericLoggerContextp = (genericLoggerContext_t *) (*envp)->GetDirectBufferAddress(envp, directByteBuffer_objectp);
      if (genericLoggerContextp != NULL) {
        genericLoggerContextp->Logger_objectp = (*envp)->CallObjectMethod(envp, ESLIFOption_objectp, ESLIFOPTION_CLASS_get_logger_METHODP);
      }
    }
  }
}

/*****************************************************************************/
static jchar *stringToUTF16(JNIEnv *envp, char *srcs, size_t *lenlp)
/*****************************************************************************/
{
  /* This is more or less a copy of _marpaESLIF_charconvp */
  char              *inbuforigp  = srcs;
  size_t             srcl        = strlen(srcs);
  size_t             inleftorigl = srcl;
  char              *outbuforigp = NULL;
  size_t             outbuforigl = 0;
  tconv_option_t     tconvOption = { NULL /* charsetp */, NULL /* convertp */, NULL /* traceCallbackp */, NULL /* traceUserDatavp */ };
  tconv_t            tconvp;
  char              *inbufp;
  size_t             inleftl;
  char              *outbufp;
  size_t             outleftl;
  size_t             nconvl;

  /* This is to avoid the BOM */
  tconvp = tconv_open_ext(IS_BIGENDIAN() ? "UTF-16BE" : "UTF-16LE", NULL, &tconvOption);
  if (tconvp == NULL) {
    RAISEEXCEPTION(envp, "tconv_open failure, %s at %s:%d", strerror(errno), FILENAMES, __LINE__);
    goto err;
  }

  /* We start with an output buffer of the same size of input buffer.                  */
  /* Whatever the destination encoding, we always reserve one byte more to place a NUL */
  /* just in case. This NUL is absolutetly harmless but is usefull if one want to look */
  /* at the variables via a debugger -;.                                               */
  /* It is more than useful when the destination encoding is ASCII: string will be NUL */
  /* terminated by default.                                                            */
  outbuforigp = (char *) malloc(srcl + 1);
  if (outbuforigp == NULL) {
    RAISEEXCEPTION(envp, "malloc failure, %s at %s:%d", strerror(errno), FILENAMES, __LINE__);
    goto err;
  }
  /* This setting is NOT necessary because *outbufp is always set to '\0' as well. But */
  /* I do this just to ease inspection in a debugger. */
  outbuforigp[srcl] = '\0';
  outbuforigl = srcl;

  /* We want to translate descriptions in trace or error cases - these are short things, and */
  /* it does not really harm if we redo the whole translation stuff in case of E2BIG:        */
  /* - in trace mode it is documented that performance is awful                              */
  /* - in error mode this call will happen once                                              */

  inbufp   = inbuforigp;
  inleftl  = inleftorigl;
  outbufp  = outbuforigp;
  outleftl = outbuforigl;
  
  while (1) {
  again:
    nconvl = tconv(tconvp, (inbufp != NULL) ? &inbufp : NULL, &inleftl, &outbufp, &outleftl);

    if (nconvl == (size_t) -1) {
      char  *tmps;
      size_t outleftdeltal;
      size_t outbufdeltal;
      
      /* The only acceptable error is E2BIG */
      if (errno != E2BIG) {
        RAISEEXCEPTION(envp, "tconv failure, %s at %s:%d", tconv_error(tconvp), FILENAMES, __LINE__);
	goto err;
      }
      /* Try to alloc more */
      if (outbuforigl > 0) {
        outbuforigl *= 2;
        outleftdeltal = outbuforigl;
      } else {
        outbuforigl = 1023;    /* Totally subjective number, just to avoid to do 1, then 2, then 4, then 8, then etc... */
        outleftdeltal = 1023;  /* when 1024 is suppoidly enough. */
      }
      outbufdeltal = outbufp - outbuforigp; /* Destination buffer is never NULL because of the +1 above */
      /* Will this ever happen ? */
      if (outbuforigl < srcl) {
        RAISEEXCEPTION(envp, "size_t flip at %s:%d", FILENAMES, __LINE__);
	goto err;
      }
      /* Note the "+ 1" */
      tmps = realloc(outbuforigp, outbuforigl + 1); /* Still the +1 to put a NUL just to ease debug of UTF-8 but also its makes sure that ASCII string are ALWAYS NUL terminated */
      if (tmps == NULL) {
        RAISEEXCEPTION(envp, "realloc failure, %s at %s:%d", tconv_error(tconvp), FILENAMES, __LINE__);
	goto err;
      }
      outbuforigp = tmps;
      outbuforigp[outbuforigl] = '\0';
      outleftl   += outleftdeltal;
      outbufp = outbuforigp + outbufdeltal;
      goto again;
    }

    if (inbufp == NULL) {
      /* This was the last round */
      break;
    }

    if (inleftl <= 0) {
      /* Next round is the last one */
      inbufp = NULL;
    }
  }

  /* Remember that we ALWAYS allocate one byte more. This mean that outbufp points exactly at this extra byte */
  *outbufp = '\0';

  if (lenlp != NULL) {
    *lenlp = (jsize) (outbufp - outbuforigp);
  }
  goto done;

 err:
  if (outbuforigp != NULL) {
    free(outbuforigp);
  }
  outbuforigp = NULL;

 done:
  if (tconvp != NULL) {
    if (tconv_close(tconvp) != 0) {
      RAISEEXCEPTION(envp, "tconv_close failure, %s at %s:%d", strerror(errno), FILENAMES, __LINE__);
    }
  }

  return (jchar *) outbuforigp;
}
