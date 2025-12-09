#ifndef SERVER_H
#define SERVER_H

/**
 * 创建并配置监听 socket
 *
 * @param port 监听端口
 * @return 监听 socket 文件描述符，失败返回 -1
 */
int create_listen_socket(int port);

/**
 * 启动服务器主循环
 * 持续接受客户端连接并处理
 *
 * @param listen_fd 监听 socket 文件描述符
 */
void start_server(int listen_fd);

/**
 * 处理单个客户端连接
 *
 * @param client_fd 客户端 socket 文件描述符
 * @param client_ip 客户端 IP 地址
 */
void handle_client_connection(int client_fd, const char *client_ip);

#endif // SERVER_H
