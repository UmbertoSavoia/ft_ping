#ifndef FT_PING_H
#define FT_PING_H

#include <stdio.h>
#include <string.h>

#define OPT_ARG     1
#define OPT_BOOL    2

typedef struct  s_check_opt
{
    char opt[3];
    char type;
}               t_check_opt;

int     check_arg(int ac, char **av, char *opt);

#endif
