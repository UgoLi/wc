////////////////////////////////////////////////////////////////////////
// socket.h - 声明网络通信
//
// 项目名称：WebServer (多线程版)
// 研发单位：达内科技
// 研发人员：闵卫
// 创建时间：2017年5月17日
// 修改记录：
//
#ifndef _SOCKET_H
#define _SOCKET_H

// 初始化套接字
int initSocket (short port);

// 接受客户机连接
int acceptClient (void);

// 接收请求
char* recvRequest (int conn);

// 发送响应头
int sendHead (int conn, char const* head);

// 发送响应体
int sendBody (int conn, char const* path);

// 终结化套接字
void deinitSocket (void);

#endif // _SOCKET_H
