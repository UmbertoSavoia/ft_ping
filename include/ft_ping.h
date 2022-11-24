#ifndef FT_PING_H
#define FT_PING_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <limits.h>
#include <signal.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <linux/icmp.h>
#include <linux/errqueue.h>

#define OPT_ARG     1
#define OPT_BOOL    2

#define DEFAULT_SIZE_DATA 56

typedef struct  s_check_opt
{
    char opt[3];
    char type;
}               t_check_opt;

typedef struct  s_stat
{
    uint8_t loop;
    char host[NI_MAXHOST];
    struct timeval start;
    uint32_t packet_sended;
    uint32_t packet_reiceved;
    long double max_time;
    long double min_time;
    long double avg;
    long double square;
}               t_stat;

extern t_stat global;

int             error(void);
int             check_arg(int ac, char **av, int *opt);
int             help(void);
int             resolve_hostname(const char *av, const int *opts, char *ip, struct sockaddr_in *dst);
void            ft_ping(int sockfd, const int *opts, struct sockaddr_in *dst);
long double     timeval_to_ms(struct timeval *t);

#endif
