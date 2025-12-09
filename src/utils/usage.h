#ifndef USAGE_H
#define USAGE_H

#include "../../include/types.h"

/**
 * 打印程序使用说明
 *
 * @param prog 程序名称
 */
void print_usage(const char *prog);

/**
 * 获取算法名称字符串
 *
 * @param algo 算法枚举值
 * @return 算法名称字符串
 */
const char *get_algorithm_name(LBAlgorithm algo);

#endif // USAGE_H
