#ifndef DISPLAY_H
#define DISPLAY_H

/**
 * 打印启动信息
 * 显示负载均衡器配置和服务器状态
 */
void print_startup_info();

/**
 * 状态显示线程函数
 * 定期显示负载均衡器和服务器状态
 *
 * @param arg 线程参数（未使用）
 * @return NULL
 */
void *status_display(void *arg);

#endif // DISPLAY_H
