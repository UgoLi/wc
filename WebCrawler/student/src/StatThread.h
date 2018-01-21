/// @file StatThread.h
/// @brief 声明#StatThread类
/// @author 闵卫
/// @date 2015年11月20日
/// @version 1.0.0.1

#ifndef _STATTHREAD_H
#define _STATTHREAD_H

#include "Thread.h"

/// @brief 发送线程
class StatThread : public Thread {
private:
	/// @brief 线程处理函数
	/// @note 根据状态线程的任务实现基类中的纯虚函数
	void* run (void);
};

#endif // _STATTHREAD_H
