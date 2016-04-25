/* A tconv version of iconv, using only tconv API */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
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

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef O_BINARY
#ifdef _O_BINARY
#define O_BINARY _O_BINARY
#endif
#endif

void traceCallback(void *userDatavp, const char *msgs);
void fileconvert(int outputFd, char *filenames, char *tocodes, char *fromcodes, size_t bufsizel, short verbose);

int main(int argc, char **argv) {
  int                  longindex  = 0;

  char                *fromcodes  = NULL;
  short                help       = 0;
  char                *outputs    = NULL;
  size_t              bufsizel    = 1024;
  char                *tocodes    = NULL;
  short                verbose    = 0;
  struct optparse_long longopts[] = {
    {"from-code",  'f', OPTPARSE_REQUIRED},
    {     "help",  'h', OPTPARSE_OPTIONAL},
    {   "output",  'o', OPTPARSE_REQUIRED},
    {  "bufsize",  's', OPTPARSE_REQUIRED},
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
      break;
    case 'h':
      help = 1;
      break;
    case 'o':
      outputs = options.optarg;
      break;
    case 's':
      bufsizel = atoi(options.optarg);
      break;
    case 't':
      tocodes = options.optarg;
      break;
    case 'v':
      verbose = 1;
      break;
    case '?':
      fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
      exit(EXIT_FAILURE);
    }
  }

  if ((help != 0) || (tocodes == NULL) || (bufsizel <= 0)) {
    int rci = (help != 0) ? EXIT_SUCCESS : EXIT_FAILURE;
    fprintf(stderr,
	    "Usage:\n"
	    "\n"
	    "%s [--from-code fromcode] --to-code tocode [--output output] [--verbose] [--help]\n"
	    ,
	    argv[0]
	    );
    exit(rci);
  }
  
  if (outputs != NULL) {
    outputFd = open(outputs, O_RDWR|O_CREAT|O_TRUNC
#ifdef O_BINARY
                    |O_BINARY
#endif
                    );
    if (outputFd < 0) {
      fprintf(stderr, "Failed to open %s: %s\n", outputs, strerror(errno));
      exit(EXIT_FAILURE);
    }
  } else {
    outputFd = fileno(stdout);
  }

  while ((args = optparse_arg(&options)) != NULL) {
    fileconvert(outputFd, args, tocodes, fromcodes, bufsizel, verbose);
  }

  if (outputs != NULL) {
    if (close(outputFd) != 0) {
      fprintf(stderr, "Failed to close %s: %s\n", outputs, strerror(errno));
    }
  }

  exit(EXIT_SUCCESS);
}

void fileconvert(int outputFd, char *filenames, char *tocodes, char *fromcodes, size_t bufsizel, short verbose) {
  char           *inbufp  = NULL, *inbuforigp  = NULL, **inbufpp = NULL;
  char           *outbufp = NULL, *outbuforigp = NULL;
  size_t          inleftl  = 0, insizel  = 0, inleftorigl = 0, *inleftlp = NULL;
  size_t          outleftl = 0, outsizel = 0;
  tconv_t         tconvp = NULL;
  int             fd;
  tconv_option_t  tconvOption;
  size_t          nconvl;
  size_t          nwritel;
  int             eofb = 0;

  inbuforigp = malloc(bufsizel);
  if (inbuforigp == NULL) {
    fprintf(stderr, "malloc: %s\n", strerror(errno));
    goto end;
  }
  insizel = bufsizel;

  /* We start with an outbuf size the same as inbuf */
  outbuforigp = malloc(bufsizel);
  if (outbuforigp == NULL) {
    fprintf(stderr, "malloc: %s\n", strerror(errno));
    goto end;
  }
  outsizel = bufsizel;

  fd = open(filenames, O_RDONLY);
  if (fd < 0) {
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

  while (eofb == 0) {

    inleftorigl = read(fd, inbuforigp, insizel);
    if (inleftorigl < 0) {
      fprintf(stderr, "Failed to read from %s: %s\n", filenames, strerror(errno));
      goto end;
    }
    eofb = (inleftorigl == 0) ?  1 : 0;
    inbufpp     = eofb ? NULL : &inbufp;
    inleftlp    = eofb ? NULL : &inleftl;
    inbufp   = inbuforigp;
    inleftl  = inleftorigl;
    outbufp  = outbuforigp;
    outleftl = outsizel;

    while ((eofb != 0) || (inleftl > 0)) {

      nconvl = tconv(tconvp, inbufpp, inleftlp, &outbufp, &outleftl);
      if (nconvl == (size_t) -1) {
	switch (errno) {
	case E2BIG:
	  {
	    char *tmp;
	    
	    outsizel *= 2;
	    tmp = realloc(outbuforigp, outsizel);
	    if (tmp == NULL) {
	      fprintf(stderr, "realloc: %s\n", strerror(errno));
	      goto end;
	    }
	    outbuforigp  = tmp;

	    inbufp   = inbuforigp;
	    inleftl  = inleftorigl;
	    outbufp  = outbuforigp;
	    outleftl = outsizel;
	  }
	  break;
	default:
	  fprintf(stderr, "%s: %s\n", filenames, strerror(errno));
	  goto end;
	}
      }

      if ((eofb != 0) && (nconvl >= 0)) {
	/* Flush at EOF is ok */
	break;
      }
    }

    if (outleftl < outsizel) {
      nwritel = outsizel - outleftl;
      if (write(outputFd, outbuforigp, nwritel) != nwritel) {
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
  if (fd >= 0) {
    if (close(fd) != 0) {
      fprintf(stderr, "Failed to close %s: %s\n", filenames, strerror(errno));
    }
  }
  if (tconvp != NULL) {
    if (tconv_close(tconvp) != 0) {
      fprintf(stderr, "Failed to close tconv: %s\n", strerror(errno));
    }
  }
  if (outbuforigp != NULL) {
    free(outbuforigp);
  }
  if (inbuforigp != NULL) {
    free(inbuforigp);
  }
}

void traceCallback(void *userDatavp, const char *msgs) {
  GENERICLOGGER_TRACE(NULL, msgs);
}

