#include "ft_ping.h"

int     error(void)
{
    return printf("ft_ping: usage error: Destination address required\n");
}

int help(void)
{
    printf("\
Usage \n\
  ft_ping [options] <destination> \n\
    \n\
Options: \n\
  <destination>      dns name or ip address \n\
  -h                 print help and exit \n\
  -v                 verbose output \n\
  -c <count>         stop after <count> replies \n\
  -d                 use SO_DEBUG socket option \n\
  -s <size>          use <size> as number of data bytes to be sent \n\
  -i <interval>      seconds between sending each packet \n\
  -t <ttl>           define time to live \n\
  -W <timeout>       time to wait for response \n");
    return 0;
}