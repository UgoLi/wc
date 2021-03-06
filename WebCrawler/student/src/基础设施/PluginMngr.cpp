/// @file PluginMngr.cpp
/// @brief 实现#PluginMngr类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "Plugin.h"
#include "StrKit.h"

// 加载插件
void PluginMngr::load (void) {
	// 遍历配置器中的插件列表向量
	// 此处添加代码
		// 利用配置器中的插件路径，构造插件共享库路径：
		// <插件路径>/<插件名>.so，例如：
		// ../plugins/MaxDepth.so
		// ../plugins/DomainLimit.so，等等
		// 此处添加代码

		// 记录调试日志
		// 此处添加代码

		// 打开插件共享库
		// 此处添加代码
		// 若失败
		// 此处添加代码
			// 记录一般错误日志
			// 此处添加代码

		// 利用插件名，构造插件对象名：
		// g_plugin<插件名>，例如：
		// g_pluginMaxDepth
		// g_pluginDomainLimit，等等
		// 此处添加代码

		// 从插件共享库中获取插件对象地址，并转换为其基类类型的指针
		// 此处添加代码
		// 若失败
		// 此处添加代码
			// 记录一般错误日志
			// 此处添加代码

		// 通过插件基类调用插件子类的初始化接口
		// 此处添加代码
	}
}

// 注册统一资源定位符插件
// 统一资源定位符插件通过此接口将其自身注册到插件管理器中
void PluginMngr::registerUrlPlugin (
	Plugin* plugin // [输入] 统一资源定位符插件
	) {
	// 将统一资源定位符插件指针压入统一资源定位符插件向量
	// 此处添加代码
}

// 注册超文本传输协议响应包头插件
// 超文本传输协议响应包头插件通过此接口将其自身注册到插件管理器中
void PluginMngr::registerHeaderPlugin (
	Plugin* plugin // [输入] 超文本传输协议响应包头插件
	) {
	// 将超文本传输协议响应包头插件指针压
	// 入超文本传输协议响应包头插件向量
	// 此处添加代码
}

// 注册超文本标记语言插件
// 超文本标记语言插件通过此接口将其自身注册到插件管理器中
void PluginMngr::registerHtmlPlugin (
	Plugin* plugin // [输入] 超文本标记语言插件
	) {
	// 将超文本标记语言插件指针压入超文本标记语言插件向量
	// 此处添加代码
}

// 调用统一资源定位符插件处理函数
// 成功返回true，失败返回false
// 依次调用每个统一资源定位符插件处理函数，只要有一个失败即返回失败
bool PluginMngr::invokeUrlPlugins (
	void* arg // [输入/输出] 插件参数
	) const {
	// 遍历统一资源定位符插件向量
	// 此处添加代码
		// 依次调用每个统一资源定位符插件的插件处理接口，若失败
		// 此处添加代码
			// 返回失败
			// 此处添加代码

	// 返回成功
	// 此处添加代码
}

// 调用超文本传输协议响应包头插件处理函数
// 成功返回true，失败返回false
// 依次调用每个超文本传输协议响应包头插
// 件处理函数，只要有一个失败即返回失败
bool PluginMngr::invokeHeaderPlugins (
	void* arg // [输入/输出] 插件参数
	) const {
	// 遍历超文本传输协议响应包头插件向量
	// 此处添加代码
		// 依次调用每个超文本传输协议响应
		// 包头插件的插件处理接口，若失败
		// 此处添加代码
			// 返回失败
			// 此处添加代码

	// 返回成功
	// 此处添加代码
}

// 调用超文本标记语言插件处理函数
// 依次调用每个超文本标记语言插件处理函数，忽略其成功失败
void PluginMngr::invokeHtmlPlugins (
	void* arg // [输入/输出] 插件参数
	) const {
	// 遍历超文本标记语言插件向量
	// 此处添加代码
		// 依次调用每个超文本标记语言插件
		// 的插件处理接口，不管成功失败
		// 此处添加代码
}
