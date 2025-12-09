#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

/**
 * 信号处理函数
 * 用于处理 SIGINT 和 SIGTERM 信号，优雅地关闭程序
 *
 * @param sig 信号编号
 */
void signal_handler(int sig);

#endif // SIGNAL_HANDLER_H
