#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>

/**
 * 打印带时间戳的日志
 *
 * @param level 日志级别（INFO、ERROR、HEALTH 等）
 * @param fmt   格式化字符串
 * @param ...   可变参数
 *
 * 输出示例：[14:30:45] [INFO] Client 192.168.1.1 -> 172.20.0.11:80 (RR)
 */
void log_msg(const char *level, const char *fmt, ...);

#endif // LOGGER_H
