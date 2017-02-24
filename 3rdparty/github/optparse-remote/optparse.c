#include "optparse.h"

#define MSG_INVALID "invalid option"
#define MSG_MISSING "option requires an argument"
#define MSG_TOOMANY "option takes no arguments"

#ifndef C_INLINE
#define C_INLINE
#endif

static int
opterror(struct optparse *options, const char *message, const char *data)
{
    const char *sep = " -- '";
    unsigned p = 0;
    while (*message)
        options->errmsg[p++] = *message++;
    while (*sep)
        options->errmsg[p++] = *sep++;
    while (p < sizeof(options->errmsg) - 2 && *data)
        options->errmsg[p++] = *data++;
    options->errmsg[p++] = '\'';
    options->errmsg[p++] = '\0';
    return '?';
}

void optparse_init(struct optparse *options, char **argv)
{
    options->argv = argv;
    options->permute = 1;
    options->optind = 1;
    options->subopt = 0;
    options->optarg = 0;
    options->errmsg[0] = '\0';
}

static C_INLINE int
is_dashdash(const char *arg)
{
    return arg != 0 && arg[0] == '-' && arg[1] == '-' && arg[2] == '\0';
}

static C_INLINE int
is_shortopt(const char *arg)
{
    return arg != 0 && arg[0] == '-' && arg[1] != '-' && arg[1] != '\0';
}

static C_INLINE int
is_longopt(const char *arg)
{
    return arg != 0 && arg[0] == '-' && arg[1] == '-' && arg[2] != '\0';
}

static void
permute(struct optparse *options, int index)
{
    int i;
    char *nonoption = options->argv[index];
    for (i = index; i < options->optind - 1; i++)
        options->argv[i] = options->argv[i + 1];
    options->argv[options->optind - 1] = nonoption;
}

static int
argtype(const char *optstring, char c)
{
    int count;
    if (c == ':')
        return -1;
    for (; *optstring && c != *optstring; optstring++);
    if (!*optstring)
        return -1;
    count = OPTPARSE_NONE;
    if (optstring[1] == ':')
        count += optstring[2] == ':' ? 2 : 1;
    return count;
}

int optparse(struct optparse *options, const char *optstring)
{
    char *option;
    int type;
    char *next;
    options->errmsg[0] = '\0';
    options->optopt = 0;
    options->optarg = 0;
    option = options->argv[options->optind];
    if (option == 0) {
        return -1;
    } else if (is_dashdash(option)) {
        options->optind++; /* consume "--" */
        return -1;
    } else if (!is_shortopt(option)) {
        if (options->permute) {
            int r;
            int index = options->optind;
            options->optind++;
            r = optparse(options, optstring);
            permute(options, index);
            options->optind--;
            return r;
        } else {
            return -1;
        }
    }
    option += options->subopt + 1;
    options->optopt = option[0];
    type = argtype(optstring, option[0]);
    next = options->argv[options->optind + 1];
    switch (type) {
    case -1: {
        char str[2] = {option[0]};
        options->optind++;
        return opterror(options, MSG_INVALID, str);
    }
    case OPTPARSE_NONE:
        if (option[1]) {
            options->subopt++;
        } else {
            options->subopt = 0;
            options->optind++;
        }
        return option[0];
    case OPTPARSE_REQUIRED:
        options->subopt = 0;
        options->optind++;
        if (option[1]) {
            options->optarg = option + 1;
        } else if (next != 0) {
            options->optarg = next;
            options->optind++;
        } else {
            char str[2] = {option[0]};
            options->optarg = 0;
            return opterror(options, MSG_MISSING, str);
        }
        return option[0];
    case OPTPARSE_OPTIONAL:
        options->subopt = 0;
        options->optind++;
        if (option[1])
            options->optarg = option + 1;
        else
            options->optarg = 0;
        return option[0];
    }
    return 0;
}

char *optparse_arg(struct optparse *options)
{
    char *option;
    options->subopt = 0;
    option = options->argv[options->optind];
    if (option != 0)
        options->optind++;
    return option;
}

static C_INLINE int
longopts_end(const struct optparse_long *longopts, int i)
{
    return !longopts[i].longname && !longopts[i].shortname;
}

static void
optstring_from_long(const struct optparse_long *longopts, char *optstring)
{
    int i, a;
    char *p = optstring;
    for (i = 0; !longopts_end(longopts, i); i++) {
        if (longopts[i].shortname) {
            *p++ = longopts[i].shortname;
            for (a = 0; a < (int)longopts[i].argtype; a++)
                *p++ = ':';
        }
    }
    *p = '\0';
}

/* Unlike strcmp(), handles options containing "=". */
static int
longopts_match(const char *longname, const char *option)
{
    const char *a = option, *n = longname;
    if (longname == 0)
        return 0;
    for (; *a && *n && *a != '='; a++, n++)
        if (*a != *n)
            return 0;
    return *n == '\0' && (*a == '\0' || *a == '=');
}

/* Return the part after "=", or NULL. */
static char *
longopts_arg(char *option)
{
    for (; *option && *option != '='; option++);
    if (*option == '=')
        return option + 1;
    else
        return 0;
}

static int
long_fallback(struct optparse *options,
              const struct optparse_long *longopts,
              int *longindex)
{
    int result, i;
    char optstring[96 * 3 + 1]; /* 96 ASCII printable characters */
    optstring_from_long(longopts, optstring);
    result = optparse(options, optstring);
    if (longindex != 0) {
        *longindex = -1;
        if (result != -1)
            for (i = 0; !longopts_end(longopts, i); i++)
                if (longopts[i].shortname == options->optopt)
                    *longindex = i;
    }
    return result;
}

int
optparse_long(struct optparse *options,
              const struct optparse_long *longopts,
              int *longindex)
{
    int i;
    char *option = options->argv[options->optind];
    if (option == 0) {
        return -1;
    } else if (is_dashdash(option)) {
        options->optind++; /* consume "--" */
        return -1;
    } else if (is_shortopt(option)) {
        return long_fallback(options, longopts, longindex);
    } else if (!is_longopt(option)) {
        if (options->permute) {
            int r, index = options->optind;
            options->optind++;
            r = optparse_long(options, longopts, longindex);
            permute(options, index);
            options->optind--;
            return r;
        } else {
            return -1;
        }
    }

    /* Parse as long option. */
    options->errmsg[0] = '\0';
    options->optopt = 0;
    options->optarg = 0;
    option += 2; /* skip "--" */
    options->optind++;
    for (i = 0; !longopts_end(longopts, i); i++) {
        const char *name = longopts[i].longname;
        if (longopts_match(name, option)) {
            char *arg;
            if (longindex)
                *longindex = i;
            options->optopt = longopts[i].shortname;
            arg = longopts_arg(option);
            if (longopts[i].argtype == OPTPARSE_NONE && arg != 0) {
                return opterror(options, MSG_TOOMANY, name);
            } if (arg != 0) {
                options->optarg = arg;
            } else if (longopts[i].argtype == OPTPARSE_REQUIRED) {
                options->optarg = options->argv[options->optind++];
                if (options->optarg == 0)
                    return opterror(options, MSG_MISSING, name);
            }
            return options->optopt;
        }
    }
    return opterror(options, MSG_INVALID, option);
}
