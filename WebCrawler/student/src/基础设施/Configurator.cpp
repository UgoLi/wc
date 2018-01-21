/// @file Configurator.cpp
/// @brief 实现#Configurator类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "StrKit.h"

// 构造器
Configurator::Configurator (void) :
	m_logLevel     (Log::LEVEL_DBG), // 最低日志等级缺省为调试
	m_maxJobs      (-1),             // 最大抓取任务数缺省为无限
	m_maxDepth     (-1),             // 最大递归深度缺省为无限
	m_maxFailures  (-1),             // 最大失败次数缺省为无限
	m_maxRawUrls   (-1),             // 原始统一资源定位符队
	                                 // 列最大容量缺省为无限
	m_maxDnsUrls   (-1),             // 解析统一资源定位符队
	                                 // 列最大容量缺省为无限
	m_maxDomains   (-1),             // 域名映射表最大容量缺省为无限
	m_resTimeout   (-1),             // 响应超时缺省为不设超时
	m_statInterval (0),              // 状态间隔缺省为不设定时器
	m_downloadPath ("."),            // 下载路径
	m_pluginsPath  (".") {}          // 插件路径

// 从指定的配置文件中加载配置信息
void Configurator::load (
	string const& cfgFile // [输入] 配置文件路径
	) {
	// 根据路径打开配置文件输入流
	ifstream ifs(cfgFile.c_str());
    // 若失败
    if(!ifs){
		// 记录一般错误日志
        g_app->m_log.printf(Log::LEVEL_ERR,__FILE__,__LINE__,
                           "打开文件(%s)失败",cfgFile.c_str());
    }

	// 文件行字符串
    string line;
	// 逐行读取配置文件
    for(int lineNo=0;getline(ifs,line);++lineNo){
		// 修剪行字符串
        StrKit::trim(line);

		// 若为注释行或空行
		if(line[0]=='#'||line[0]=='\0')
			// 继续下一轮循环
			continue;

		// 拆分行字符串，以等号为分隔符，最多拆分一次
        vector<string> strv= StrKit::split(line,"=",1);
		// 若成功拆分出键和值两个子串
		if(strv.size()==2){
            // 若键为"LOG_LEVEL"
            if(strv[0]=="LOG_LEVEL"){
				// 则值为"最低日志等级"
				m_logLevel=strv[1].toInt();
            }
            else{
			// 此处添加代码
            }

			// 若键为"LOG_FILE"
			if(){
                // 则值为"日志文件路径"
				// 此处添加代码
            }
            else{
			// 此处添加代码
            }

			// 若键为"MAX_JOBS"
			if(){
				// 则值为"最大抓取任务数"
				// 此处添加代码
            }else{
			// 此处添加代码
            }

			// 若键为"MAX_DEPTH"
			if(){
				// 则值为"最大递归深度"
				// 此处添加代码
            }
			else{
			// 此处添加代码
            }

			// 若键为"MAX_FAILURES"
			if(){
				// 则值为"最大失败次数"
				// 此处添加代码
            }
			else{
			// 此处添加代码
            }

			// 若键为"MAX_RAW_URLS"
			if(){
				// 则值为"原始统一资源定位符队列最大容量"
				// 此处添加代码
            }
			else{
			// 此处添加代码
            }

			// 若键为"MAX_DNS_URLS"
			if(){
				// 则值为"解析统一资源定位符队列最大容量"
				// 此处添加代码
            }
			else{
			// 此处添加代码
            }

			// 若键为"MAX_DOMAINS"
			if(){
				// 则值为"域名映射表最大容量"
				// 此处添加代码
            }
			else{
			// 此处添加代码
            }

			// 若键为"RES_TIMEOUT"
		    if(){
				// 则值为"响应超时"
				// 此处添加代码
            }
			else{
			// 此处添加代码
            }

			// 若键为"STAT_INTERVAL"
			if(){
				// 则值为"状态间隔"
				// 此处添加代码
            }
			else{
			// 此处添加代码
            }

			//若键为"SEED_URLS"
			if(){
				// 则值为"种子统一资源定位符"
				// 此处添加代码
            }
            else{			
			// 此处添加代码
            }

			// 若键为"INCLUDE_PREFIXES"
			if(){
				// 则值为"包含前缀"
				// 此处添加代码
            }
			else{
			// 此处添加代码
            }

			// 若键为"EXCLUDE_PREFIXES"
			if(){
				// 则值为"排除前缀"
				// 此处添加代码
            }
			else{
			// 此处添加代码
            }

			// 若键为"DOWNLOAD_PATH"
			if(){
				// 则值为"下载路径"
				// 此处添加代码
            }
            else{
			// 此处添加代码
            }

			// 若键为"PLUGINS_PATH"
			if(){
				// 则值为"插件路径"
				// 此处添加代码
            }
			else{
			// 此处添加代码
            }

			// 若键为"LOAD_PLUGIN"
			if()
				// 则值为"插件名"
			    ;
			else
			// 此处添加代码
			
            // 若键为"ACCEPT_TYPE"
			if(){
				// 则值为"接受类型"
				// 此处添加代码
            }
			else{
			// 此处添加代码
				// 记录一般错误日志
				// 此处添加代码
            }
        }
		else{
            // 此处添加代码
			// 记录一般错误日志
			// 此处添加代码        
        }
    }
	// 关闭配置文件输入流
	ifs.close();
}
