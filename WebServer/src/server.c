////////////////////////////////////////////////////////////////////////
// server.c - 实现服务器
//
// 项目名称：WebServer (多线程版)
// 研发单位：达内科技
// 研发人员：闵卫
// 创建时间：2017年5月17日
// 修改记录：
//
#include <pthread.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "signals.h"
#include "socket.h"
#include "client.h"

// 初始化最大文件描述符数
static int initMaxFiles (void) {
	// 资源限制结构
	struct rlimit rl;

	// 获取当前进程最大文件描述符数
	if (getrlimit (RLIMIT_NOFILE, &rl) == -1) {
		perror ("getrlimit");
		return -1;
	}

	// 若当前进程最大文件描述符数未达极限
	if (rl.rlim_cur < rl.rlim_max) {
		// 将当前进程最大文件描述符数设为极限
		rl.rlim_cur = rl.rlim_max;

		// 设置当前进程最大文件描述符数
		if (setrlimit (RLIMIT_NOFILE, &rl) == -1) {
			perror ("setrlimit");
			return -1;
		}
	}

	return 0;
}

// 初始化服务器
int initServer (short port) {
	// 初始化最大文件描述符数
	if (initMaxFiles () == -1)
		return -1;

	// 初始化信号
	if (initSignals () == -1)
		return -1;

	// 初始化套接字
	if (initSocket (port) == -1)
		return -1;

	return 0;
}

// 运行服务器
int runServer (char const* home) {
	for (;;) {
		// 接受客户机连接
		int conn = acceptClient ();
		if (conn == -1)
			return -1;

		// 客户机处理
		pthread_t tid;
		pthread_attr_t attr;
		pthread_attr_init (&attr);
		pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
		CA* ca = (CA*)malloc (sizeof (CA));
		ca->home = home;
		ca->conn = conn;
		int error = pthread_create (&tid, &attr, client, ca);
		pthread_attr_destroy (&attr);
		if (error) {
			printf ("pthread_create: %s\n", strerror (error));
			return -1;
		}
	}

	return 0;
}

// 终结化服务器
void deinitServer (void) {
	// 终结化套接字
	deinitSocket ();
}
