#ifndef ARGS_H
#define ARGS_H

#include "../../include/types.h"

/**
 * 命令行参数解析结果
 */
typedef struct {
    LBAlgorithm algorithm;  // 选择的负载均衡算法
    int should_exit;        // 是否应该退出程序
    int show_help;          // 是否显示帮助信息
} ParsedArgs;

/**
 * 解析命令行参数
 *
 * @param argc 参数个数
 * @param argv 参数数组
 * @return 解析后的参数结构
 */
ParsedArgs parse_arguments(int argc, char *argv[]);

#endif // ARGS_H
