#ifndef _NETINET_TCP_H
#define _NETINET_TCP_H

#include <sys/types.h>
#include <stdint.h>

/* BSD-style type definitions */
#ifndef u_int8_t
#define u_int8_t uint8_t
#endif
#ifndef u_int16_t
#define u_int16_t uint16_t
#endif
#ifndef u_int32_t
#define u_int32_t uint32_t
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* TCP socket options */
#define TCP_NODELAY     1   /* Don't delay send to coalesce packets */
#define TCP_MAXSEG      2   /* Set maximum segment size */
#define TCP_CORK        3   /* Control sending of partial frames */
#define TCP_KEEPIDLE    4   /* Start keeplives after this period */
#define TCP_KEEPINTVL   5   /* Interval between keepalives */
#define TCP_KEEPCNT     6   /* Number of keepalives before death */
#define TCP_SYNCNT      7   /* Number of SYN retransmits */
#define TCP_LINGER2     8   /* Life time of orphaned FIN-WAIT-2 state */
#define TCP_DEFER_ACCEPT 9  /* Wake up listener only when data arrive */
#define TCP_WINDOW_CLAMP 10 /* Bound advertised window */
#define TCP_INFO        11  /* Information about this connection */
#define TCP_QUICKACK    12  /* Bock/reenable quick ACKs */
#define TCP_CONGESTION  13  /* Congestion control algorithm */
#define TCP_MD5SIG      14  /* TCP MD5 Signature (RFC2385) */
#define TCP_THIN_LINEAR_TIMEOUTS 16 /* Use linear timeouts for thin streams*/
#define TCP_THIN_DUPACK 17  /* Fast retrans. after 1 dupack */
#define TCP_USER_TIMEOUT 18 /* How long for loss retry before timeout */
#define TCP_REPAIR      19  /* TCP sock is under repair right now */
#define TCP_REPAIR_QUEUE 20 /* Set TCP queue to repair */
#define TCP_QUEUE_SEQ   21  /* Set sequence number of repaired queue */
#define TCP_REPAIR_OPTIONS 22 /* Repair TCP connection options */
#define TCP_FASTOPEN    23  /* Enable FastOpen on listeners */
#define TCP_TIMESTAMP   24  /* TCP time stamp */

/* TCP states */
#define TCP_ESTABLISHED  1
#define TCP_SYN_SENT     2
#define TCP_SYN_RECV     3
#define TCP_FIN_WAIT1    4
#define TCP_FIN_WAIT2    5
#define TCP_TIME_WAIT    6
#define TCP_CLOSE        7
#define TCP_CLOSE_WAIT   8
#define TCP_LAST_ACK     9
#define TCP_LISTEN       10
#define TCP_CLOSING      11

/* TCP header structure */
struct tcphdr {
    u_int16_t th_sport;     /* source port */
    u_int16_t th_dport;     /* destination port */
    u_int32_t th_seq;       /* sequence number */
    u_int32_t th_ack;       /* acknowledgement number */
    u_int8_t th_x2:4;       /* (unused) */
    u_int8_t th_off:4;      /* data offset */
    u_int8_t th_flags;
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80
    u_int16_t th_win;       /* window */
    u_int16_t th_sum;       /* checksum */
    u_int16_t th_urp;       /* urgent pointer */
};

/* TCP connection info structure */
struct tcp_info {
    u_int8_t tcpi_state;
    u_int8_t tcpi_ca_state;
    u_int8_t tcpi_retransmits;
    u_int8_t tcpi_probes;
    u_int8_t tcpi_backoff;
    u_int8_t tcpi_options;
    u_int8_t tcpi_snd_wscale : 4, tcpi_rcv_wscale : 4;
    u_int32_t tcpi_rto;
    u_int32_t tcpi_ato;
    u_int32_t tcpi_snd_mss;
    u_int32_t tcpi_rcv_mss;
    u_int32_t tcpi_unacked;
    u_int32_t tcpi_sacked;
    u_int32_t tcpi_lost;
    u_int32_t tcpi_retrans;
    u_int32_t tcpi_fackets;
    u_int32_t tcpi_last_data_sent;
    u_int32_t tcpi_last_ack_sent;
    u_int32_t tcpi_last_data_recv;
    u_int32_t tcpi_last_ack_recv;
    u_int32_t tcpi_pmtu;
    u_int32_t tcpi_rcv_ssthresh;
    u_int32_t tcpi_rtt;
    u_int32_t tcpi_rttvar;
    u_int32_t tcpi_snd_ssthresh;
    u_int32_t tcpi_snd_cwnd;
    u_int32_t tcpi_advmss;
    u_int32_t tcpi_reordering;
};

#ifdef __cplusplus
}
#endif

#endif /* _NETINET_TCP_H */