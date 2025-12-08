//
// Created by wangzhaobin on 25-12-8.
//

#ifndef TYPES_H
#define TYPES_H
#include <pthread.h>

void init_load_balancer();

void print_usage(const char *prog);

void signal_handler(int sig);

int check_server_alive(const char *ip, int port);

void log_msg(const char *level, const char *fmt, ...);

void *status_display(void *arg);

void *health_check(void *arg);


#define LB_PORT               8080    // 负载均衡器监听端口
#define MAX_SERVERS           10      // 最大后端服务器数量
#define BUFFER_SIZE           65536   // 读写缓冲区大小 (64KB)
#define HEALTH_CHECK_INTERVAL 3       // 健康检查间隔（秒）
#define STICKY_TABLE_SIZE     1024    // 粘性会话表大小
#define SESSION_TIMEOUT       1800    // 会话超时时间（秒）= 30分钟
#ifdef _WIN32
// Windows
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#define close closesocket          // Windows 用 closesocket
#define sleep(s) Sleep((s) * 1000) // Windows 用 Sleep（毫秒）
#else
// Linux/Mac
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif


typedef enum {
    LB_ROUND_ROBIN, // 1. 轮询
    LB_STICKY_ROUND_ROBIN, // 2. 粘性轮询
    LB_WEIGHTED_ROUND_ROBIN, // 3. 权重轮询
    LB_HASH, // 4. 散列
    LB_LEAST_CONNECTIONS, // 5. 最少连接数
    LB_LEAST_RESPONSE_TIME // 6. 最短响应时间
} LBAlgorithm;



typedef struct {
    char ip[16]; // 服务器 IP 地址，如 "172.20.0.11"
    int port; // 服务器端口，如 80
    int weight; // 权重（用于加权轮询），值越大分配越多
    int current_weight; // 当前权重（平滑加权轮询算法使用）
    int active_connections; // 当前活跃连接数
    int is_alive; // 是否存活：1=存活，0=宕机
    double response_time; // 平均响应时间（毫秒）
    pthread_mutex_t lock; // 互斥锁，保护并发访问
} Server;



/**
 * 负载均衡器结构体
 *
 * 管理所有后端服务器和负载均衡状态
 */
typedef struct {
    Server servers[MAX_SERVERS]; // 后端服务器数组
    int server_count; // 服务器数量
    int current_index; // 当前索引（轮询算法使用）
    LBAlgorithm algorithm; // 当前使用的负载均衡算法
    pthread_mutex_t lock; // 互斥锁，保护并发访问
} LoadBalancer;

/**
 * 粘性会话结构体
 *
 * 用于记录客户端 IP 和对应的服务器，实现会话保持
 *
 * 示意：
 *   客户端 192.168.1.100 → 总是分配到 Server1
 *   客户端 192.168.1.101 → 总是分配到 Server2
 */
typedef struct {
    char client_ip[16]; // 客户端 IP
    int server_index; // 分配的服务器索引（-1 表示未分配）
    time_t last_access; // 最后访问时间（用于过期清理）
} StickySession;

/**
 * 连接结构体
 *
 * 存储一次客户端请求的所有信息
 *
 * 数据流：
 *   客户端 ←──client_fd──→ 负载均衡器 ←──server_fd──→ 后端服务器
 */
typedef struct {
    int client_fd; // 客户端 socket 文件描述符
    int server_fd; // 后端服务器 socket 文件描述符
    Server *server; // 指向选中的后端服务器
    char client_ip[16]; // 客户端 IP 地址
    char request_uri[256]; // 请求 URI（用于哈希算法）
    time_t start_time; // 请求开始时间（用于计算响应时间）
} Connection;
#endif
