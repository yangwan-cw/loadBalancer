#include "logger.h"
#include <stdio.h>
#include <time.h>

/**
 * 打印带时间戳的日志
 *
 * @param level 日志级别（INFO、ERROR、HEALTH 等）
 * @param fmt   格式化字符串
 * @param ...   可变参数
 *
 * 输出示例：[14:30:45] [INFO] Client 192.168.1.1 -> 172.20.0.11:80 (RR)
 */
void log_msg(const char *level, const char *fmt, ...) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // 打印时间和级别
    printf("[%02d:%02d:%02d] [%s] ",
           t->tm_hour, t->tm_min, t->tm_sec, level);

    // 打印格式化内容
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
    fflush(stdout); // 立即刷新，避免日志延迟
}
