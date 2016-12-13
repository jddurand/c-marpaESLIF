/* Java JNI bindings for marpaESLIF                          */
#include <jni.h>
#include <marpaESLIF.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <genericLogger.h>

/* It is safe to store this value via JNI_OnLoad */
static JavaVM *g_vmp;

/* ---------------- */
/* Exported methods */
/* ---------------- */
JNIEXPORT jint    JNICALL JNI_OnLoad(JavaVM *vmp, void* reservedp);
JNIEXPORT jobject JNICALL Java_org_marpa_ESLIFParser_marpaESLIF_newp(JNIEnv *envp, jobject objp, jobject loggerp);

/* ------------------------------ */
/* Internal types and definitions */
/* ------------------------------ */
#define EXCEPTION_CLASS_STRING   "java/lang/Exception"
#define ESLIFPARSER_CLASS_STRING "org/marpa/ESLIFParser"
#define ESLIFPARSER_LOG_SIGNATURE "(Ljava/lang/String;)V"

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
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vmp, void* reservedp)
/*****************************************************************************/
{
  JNIEnv   *envp = NULL;
  jint      versioni;
  jclass    eslifParserClassp;
  jmethodID methodp;
  int       i;

  /* It is safe to store vmp in a global variable */
  g_vmp = vmp;

  if (((*vmp)->GetEnv(vmp, (void **) &envp, JNI_VERSION_1_1) != JNI_OK) || (envp == NULL)) {
    goto err;
  }

  eslifParserClassp = (*envp)->FindClass(envp, ESLIFPARSER_CLASS_STRING);
  if (eslifParserClassp == NULL) {
    raiseException(envp, "Failed to find class " ESLIFPARSER_CLASS_STRING);
    goto err;
  }
  for (i = 0; i < _MARPAESLIF_LOGLEVEL_MAX; i++) {
    methodp = (*envp)->GetMethodID(envp,
                                   eslifParserClassp,
                                   marpaESLIF_logMethod[i],
                                   ESLIFPARSER_LOG_SIGNATURE);
    if (methodp == NULL) {
      raiseException(envp, "Failed to find logging method in " ESLIFPARSER_CLASS_STRING);
      goto err;
    }

    marpaESLIF_logMethodp[i] = methodp;
  }

  versioni = JNI_VERSION_1_1;
  goto done;

 err:
  versioni = -1;

 done:
  return versioni;
}

/*****************************************************************************/
JNIEXPORT jobject JNICALL Java_org_marpa_ESLIFParser_marpaESLIF_newp(JNIEnv *envp, jobject objectp, jobject loggerp)
/*****************************************************************************/
{
  marpaESLIF_t           *marpaESLIFp           = NULL;
  genericLogger_t        *genericLoggerp        = NULL;
  jobject                 newDirectByteBufferp  = NULL;
  marpaESLIFOption_t      marpaESLIFOption;
 
  if (loggerp != NULL) {
    genericLoggerp = genericLogger_newp(genericLoggerCallback, (void *) objectp, GENERICLOGGER_LOGLEVEL_TRACE);
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

  newDirectByteBufferp = (*envp)->NewDirectByteBuffer(envp, marpaESLIFp, sizeof(marpaESLIF_t *));
  if (newDirectByteBufferp == NULL) {
    raiseException(envp, "NewDirectByteBuffer failure");
    goto err;
  }

  goto done;

 err:
  genericLogger_freev(&genericLoggerp);
  marpaESLIF_freev(marpaESLIFp);

 done:
  return newDirectByteBufferp;
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
