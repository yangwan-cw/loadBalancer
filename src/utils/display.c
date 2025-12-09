#include "display.h"
#include "../../include/types.h"
#include "usage.h"
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep((x) * 1000)
#else
#include <unistd.h>
#endif

extern LoadBalancer lb;
extern volatile int running;

/**
 * 打印启动信息
 * 显示负载均衡器配置和服务器状态
 */
void print_startup_info() {
    printf("\n");
    printf("+--------------------------------------------------------------+\n");
    printf("|                  Load Balancer Status                        |\n");
    printf("+--------------------------------------------------------------+\n");
    printf("| Algorithm: %-50s |\n", get_algorithm_name(lb.algorithm));
    printf("+----------------+-------+--------+-----------+--------+-------+\n");
    printf("|     Server     | Port  | Weight |   Conns   | RT(ms) |Status |\n");
    printf("+----------------+-------+--------+-----------+--------+-------+\n");
    for (int i = 0; i < lb.server_count; i++) {
        Server *s = &lb.servers[i];
        printf("| %-14s | %-5d | %-6d | %-9d | %-6.1f | %-5s |\n",
               s->ip, s->port, s->weight, s->active_connections,
               s->response_time, s->is_alive ? "UP" : "DOWN");
    }
    printf("+----------------+-------+--------+-----------+--------+-------+\n\n");
}

void *status_display(void *arg) {
    while (running) {
        sleep(10);

        printf("\n");
        printf("+----------------------------------------------------------------+\n");
        printf("|                  Load Balancer Status                          |\n");
        printf("+----------------------------------------------------------------+\n");
        printf("| Algorithm: %-52s |\n", get_algorithm_name(lb.algorithm));
        printf("+----------------+-------+--------+-----------+--------+---------+\n");
        printf("|     Server     | Port  | Weight |   Conns   | RT(ms) | Status  |\n");
        printf("+----------------+-------+--------+-----------+--------+---------+\n");

        for (int i = 0; i < lb.server_count; i++) {
            Server *s = &lb.servers[i];
            printf("| %-14s | %-5d | %-6d | %-9d | %-6.1f | %-7s |\n",
                   s->ip, s->port, s->weight, s->active_connections,
                   s->response_time, s->is_alive ? "UP" : "DOWN");
        }

        printf("+----------------+-------+--------+-----------+--------+---------+\n\n");
    }
    return NULL;
}
