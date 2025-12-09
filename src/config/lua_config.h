//
// Created by wangzhaobin on 25-12-8.
//


#ifndef LUA_CONFIG_H
#define LUA_CONFIG_H
#include "lua.h"

lua_config_t *lua_config_init(const char *config_file);

/**
 * 重新加载 Lua 配置文件
 *
 * @param ctx Lua 配置上下文
 * @return 0=成功, -1=失败
 */
int lua_config_reload(lua_config_t *ctx);

#endif //LUA_CONFIG_H
