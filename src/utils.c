#include "ft_ping.h"

double  timeval_to_ms(struct timeval *t)
{
    return t->tv_sec + t->tv_usec / 1000.0;
}