#include "ft_ping.h"

t_stat global = {0};

void    handler_sig(int sig)
{
    if (sig == SIGINT)
        global.loop = 0;
    else if (sig == SIGALRM) {
        global.loop ^= 0b00000010;
    }
}

int     main(int ac, char **av)
{
    int opts[128] = {0};
    int arg = 0, sockfd = 0;
    char ip[INET_ADDRSTRLEN] = {0};
    struct sockaddr_in dst = {0};

    if (ac < 2) return error();
    if (getuid() != 0)
        return printf("ft_ping: You must be root to use ft_ping\n");

    global.loop = 0b00000011;
    signal(SIGINT, handler_sig);
    signal(SIGALRM, handler_sig);
    switch ((arg = check_arg(ac, av, opts))) {
        case -1:
            printf("ft_ping: invalid option\n");
            return help();
        case 0:
            return opts['h'] ? help() : error();
    }
    if ((sockfd = resolve_hostname(av[arg], opts, ip, &dst)) < 0)
        return printf("ft_ping: %s: Temporary failure in name resolution\n", av[arg]);
    ft_ping(sockfd, ip, opts, &dst);
}