/* Java JNI bindings for marpaESLIF                          */
#include <jni.h>
#include <marpaESLIF.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <genericLogger.h>

/* ------------------------------------- */
/* Handy defines to not mess with naming */
/* ------------------------------------- */
#define MARPAESLIF_FINDCLASS(envp, classp, strings) do {                \
    classp = (*envp)->FindClass(envp, strings);                         \
    if (classp == NULL) {                                               \
      raiseException(envp, "Failed to find class " strings);            \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define MARPAESLIF_GETMETHOD(envp, methodp, classp, string, signature) do { \
    methodp = (*envp)->GetMethodID(envp, classp, string, signature);    \
    if (methodp == NULL) {                                              \
      raiseException(envp, "Failed to find method " string " with signature " signature); \
      goto err;                                                         \
    }                                                                   \
  } while (0)

#define MARPAESLIF_OBJ2CLASS(envp, classp, objectp) do {                \
    classp = (*envp)->GetObjectClass(envp, objectp);                    \
    if (classp == NULL) {                                               \
      raiseException(envp, "Failed to get class from object");          \
      goto err;                                                         \
    }                                                                   \
  } while (0)

/* ---------------- */
/* Global variables */
/* ---------------- */
/* It is safe to store this value via JNI_OnLoad */
static JavaVM *g_vmp;

/* ---------------- */
/* Exported methods */
/* ---------------- */
JNIEXPORT jint    JNICALL JNI_OnLoad(JavaVM *vmp, void* reservedp);
JNIEXPORT jobject JNICALL Java_org_marpa_ESLIF_newp(JNIEnv *envp, jobject ESLIF_objectp);

/* ------------------------------ */
/* Internal types and definitions */
/* ------------------------------ */
#define EXCEPTION_CLASS_STRING   "java/lang/Exception"
#define ESLIF_CLASS_STRING "org/marpa/ESLIF"
#define ESLIF_LOG_SIGNATURE "(Ljava/lang/String;)V"

#define GET_ESLIFOPTION_STRING    "get_ESLIFOption"
#define GET_ESLIFOPTION_SIGNATURE "()Lorg/marpa/ESLIFOption;"

typedef struct marpaESLIF_genericLoggerContext marpaESLIF_genericLoggerContext_t;

enum marpaESLIF_genericLoggerLevel {
  MARPAESLIF_LOGLEVEL_TRACE = 0,
  MARPAESLIF_LOGLEVEL_DEBUG,
  MARPAESLIF_LOGLEVEL_INFO,
  MARPAESLIF_LOGLEVEL_NOTICE,
  MARPAESLIF_LOGLEVEL_WARNING,
  MARPAESLIF_LOGLEVEL_ERROR,
  MARPAESLIF_LOGLEVEL_CRITICAL,
  MARPAESLIF_LOGLEVEL_ALERT,
  MARPAESLIF_LOGLEVEL_EMERGENCY,
  _MARPAESLIF_LOGLEVEL_MAX
};

const static char *marpaESLIF_logMethod[_MARPAESLIF_LOGLEVEL_MAX] = {
  "trace", "debug", "info", "notice", "warning", "error", "critical", "alert", "emergency"
};
static jmethodID marpaESLIF_logMethodp[_MARPAESLIF_LOGLEVEL_MAX];

/* -------------- */
/* Static methods */
/* -------------- */
static void  genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static void  raiseException(JNIEnv *envp, char *msgs);

/*****************************************************************************/
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *JavaVMp, void* reservedp)
/*****************************************************************************/
{
  /* It is safe to store JavaVMp in a global variable */
  g_vmp = JavaVMp;

  return JNI_VERSION_1_1;
}

/*****************************************************************************/
JNIEXPORT jobject JNICALL Java_org_marpa_ESLIF_jni_init(JNIEnv *envp, jobject ESLIF_objectp)
/*****************************************************************************/
{
  jclass                  ESLIF_classp             = NULL;
  jmethodID               get_ESLIFOption_methodp  = NULL;
  jobject                 ESLIFOption_objectp      = NULL;
  jobject                 directByteBuffer_objectp = NULL;

  marpaESLIF_t           *marpaESLIFp       = NULL;
  genericLogger_t        *genericLoggerp    = NULL;
  marpaESLIFOption_t      marpaESLIFOption;

  MARPAESLIF_OBJ2CLASS(envp, ESLIF_classp, ESLIFOption_objectp);
  MARPAESLIF_GETMETHOD(envp, get_ESLIFOption_methodp, ESLIF_classp, GET_ESLIFOPTION_STRING, GET_ESLIFOPTION_SIGNATURE);
  ESLIFOption_objectp = (*envp)->CallObjectMethod(envp, ESLIF_objectp, get_ESLIFOption_methodp);

  if (ESLIFOption_objectp != NULL) {
    genericLoggerp = genericLogger_newp(genericLoggerCallback, (void *) ESLIFOption_objectp, GENERICLOGGER_LOGLEVEL_TRACE);
    if (genericLoggerp == NULL) {
      raiseException(envp, strerror(errno));
      goto err;
    }
  }

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  directByteBuffer_objectp = (*envp)->NewDirectByteBuffer(envp, marpaESLIFp, sizeof(marpaESLIF_t *));
  if (directByteBuffer_objectp == NULL) {
    raiseException(envp, "NewDirectByteBuffer failure");
    goto err;
  }

  goto done;

 err:
  genericLogger_freev(&genericLoggerp);
  marpaESLIF_freev(marpaESLIFp);

 done:
  return directByteBuffer_objectp;
}

/*****************************************************************************/
static void raiseException(JNIEnv *envp, char *msgs)
/*****************************************************************************/
{
  jclass exceptionp = NULL;

  exceptionp = (*envp)->FindClass(envp, EXCEPTION_CLASS_STRING);
  if (exceptionp != NULL) {
    (*envp)->ThrowNew(envp, exceptionp, msgs);
  }
}

/*****************************************************************************/
static void genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  jobject marpaESLIFp = (jobject) userDatavp;
  JNIEnv *envp        = NULL;

  /* Logging callack is never running in another thread - no need to attach */
  if (((*g_vmp)->GetEnv(g_vmp, (void **) &envp, JNI_VERSION_1_1) != JNI_OK) || (envp == NULL)) {
    return;
  }

  switch (logLeveli) {
  case GENERICLOGGER_LOGLEVEL_TRACE:
    (*envp)->CallVoidMethod(envp, marpaESLIFp, marpaESLIF_logMethodp[MARPAESLIF_LOGLEVEL_TRACE], (*envp)->NewStringUTF(envp, msgs));
    break;
  default:
    break;
  }
}
