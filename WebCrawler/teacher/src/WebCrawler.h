/// @file WebCrawler.h
/// @brief 声明#WebCrawler类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _WEBCRAWLER_H
#define _WEBCRAWLER_H

#include "Log.h"
#include "Configurator.h"
#include "MultiIo.h"
#include "PluginMngr.h"
#include "UrlQueues.h"
#include "DnsThread.h"
#include "SendThread.h"
#include "StatThread.h"

class UrlFilter;
class Socket;

/// @brief 网络爬虫
class WebCrawler {
public:
	/// @brief 构造器
	WebCrawler (
		UrlFilter& filter ///< [in] 统一资源定位符过滤器
	);
	/// @brief 析构器
	~WebCrawler (void);

	/// @brief 初始化
	void init (
		bool daemon = false ///< [in] 是否以精灵进程方式运行
	);
	/// @brief 执行多路输入输出循环
	void exec (void);

	/// @brief 启动抓取任务
	/// @retval true  成功
	/// @retval false 失败
	bool startJob (void);

	/// @brief 停止抓取任务
	void stopJob (
		bool success = true ///< [in] 是否成功
	);

	/// @brief 取消抓取任务
	void cancelJob (void);

	/// @brief 记录状态
	void logStat (void);

	/// @brief 日志
	Log m_log;
	/// @brief 配置器
	Configurator m_cfg;
	/// @brief 插件管理器
	PluginMngr m_pluginMngr;
	/// @brief 统一资源定位符队列
	UrlQueues m_urlQueues;

private:
	/// @brief 使调用进程成为精灵进程
	void initDaemon (void) const;
	/// @brief 初始化最大文件描述符数
	void initMaxFiles (void) const;
	/// @brief 将种子统一资源定位符压入原始统一资源定位符队列
	void initSeedUrls (void);
	/// @brief 启动域名解析线程
	void initDns (void);
	/// @brief 启动发送线程
	void initSend (void);
	/// @brief 启动状态线程
	void initStat (void);

	/// @brief 域名解析线程
	DnsThread m_dnsThread;
	/// @brief 发送线程
	SendThread m_sendThread;
	/// @brief 状态线程
	StatThread m_statThread;
	/// @brief 启动时间
	time_t m_start;
	/// @brief 当前抓取任务数
	int m_curJobs;
	/// @brief 成功次数
	unsigned int m_success;
	/// @brief 失败次数
	unsigned int m_failure;
	/// @brief 多路输入输出
	MultiIo m_multiIo;
	/// @brief 套接字列表
	list<Socket*> m_sockets;
	/// @brief 互斥锁
	pthread_mutex_t m_mutex;
	/// @brief 条件变量
	pthread_cond_t m_cond;
};

/// @brief 应用程序对象
extern WebCrawler* g_app;

#endif // _WEBCRAWLER_H
