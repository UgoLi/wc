////////////////////////////////////////////////////////////////////////
// http.c - 实现HTTP协议
//
// 项目名称：WebServer (多线程版)
// 研发单位：达内科技
// 研发人员：闵卫
// 创建时间：2017年5月17日
// 修改记录：
// 2017年5月18日 - 支持浏览器长连接
//
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#define __USE_GNU
#include <string.h>
#include <time.h>

#include "http.h"

// 解析请求
int parseRequest (char const* req, HTTP_REQUEST* hreq) {
	sscanf (req, "%s%s%s", hreq->method, hreq->path, hreq->protocol);
	char* connection = strcasestr (req, "connection:");
	if (connection)
		sscanf (connection, "%*s%s", hreq->connection);
	printf ("%d.%ld> [%s][%s][%s][%s]\n", getpid (), syscall (SYS_gettid), hreq->method, hreq->path, hreq->protocol, hreq->connection);

	if (strcasecmp (hreq->method, "get")) {
		printf ("%d.%ld> 无效方法\n", getpid (), syscall (SYS_gettid));
		return -1;
	}

	if (strcasecmp (hreq->protocol, "http/1.0") && strcasecmp (hreq->protocol, "http/1.1")) {
		printf ("%d.%ld> 无效协议\n", getpid (), syscall (SYS_gettid));
		return -1;
	}

	return 0;
}

// 构造响应头
void constructHead (HTTP_RESPOND const* hres, char* head) {
	char dateTime[32];
	time_t now = time (NULL);
	strftime (dateTime, sizeof (dateTime), "%a, %d %b %Y %T GMT", gmtime (&now));

	sprintf (head,
		"%s %d %s\r\n"
		"Server: Tarena WebServer 1.0\r\n"
		"Date: %s\r\n"
		"Content-Type: %s\r\n"
		"Content-Length: %ld\r\n"
		"Connection: %s\r\n\r\n",
		hres->protocol, hres->status, hres->desc, dateTime, hres->type, hres->length, hres->connection);
}
