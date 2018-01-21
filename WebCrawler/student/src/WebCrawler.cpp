/// @file WebCrawler.cpp
/// @brief 实现#WebCrawler类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#include "Precompile.h"
#include "WebCrawler.h"
#include "RecvThread.h"
#include "Socket.h"
#include "StrKit.h"

// 构造器
WebCrawler::WebCrawler (
	UrlFilter& filter // [输入] 统一资源定位符过滤器
	) : m_urlQueues (filter),      // 初始化统一资源定位符队列
	    m_start     (time (NULL)), // 初始化启动时间为当前系统时间
	    m_curJobs   (0),           // 初始化当前抓取任务数为0
	    m_success   (0),           // 初始化成功次数为0
	    m_failure   (0) {          // 初始化失败次数为0
	// 初始化互斥锁
	pthread_mutex_init (&m_mutex, NULL);
	// 初始化条件变量
	pthread_cond_init (&m_cond, NULL);
}

// 析构器
WebCrawler::~WebCrawler (void) {
	// 销毁条件变量
	pthread_cond_destroy (&m_cond);
	// 销毁互斥锁
	pthread_mutex_destroy (&m_mutex);
}

// 初始化
void WebCrawler::init (
	bool daemon /* = false */ // [输入] 是否以精灵进程方式运行
	) {
	// 通过配置器从指定的配置文件中加载配置信息
	// 此处添加代码

	// 若以精灵进程方式运行
	// 此处添加代码
		// 使调用进程成为精灵进程
		// 此处添加代码

	// 通过插件管理器加载插件
	// 此处添加代码

	// 若切换工作目录至下载路径失败
	// 此处添加代码
		// 记录一般错误日志
		// 此处添加代码

	// 初始化最大文件描述符数
	// 此处添加代码
	// 将种子统一资源定位符压入原始统一资源定位符队列
	// 此处添加代码

	// 启动域名解析线程
	// 此处添加代码
	// 启动发送线程
	// 此处添加代码
	// 若配置器中的状态间隔有效
	// 此处添加代码
		// 启动状态线程
		// 此处添加代码
}

// 执行多路输入输出循环
void WebCrawler::exec (void) {
	// 无限循环
	for (;;) {
		// 输入输出事件结构数组
		epoll_event events[10];
		// 等待所关注的文件描述符，两秒超时
		int fds = m_multiIo.wait (events, sizeof (events) /
			sizeof (events[0]), 2000);

		// 加锁互斥锁
		pthread_mutex_lock (&m_mutex);

		// 若超时或被信号中断
		if (fds <= 0) {
			// 若没有抓取任务且原始统一资源定位符
			// 队列和解析统一资源定位符队列都为空
			if (! m_curJobs &&
				m_urlQueues.emptyRawUrl () &&
				m_urlQueues.emptyDnsUrl ()) {

				// 等一秒
				sleep (1);

				// 若没有抓取任务且原始统一资源定位符
				// 队列和解析统一资源定位符队列都为空
				if (! m_curJobs &&
					m_urlQueues.emptyRawUrl () &&
					m_urlQueues.emptyDnsUrl ())
					// 抓取任务完成，跳出循环
					break;
			}

			// 若抓取任务到限且原始统一资源定位符
			// 队列和解析统一资源定位符队列都为满
			if (m_curJobs == m_cfg.m_maxJobs &&
				m_urlQueues.fullRawUrl () &&
				m_urlQueues.fullDnsUrl ()) {

				// 等一秒
				sleep (1);

				// 若抓取任务到限且原始统一资源定位符
				// 队列和解析统一资源定位符队列都为满
				if (m_curJobs == m_cfg.m_maxJobs &&
					m_urlQueues.fullRawUrl () &&
					m_urlQueues.fullDnsUrl ())
					// 清空原始统一资源定位符队列，避免死锁
					m_urlQueues.clearRawUrl ();
			}

			// 遍历套接字列表
			for (list<Socket*>::iterator it = m_sockets.begin ();
				it != m_sockets.end ();)
				// 若未超时
				if (! (*it)->timeout ())
					// 下一个
					++it;
				// 否则
				else {
					// 记录警告日志
					m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
						"套接字(%d)超时", (*it)->sockfd ());
					// 删除需要被关注的文件描述符
					m_multiIo.del ((*it)->sockfd ());
					// 销毁套接字对象
					delete *it;
					// 从套接字列表中删除
					it = m_sockets.erase (it);
					// 取消抓取任务
					cancelJob ();
				}
		}

		// 依次处理每个处于就绪状态的文件描述符
		for (int i = 0; i < fds; ++i) {
			// 从事件结构中取出套接字对象指针
			Socket* socket = (Socket*)events[i].data.ptr;

			// 若为异常事件
			if (events[i].events & EPOLLERR ||
				events[i].events & EPOLLHUP ||
				! (events[i].events & EPOLLIN)) {
				// 记录警告日志
				m_log.printf (Log::LEVEL_WAR, __FILE__, __LINE__,
					"套接字(%d)异常", socket->sockfd ());
				// 删除需要被关注的文件描述符
				m_multiIo.del (socket->sockfd ());
				// 销毁套接字对象
				delete socket;
				// 从套接字列表中删除
				m_sockets.remove (socket);
				// 取消抓取任务
				cancelJob ();
				// 继续下一轮循环
				continue;
			}

			// 删除需要被关注的文件描述符
			m_multiIo.del (socket->sockfd ());
			// 从套接字列表中删除
			m_sockets.remove (socket);

			// 记录调试日志
			m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
				"套接字(%d)读就绪，创建接收线程", socket->sockfd ());

			// 创建接收线程对象并启动接收线程
			// 此处添加代码
		}

		// 解锁互斥锁
		pthread_mutex_unlock (&m_mutex);
	}

	// 记录调试日志
	m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"任务完成，虫虫回家 。 。 。 ╮(╯ɛ╰)╭");
}

