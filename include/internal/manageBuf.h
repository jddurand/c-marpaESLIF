#ifndef MARPAWRAPPER_INTERNAL_MANAGEBUF_H
#define MARPAWRAPPER_INTERNAL_MANAGEBUF_H

#include <stddef.h> /* size_t definition */
#include "config.h"
#include "marpaWrapper/export.h"

#ifdef __cpluscplus
extern "C" {
#endif
  MARPAWRAPPER_NO_EXPORT void *manageBuf_createp(void ***ppp, size_t *sizeip, const size_t wantedNumberi, const size_t elementSizei);
  MARPAWRAPPER_NO_EXPORT void  manageBuf_freev  (void ***ppp, size_t *usedNumberip);
#ifdef __cpluscplus
}
#endif

#endif /* MARPAWRAPPER_INTERNAL_MANAGEBUF_H */

