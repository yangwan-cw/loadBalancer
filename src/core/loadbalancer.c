//
// Created by wangzhaobin on 25-12-8.
//

#include "loadbalancer.h"

#include <stdio.h>

#include "config/lua_config.h"

void init() {
    printf("Load balancer initialized.\n");
    printf("Init function called.\n");
}


lua_config_t *initConfig(const char *config_file) {
    printf("Configuration initialized from %s.\n", config_file);
    lua_config_t *config_init = lua_config_init(config_file);
    return config_init;
}
