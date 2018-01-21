////////////////////////////////////////////////////////////////////////
// socket.c - 实现网络通信
//
// 项目名称：WebServer (多线程版)
// 研发单位：达内科技
// 研发人员：闵卫
// 创建时间：2017年5月17日
// 修改记录：
//
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "socket.h"

static int s_sock = -1; // 侦听套接字

// 初始化套接字
int initSocket (short port) {
	printf ("%d> 创建套接字\n", getpid ());

	if ((s_sock = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
		perror ("socket");
		return -1;
	}

	printf ("%d> 设置套接字\n", getpid ());

	int on = 1;
	if (setsockopt (s_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on)) == -1) {
		perror ("setsockopt");
		return -1;
	}

	printf ("%d> 绑定端口号\n", getpid ());

	struct sockaddr_in addr;
	addr.sin_family      = AF_INET;
	addr.sin_port        = htons (port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind (s_sock, (struct sockaddr*)&addr, sizeof (addr)) == -1) {
		perror ("bind");
		return -1;
	}

	printf ("%d> 侦听套接字\n", getpid ());

	if (listen (s_sock, 1024) == -1) {
		perror ("listen");
		return -1;
	}

	return 0;
}

// 接受客户机连接
int acceptClient (void) {
	printf ("%d> 等待客户机连接\n", getpid ());

	struct sockaddr_in addrcli = {};
	socklen_t addrlen = sizeof (addrcli);
	int conn = accept (s_sock, (struct sockaddr*)&addrcli, &addrlen);
	if (conn == -1) {
		perror ("accept");
		return -1;
	}

	printf ("%d> 已连接客户机%s:%hu\n", getpid (), inet_ntoa (addrcli.sin_addr), ntohs (addrcli.sin_port));
	return conn;
}

// 接收请求
char* recvRequest (int conn) {
	char*   req = NULL;
	ssize_t len = 0;

	for (;;) {
		char buf[1024] = {};
		ssize_t rlen = recv (conn, buf, sizeof (buf) - 1, 0);

		if (rlen == -1) {
			perror ("recv");
			free (req);
			return NULL;
		}

		if (! rlen) {
			printf ("%d.%ld> 客户机关闭连接\n", getpid (), syscall (SYS_gettid));
			free (req);
			return NULL;
		}

		req = (char*)realloc (req, len + rlen + 1);
		memcpy (req + len, buf, rlen + 1);
		len += rlen;

		if (strstr (req, "\r\n\r\n"))
			break;
	}

	return req;
}

// 发送响应头
int sendHead (int conn, char const* head) {
	if (send (conn, head, strlen (head), 0) == -1) {
		perror ("send");
		return -1;
	}

	return 0;
}

// 发送响应体
int sendBody (int conn, char const* path) {
	int fd = open (path, O_RDONLY);
	if (fd == -1) {
		perror ("open");
		return -1;
	}

	char buf[1024];
	ssize_t len;

	while ((len = read (fd, buf, sizeof (buf))) > 0)
		if (send (conn, buf, len, 0) == -1) {
			perror ("send");
			return -1;
		}

	if (len == -1) {
		perror ("read");
		return -1;
	}

	close (fd);
	return 0;
}

// 终结化套接字
void deinitSocket (void) {
	close (s_sock);
}
