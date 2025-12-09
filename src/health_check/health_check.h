#ifndef HEALTH_CHECK_H
#define HEALTH_CHECK_H

#include "../../include/types.h"

/**
 * 检测服务器是否可访问
 *
 * @param ip   服务器 IP
 * @param port 服务器端口
 * @return 1=可访问, 0=不可访问
 */
int check_server_alive(const char *ip, int port);

/**
 * 健康检查线程函数
 * 定期检测后端服务器的健康状态
 *
 * @param arg 线程参数（未使用）
 * @return NULL
 */
void *health_check(void *arg);

#endif // HEALTH_CHECK_H
