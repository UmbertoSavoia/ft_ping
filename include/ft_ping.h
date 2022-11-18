#ifndef FT_PING_H
#define FT_PING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/icmp.h>

#define OPT_ARG     1
#define OPT_BOOL    2

#define DEFAULT_SIZE_DATA 56

typedef struct  s_check_opt
{
    char opt[3];
    char type;
}               t_check_opt;

int check_arg(int ac, char **av, int *opt);
int help(void);

#endif
