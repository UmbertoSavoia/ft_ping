#include "ft_ping.h"

long double  timeval_to_ms(struct timeval *t)
{
    return (t->tv_sec * (uint64_t)1000) + (t->tv_usec / 1000.0);
}