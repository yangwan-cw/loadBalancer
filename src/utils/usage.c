#include "usage.h"
#include <stdio.h>

void print_usage(const char *prog) {
    printf("\n");
    printf("Usage: %s [algorithm]\n", prog);
    printf("\n");
    printf("Algorithms:\n");
    printf("  rr     - Round Robin  [default]\n");
    printf("  sticky - Sticky Round Robin \n");
    printf("  wrr    - Weighted Round Robin\n");
    printf("  hash   - Hash \n");
    printf("  lc     - Least Connections\n");
    printf("  lrt    - Least Response Time\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s           # Use the default polling algorithm\n", prog);
    printf("  %s wrr       # Use the re-polling algorithm\n", prog);
    printf("  %s lc        # Use the least join algorithm\n", prog);
    printf("\n");
}

/**
 * 获取算法名称字符串
 */
const char *get_algorithm_name(LBAlgorithm algo) {
    switch (algo) {
        case LB_ROUND_ROBIN: return "Round Robin";
        case LB_STICKY_ROUND_ROBIN: return "Sticky Round Robin";
        case LB_WEIGHTED_ROUND_ROBIN: return "Weighted Round Robin";
        case LB_HASH: return "Hash";
        case LB_LEAST_CONNECTIONS: return "Least Connections";
        case LB_LEAST_RESPONSE_TIME: return "Least Response Time";
        default: return "Unknown";
    }
}
