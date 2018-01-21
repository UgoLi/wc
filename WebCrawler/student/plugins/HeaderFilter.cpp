/// @file HeaderFilter.cpp
/// @brief 实现#HeaderFilter类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "HeaderFilter.h"
#include "PluginMngr.h"
#include "Http.h"

HeaderFilter g_pluginHeaderFilter; // 超文本传输协议响应包头过滤器插件对象
WebCrawler*  g_app;                // 应用程序对象

// 插件初始化
// 成功返回true，失败返回false
// 根据超文本传输协议响应包头过滤器插件的功能实现基类中的虚函数
bool HeaderFilter::init (
	WebCrawler* app // [输入/输出] 应用程序对象
	) {
	// 以超文本传输协议响应包头插件的身份
	// 注册到应用程序对象的插件管理器中
	(g_app = app)->m_pluginMngr.registerHeaderPlugin (this);

	// 返回成功
	return true;
}

// 插件处理
// 成功返回true，失败返回false
// 根据超文本传输协议响应包头过滤器插件的功能实现基类中的虚函数
bool HeaderFilter::handler (
	void* arg // [输入/输出] 插件参数
	) {
	// 超文本传输协议响应包头
	// 此处添加代码

	// 若超文本传输协议响应状态码不在合理区间[200,300)之内
	// 此处添加代码
		// 记录调试日志
		// 此处添加代码
		// 返回失败，不再接收该响应包包体
		// 此处添加代码

	// 若超文本传输协议响应内容类型不是超文本标记语言
	// 此处添加代码
		// 字符串向量只读迭代器
		// 此处添加代码

		// 超文本传输协议响应内容类型与
		// 配置器中的哪个接受类型匹配
		// 此处添加代码

		// 若超文本传输协议响应内容类型与
		// 配置器中任何接受类型都不匹配
		// 此处添加代码
			// 记录调试日志
			// 此处添加代码
			// 返回失败，不再接收该响应包包体
			// 此处添加代码

	// 返回成功，继续接收该响应包包体
	// 此处添加代码
}
