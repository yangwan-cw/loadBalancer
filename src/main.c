#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>

#ifdef _WIN32
#include <winsock2.h>
#endif

#include "types.h"
#include "core/loadbalancer.h"
#include "health_check/health_check.h"
#include "networking/server.h"
#include "utils/args.h"
#include "utils/display.h"
#include "utils/signal_handler.h"
#include "utils/usage.h"

// 全局变量声明
LoadBalancer lb;
StickySession sticky_table[STICKY_TABLE_SIZE];
pthread_mutex_t sticky_lock;
volatile int running = 1;

int main(int argc, char *argv[]) {
    // 1. 解析命令行参数
    ParsedArgs args = parse_arguments(argc, argv);

    if (args.should_exit) {
        printf("Exiting program.\n");
        return 0;
    }

    if (args.show_help) {
        print_usage(argv[0]);
        return 1;
    }

    // 2. 初始化基础设施
    init();

    // 3. 初始化 Lua 配置
    lua_config_t *g_config = initConfig(NULL);
    if (!g_config) {
        fprintf(stderr, "Failed to initialize Lua config\n");
        return EXIT_FAILURE;
    }

    #ifdef _WIN32
        // Windows 网络库初始化
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            printf("WSAStartup failed\n");
            return 1;
        }
    #endif

    //  初始化负载均衡器
    init_load_balancer();

    // 应用命令行参数中的算法配置
    lb.algorithm = args.algorithm;

    // 注册信号处理函数
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // 创建监听 socket
    int listen_fd = create_listen_socket(LB_PORT);
    if (listen_fd < 0) {
        return 1;
    }

    // 打印启动信息
    print_startup_info();

    // 启动后台线程
    pthread_t health_thread, status_thread;
    pthread_create(&health_thread, NULL, health_check, NULL);
    pthread_create(&status_thread, NULL, status_display, NULL);

    // 启动服务器主循环
    start_server(listen_fd);

    // 清理资源
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
