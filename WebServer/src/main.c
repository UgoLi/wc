////////////////////////////////////////////////////////////////////////
// main.c - 实现进程入口
//
// 项目名称：WebServer (多线程版)
// 研发单位：达内科技
// 研发人员：闵卫
// 创建时间：2017年5月17日
// 修改记录：
//
#include <stdio.h>
#include <stdlib.h>

#include "server.h"

int main (int argc, char* argv[]) {
	// 初始化服务器
	if (initServer (argc < 3 ? 80 : atoi (argv[2])) == -1)
		return EXIT_FAILURE;

	// 运行服务器
	if (runServer (argc < 2 ? "../www" : argv[1]) == -1)
		return EXIT_FAILURE;

	// 终结化服务器
	deinitServer ();

	return EXIT_SUCCESS;
}
