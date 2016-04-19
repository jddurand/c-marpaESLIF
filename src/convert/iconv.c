#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iconv.h>

#include "convert/iconv.h"

/* Our context is iconv itself */
typedef iconv_t tconv_convert_iconv_context_t;

#define TCONV_CONVERT_ICONV_TRANSLIT "//TRANSLIT"
#define TCONV_CONVERT_ICONV_IGNORE "//IGNORE"

/*****************************************************************************/
void  *tconv_convert_iconv_open(const char *tocodes, const char *fromcodes, void *voidp)
/*****************************************************************************/
{
  tconv_convert_iconv_t *optionp     = (tconv_convert_iconv_t *) voidp;
  char                  *realtocodes = NULL;
  char                  *p;
  iconv_t                iconvp;

  
  if ((optionp == NULL) || (tocodes == NULL) || (fromcodes == NULL)) {
    goto err;
  }

  /* //TRANSLIT and //IGNORE are managed via the options */
  realtocodes = strdup(tocodes);
  if (realtocodes == NULL) {
    goto err;
  }
  p = strchr(realtocodes, '/');
  if (p != NULL) {
    *p = '\0';
  }
  if (optionp->translitb != 0) {
    p = realloc(realtocodes, strlen(realtocodes) + strlen(TCONV_CONVERT_ICONV_TRANSLIT) + 1);
    if (p == NULL) {
      goto err;
    }
    realtocodes = p;
    strcat(realtocodes, TCONV_CONVERT_ICONV_TRANSLIT);
  }
  if (optionp->ignoreb != 0) {
    p = realloc(realtocodes, strlen(realtocodes) + strlen(TCONV_CONVERT_ICONV_IGNORE) + 1);
    if (p == NULL) {
      goto err;
    }
    realtocodes = p;
    strcat(realtocodes, TCONV_CONVERT_ICONV_IGNORE);
  }

  iconvp = iconv_open(realtocodes, fromcodes);
  if (iconvp == NULL) {
    goto err;
  }

  return iconvp;
  
 err:
  {
    int errnol = errno;
    if (realtocodes != NULL) { free(realtocodes); }
    errno = errnol;
  }
  return NULL;
}

/*****************************************************************************/
size_t tconv_convert_iconv_run(void *contextp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  iconv_t iconvp = (iconv_t) contextp;

  if (iconvp == NULL) {
    goto err;
  }

  return iconv(iconvp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);

 err:
  errno = EINVAL;
  return (size_t)-1;
}

/*****************************************************************************/
int tconv_convert_iconv_close(void *contextp)
/*****************************************************************************/
{
  iconv_t iconvp = (iconv_t) contextp;

  if (iconvp == NULL) {
    goto err;
  }

  return iconv_close(iconvp);

 err:
  errno = EINVAL;
  return -1;
}

