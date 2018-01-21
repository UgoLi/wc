////////////////////////////////////////////////////////////////////////
// client.c - 实现客户机处理
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
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "socket.h"
#include "http.h"
#include "resource.h"

// 客户机处理
void* client (void* arg) {
	CA* ca = (CA*)arg;
	printf ("%d.%ld> 客户机处理开始\n", getpid (), syscall (SYS_gettid));

	for (;;) {
		printf ("%d.%ld> 接收请求\n", getpid (), syscall (SYS_gettid));

		// 接收请求
		char* req = recvRequest (ca->conn);
		if (! req)
			goto escape;

		printf ("%d.%ld> 请求电文\n\n%s", getpid (), syscall (SYS_gettid), req);

		printf ("%d.%ld> 解析请求\n", getpid (), syscall (SYS_gettid));

		// 解析请求
		HTTP_REQUEST hreq = {};
		if (parseRequest (req, &hreq) == -1) {
			free (req);
			goto escape;
		}

		free (req);

		// 资源路径
		char path[PATH_MAX + 1];
		strcpy (path, ca->home);
		if (path[strlen (path) - 1] == '/')
			path[strlen (path) - 1] = '\0';
		strcat (path, hreq.path);
		if (! strcmp (hreq.path, "/"))
			strcat (path, "index.html");
		printf ("%d.%ld> 资源路径：%s\n", getpid (), syscall (SYS_gettid), path);

		// 搜索资源
		HTTP_RESPOND hres = {"HTTP/1.1", 200, "OK", "text/html"};
		if (searchResource (path) == -1) {
			hres.status = 404;
			strcpy (hres.desc, "Not Found");
			strcpy (path, "../www/404.html");
		}
		else
		// 识别类型
		if (indentifyType (path, hres.type) == -1) {
			hres.status = 404;
			strcpy (hres.desc, "Not Found");
			strcpy (path, "../www/404.html");
		}

		// 长度
		struct stat st;
		if (stat (path, &st) == -1) {
			perror ("stat");
			goto escape;
		}
		hres.length = st.st_size;

		// 连接
		if (strlen (hreq.connection))
			strcpy (hres.connection, hreq.connection);
		else
		if (! strcasecmp (hreq.protocol, "HTTP/1.0"))
			strcpy (hres.connection, "close");
		else
			strcpy (hres.connection, "keep-alive");

		printf ("%d.%ld> 构造响应\n", getpid (), syscall (SYS_gettid));

		// 构造响应头
		char head[1024];
		constructHead (&hres, head);

		printf ("%d.%ld> 响应电文\n\n%s", getpid (), syscall (SYS_gettid), head);

		printf ("%d.%ld> 发送响应\n", getpid (), syscall (SYS_gettid));

		// 发送响应头
		if (sendHead (ca->conn, head) == -1)
			goto escape;

		// 发送响应体
		if (sendBody (ca->conn, path) == -1)
			goto escape;

		if (! strcasecmp (hres.connection, "close"))
			break;
	}

escape:
	close (ca->conn);
	free (ca);
	printf ("%d.%ld> 客户机处理结束\n", getpid (), syscall (SYS_gettid));
	return NULL;
}
