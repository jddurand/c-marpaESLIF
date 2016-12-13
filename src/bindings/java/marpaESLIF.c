/* Java JNI bindings for marpaESLIF */
#include <jni.h>
#include <marpaESLIF.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <genericLogger.h>

static JavaVM *g_vmp;

#define JAVA_UTIL_LOGGING_LOGGER_STRING                  "java/util/logging/Logger"
#define SEVERE_STRING                                    "severe"
#define JAVA_UTIL_LOGGING_LOGGER_SEVERE_SIGNATURE_STRING "(Ljava/lang/String;)V"

typedef struct genericLoggerContext       genericLoggerContext_t;
typedef enum   genericLoggerCallbackLevel genericLoggerCallbackLevel_t;

enum genericLoggerCallbackLevel {
  GENERICLOGGERCONTEXT_SEVERE,
  GENERICLOGGERCONTEXT_WARNING,
  GENERICLOGGERCONTEXT_INFO,
  GENERICLOGGERCONTEXT_FINE,
  GENERICLOGGERCONTEXT_FINER,
  GENERICLOGGERCONTEXT_FINEST,
  _GENERICLOGGERCONTEXT_MAX,
};

struct genericLoggerContext {
  genericLoggerContext_t *selfp;
  jmethodID               logMethodID[_GENERICLOGGERCONTEXT_MAX];
}

JNIEXPORT jobject JNICALL Java_org_marpa_ESLIFParser_marpaESLIF_newp(JNIEnv *envp, jobject objp, jobject loggerp);

static void  genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static short initLogging(genericLoggerCallbackContext_t *genericLoggerCallbackContextp);
static void  raiseException(char *msgs);

/*****************************************************************************/
jint JNI_OnLoad(JavaVM *vmp, void* reservedp)
/*****************************************************************************/
{
  /* It is safe to store vmp in a global variable */
  g_vmp = vmp;

  return JNI_VERSION_1_1;
}

/*****************************************************************************/
JNIEXPORT jobject JNICALL Java_org_marpa_ESLIFParser_marpaESLIF_newp(JNIEnv *envp, jobject objp, jobject loggerp)
/*****************************************************************************/
{
  marpaESLIF_t           *marpaESLIFp           = NULL;
  genericLoggerContext_t *genericLoggerContextp = NULL;
  genericLogger_t        *genericLoggerp        = NULL;
  jobject                 newDirectByteBufferp  = NULL;
  marpaESLIFOption_t      marpaESLIFOption;
 
  if (loggerp != NULL) {
    genericLoggerContextp = (genericLoggerContext_t *) malloc(sizeof(genericLoggerContext_t));
    if (genericLoggerContextp == NULL) {
      raiseException(strerror(errno));
      goto err;
    }
    if (! initLogging(genericLoggerContextp)) {
      goto err;
    }
    genericLoggerp = genericLogger_newp(genericLoggerCallback, (void *) genericLoggerContextp, GENERICLOGER_LOGLEVEL_TRACE);
    if (genericLoggerp == NULL) {
      raiseException(strerror(errno));
      goto err;
    }
  }

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  newDirectByteBufferp = NewDirectByteBuffer(envp, marpaESLIFp, sizeof(marpaESLIF_t));
  if (newDirectByteBufferp == NULL) {
    raiseException("NewDirectByteBuffer failure");
    goto err;
  }

  goto done;

 err:
  if (genericLoggerContextp == NULL) {
    free(genericLoggerContextp);
  }
  genericLogger_freev(&genericLoggerp);
  marpaESLIF_freev(marpaESLIFp);

 done:
  return newDirectByteBufferp;
}

/*****************************************************************************/
static void raiseException(char *msgs)
/*****************************************************************************/
{
  JNIEnv *envp       = NULL;
  jclass  exceptionp = NULL;

  if ((GetEnv(g_vmp, &envp, JNI_VERSION_1_1) != JNI_OK) || (envp == NULL)) {
    return;
  }

  exceptionp = FindClass(envp, "java/lang/Exception");
  if (exceptionp != NULL) {
    ThrowNew(envp, exceptionp, msgs);
  }
}

/*****************************************************************************/
static void genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  genericLoggerContext_t *genericLoggerContextp = (genericLoggerContext_t *) userDatavp;
  JNIEnv                 *envp                  = NULL;

  /* The logging callack is never running in another thread - no need to attach */
  if ((GetEnv(g_vmp, &envp, JNI_VERSION_1_1) != JNI_OK) || (envp == NULL)) {
    return;
  }

  switch (logLeveli) {
  case GENERICLOGGER_LOGLEVEL_TRACE:
    CallVoidMethod(envp, logger, log_method_table[level], NewStringUTF(envp, msgs));
    break;
  default:
    break;
  }
}

/*****************************************************************************/
static void initLogging(genericLoggerCallbackContext_t *genericLoggerCallbackContextp)
/*****************************************************************************/
{
  jclass loggerClassp;

  loggerClassp = FindClass(envp, JAVA_UTIL_LOGGING_LOGGER_STRING);
  if (loggerClassp == NULL) {
    raiseException("Failed to find " JAVA_UTIL_LOGGING_LOGGER_STRING);
  }

  genericLoggerContextp->logMethodID[GENERICLOGGERCONTEXT_SEVERE] = GetMethodID(envp, loggerClassp, SEVERE_STRING, JAVA_UTIL_LOGGING_LOGGER_SEVERE_SIGNATURE_STRING);
  if (genericLoggerContextp->logMethodID[GENERICLOGGERCONTEXT_SEVERE] == NULL) {
    raiseException("Failed to find " JAVA_UTIL_LOGGING_LOGGER_STRING " method " JAVA_UTIL_LOGGING_LOGGER_SEVERE_SIGNATURE_STRING);
  }
}
