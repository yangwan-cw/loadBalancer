//
// Created by wangzhaobin on 25-12-8.
//
#include "lua.h"

#include <stdlib.h>
#include <string.h>

int main() {
    /* 初始化 Lua 配置 */
    static lua_config_t *g_config = lua_config_init("config.lua");
    if (!g_config) {
        fprintf(stderr, "Failed to initialize Lua config\n");
        return EXIT_FAILURE;
    }

    /* 显示初始配置 */
    server_config_t config = g_config->config;

    printf("\n【初始配置】\n");
    printf("  应用名称: %s\n", config.name);

}


