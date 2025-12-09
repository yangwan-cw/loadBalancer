//
// Created by wangzhaobin on 25-12-8.
//

#include "loadbalancer.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "../config/lua_config.h"
#include "../health_check/health_check.h"

extern LoadBalancer lb;
extern StickySession sticky_table[STICKY_TABLE_SIZE];
extern pthread_mutex_t sticky_lock;

void init() {
    printf("Load balancer initialized.\n");
    printf("Init function called.\n");
}


lua_config_t *initConfig(const char *config_file) {
    printf("Configuration initialized from %s.\n", config_file);
    lua_config_t *config_init = lua_config_init(config_file);
    return config_init;
}

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
