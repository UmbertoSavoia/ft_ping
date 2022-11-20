#include "ft_ping.h"

char g_loop = 1;

uint16_t checksum(void *packet, uint32_t len)
{
    uint16_t *buf = packet;
    uint32_t sum = 0;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(uint8_t *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}

uint8_t *create_packet(uint32_t len_data)
{
    uint8_t *packet = 0;
    struct icmphdr *hdr_packet = 0;
    uint8_t *data_packet = 0;

    if (!(packet = malloc(sizeof(struct icmphdr) + len_data)))
        return 0;
    hdr_packet = (struct icmphdr *)packet;
    data_packet = packet + sizeof(struct icmphdr);

    memset(data_packet, 'A', len_data);

    hdr_packet->type = ICMP_ECHO;
    hdr_packet->code = 0;
    hdr_packet->checksum = 0;
    //hdr_packet->un.echo.id = getpid();

    return packet;
}

int     recv_packet(int sockfd, uint8_t *packet, uint32_t len_packet)
{
    uint32_t bytes = 0;
    char name[NI_MAXHOST] = {0};
    char ip_str[INET_ADDRSTRLEN] = {0};
    struct msghdr msg = {0};
    struct iovec iov = {0};
    struct sockaddr_in buf_in = {0};
    struct icmphdr *icmp = 0;
    struct iphdr *ip = 0;
    int hlen = 0;

    memset(packet, 0, len_packet);
    iov.iov_base = packet;
    iov.iov_len = len_packet;
    msg.msg_name = &buf_in;
    msg.msg_namelen = sizeof(buf_in);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    if ((bytes = recvmsg(sockfd, &msg, MSG_WAITALL)) < 0) {
        printf("ft_ping: recvmsg error\n");
        return -1;
    }
    ip = (struct iphdr *)msg.msg_iov->iov_base;
    hlen = ip->ihl * 4;
    icmp = (struct icmphdr *)(msg.msg_iov->iov_base + hlen);
    inet_ntop(buf_in.sin_family, &buf_in.sin_addr, ip_str, INET_ADDRSTRLEN);
    getnameinfo((struct sockaddr *)&buf_in, sizeof(struct sockaddr), name, sizeof(name), 0, 0, NI_IDN);

    printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%d ms\n",
           bytes, name, ip_str, ntohs(icmp->un.echo.sequence),
           ip->ttl, 0);
    return 0;
}

void    ft_ping(int sockfd, const char *ip, const int *opts, struct sockaddr_in *dst)
{
    uint8_t *packet = 0;
    struct icmphdr *hdr_packet = 0;
    uint32_t len_data = opts['s'] ? opts['s'] : DEFAULT_SIZE_DATA;
    uint32_t len_packet = len_data + sizeof(struct icmphdr);

    if (!(packet = create_packet(len_data)))
        return;
    hdr_packet = (struct icmphdr *)packet;

    for (uint32_t seq = 1; g_loop; ++seq) {
        hdr_packet->un.echo.sequence = htons(seq);
        hdr_packet->checksum = checksum(packet, len_packet);

        if (sendto(sockfd, packet, len_packet, 0, (struct sockaddr *)dst, sizeof(struct sockaddr_in)) <= 0) {
            printf("ft_ping: sendto error\n");
            return;
        }
        if (recv_packet(sockfd, packet, len_packet) < 0)
            return;
        break;
    }
    free(packet);
    close(sockfd);
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
