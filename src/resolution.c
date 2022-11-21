#include "ft_ping.h"

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