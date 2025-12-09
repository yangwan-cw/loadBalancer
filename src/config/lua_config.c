#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lua_config.h"

char *find_config(const char *config_name) {
    char *filePath = NULL;

    char *locations[] = {
        config_name, // config.lua
        "../config.lua", // ../config.lua
        "../config/config.lua", // ../config/config.lua
        "../../config.lua", // ../../config.lua
        NULL
    };
    for (int i = 0; locations[i]; i++) {
        FILE *f = fopen(locations[i], "r");
        if (f) {
            fclose(f);
            filePath = (char *) malloc(strlen(locations[i]) + 1);
            if (filePath == NULL) {
                fprintf(stderr, "内存分配失败\n");
                return NULL;
            }

            strcpy(filePath, locations[i]);
            return filePath;
        }
    }
    return NULL;
}



lua_config_t *lua_config_init(const char *config_file) {
    if (!config_file) {
        config_file = "config.lua";
    }
    char *found_path = find_config(config_file);
    if (!found_path) {
        fprintf(stderr, " Cannot find %s\n", config_file);
        return NULL;
    }

    lua_config_t *config = malloc(sizeof(lua_config_t));
    if (!config) {
        return NULL;
    }

    config->L = luaL_newstate();
    if (!config->L) {
        free(config);
        return NULL;
    }

    luaL_openlibs(config->L);

    // 使用找到的路径
    printf("Loading config from: %s\n", found_path);
    if (luaL_dofile(config->L, found_path) != 0) {
        fprintf(stderr, "Error: %s\n", lua_tostring(config->L, -1));
        lua_close(config->L);
        free(config);
        return NULL;
    }
    return config;
}