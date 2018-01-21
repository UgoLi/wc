////////////////////////////////////////////////////////////////////////
// signals.c - 实现信号处理
//
// 项目名称：WebServer (多线程版)
// 研发单位：达内科技
// 研发人员：闵卫
// 创建时间：2017年5月17日
// 修改记录：
//
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#include "signals.h"

// 初始化信号
int initSignals (void) {
	printf ("%d> 忽略大部分信号\n", getpid ());

	int signum;
	for (signum = 1; signum <= 64; ++signum)
		if (signum != SIGINT && signum != SIGTERM)
			signal (signum, SIG_IGN);

	return 0;
}
