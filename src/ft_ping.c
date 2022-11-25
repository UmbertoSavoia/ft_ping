#include "ft_ping.h"

void    statistics(void)
{
    struct timeval end;
    long double mdev = 0;

    gettimeofday(&end, 0);
    timersub(&end, &global.start, &end);
    global.avg /= global.packet_sended;
    mdev = (global.square / global.packet_sended) - global.avg * global.avg;
    mdev = sqrt(mdev);

    printf("\n--- %s ft_ping statistics ---\n", global.host);
    printf("%d packets transmitted, %d received, %.0f%% packet loss, time %.0Lfms\n",
           global.packet_sended,
           global.packet_reiceved,
           (global.packet_sended - global.packet_reiceved) / global.packet_sended * 100.0,
           timeval_to_ms(&end));
    if (global.packet_reiceved) {
        printf("rtt min/avg/max/mdev = %.3Lf/%.3Lf/%.3Lf/%.3Lf ms\n",
               global.min_time,
               global.avg,
               global.max_time,
               mdev);
    }
}

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
    uint8_t *data_packet = 0;

    if (!(packet = malloc(sizeof(struct icmphdr) + len_data)))
        return 0;
    data_packet = packet + sizeof(struct icmphdr);
    memset(data_packet, 'A', len_data);
    return packet;
}

int     recv_packet(int sockfd, uint8_t *packet, uint32_t len_packet)
{
    struct timeval s_recv = {0}, e_recv = {0};
    ssize_t bytes = 0;
    char name[NI_MAXHOST] = {0};
    char ip_str[INET_ADDRSTRLEN] = {0};
    struct msghdr msg = {0};
    struct iovec iov = {0};
    struct sockaddr_in buf_in = {0};
    struct icmphdr *icmp = 0;
    struct iphdr *ip = 0;
    int hlen = 0;
    long double time = 0;

    memset(packet, 0, len_packet);
    iov.iov_base = packet;
    iov.iov_len = len_packet;
    msg.msg_name = &buf_in;
    msg.msg_namelen = sizeof(buf_in);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    gettimeofday(&s_recv, 0);
    if ((bytes = recvmsg(sockfd, &msg, MSG_WAITALL /*MSG_DONTWAIT*/)) < 0) {
        return -1;
    }
    gettimeofday(&e_recv, 0);
    ip = (struct iphdr *)msg.msg_iov->iov_base;
    hlen = ip->ihl * 4;
    icmp = (struct icmphdr *)(msg.msg_iov->iov_base + hlen);
    if (icmp->un.echo.id != getpid())
        return -1;
    inet_ntop(buf_in.sin_family, &buf_in.sin_addr, ip_str, INET_ADDRSTRLEN);
    getnameinfo((struct sockaddr *)&buf_in, sizeof(struct sockaddr),
            name, sizeof(name), 0, 0, NI_IDN);
    timersub(&e_recv, &s_recv, &s_recv);
    time = timeval_to_ms(&s_recv);
    printf("%ld bytes from %s (%s): icmp_seq=%d ttl=%d time=%.2Lf ms\n",
           bytes, name, ip_str, ntohs(icmp->un.echo.sequence), ip->ttl, time);
    global.avg += time;
    if (time < global.min_time || !global.min_time)
        global.min_time = time;
    if (time > global.max_time)
        global.max_time = time;
    global.square += time * time;
    return 0;
}

void    recv_error(int sockfd, const int *opts)
{
    char cbuf[4096] = {0};
    struct iovec iov = {0};
    struct msghdr msg = {0};
    struct sockaddr_in target = {0};
    struct cmsghdr *cmsgh = 0;
    struct sock_extended_err *e = 0;
    struct icmphdr icmph = {0};
    char ip_str[INET_ADDRSTRLEN] = {0};
    char name[NI_MAXHOST] = {0};

    iov.iov_base = &icmph;
    iov.iov_len = sizeof(icmph);

    msg.msg_name = &target;
    msg.msg_namelen = sizeof(target);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;
    msg.msg_control = cbuf;
    msg.msg_controllen = sizeof(cbuf);

    recvmsg(sockfd, &msg, MSG_ERRQUEUE /*| MSG_DONTWAIT*/);

    for (cmsgh = CMSG_FIRSTHDR(&msg); cmsgh; cmsgh = CMSG_NXTHDR(&msg, cmsgh)) {
        if (cmsgh->cmsg_level == SOL_IP) {
            if (cmsgh->cmsg_type == IP_RECVERR)
                e = (struct sock_extended_err *)CMSG_DATA(cmsgh);
        }
    }
    if (e && opts['v']) {
        struct sockaddr_in *sin = (struct sockaddr_in *)(e + 1);
        inet_ntop(AF_INET, &sin->sin_addr, ip_str, INET_ADDRSTRLEN);
        getnameinfo((struct sockaddr *)sin, sizeof(struct sockaddr),
                    name, sizeof(name), 0, 0, NI_IDN);
        if (e->ee_type == ICMP_TIME_EXCEEDED) {
            printf("From %s (%s) Time to live exceeded\n", name, ip_str);
        } else {
            printf("From %s: type=%d code=%d info=%d\n", global.host, e->ee_type, e->ee_code, e->ee_info);
        }
    } else if (opts['v']) {
        printf("From %s: type=%d code=%d\n", global.host, icmph.type, icmph.code);
    }
}

void    ft_ping(int sockfd, const int *opts, struct sockaddr_in *dst)
{
    uint8_t *packet = 0;
    struct icmphdr *hdr_packet = 0;
    uint32_t len_data = opts['s'] ? opts['s'] : DEFAULT_SIZE_DATA;
    uint32_t len_packet = len_data + sizeof(struct icmphdr);

    if (!(packet = create_packet(len_data)))
        return;
    hdr_packet = (struct icmphdr *)packet;
    gettimeofday(&global.start, 0);
    for (int seq = 1; global.loop & 0b00000001;) {
        if (global.loop & 0b00000010) {
            hdr_packet->type = ICMP_ECHO;
            hdr_packet->code = 0;
            hdr_packet->checksum = 0;
            hdr_packet->un.echo.id = getpid();
            hdr_packet->un.echo.sequence = htons(seq);
            hdr_packet->checksum = checksum(packet, len_packet);

            if (sendto(sockfd, packet, len_packet, 0,
                       (struct sockaddr *)dst, sizeof(struct sockaddr_in)) <= 0) {
                printf("ft_ping: sendto error\n");
            } else {
                global.packet_sended++;
            }
            if (recv_packet(sockfd, packet, len_packet) < 0) {
                recv_error(sockfd, opts);
            } else {
                global.packet_reiceved++;
            }
            if (opts['c'] && opts['c'] == seq)
                break;
            ++seq;
            global.loop ^= 0b00000010;
            alarm(opts['i']);
        }
    }
    free(packet);
    close(sockfd);
    statistics();
}
