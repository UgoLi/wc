/// @file Socket.cpp
/// @brief 实现#Socket类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "Socket.h"
#include "StrKit.h"

// 构造器
Socket::Socket (
	DnsUrl const& dnsUrl // [输入] 服务器统一资源定位符
	) : m_sockfd (-1),       // 套接字描述符缺省为无效
	    m_dnsUrl (dnsUrl) {} // 初始化服务器统一资源定位符

// 析构器
Socket::~Socket (void) {
	// 若套接字描述符有效
	if (m_sockfd >= 0) {
		// 关闭套接字描述符
		close (m_sockfd);
		// 将套接字描述符设置为无效
		m_sockfd = -1;
	}
}

// 发送超文本传输协议请求
// 成功返回true，失败返回false
bool Socket::sendRequest (void) {
	// 创建套接字(TCP)，并获得其描述符，若失败
	if ((m_sockfd = socket (PF_INET, SOCK_STREAM, 0)) == -1) {
		// 记录警告日志
		g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"socket: %s", strerror (errno));
		// 返回失败
		return false;
	}

	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"创建套接字(%d)成功", m_sockfd);

	// 服务器地址结构
	sockaddr_in addr;
	// 清零
	bzero (&addr, sizeof (addr));
	// 地址族
	addr.sin_family = AF_INET;
	// 端口号
	addr.sin_port = htons (m_dnsUrl.m_port);
	// 将点分十进制字符串形式的IPv4地址转换为32位无符号整数，若失败
	if (! inet_aton (m_dnsUrl.m_ip.c_str (), &(addr.sin_addr))) {
		// 记录警告日志
		g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"inet_aton: %s", strerror (errno));
		// 返回失败
		return false;
	}

	// 向服务器发起连接请求，若连接失败
	if (connect (m_sockfd, (sockaddr*)&addr, sizeof (addr)) == -1) {
		// 记录警告日志
		g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"connect: %s", strerror (errno));
		// 返回失败
		return false;
	}

	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"连接服务器(%s:%hd)成功", m_dnsUrl.m_ip.c_str (),
		m_dnsUrl.m_port);

	// 获取套接字的状态标志
	int flags = fcntl (m_sockfd, F_GETFL);
	// 若失败
	if (flags == -1) {
		// 记录警告日志
		g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"fcntl: %s", strerror (errno));
		// 返回失败
		return false;
	}

	// 为套接字增加非阻塞状态标志，若失败
	if (fcntl (m_sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		// 记录警告日志
		g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
			"fcntl: %s", strerror (errno));
		// 返回失败
		return false;
	}

	// 输出字符串流
	ostringstream oss;
	// 在输出字符串流中格式化超文本传输协议请求
	oss <<
		"GET /" << m_dnsUrl.m_path << " HTTP/1.0\r\n"
		"Host: " << m_dnsUrl.m_domain << "\r\n"
		"Accept: */*\r\n"
		"Connection: Close\r\n"
		"User-Agent: Mozilla/5.0\r\n"
		"Referer: " << m_dnsUrl.m_domain << "\r\n\r\n";
	// 从输出字符串流中获取超文本传输协议请求字符串
	string request = oss.str ();
	// 从超文本传输协议请求字符串中获取其内存缓冲区指针
	char const* buf = request.c_str ();
	// 成功发送的字节数
	ssize_t slen;
	// 失败次数
	int failures = 0;

	// 分多次将超文本传输协议请求包发完
	for (size_t len = request.size (); len;
		len -= slen, buf += slen) {
		// 发送超文本传输协议请求，若失败
		if ((slen = send (m_sockfd, buf, len, 0)) == -1) {
			// 若因内核发送缓冲区满而导致失败
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				// 若失败次数太多
				if (0 <= g_app->m_cfg.m_maxFailures &&
					g_app->m_cfg.m_maxFailures < ++failures) {
					// 记录警告日志
					g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
						"发送超文本传输协议请求失败次数超限(%d>%d)，放弃发送",
						failures, g_app->m_cfg.m_maxFailures);
					// 返回失败
					return false;
				}

				// 延迟重发
				usleep (100000);
				slen = 0;
				continue;
			}

			// 其它原因导致失败，记录警告日志
			g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
				"send: %s", strerror (errno));
			// 返回失败
			return false;
		}

		// 失败次数清零
		failures = 0;
	}

	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"发送超文本传输协议请求包%u字节\n\n%s", request.size (),
		request.c_str ());

	// 发送超文本传输协议请求时间
	m_send = time (NULL);

	// 返回成功
	return true;
}

