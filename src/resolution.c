#include "ft_ping.h"

void    config_socket(int sockfd, const int *opts)
{
    int hold = 1;
    struct timeval tv = {
            .tv_sec = 2,
            .tv_usec = 0,
    };

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sockfd, SOL_IP, IP_RECVERR, &hold, sizeof(hold));
    if (opts['d'])
        setsockopt(sockfd, SOL_SOCKET, SO_DEBUG, &hold, sizeof(hold));
    if (opts['t'])
        setsockopt(sockfd, IPPROTO_IP, IP_TTL, &opts['t'], sizeof(int));
}

int     resolve_hostname(const char *av, const int *opts, char *ip, struct sockaddr_in *dst)
{
    int error = 0, sockfd = 0;
    struct addrinfo hints = {0};
    struct addrinfo *addrinfo_list = 0, *tmp = 0;
    struct sockaddr_storage addr;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;
    hints.ai_flags = AI_CANONNAME;

    if ((error = getaddrinfo(av, 0, &hints, &addrinfo_list))) {
        printf("ft_ping: %s: %s\n", av, gai_strerror(error));
        return -2;
    }
    for (tmp = addrinfo_list; tmp; tmp = tmp->ai_next) {
        if ((sockfd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol)) >= 0)
            break;
    }
    if (sockfd < 0 || !tmp)
        return -1;

    config_socket(sockfd, opts);
    memcpy(&addr, tmp->ai_addr, tmp->ai_addrlen);
    inet_ntop(addr.ss_family, (void *)&((struct sockaddr_in *)&addr)->sin_addr, ip, INET_ADDRSTRLEN);
    memcpy(&(dst->sin_addr), &((struct sockaddr_in *)&addr)->sin_addr, sizeof(struct in_addr));
    dst->sin_port = htons(0);
    dst->sin_family = AF_INET;

    printf("PING %s (%s) %d bytes of data.\n",
           tmp->ai_canonname, ip, opts['s'] ? opts['s'] : DEFAULT_SIZE_DATA);
    memcpy(global.host, tmp->ai_canonname, strlen(tmp->ai_canonname));
    freeaddrinfo(addrinfo_list);
    return sockfd;
}