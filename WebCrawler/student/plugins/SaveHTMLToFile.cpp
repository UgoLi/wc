/// @file SaveHTMLToFile.cpp
/// @brief 实现#SaveHTMLToFile类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "SaveHTMLToFile.h"
#include "PluginMngr.h"
#include "Http.h"

SaveHTMLToFile g_pluginSaveHTMLToFile; // 超文本标记语言文件存储插件对象
WebCrawler*    g_app;                  // 应用程序对象

// 插件初始化
// 成功返回true，失败返回false
// 根据超文本标记语言文件存储插件的功能实现基类中的虚函数
bool SaveHTMLToFile::init (
	WebCrawler* app // [输入/输出] 应用程序对象
	) {
	// 以超文本标记语言插件的身份
	// 注册到应用程序对象的插件管理器中
	(g_app = app)->m_pluginMngr.registerHtmlPlugin (this);

	// 返回成功
	return true;
}

// 插件处理
// 成功返回true，失败返回false
// 根据超文本标记语言文件存储插件的功能实现基类中的虚函数
bool SaveHTMLToFile::handler (
	void* arg // [输入/输出] 插件参数
	) {
	// 超文本传输协议响应
	// 此处添加代码

	// 若超文本传输协议响应内容类型不是超文本标记语言
	// 此处添加代码
		// 返回失败
		// 此处添加代码

	// 将解析统一资源定位符转换为文件名字符串
	// 此处添加代码

	// 根据文件名打开超文本标记语言文件输出流
	// 此处添加代码
	// 若失败
	// 此处添加代码
		// 记录警告日志
		// 此处添加代码
		// 返回失败
		// 此处添加代码

	// 将超文本传输协议响应包体写入超文本标记语言文件输出流，若失败
	// 此处添加代码
		// 记录警告日志
		// 此处添加代码
		// 关闭超文本标记语言文件输出流
		// 此处添加代码
		// 删除超文本标记语言文件
		// 此处添加代码
		// 返回失败
		// 此处添加代码

	// 记录调试日志
	// 此处添加代码

	// 关闭超文本标记语言文件输出流
	// 此处添加代码
	// 返回成功
	// 此处添加代码
}
