/* This file is used after regeneration of libmarpa from the 'tested' branch: */
/*
  make clean dist
  cp -p work/stage/*.[ch] ../c-marpaWrapper/libmarpa/work/stage
  #
  # Bad luck it generates a config.h file that will conflict with our config.h
  #
  rm ../c-marpaWrapper/libmarpa/work/stage/config.h
*/

#define MARPA_LINKAGE static
#define MARPA_AVL_LINKAGE static
#define MARPA_TAVL_LINKAGE static
#define MARPA_OBS_LINKAGE static

#include "../libmarpa/work/stage/marpa_ami.c"
#include "../libmarpa/work/stage/marpa_avl.c"
#include "../libmarpa/work/stage/marpa.c"
#include "../libmarpa/work/stage/marpa_codes.c"
#include "../libmarpa/work/stage/marpa_obs.c"
#include "../libmarpa/work/stage/marpa_tavl.c"
#include "../src/asf.c"
#include "../src/grammar.c"
#include "../src/recognizer.c"
#include "../src/value.c"
