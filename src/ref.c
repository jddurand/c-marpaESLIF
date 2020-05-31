/* *************************************************** */
/* Reference counter methods is the sqme for any objet */
/* *************************************************** */
#define MARPAESLIF_REFB(type)						\
  short type##_refb(type##_t *p, int *refip) {				\
    if (p == NULL) {							\
      errno = EINVAL;							\
      return 0;								\
    }									\
									\
    if (refip != NULL) {						\
      *refip = p->refi;							\
    }									\
									\
    return 1;								\
  }

#define MARPAESLIF_REF_INCB(type)					\
  short type##_ref_incb(type##_t *p, int *refip) {			\
    int refi; /* For turnaround */					\
    									\
    if (p == NULL) {							\
      errno = EINVAL;							\
      return 0;								\
    }									\
    									\
    refi = p->refi;							\
    if (++refi < p->refi) {						\
      /* Turnaround -; */						\
      errno = ENOSYS;							\
      return 0;								\
    }									\
    									\
    p->refi = refi;							\
    if (refip != NULL) {						\
      *refip = p->refi;							\
    }									\
									\
    return 1;								\
  }

#define MARPAESLIF_REF_DECB(type)					\
  short type##_ref_decb(type##_t *p, int *refip) {			\
    int refi; /* For turnaround */					\
    									\
    if (p == NULL) {							\
      errno = EINVAL;							\
      return 0;								\
    }									\
    									\
    refi = p->refi;							\
    if (--refi > p->refi) {						\
      /* Turnaround -; */						\
      errno = ENOSYS;							\
      return 0;								\
    }									\
    									\
    p->refi = refi;							\
    if (refip != NULL) {						\
      *refip = p->refi;							\
    }									\
									\
    return 1;								\
  }

MARPAESLIF_REFB(marpaESLIFGrammar)
MARPAESLIF_REF_INCB(marpaESLIFGrammar)
MARPAESLIF_REF_DECB(marpaESLIFGrammar)

MARPAESLIF_REFB(marpaESLIFRecognizer)
MARPAESLIF_REF_INCB(marpaESLIFRecognizer)
MARPAESLIF_REF_DECB(marpaESLIFRecognizer)

MARPAESLIF_REFB(marpaESLIFValue)
MARPAESLIF_REF_INCB(marpaESLIFValue)
MARPAESLIF_REF_DECB(marpaESLIFValue)
