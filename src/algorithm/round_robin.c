#include "round_robin.h"
#include <pthread.h>

/* ============================================================================
 *                          算法 1：轮询 (Round Robin)
 * ============================================================================
 *
 * 原理：
 *   按顺序依次选择服务器，循环往复，像发牌一样
 *
 * 示意图：
 *   请求1 → Server1    current_index: 0 → 1
 *   请求2 → Server2    current_index: 1 → 2
 *   请求3 → Server3    current_index: 2 → 0 (循环)
 *   请求4 → Server1    current_index: 0 → 1
 *   ...
 *
 * 优点：简单、公平、无状态
 * 缺点：不考虑服务器性能差异和当前负载
 *
 * 时间复杂度：O(n) 最坏情况（所有服务器都宕机时遍历一遍）
 */
Server * round_robin(LoadBalancer *lb) {
    pthread_mutex_lock(&lb->lock); // 加锁，防止多线程并发修改 current_index

    Server *selected = NULL;
    int attempts = 0; // 尝试次数，防止所有服务器都宕机时死循环

    // 最多尝试 server_count 次
    while (attempts < lb->server_count) {
        int idx = lb->current_index;

        // 移动到下一个，取模实现循环
        lb->current_index = (lb->current_index + 1) % lb->server_count;

        // 检查服务器是否存活
        if (lb->servers[idx].is_alive) {
            selected = &lb->servers[idx];
            break;
        }

        attempts++;
    }

    pthread_mutex_unlock(&lb->lock);
    return selected; // 全部宕机返回 NULL
}
