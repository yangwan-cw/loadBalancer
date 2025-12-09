#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../include/types.h"
#include "core/loadbalancer.h"
#include "lua.h"
LoadBalancer lb; // 全局负载均衡器实例
StickySession sticky_table[STICKY_TABLE_SIZE]; // 粘性会话表
pthread_mutex_t sticky_lock; // 粘性会话表锁
volatile int running = 1; // 运行标志，Ctrl+C 时设为 0


void init_load_balancer() {
    memset(&lb, 0, sizeof(lb)); // 分配初始化空间
    printf("lb size =%zu\n", sizeof(lb));
    pthread_mutex_init(&sticky_lock, NULL); // 初始化锁，粘性表
    pthread_mutex_init(&lb.lock, NULL); // 初始化锁，负载均衡器

    // 初始化粘性会话表
    for (int i = 0; i < STICKY_TABLE_SIZE; i++) {
        sticky_table[i].server_index = -1;
        sticky_table[i].client_ip[0] = '\0';
    }

    lb.algorithm = LB_ROUND_ROBIN;


    const char *server[][2] = {
        {"111.228.6.56", "8081"},
        {"111.228.6.56", "8082"},
        {"111.228.6.56", "8083"}
    };
    int weights[] = {5, 3, 2}; // 权重 5:3:2

    for (int i = 0; i < 3; i++) {
        Server *s = &lb.servers[lb.server_count]; // lb.server_count在使用memset时已经赋值了
        strncpy(s->ip, server[i][0], sizeof(s->ip) - 1);
        s->ip[15] = '\0';
        s->port = atoi(server[i][1]);
        s->weight = weights[i];
        s->current_weight = 0;
        s->active_connections = 0;
        s->is_alive = check_server_alive(s->ip, s->port);
        s->response_time = 0;
        pthread_mutex_init(&s->lock, NULL);
        lb.server_count++;
        // 打印检测结果
        printf("  [%d] %s:%d - %s\n",
               i + 1, s->ip, s->port,
               s->is_alive ? "UP" : "DOWN");
    }
}

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


void signal_handler(int sig) {
    running = 0;
    printf("\n[INFO] Shutting down...\n");
}

const char *get_algorithm_name(LBAlgorithm algo);

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

/**
 * 打印带时间戳的日志
 *
 * @param level 日志级别（INFO、ERROR、HEALTH 等）
 * @param fmt   格式化字符串
 * @param ...   可变参数
 *
 * 输出示例：[14:30:45] [INFO] Client 192.168.1.1 -> 172.20.0.11:80 (RR)
 */
void log_msg(const char *level, const char *fmt, ...) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // 打印时间和级别
    printf("[%02d:%02d:%02d] [%s] ",
           t->tm_hour, t->tm_min, t->tm_sec, level);

    // 打印格式化内容
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
    fflush(stdout); // 立即刷新，避免日志延迟
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

/* ============================================================================
 *                          算法 1：轮询 (Round Robin)
 * ============================================================================
 *
 * 原理：
 *   按顺序依次选择服务器，循环往复，像发牌一样
 *
 * 示意图：
 *   请求1 → Server1    current_index: 0 → 1
 *   请求2 → Server2    current_index: 1 → 2
 *   请求3 → Server3    current_index: 2 → 0 (循环)
 *   请求4 → Server1    current_index: 0 → 1
 *   ...
 *
 * 优点：简单、公平、无状态
 * 缺点：不考虑服务器性能差异和当前负载
 *
 * 时间复杂度：O(n) 最坏情况（所有服务器都宕机时遍历一遍）
 */
Server *round_robin(LoadBalancer *lb) {
    pthread_mutex_lock(&lb->lock); // 加锁，防止多线程并发修改 current_index

    Server *selected = NULL;
    int attempts = 0; // 尝试次数，防止所有服务器都宕机时死循环

    // 最多尝试 server_count 次
    while (attempts < lb->server_count) {
        int idx = lb->current_index;

        // 移动到下一个，取模实现循环
        lb->current_index = (lb->current_index + 1) % lb->server_count;

        // 检查服务器是否存活
        if (lb->servers[idx].is_alive) {
            selected = &lb->servers[idx];
            break;
        }

        attempts++;
    }

    pthread_mutex_unlock(&lb->lock);
    return selected; // 全部宕机返回 NULL
}


int main(int argc, char *argv[]) {
    init();
    /* 初始化 Lua 配置 */
    lua_config_t *g_config = initConfig(NULL);
    if (!g_config) {
        fprintf(stderr, "Failed to initialize Lua config\n");
        return EXIT_FAILURE;
    }

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }
#endif
    init_load_balancer();
    if (argc > 1) {
        if (strcmp(argv[1], "exit") == 0) {
            printf("Exiting program.\n");
            exit(0);
        } else if (strcmp(argv[1], "rr") == 0) {
            lb.algorithm = LB_ROUND_ROBIN;
        } else {
            printf("Unknown algorithm: %s\n", argv[1]);
            print_usage(argv[0]);
            return 1;
        }
    }
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);


    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return 1;
    }

    // 设置端口复用（避免 TIME_WAIT 导致绑定失败）
    char opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // 绑定地址
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // 监听所有网卡
    addr.sin_port = htons(LB_PORT);
    if (bind(listen_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    // 开始监听
    if (listen(listen_fd, 128) < 0) {
        perror("listen");
        return 1;
    }

    // 打印启动信息

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


    // 启动健康检查线程
    pthread_t health_thread;
    pthread_create(&health_thread, NULL, health_check, NULL);

    // 启动状态显示线程
    pthread_t status_thread;
    pthread_create(&status_thread, NULL, status_display, NULL);

    log_msg("INFO", "Waiting for connections...");
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len);
        if (client_fd < 0) {
            if (running) {
                perror("accept");
            }
            continue;
        }


        // 为新连接分配 Connection 结构
        Connection *conn = (Connection *) malloc(sizeof(Connection));
        if (conn == NULL) {
            close(client_fd);
            continue;
        }
        conn->client_fd = client_fd;
        conn->server_fd = -1;
        conn->server = NULL;
        conn->start_time = time(NULL);
        conn->request_uri[0] = '\0';
        inet_ntop(AF_INET, &client_addr.sin_addr, conn->client_ip, sizeof(conn->client_ip));
        printf("New connection from %s\n", conn->client_ip);
        printf("Connection struct size = %zu bytes\n", sizeof(Connection));
    }

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}


int lua_config_reload(lua_config_t *ctx) {
    if (!ctx || !ctx->L) {
        fprintf(stderr, "Invalid lua_config_t context\n");
        return -1;
    }

    /* 加载 Lua 配置文件 */
    int ret = luaL_dofile(ctx->L, ctx->config_file);
    if (ret != LUA_OK) {
        fprintf(stderr, "Lua error: %s\n", lua_tostring(ctx->L, -1));
        lua_pop(ctx->L, 1);
        return -1;
    }

    /* 获取全局 APPNAME 变量 */
    lua_getglobal(ctx->L, "APPNAME");
    if (lua_isstring(ctx->L, -1)) {
        const char *name = lua_tostring(ctx->L, -1);
        strncpy(ctx->config.name, name, sizeof(ctx->config.name) - 1);
        ctx->config.name[sizeof(ctx->config.name) - 1] = '\0';
    } else {
        fprintf(stderr, "APPNAME not found in config\n");
        lua_pop(ctx->L, 1);
        return -1;
    }
    lua_pop(ctx->L, 1);

    return 0;
}
