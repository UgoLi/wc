/// @file DnsThread.cpp
/// @brief 实现#DnsThread类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"

// 线程处理函数
// 根据域名解析线程的任务实现基类中的纯虚函数
void* DnsThread::run (void) {
	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"域名解析线程开始");

	// 无限循环
	for (;;) {
		// 从原始统一资源定位符队列，弹出原始统一资
		// 源定位符，并显式转换为解析统一资源定位符
		DnsUrl dnsUrl = static_cast<DnsUrl> (
			g_app->m_urlQueues.popRawUrl ());

		// 在域名映射表中查找原始统一资源定位符中的主机域名
		map<string, string>::const_iterator it =
			m_domains.find (dnsUrl.m_domain);

		// 若找到了
		if (it != m_domains.end ()) {
			// 将与该主机域名对应的IP地址存入解析统一资源定位符
			dnsUrl.m_ip = it->second;

			// 记录调试日志
			g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
				"域名(%s)被映射为IP地址(%s)", dnsUrl.m_domain.c_str (),
				dnsUrl.m_ip.c_str ());

			// 将解析统一资源定位符压入解析统一资源定位符队列
			g_app->m_urlQueues.pushDnsUrl (dnsUrl);
			// 继续下一轮循环
			continue;
		}
                cout<<"###### 1 #########"<<endl;
		// 若没找到，则通过域名系统获取与该主机域名对应的IP地址
		hostent* host = gethostbyname (dnsUrl.m_domain.c_str ());

		// 若失败
		if (! host) {
			// 记录警告日志
			g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
				"gethostbyname: %s", hstrerror (h_errno));
			// 继续下一轮循环
			continue;
		}

		//     hostent
		// +-------------+
		// | h_name      -> xxx\0          - 正式主机名
		// | h_aliases   -> * * * ... NULL - 别名表
		// | h_addrtype  |  AF_INET        - 地址类型
		// | h_length    |  4              - 地址字节数
		// | h_addr_list -> * * * ... NULL - 地址表
		// +-------------+  +-> in_addr    - IPv4地址结构

		// 若地址类型不是IPv4
		if (host->h_addrtype != AF_INET) {
			// 记录警告日志
			g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
				"无效地址类型");
			// 继续下一轮循环
			continue;
		}

		// 将IPv4地址结构转换为点分十进制字符串，存入解析统一资源定位符
		dnsUrl.m_ip = inet_ntoa (**(in_addr**)host->h_addr_list);

		// 记录调试日志
		g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
			"域名(%s)被解析为IP地址(%s)", dnsUrl.m_domain.c_str (),
			dnsUrl.m_ip.c_str ());

		// 将解析统一资源定位符压入解析统一资源定位符队列
		g_app->m_urlQueues.pushDnsUrl (dnsUrl);

		// 若域名映射表最大容量无限或非满
		if (g_app->m_cfg.m_maxDomains < 0 ||
			m_domains.size () < (size_t)g_app->m_cfg.m_maxDomains)
			// 将主机域名及其IP地址加入域名映射表
			m_domains[dnsUrl.m_domain] = dnsUrl.m_ip;
		// 否则
		else
		// 若域名映射表最大容量非零
		if (g_app->m_cfg.m_maxDomains) {
			// 清空域名映射表
			m_domains.clear ();
			// 将主机域名及其IP地址加入域名映射表
			m_domains[dnsUrl.m_domain] = dnsUrl.m_ip;
                }
		/*
		// 初始化libevent库
		event_base* base = event_init ();
		// 初始化dns模块
		evdns_init ();

		// 一旦完成对该主机域名的解析即调用回调函数
		evdns_resolve_ipv4 (dnsUrl.m_domain.c_str (), 0,
			callback, &dnsUrl);
		// 进入事件循环
		event_dispatch ();

		// 释放libevent库
		event_base_free (base);
		*/
	}

	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"域名解析线程终止");
	// 终止线程
	return NULL;
}
/*
// 域名解析回调函数
void DnsThread::callback (
	int   res,   // [输入] 结果码
	char  type,  // [输入] 地址类型
	int   cnt,   // [输入] 地址数
	int   ttl,   // [输入] 解析记录缓存时间(秒)
	void* addrs, // [输入] 地址列表
	void* arg    // [输入] 回调参数
	) {
	// 回调参数指向解析统一资源定位符
	DnsUrl* dnsUrl = (DnsUrl*)arg;

	// 若结果错误或无地址
	if (res != DNS_ERR_NONE || ! cnt)
		// 记录警告日志
		g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"域名(%s)解析失败", dnsUrl->m_domain.c_str ());
	// 否则
	else
	// 若地址类型不是IPv4
	if (type != DNS_IPv4_A)
		// 记录警告日志
		g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"无效地址类型");
	// 否则
	else {
		// 将IPv4地址结构转换为点分十进制字符串，存入解析统一资源定位符
		dnsUrl->m_ip = inet_ntoa (*(in_addr*)addrs);

		// 记录调试日志
		g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
			"域名(%s)被解析为IP地址(%s)", dnsUrl.m_domain.c_str (),
			dnsUrl.m_ip.c_str ());

		// 将解析统一资源定位符压入解析统一资源定位符队列
		g_app->m_urlQueues.pushDnsUrl (dnsUrl);

		// 若域名映射表最大容量无限或非满
		if (g_app->m_cfg.m_maxDomains < 0 ||
			m_domains.size () < (size_t)g_app->m_cfg.m_maxDomains)
			// 将主机域名及其IP地址加入域名映射表
			m_domains[dnsUrl.m_domain] = dnsUrl.m_ip;
		// 否则
		else
		// 若域名映射表最大容量非零
		if (g_app->m_cfg.m_maxDomains) {
			// 清空域名映射表
			m_domains.clear ();
			// 将主机域名及其IP地址加入域名映射表
			m_domains[dnsUrl.m_domain] = dnsUrl.m_ip;
		}
	}

	// 退出事件循环
	event_loopexit (NULL);
}
*/