// 启动抓取任务
// 成功返回true，失败返回false
bool WebCrawler::startJob (void) {
	// 加锁互斥锁
	// 此处添加代码

	// 若当前抓取任务数到限
	// 此处添加代码
		// 等待条件变量
		// 此处添加代码

	// 解锁互斥锁
	// 此处添加代码

	// 用从解析统一资源定位符队列弹出的解析统一资源定位符，创建套接字对象
	// 此处添加代码

	// 若发送超文本传输协议请求失败
	// 此处添加代码
		// 销毁套接字对象
		// 此处添加代码
		// 返回失败
		// 此处添加代码

	// 加锁互斥锁
	// 此处添加代码

	// 关注边沿触发的输入事件，并将套接字对象指针存入事件结
	// 构中。一旦发自服务器的超文本传输协议响应到达，即套接
	// 字接收缓冲区由空变为非空，MultiIo::wait便会立即发现，
	// 并在独立的接收线程中通过此套接字对象接收响应数据
	// 此处添加代码
	// 增加需要被关注的文件描述符，若失败
	// 此处添加代码
		// 销毁套接字对象
		// 此处添加代码
		// 解锁互斥锁
		// 此处添加代码
		// 返回失败
		// 此处添加代码

	// 记录调试日志
	// 此处添加代码

	// 加入套接字列表
	// 此处添加代码
	// 当前抓取任务数加一
	// 此处添加代码

	// 解锁互斥锁
	// 此处添加代码
	// 返回成功
	// 此处添加代码
}

// 停止抓取任务
void WebCrawler::stopJob (
	bool success /* = true */ // [输入] 是否成功
	) {
	// 加锁互斥锁
	// 此处添加代码

	// 当前抓取任务数减一，若其由到限变为未到限
	// 此处添加代码
		// 唤醒等待条件变量的线程
		// 此处添加代码

	// 若成功抓取
	// 此处添加代码
		// 成功次数加一
		// 此处添加代码
	// 否则
	// 此处添加代码
		// 失败次数加一
		// 此处添加代码

	// 解锁互斥锁
	// 此处添加代码
}

// 取消抓取任务
void WebCrawler::cancelJob (void) {
	// 当前抓取任务数减一，若其由到限变为未到限
	// 此处添加代码
		// 唤醒等待条件变量的线程
		// 此处添加代码

	// 失败次数加一
	// 此处添加代码
}