// 接收超文本传输协议响应
// 成功返回true，失败返回false
bool Socket::recvResponse (void) {
	// 失败次数
	int failures = 0;
	// 超文本传输协议响应对象
	HttpResponse res (m_dnsUrl);
	// 超文本传输协议响应包头尚未解析
	bool headerParsed = false;

	// 分多次将超文本传输协议响应包收完
	for (;;) {
		// 接收缓冲区
		char buf[1024] = {};
		// 接收超文本传输协议响应，注意留一个字符放'\0'
		ssize_t rlen = recv (m_sockfd, buf,
			sizeof (buf) - sizeof (buf[0]), 0);

		// 若失败
		if (rlen == -1) {
			// 若因内核接收缓冲区空或被信号中断而导致失败
			if (errno == EAGAIN || errno == EWOULDBLOCK ||
				errno == EINTR) {
				// 若失败次数太多
				if (0 <= g_app->m_cfg.m_maxFailures &&
					g_app->m_cfg.m_maxFailures < ++failures) {
					// 记录警告日志
					g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
						"接收超文本传输协议响应失败次数超限(%d>%d)，放弃接收",
						failures, g_app->m_cfg.m_maxFailures);
					// 返回失败
					return false;
				}

				// 延迟重收
				usleep (100000);
				continue;
			}

			// 其它原因导致失败，记录警告日志
			g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
				"recv: %s", strerror (errno));
			// 返回失败
			return false;
		}

		// 失败次数清零
		failures = 0;

		// 若服务器已关闭连接
		if (! rlen) {
			// 记录调试日志
			g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
				"接收超文本传输协议响应包体%u字节", res.m_len);

			// 若超文本传输协议响应的内容类型为超文本标记语言
			if (res.m_header.m_contentType.find (
				"text/html", 0) != string::npos)
				// 从超文本标记语言页面内容中抽取统一资源定位符
				g_app->m_urlQueues.extractUrl (res.m_body, m_dnsUrl);

			// 调用超文本标记语言插件处理函数
			g_app->m_pluginMngr.invokeHtmlPlugins (&res);
			// 超文本传输协议响应接收并处理完毕，跳出循环
			break;
		}
		/*
		// 记录调试日志
		g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
			"接收超文本传输协议响应%u字节", rlen);
		// 按十六进制打印接收到的数据
		g_app->m_log.printh (buf, rlen);
		*/
		// 扩展超文本传输协议响应包体缓冲区，以容纳新
		// 接收到的响应数据，注意多分配一个字符放'\0'
		char* body = (char*)realloc (res.m_body,
			res.m_len + rlen + sizeof (res.m_body[0]));
		// 若失败
		if (! body) {
			// 记录警告日志
			g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
				"realloc: %s", strerror (errno));
			// 返回失败
			return false;
		}

		// 将新接收到的响应数据，连同终止空字符('\0')一起，
		// 从接收缓冲区复制到超文本传输协议响应包体缓冲区
		memcpy (body + res.m_len, buf, rlen + sizeof (res.m_body[0]));
		// 更新超文本传输协议响应包体指针
		res.m_body = body;
		// 更新超文本传输协议响应包体长度
		res.m_len += rlen;

		// 若超文本传输协议响应包头已经解析
		if (headerParsed) {
			// 若超文本传输协议响应包体长度已达到
			// 超文本传输协议响应包头中的内容长度
			if (0 <= res.m_header.m_contentLength &&
				(size_t)res.m_header.m_contentLength <= res.m_len) {
				// 记录调试日志
				g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
					"接收超文本传输协议响应包体%u字节", res.m_len);

				// 若超文本传输协议响应的内容类型为超文本标记语言
				if (res.m_header.m_contentType.find (
					"text/html", 0) != string::npos)
					// 从超文本标记语言页面内容中抽取统一资源定位符
					g_app->m_urlQueues.extractUrl (res.m_body, m_dnsUrl);

				// 调用超文本标记语言插件处理函数
				g_app->m_pluginMngr.invokeHtmlPlugins (&res);
				// 超文本传输协议响应接收并处理完毕，跳出循环
				break;
			}
		}
		// 否则
		else {
			// 在已收到的超文本传输协议响应中查找包
			// 头结束标志————连续出现的两个回车换行
			char* p = strstr (res.m_body, "\r\n\r\n");
			// 若找到了
			if (p) {
				// 截取超文本传输协议响应包头
				string header (res.m_body, p + 2 - res.m_body);

				// 记录调试日志
				g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
					"接收超文本传输协议响应包头%u字节\n\n%s",
					header.size (), header.c_str ());

				// 解析超文本传输协议响应包头
				res.m_header = parseHeader (header);

				// 调用超文本传输协议响应包头插件处理函数，若失败
				if (! g_app->m_pluginMngr.invokeHeaderPlugins (
					&res.m_header))
					// 返回失败
					return false;

				// 跳过超文本传输协议响应包头结束标志————
				// 连续出现的两个回车换行————共四个字节，
				// 得到超文本传输协议响应包体在超文本传
				// 输协议响应包体缓冲区中的起始地址
				p += 4;
				// 计算已接收超文本传输协议响应包体长度
				res.m_len -= p - res.m_body;
				// 分配足以容纳已接收超文本传输协议响应
				// 包体(含终止空字符('\0'))的临时缓冲区
				char* tmp = new (nothrow) char[
					res.m_len + sizeof (res.m_body[0])];
				// 若失败
				if (! tmp) {
					// 记录警告日志
					g_app->m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
						"new: %s", strerror (errno));
					// 返回失败
					return false;
				}
				// 将已接收超文本传输协议响应包体，连同
				// 终止空字符('\0')一起，从超文本传输协
				// 议响应包体缓冲区复制到临时缓冲区
				memcpy (tmp, p,
					res.m_len + sizeof (res.m_body[0]));
				// 将已接收超文本传输协议响应包体，连同
				// 终止空字符('\0')一起，从临时缓冲区复
				// 制到超文本传输协议响应包体缓冲区
				memcpy (res.m_body, tmp,
					res.m_len + sizeof (res.m_body[0]));
				// 释放临时缓冲区
				delete[] tmp;
				// 借助临时缓冲区移动位于超文本传输协议
				// 响应包体缓冲区中的已接收超文本传输协
				// 议响应包体，可以有效规避源内存和目的
				// 内存间发生重叠覆盖的潜在风险

				// 超文本传输协议响应包头已被解析
				headerParsed = true;
			}
		}
	}

	// 返回成功
	return true;
}

