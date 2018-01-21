////////////////////////////////////////////////////////////////////////
// server.h - 声明服务器
//
// 项目名称：WebServer (多线程版)
// 研发单位：达内科技
// 研发人员：闵卫
// 创建时间：2017年5月17日
// 修改记录：
//
#ifndef _SERVER_H
#define _SERVER_H

// 初始化服务器
int initServer (short port);

// 运行服务器
int runServer (char const* home);

// 终结化服务器
void deinitServer (void);

#endif // _SERVER_H