// 记录状态
void WebCrawler::logStat (void) {
	// 抓取时间 = 当前时间 - 启动时间
	time_t t = time (NULL) - m_start;

	// 记录信息日志
	m_log.printf (Log::LEVEL_INF, __FILE__, __LINE__,
		"当前任务  原始队列  解析队列  抓取时间  成功次数  失败次数  成功率\n"
		"------------------------------------------------------------------\n"
		"%8d  %8u  %8u  %02d:%02d:%02d  %8u  %8u  %5u%%",
		m_curJobs,
		m_urlQueues.sizeRawUrl (),
		m_urlQueues.sizeDnsUrl (),
		t / 3600, t % 3600 / 60, t % 60,
		m_success,
		m_failure,
		m_success ? m_success * 100 / (m_success + m_failure) : 0);
}

// 使调用进程成为精灵进程
void WebCrawler::initDaemon (void) const {
	// 创建子进程
	pid_t pid = fork ();

	// 若失败
	if (pid == -1)
		// 记录一般错误日志
		m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
			"fork: %s", strerror (errno));

	// 若为父进程
	if (pid)
		// 退出，使子进程成为孤儿进程并被init进程收养
		exit (EXIT_SUCCESS);

	// 子进程创建新会话并成为新会话中唯一进程组的组长
	// 进程，进而与原会话、原进程组和控制终端脱离关系
	setsid ();

	// 打开空设备文件
	int fd = open("/dev/null", O_RDWR, 0);
	// 若成功
	if (fd != -1) {
		// 复制空设备文件描述符到标准输入
		dup2 (fd, STDIN_FILENO);
		// 复制空设备文件描述符到标准输出
		dup2 (fd, STDOUT_FILENO);
		// 复制空设备文件描述符到标准出错
		dup2 (fd, STDERR_FILENO);
		// 若空设备文件描述符大于标准出错
		if (fd > STDERR_FILENO)
			// 关闭空设备文件描述符
			close (fd);
	}

	// 若配置器中的日志文件路径非空且打开(创建)日志文件成功
	if (! m_cfg.m_logFile.empty () &&
		(fd = open (m_cfg.m_logFile.c_str (),
		O_WRONLY | O_APPEND | O_CREAT, 0664)) != -1) {
		// 复制日志文件描述符到标准输出
		dup2 (fd, STDOUT_FILENO);
		// 若日志文件描述符大于标准出错
		if (fd > STDERR_FILENO)
			// 关闭日志文件描述符
			close (fd);
	}
}

// 初始化最大文件描述符数
void WebCrawler::initMaxFiles (void) const {
	// 资源限制结构
	rlimit rl;

	// 若获取当前进程最大文件描述符数失败
	if (getrlimit (RLIMIT_NOFILE, &rl) == -1)
		// 记录一般错误日志
		m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
			"getrlimit: %s", strerror (errno));

	// 若当前进程最大文件描述符数未达极限
	if (rl.rlim_cur < rl.rlim_max) {
		// 将当前进程最大文件描述符数设为极限
		rl.rlim_cur = rl.rlim_max;

		// 若设置当前进程最大文件描述符数失败
		if (setrlimit (RLIMIT_NOFILE, &rl) == -1)
			// 记录一般错误日志
			m_log.printf (Log::LEVEL_ERR, __FILE__, __LINE__,
				"setrlimit: %s", strerror (errno));
	}

	// 记录调试日志
	g_app->m_log.printf (Log::LEVEL_DBG, __FILE__, __LINE__,
		"本进程最多能同时打开%lu个文件描述符", rl.rlim_cur);
}

// 将种子统一资源定位符压入原始统一资源定位符队列
void WebCrawler::initSeedUrls (void) {
	// 若配置器中的种子统一资源定位符为空
	// 此处添加代码
		// 记录一般错误日志
		// 此处添加代码

	// 拆分种子统一资源定位符字符串，以逗号为分隔符，不限拆分次数
	// 此处添加代码

	// 依次处理每个种子统一资源定位符
	// 此处添加代码
		// 若规格化失败
		// 此处添加代码
			// 记录一般错误日志
			// 此处添加代码

		// 若原始统一资源定位符队列满
		// 此处添加代码
			// 记录一般错误日志
			// 此处添加代码

		// 压入原始统一资源定位符队列
		// 此处添加代码
}

// 启动域名解析线程
void WebCrawler::initDns (void) {
	// 通过域名解析线程对象启动域名解析线程
	// 此处添加代码
}

// 启动发送线程
void WebCrawler::initSend (void) {
	// 通过发送线程对象启动发送线程
	// 此处添加代码
}

// 启动状态线程
void WebCrawler::initStat (void) {
	// 通过状态线程对象启动状态线程
	// 此处添加代码
}
