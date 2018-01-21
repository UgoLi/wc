/// @file Log.cpp
/// @brief 实现#Log类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"

// 按格式打印日志
void Log::printf (
	int         level,  // [输入] 日志等级
	char const* file,   // [输入] 源码文件
	int         line,   // [输入] 源码行号
	char const* format, // [输入] 格式化串
	...                 // [输入] 打印内容，数量和类型与格式
	                    // 化串format中的格式化标记对应匹配
	) const {
	// 若日志等级不低于配置文件中的"最低日志等级"
	if ((0 <= g_app->m_cfg.m_logLevel && g_app->m_cfg.m_logLevel <= level) ||
		(g_app->m_cfg.m_logLevel < 0 && (-g_app->m_cfg.m_logLevel == level ||
		level >= LEVEL_ERR))) {
		// 格式化日期时间
		char dateTime[32];
		time_t now = time (NULL);
		tm local;
		// 2015-11-20 23:59:07
		strftime (dateTime, sizeof (dateTime),
			"%Y-%m-%d %H:%M:%S", localtime_r (&now, &local));

		// 格式化日志头
		char header[256];
		// [2015-11-20 23:59:07][inf][pid=115705 tid=115708][Log.cpp:30]
		snprintf (header, sizeof (header),
			"[%s][%s][pid=%d tid=%ld][%s:%d]",
			dateTime, s_levels[level], getpid (),
			syscall (SYS_gettid), file, line);

		// 格式化日志内容
		char content[1024];
		// 变长参数表
		va_list ap;
		// 用format以后的参数初始化变长参数表
		va_start (ap, format);
		// 按format格式打印变长参数表中的内容
		vsnprintf (content, sizeof (content), format, ap);
		// 销毁变长参数表
		va_end (ap);

		// 打印日志条目
		::printf ("%s\n%s\n\n", header, content);
	}

	// 若日志等级不低于一般错误
	if (level >= LEVEL_ERR)
		// 提前终止进程
		exit (EXIT_FAILURE);
}
/*
// 按十六进制打印内存
void Log::printh (
	void*  buf, // [输入] 内存缓冲区
	size_t len  // [输入] 内存字节数
	) const {
	// 打印日志头：
	// OFFSET  01 ... 08  09 ... 16  ---- ASCII -----
	::printf ("OFFSET  01-02-03-04-05-06-07-08 " \
		" 09-10-11-12-13-14-15-16  ---- ASCII -----\n");

	// 将buf转换为数组指针p，指向由16个字节组成的数组
	unsigned char (*p)[16] = (unsigned char (*)[16])buf;
	// 按每行16个字节计算总行数，最后一行可能不到16个字节
	size_t lines = (len + 15) / 16;

	// 逐行打印
	for (size_t i = 0; i < lines; ++i) {
		// 行缓冲区和字节缓冲区
		char line[81], tmp[4];
		// 按六位16进制格式打印偏移地址
		sprintf (line, "%06X  ", (unsigned int)i * 16);

		// 逐字节打印16进制部分
		for (size_t j = 0; j < 16; ++j) {
			// 若第i行第j列的字节超出内存缓冲区范围
			if (i * 16 + j >= len)
				// 则打印空格
				sprintf (tmp, "   ");
			// 否则
			else
				// 按两位16进制格式打印该字节
				sprintf (tmp, "%02x ", p[i][j]);

			// 将格式化好的字节子串追加到行缓冲区
			strcat (line, tmp);

			// 第八列和第九列之间留两个空格
			if (j == 7)
				strcat (line, " ");
		}

		// 16进制部分和ASCII码部分之间留两个空格
		strcat (line, " ");

		// 逐字节打印ASCII码部分
		for (size_t j = 0; j < 16; ++j) {
			// 若第i行第j列的字节超出内存缓冲区范围
			if (i * 16 + j >= len)
				// 则提前退出循环
				break;

			// 若为可显示字符
			if (' ' <= p[i][j] && p[i][j] <= '~')
				// 则按字符格式打印该字节
				sprintf (tmp, "%c", p[i][j]);
			// 否则
			else
				// 打印"."
				sprintf (tmp, ".");

			// 将格式化好的字节子串追加到行缓冲区
			strcat (line, tmp);
		}

		// 将格式化好的行缓冲区打印输出
		::printf ("%s\n", line);
	}

	// 打印空行
	::printf ("\n");
}
*/
// 日志等级标签数组
// 从低到高依次为：
// dbg - 调试
// inf - 信息
// war - 警告
// err - 一般错误
// crt - 致命错误
char const* Log::s_levels[] = {"dbg", "inf", "war", "err", "crt"};
