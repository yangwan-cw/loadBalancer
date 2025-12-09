#include "health_check.h"
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../utils/logger.h"

#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep((x) * 1000)
#else
#include <unistd.h>
#endif

extern LoadBalancer lb;
extern volatile int running;

/**
 * 检测服务器是否可访问
 *
 * @param ip   服务器 IP
 * @param port 服务器端口
 * @return 1=可访问, 0=不可访问
 */
int check_server_alive(const char *ip, int port) {
    // 创建 socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return 0;
    }

    // 设置连接超时（2秒）
    #ifdef _WIN32
        DWORD timeout = 2000; // 毫秒
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout));
    #else
        struct timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    #endif

    // 设置服务器地址
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    // 尝试连接
    int result = connect(fd, (struct sockaddr *) &addr, sizeof(addr));

    // 关闭 socket
    #ifdef _WIN32
        closesocket(fd);
    #else
        close(fd);
    #endif

    return (result == 0) ? 1 : 0;
}

/* ============================================================================
 *                              健康检查
 * ============================================================================
 *
 * 原理：
 *   定期尝试连接后端服务器，检测是否存活
 *   如果连接失败，标记为宕机，不再分配请求
 *   如果恢复，标记为存活，重新参与负载均衡
 */
void *health_check(void *arg) {
    while (running) {
        for (int i = 0; i < lb.server_count; i++) {
            Server *s = &lb.servers[i];

            // 尝试 TCP 连接
            int fd = socket(AF_INET, SOCK_STREAM, 0);
            if (fd < 0) continue;

            // 设置 1 秒超时
            struct timeval tv = {0.5, 0};
            setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

            struct sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(s->port);
            inet_pton(AF_INET, s->ip, &addr.sin_addr);

            int was_alive = s->is_alive;
            s->is_alive = (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == 0);
            // 关闭 socket（跨平台）
            #ifdef _WIN32
                        closesocket(fd);
            #else
                        close(fd);
            #endif

            // 状态变化时打印日志
            if (was_alive != s->is_alive) {
                log_msg("HEALTH", "%s:%d is now %s",
                        s->ip, s->port, s->is_alive ? "UP   " : "DOWN");
            }
        }

        sleep(HEALTH_CHECK_INTERVAL);
    }
    return NULL;
}
