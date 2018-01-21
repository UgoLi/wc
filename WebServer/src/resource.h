////////////////////////////////////////////////////////////////////////
// resource.h - 声明资源管理
//
// 项目名称：WebServer (多线程版)
// 研发单位：达内科技
// 研发人员：闵卫
// 创建时间：2017年5月17日
// 修改记录：
//
#ifndef _RESORUCE_H
#define _RESOURCE_H

// 搜索资源
int searchResource (char const* path);

// 识别类型
int indentifyType (char const* path, char* type);

#endif // _RESOURCE_H
