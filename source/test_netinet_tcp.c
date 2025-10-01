#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_PASSED 0
#define TEST_FAILED 1

int test_tcp_constants() {
    printf("Testing TCP constants...\n");
    
    if (TCP_NODELAY != 1) {
        printf("TCP_NODELAY test failed - expected 1, got %d\n", TCP_NODELAY);
        return TEST_FAILED;
    }
    
    if (TCP_MAXSEG != 2) {
        printf("TCP_MAXSEG test failed - expected 2, got %d\n", TCP_MAXSEG);
        return TEST_FAILED;
    }
    
    if (TCP_KEEPIDLE != 4) {
        printf("TCP_KEEPIDLE test failed - expected 4, got %d\n", TCP_KEEPIDLE);
        return TEST_FAILED;
    }
    
    printf("TCP constants test passed\n");
    return TEST_PASSED;
}

int test_tcp_states() {
    printf("Testing TCP states...\n");
    
    if (TCP_ESTABLISHED != 1) {
        printf("TCP_ESTABLISHED test failed - expected 1, got %d\n", TCP_ESTABLISHED);
        return TEST_FAILED;
    }
    
    if (TCP_SYN_SENT != 2) {
        printf("TCP_SYN_SENT test failed - expected 2, got %d\n", TCP_SYN_SENT);
        return TEST_FAILED;
    }
    
    if (TCP_LISTEN != 10) {
        printf("TCP_LISTEN test failed - expected 10, got %d\n", TCP_LISTEN);
        return TEST_FAILED;
    }
    
    printf("TCP states test passed\n");
    return TEST_PASSED;
}

int test_tcp_header_structure() {
    printf("Testing TCP header structure...\n");
    
    struct tcphdr header;
    
    // Test that structure can be initialized
    memset(&header, 0, sizeof(header));
    
    header.th_sport = 80;
    header.th_dport = 8080;
    header.th_seq = 12345;
    header.th_ack = 67890;
    header.th_flags = TH_SYN | TH_ACK;
    header.th_win = 65535;
    
    if (header.th_sport != 80) {
        printf("TCP header sport test failed\n");
        return TEST_FAILED;
    }
    
    if (header.th_dport != 8080) {
        printf("TCP header dport test failed\n");
        return TEST_FAILED;
    }
    
    if (header.th_flags != (TH_SYN | TH_ACK)) {
        printf("TCP header flags test failed\n");
        return TEST_FAILED;
    }
    
    printf("TCP header structure test passed\n");
    return TEST_PASSED;
}

int test_tcp_flags() {
    printf("Testing TCP flags...\n");
    
    if (TH_FIN != 0x01) {
        printf("TH_FIN test failed - expected 0x01, got 0x%02x\n", TH_FIN);
        return TEST_FAILED;
    }
    
    if (TH_SYN != 0x02) {
        printf("TH_SYN test failed - expected 0x02, got 0x%02x\n", TH_SYN);
        return TEST_FAILED;
    }
    
    if (TH_ACK != 0x10) {
        printf("TH_ACK test failed - expected 0x10, got 0x%02x\n", TH_ACK);
        return TEST_FAILED;
    }
    
    printf("TCP flags test passed\n");
    return TEST_PASSED;
}

int test_tcp_info_structure() {
    printf("Testing TCP info structure...\n");
    
    struct tcp_info info;
    
    // Test that structure can be initialized
    memset(&info, 0, sizeof(info));
    
    info.tcpi_state = TCP_ESTABLISHED;
    info.tcpi_rto = 1000;
    info.tcpi_snd_mss = 1460;
    info.tcpi_rcv_mss = 1460;
    
    if (info.tcpi_state != TCP_ESTABLISHED) {
        printf("TCP info state test failed\n");
        return TEST_FAILED;
    }
    
    if (info.tcpi_snd_mss != 1460) {
        printf("TCP info MSS test failed\n");
        return TEST_FAILED;
    }
    
    printf("TCP info structure test passed\n");
    return TEST_PASSED;
}

int test_tcp_socket_options() {
    printf("Testing TCP socket options...\n");
    
    // Test that constants are properly defined for socket options
    int nodelay = TCP_NODELAY;
    int maxseg = TCP_MAXSEG;
    int keepidle = TCP_KEEPIDLE;
    int keepintvl = TCP_KEEPINTVL;
    int keepcnt = TCP_KEEPCNT;
    
    if (nodelay != 1 || maxseg != 2 || keepidle != 4) {
        printf("TCP socket options test failed - basic constants wrong\n");
        return TEST_FAILED;
    }
    
    printf("TCP socket options test passed\n");
    return TEST_PASSED;
}

int test_tcp_state_transitions() {
    printf("Testing TCP state transitions...\n");
    
    // Test all TCP states are properly defined
    int states[] = {
        TCP_ESTABLISHED, TCP_SYN_SENT, TCP_SYN_RECV,
        TCP_FIN_WAIT1, TCP_FIN_WAIT2, TCP_TIME_WAIT,
        TCP_CLOSE, TCP_CLOSE_WAIT, TCP_LAST_ACK,
        TCP_LISTEN, TCP_CLOSING
    };
    
    int expected[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    
    for (int i = 0; i < 11; i++) {
        if (states[i] != expected[i]) {
            printf("TCP state test failed - state %d expected %d, got %d\n", i, expected[i], states[i]);
            return TEST_FAILED;
        }
    }
    
    printf("TCP state transitions test passed\n");
    return TEST_PASSED;
}

int main() {
    int failures = 0;
    
    printf("===== netinet/tcp Test Suite =====\n");
    
    printf("\n[TEST] TCP constants\n");
    failures += test_tcp_constants();
    
    printf("\n[TEST] TCP states\n");
    failures += test_tcp_states();
    
    printf("\n[TEST] TCP header structure\n");
    failures += test_tcp_header_structure();
    
    printf("\n[TEST] TCP flags\n");
    failures += test_tcp_flags();
    
    printf("\n[TEST] TCP info structure\n");
    failures += test_tcp_info_structure();
    
    printf("\n[TEST] TCP socket options\n");
    failures += test_tcp_socket_options();
    
    printf("\n[TEST] TCP state transitions\n");
    failures += test_tcp_state_transitions();
    
    printf("\n===== Test Suite Completed =====\n");
    printf("%d tests failed\n", failures);
    
    return failures ? TEST_FAILED : TEST_PASSED;
}