#include <stdlib.h>
#include "tconv.h"

int main(int argc, char **argv)
{
  tconv_t tconvp;

  tconvp = tconv_open(NULL, NULL);
  tconv_close(tconvp);

  return 0;
}
