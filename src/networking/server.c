#include "server.h"
#include "../../include/types.h"
#include "../utils/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <ws2tcpip.h>

extern volatile int running;

/**
 * 创建并配置监听 socket
 *
 * @param port 监听端口
 * @return 监听 socket 文件描述符，失败返回 -1
 */
int create_listen_socket(int port) {
    // 创建 socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return -1;
    }

    // 设置端口复用（避免 TIME_WAIT 导致绑定失败）
    char opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定地址
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // 监听所有网卡
    addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }

    // 开始监听
    if (listen(listen_fd, 128) < 0) {
        perror("listen");
        return -1;
    }

    return listen_fd;
}

/**
 * 处理单个客户端连接
 *
 * @param client_fd 客户端 socket 文件描述符
 * @param client_ip 客户端 IP 地址
 */
void handle_client_connection(int client_fd, const char *client_ip) {
    // 为新连接分配 Connection 结构
    Connection *conn = (Connection *) malloc(sizeof(Connection));
    if (conn == NULL) {
        close(client_fd);
        return;
    }

    conn->client_fd = client_fd;
    conn->server_fd = -1;
    conn->server = NULL;
    conn->start_time = time(NULL);
    conn->request_uri[0] = '\0';
    strncpy(conn->client_ip, client_ip, sizeof(conn->client_ip) - 1);
    conn->client_ip[sizeof(conn->client_ip) - 1] = '\0';

    printf("New connection from %s\n", conn->client_ip);
    printf("Connection struct size = %zu bytes\n", sizeof(Connection));

    // TODO: 实际的请求处理逻辑
    // 这里应该选择后端服务器并转发请求
}

/**
 * 启动服务器主循环
 * 持续接受客户端连接并处理
 *
 * @param listen_fd 监听 socket 文件描述符
 */
void start_server(int listen_fd) {
    log_msg("INFO", "Waiting for connections...");

    // 主循环：接受客户端连接
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len);

        if (client_fd < 0) {
            if (running) {
                perror("accept");
            }
            continue;
        }

        // 获取客户端 IP
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

        // 处理连接
        handle_client_connection(client_fd, client_ip);
    }
}
