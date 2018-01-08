#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <tconv.h>
#include <optparse.h>

static short producer(tconv_helper_t *tconv_helperp, void *voidp, char **bufpp, size_t *countlp);
static short byte_producer(tconv_helper_t *tconv_helperp, void *voidp, char **bufpp, size_t *countlp);
static short consumer(tconv_helper_t *tconv_helperp, void *voidp, char *bufp, size_t countl, size_t *countlp);
static short byte_consumer(tconv_helper_t *tconv_helperp, void *voidp, char *bufp, size_t countl, size_t *countlp);
static short my_tconv_helper(tconv_t tconvp, void *contextp, tconv_producer_t producerp, tconv_consumer_t consumerp);

char *datas    = "\xe4\xb8\xad\xe6\x96\x87\x00"; /* Chinese sample */
size_t datal   = 6;

/*****************************************************************************/
int main(int argc, char **argv)
/*****************************************************************************/
{
  char    *froms  = "UTF-8";
  char    *tos    = (argc >= 2) ? argv[1] : "UTF-32";
  tconv_t  tconvp = (tconv_t)-1;
  char    *p      = datas;
  int      exiti;

  tconvp = tconv_open(tos, froms);
  if (tconvp == (tconv_t)-1) {
    perror("tconv_open");
    goto err;
  }

  if (! tconv_helper(tconvp, NULL /* contextp */, producer, consumer)) {
    perror("tconv_helper");
    goto err;
  }

  if (! my_tconv_helper(tconvp, &p, byte_producer, byte_consumer)) {
    perror("my_tconv_helper");
    goto err;
  }

  exiti = 0;
  goto done;

 err:
  if (tconvp != (tconv_t) -1) {
    tconv_close(tconvp);
  }
  exiti = 1;

 done:
  exit(exiti);
}

/*****************************************************************************/
static short producer(tconv_helper_t *tconv_helperp, void *voidp, char **bufpp, size_t *countlp)
/*****************************************************************************/
{
  *bufpp   = datas;
  *countlp = datal;
  fprintf(stderr, "Produced %ld bytes\n", (unsigned long) *countlp);
  return tconv_helper_set_endb(tconv_helperp, 1);
}

/*****************************************************************************/
static short byte_producer(tconv_helper_t *tconv_helperp, void *voidp, char **bufpp, size_t *countlp)
/*****************************************************************************/
{
  char **pp = (char **) voidp;
  char  *p  = *pp;
  char   c;

  *bufpp   = p++;
  *countlp = 1;

  fprintf(stderr, "Produced 1 byte (0x%2x)\n", (unsigned char) **bufpp);

  c = *p;
  *pp = p;

  if (c == '\x00') {
    fprintf(stderr, "... Say stop\n");
    return tconv_helper_set_stopb(tconv_helperp, 1);
  } else {
    return 1;
  }
}

/*****************************************************************************/
static short consumer(tconv_helper_t *tconv_helperp, void *voidp, char *bufp, size_t countl, size_t *countlp)
/*****************************************************************************/
{
  *countlp = countl;
  fprintf(stderr, "Consumed %ld bytes from %ld\n", (unsigned long) *countlp, (unsigned long) countl);
  return 1;
}

/*****************************************************************************/
static short byte_consumer(tconv_helper_t *tconv_helperp, void *voidp, char *bufp, size_t countl, size_t *countlp)
/*****************************************************************************/
{
  *countlp = (countl > 0) ? 1 : 0;
  if (*countlp > 0) {
    fprintf(stderr, "Consumed 1 byte (0x%2x) from %ld\n", (unsigned char) *bufp, (unsigned long) countl);
  } else {
    fprintf(stderr, "Consumed 0 byte from %ld\n", (unsigned long) countl);
  }
  fprintf(stderr, "... Say pause\n");
  return tconv_helper_set_pauseb(tconv_helperp, 1);
}

/****************************************************************************/
static short my_tconv_helper(tconv_t tconvp, void *contextp, tconv_producer_t producerp, tconv_consumer_t consumerp)
/****************************************************************************/
{
  tconv_helper_t *tconv_helperp = NULL;
  short           endb;
  short           stopb;
  short           rcb;

  tconv_helperp = tconv_helper_newp(tconvp, contextp, producerp, consumerp);
  if (tconv_helperp == NULL) {
    perror("tconv_helper_newp");
    goto err;
  }

  if (! tconv_helper_runb(tconv_helperp)) {
    perror("tconv_helper_runb");
    goto err;
  }

  fprintf(stderr, "... Say reset\n");
  if (! tconv_helper_set_resetb(tconv_helperp, 1)) {
    perror("tconv_helper_set_resetb");
    goto err;
  }

  if (! tconv_helper_runb(tconv_helperp)) {
    perror("tconv_helper_runb");
    goto err;
  }

  if (! tconv_helper_runb(tconv_helperp)) {
    perror("tconv_helper_runb");
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  tconv_helper_freev(tconv_helperp);
  return rcb;
}
