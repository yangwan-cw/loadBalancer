//
// Created by wangzhaobin on 25-12-8.
//


#ifndef LOADBALANCER_H
#define LOADBALANCER_H
#include "lua.h"
#include "../../include/types.h"

void init();
lua_config_t *initConfig(const char *config_file);

/**
 * 初始化负载均衡器
 * 设置服务器列表、权重、算法等
 */
void init_load_balancer();

#endif //LOADBALANCER_H
