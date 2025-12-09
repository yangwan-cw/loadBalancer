#include "args.h"
#include "usage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * 解析命令行参数
 *
 * @param argc 参数个数
 * @param argv 参数数组
 * @return 解析后的参数结构
 */
ParsedArgs parse_arguments(int argc, char *argv[]) {
    ParsedArgs result = {
        .algorithm = LB_ROUND_ROBIN,  // 默认使用轮询算法
        .should_exit = 0,
        .show_help = 0
    };

    // 如果没有参数，使用默认配置
    if (argc <= 1) {
        return result;
    }

    const char *arg = argv[1];

    // 处理退出命令
    if (strcmp(arg, "exit") == 0) {
        result.should_exit = 1;
        return result;
    }

    // 处理帮助命令
    if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0 || strcmp(arg, "help") == 0) {
        result.show_help = 1;
        return result;
    }

    // 解析算法类型
    if (strcmp(arg, "rr") == 0) {
        result.algorithm = LB_ROUND_ROBIN;
    } else if (strcmp(arg, "sticky") == 0) {
        result.algorithm = LB_STICKY_ROUND_ROBIN;
    } else if (strcmp(arg, "wrr") == 0) {
        result.algorithm = LB_WEIGHTED_ROUND_ROBIN;
    } else if (strcmp(arg, "hash") == 0) {
        result.algorithm = LB_HASH;
    } else if (strcmp(arg, "lc") == 0) {
        result.algorithm = LB_LEAST_CONNECTIONS;
    } else if (strcmp(arg, "lrt") == 0) {
        result.algorithm = LB_LEAST_RESPONSE_TIME;
    } else {
        // 未知算法，显示帮助
        printf("Unknown algorithm: %s\n", arg);
        result.show_help = 1;
    }

    return result;
}
