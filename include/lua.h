//
// Created by wangzhaobin on 25-12-8.
//

#ifndef LUA_H
#define LUA_H
#include <stdbool.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdint.h>
#include <stdbool.h>



/** 完整的服务器配置 */
typedef struct {

    char name[256];
} server_config_t;

/** Lua 配置上下文 */
typedef struct {
    lua_State *L; /* Lua 虚拟机 */
    server_config_t config; /* 加载的配置 */
    char config_file[512]; /* 配置文件路径 */
} lua_config_t;


lua_config_t  * lua_config_init(const char *config_file);

int lua_config_reload(lua_config_t *config);

char * find_config_file(const char *config_name);
#endif //LUA_H
