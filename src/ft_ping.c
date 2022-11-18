#include "ft_ping.h"

int     error(void)
{
    return printf("ft_ping: usage error: Destination address required\n");
}

int    resolve_hostname(char *av, int *opts)
{
    int error = 0;
    int sockfd = 0;
    char buf[INET_ADDRSTRLEN] = {0};
    struct addrinfo hints = {0};
    struct addrinfo *addrinfo_list = 0, *tmp = 0;
    struct sockaddr_storage addr;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;
    hints.ai_flags = AI_CANONNAME;

    if ((error = getaddrinfo(av, 0, &hints, &addrinfo_list))) {
        printf("error: %s\n", gai_strerror(error));
        return -1;
    }
    for (tmp = addrinfo_list; tmp; tmp = tmp->ai_next) {
        if ((sockfd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol)) >= 0)
            break;
    }
    if (sockfd < 0 || !tmp)
        return -1;
    memcpy(&addr, tmp->ai_addr, tmp->ai_addrlen);
    inet_ntop(addr.ss_family, (void *)&((struct sockaddr_in *)&addr)->sin_addr, buf, sizeof(buf));

    printf("PING %s (%s) %d bytes of data.\n",
           tmp->ai_canonname, buf, opts['s'] ? opts['s'] : DEFAULT_SIZE_DATA);
    return sockfd;
}

int     main(int ac, char **av)
{
    if (ac < 2)
        return error();

    int opts[128] = {0};
    int arg = 0, sockfd = 0;

    switch ((arg = check_arg(ac, av, opts))) {
        case -1:
            printf("ft_ping: invalid option\n");
            return help();
        case 0:
            return opts['h'] ? help() : error();
    }
    if ((sockfd = resolve_hostname(av[arg], opts)) < 0)
        return printf("ft_ping: %s: Temporary failure in name resolution", av[arg]);
}
