//
// Created by wangzhaobin on 25-12-8.
//


#ifndef LOADBALANCER_H
#define LOADBALANCER_H
#include "lua.h"
void init();

lua_config_t * initConfig(const char *config_file);



#endif //LOADBALANCER_H
