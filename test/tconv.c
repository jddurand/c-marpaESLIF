/* A tconv version of iconv, using only tconv API */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <tconv.h>
#include <optparse.h>
#include <genericLogger.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif

#ifndef BUFSIZ
#define BUFSIZ 1024*1024
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

void traceCallback(void *userDatavp, const char *msgs);
void fileconvert(int outputFd, char *filenames, char *tocodes, char *fromcodes, short verbose);

int main(int argc, char **argv) {
  char                *fromcodes = NULL;
  char                *tocodes = NULL;
  char                *outputs = NULL;
  short                verbose = 0;
  short                help    = 0;
  int                  longindex = 0;
  struct optparse_long longopts[] = {
    {"from-code",  'f', OPTPARSE_OPTIONAL},
    {     "help",  'h', OPTPARSE_OPTIONAL},
    {   "output",  'o', OPTPARSE_OPTIONAL},
    {  "to-code",  't', OPTPARSE_REQUIRED},
    {  "verbose",  'v', OPTPARSE_OPTIONAL},
    {0}
  };
  char                *args;
  int                  outputFd;
  int                  option;
  struct optparse      options;

  optparse_init(&options, argv);
  while ((option = optparse_long(&options, longopts, &longindex)) != -1) {
    switch (option) {
    case 'f':
      fromcodes = options.optarg;
      fprintf(stderr," ==> %s\n", fromcodes);
      break;
    case 'h':
      help = 1;
      break;
    case 'o':
      outputs = options.optarg;
      break;
    case 't':
      tocodes = options.optarg;
      fprintf(stderr," ==> %s\n", tocodes);
      break;
    case 'v':
      verbose = 1;
      break;
    case '?':
      fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
      exit(EXIT_FAILURE);
    }
  }

      fprintf(stderr," ==> %s\n", fromcodes);
      fprintf(stderr," ==> %s\n", tocodes);
  if (help != 0) {
    fprintf(stderr,
	    "Usage:\n"
	    "\n"
	    "%s [--from-code fromcode] --to-code tocode [--output output] [--verbose] [--help]\n"
	    ,
	    argv[0]
	    );
    exit(EXIT_SUCCESS);
  }
  
  if (outputs != NULL) {
    outputFd = open(outputs, O_CREAT|O_WRONLY|O_TRUNC);
    if (outputFd < 0) {
      fprintf(stderr, "Failed to open %s: %s\n", outputs, strerror(errno));
      exit(EXIT_FAILURE);
    }
  } else {
    outputFd = fileno(stdout);
  }

  while ((args = optparse_arg(&options)) != NULL) {
    fileconvert(outputFd, args, tocodes, fromcodes, verbose);
  }

  if (outputs != NULL) {
    if (close(outputFd) != 0) {
      fprintf(stderr, "Failed to close %s: %s\n", outputs, strerror(errno));
    }
  }

  exit(EXIT_SUCCESS);
}

void fileconvert(int outputFd, char *filenames, char *tocodes, char *fromcodes, short verbose) {
  FILE          *fp     = NULL;
  tconv_t        tconvp = NULL;
  char           inbuf[BUFSIZ];
  char           outbuf[BUFSIZ];
  char          *inptr;
  char          *outptr;
  tconv_option_t tconvOption;
  size_t         inleft;
  size_t         outleft;
  size_t         nread;
  size_t         nconv;
  size_t         nwrite;

  fp = fopen(filenames, "rb");
  if (fp == NULL) {
    fprintf(stderr, "Failed to open %s: %s\n", filenames, strerror(errno));
    goto end;
  }

  tconvOption.charsetp = NULL;
  tconvOption.convertp = NULL;
  tconvOption.traceCallbackp = (verbose != 0) ? traceCallback : NULL;
  tconvOption.traceUserDatavp = NULL;
  
  tconvp = tconv_open_ext(tocodes, fromcodes, &tconvOption);
  if (tconvp == (tconv_t) -1) {
    fprintf(stderr, "tconv_open_ext: %s\n", tconv_error(tconvp));
    goto end;
  }

  if (verbose != 0) {
    tconv_trace_on(tconvp);
  }
  
  while (! feof(fp)) {
    nread = fread(inbuf, 1, BUFSIZ, fp);
    if (nread <= 0) {
      fprintf(stderr, "Failed to read from %s: %s\n", filenames, strerror(errno));
      goto end;
    }

    inptr   = inbuf;
    inleft  = nread;
    outptr  = outbuf;
    outleft = BUFSIZ;

    while (inleft > 0) {
      nconv = tconv(tconvp, &inptr, &inleft, &outptr, &outleft);
      if (nconv == (size_t) -1) {
	if (errno != EINVAL) {
	  fprintf(stderr, "%s: %s\n", filenames, strerror(errno));
	  goto end;
	}
      }
    }

    if (outleft < BUFSIZ) {
      nwrite = BUFSIZ - outleft;
      if (write(outputFd, outbuf, nwrite) != nwrite) {
	fprintf(stderr, "Failed to write output: %s\n", strerror(errno));
	goto end;
      }
    }
  }

  if (tconv_close(tconvp) != 0) {
    fprintf(stderr, "Failed to close tconv: %s\n", strerror(errno));
  }
  tconvp = NULL;

  end:
  if (fp != NULL) {
    if (fclose(fp) != 0) {
      fprintf(stderr, "Failed to close %s: %s\n", filenames, strerror(errno));
    }
  }
  if (tconvp != NULL) {
    if (tconv_close(tconvp) != 0) {
      fprintf(stderr, "Failed to close tconv: %s\n", strerror(errno));
    }
  }
}

void traceCallback(void *userDatavp, const char *msgs) {
  GENERICLOGGER_TRACE(NULL, msgs);
}

