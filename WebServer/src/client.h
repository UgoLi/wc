////////////////////////////////////////////////////////////////////////
// client.h - 声明客户机处理
//
// 项目名称：WebServer (多线程版)
// 研发单位：达内科技
// 研发人员：闵卫
// 创建时间：2017年5月17日
// 修改记录：
//
#ifndef _CLIENT_H
#define _CLIENT_H

// 客户机参数
typedef struct tag_ClientArgs {
	char const* home;
	int conn;
}	CA;

// 客户机处理
void* client (void* arg);

#endif // _CLIENT_H