// 获取套接字描述符
// 返回套接字描述符
int Socket::sockfd (void) const {
	// 返回套接字描述符
	return m_sockfd;
}

// 是否超时
// 超时返回true，未超时返回false
bool Socket::timeout (void) const {
	// 若配置器中的响应超时有效，且该套
	// 接字的响应时间已超限，则返回true
	return 0 <= g_app->m_cfg.m_resTimeout &&
		g_app->m_cfg.m_resTimeout < time (NULL) - m_send;
}

// 解析超文本传输协议响应包头
// 返回超文本传输协议响应包头
HttpHeader Socket::parseHeader (
	string const& strHeader // [输入] 超文本传输协议响应包头字符串
	) const {
	// 复制超文本传输协议响应包头字符串
	string str = strHeader;
	// 超文本传输协议响应包头
	HttpHeader header;

	// 超文本传输协议响应包头实例：
	//
	// HTTP/1.1 200 OK
	// Server: nginx
	// Date: Wed, 26 Oct 2016 10:52:04 GMT
	// Content-Type: text/html;charset=UTF-8
	// Content-Length: 22955
	// Connection: close
	// Vary: Accept-Encoding
	// Server-Host: classa-study30
	// Set-Cookie: Domain=study.163.com
	// Cache-Control: no-cache
	// Pragma: no-cache
	// Expires: -1
	// Content-Language: en-US

	// 在超文本传输协议响应包头字符串中查找第一个回车换行
	string::size_type pos = str.find ("\r\n", 0);
	// 若找到了
	if (pos != string::npos) {
		// 拆分超文本传输协议响应包头的第一
		// 行，以空格为分隔符，最多拆分两次
		// HTTP/1.1 200 OK
		//    0    ^ 1 ^ 2
		vector<string> strv = StrKit::split (
			str.substr (0, pos), " ", 2);
		// 若成功拆分出三个子串
		if (strv.size () == 3) {
			// 其中的第二个子串即为状态码
			header.m_statusCode = atoi (strv[1].c_str ());

			// 截取除第一行(含回车换行)以外的其余内容，继续解析
			str = str.substr (pos + 2);

			// 在超文本传输协议响应包头字符串中逐个查找回车换行
			while ((pos = str.find ("\r\n", 0)) != string::npos) {
				// 拆分超文本传输协议响应包头字符串中的
				// 每一行，以冒号为分隔符，最多拆分一次
				strv = StrKit::split (str.substr (0, pos), ":", 1);

				// 若成功拆分出键和值两个子串
				if (strv.size () == 2) {
					// 若键为"content-type"
					if (! strcasecmp (strv[0].c_str (), "content-type"))
						// 则值为"内容类型"
						header.m_contentType = strv[1];
					// 否则
					else
					// 若键为"content-length"
					if (! strcasecmp (strv[0].c_str (), "content-length"))
						// 则值为"内容长度"
						header.m_contentLength = atoi (strv[1].c_str ());
				}

				// 若已解出"内容类型"和"内容长度"
				if (! header.m_contentType.empty () &&
					header.m_contentLength != -1)
					// 则解析完成
					break;

				// 截取除该行(含回车换行)以外的其余内容，继续解析
				str = str.substr (pos + 2);
			}
		}
	}

	// 返回超文本传输协议响应包头
	return header;
}
