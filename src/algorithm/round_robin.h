#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H

#include "../../include/types.h"

/**
 * 轮询算法
 * 按顺序依次选择服务器，循环往复
 *
 * @param lb 负载均衡器实例
 * @return 选中的服务器，如果所有服务器都宕机则返回 NULL
 */
Server *round_robin(LoadBalancer *lb);

#endif // ROUND_ROBIN_H
