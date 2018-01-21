/// @file StatThread.cpp
/// @brief 实现#StatThread类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"

// 线程处理函数
// 根据状态线程的任务实现基类中的纯虚函数
void* StatThread::run (void) {
	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"状态线程开始");

	// 无限循环
	for (;;) {
		// 记录状态
		g_app->logStat ();

		// 睡眠状态间隔
		sleep (g_app->m_cfg.m_statInterval);
	}

	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"状态线程终止");
	// 终止线程
	return NULL;
}
