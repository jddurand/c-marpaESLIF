#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <tconv.h>
#include <optparse.h>

static short producer(tconv_helper_t *tconv_helperp, void *voidp, char **bufpp, size_t *countlp);
static short consumer(tconv_helper_t *tconv_helperp, void *voidp, char *bufp, size_t countl, size_t *countlp);
static short my_byte_producer(tconv_helper_t *tconv_helperp, void *voidp, char **bufpp, size_t *countlp);
static short my_byte_consumer(tconv_helper_t *tconv_helperp, void *voidp, char *bufp, size_t countl, size_t *countlp);
static short my_tconv_helper(tconv_t tconvp, void *contextp, tconv_producer_t producerp, tconv_consumer_t consumerp);

char *datas    = "\xe4\xb8\xad\xe6\x96\x87\x00"; /* Chinese sample */
size_t datal   = 6;

/*****************************************************************************/
int main(int argc, char **argv)
/*****************************************************************************/
{
  char           *froms  = "UTF-8";
  char           *tos    = (argc >= 2) ? argv[1] : "UTF-32";
  tconv_t         tconvp = (tconv_t)-1;
  tconv_helper_t *tconv_helperp = NULL;
  char           *p      = datas;
  int             exiti;

  tconvp = tconv_open(tos, froms);
  if (tconvp == (tconv_t)-1) {
    perror("tconv_open");
    goto err;
  }

  tconv_helperp = tconv_helper_newp(tconvp, NULL /* contextp */, producer, consumer);
  if (tconv_helperp == NULL) {
    goto err;
  }
  
  if (! tconv_helper_runb(tconv_helperp)) {
    perror("tconv_helper");
    goto err;
  }

  if (! my_tconv_helper(tconvp, &p, my_byte_producer, my_byte_consumer)) {
    perror("my_tconv_helper");
    goto err;
  }

  exiti = 0;
  goto done;

 err:
  if (tconv_helperp != NULL) {
    tconv_helper_freev(tconv_helperp);
  }
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
  fprintf(stderr, "... Say end\n");
  return tconv_helper_endb(tconv_helperp);
}

/*****************************************************************************/
static short my_byte_producer(tconv_helper_t *tconv_helperp, void *voidp, char **bufpp, size_t *countlp)
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
    return tconv_helper_stopb(tconv_helperp);
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
static short my_byte_consumer(tconv_helper_t *tconv_helperp, void *voidp, char *bufp, size_t countl, size_t *countlp)
/*****************************************************************************/
{
  *countlp = (countl > 0) ? 1 : 0;
  if (*countlp > 0) {
    fprintf(stderr, "Consumed 1 byte (0x%2x) from %ld\n", (unsigned char) *bufp, (unsigned long) countl);
  } else {
    fprintf(stderr, "Consumed 0 byte from %ld\n", (unsigned long) countl);
  }
  fprintf(stderr, "... Say pause\n");
  return tconv_helper_pauseb(tconv_helperp);
}

/****************************************************************************/
static short my_tconv_helper(tconv_t tconvp, void *contextp, tconv_producer_t producerp, tconv_consumer_t consumerp)
/****************************************************************************/
{
  tconv_helper_t *tconv_helperp = NULL;
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
