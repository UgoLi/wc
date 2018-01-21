/*************************************************************************
	> File Name: thread.cpp
	> Author:UgoLi 
	> Mail: 2653920896@qq.com
	> Created Time: 2017年10月10日 星期二 10时22分20秒
 ************************************************************************/

#include<iostream>
#include<pthread.h>
#include<unistd.h>
#include<stdio.h>
using namespace std;

class Thread{
public:
    virtual ~Thread(void){}
    void start(void){
        pthread_create(&m_tid,NULL,run,this);
    }
    static void* run(void* arg){
        return ((Thread*)arg)->run();
    } 
    virtual void* run(void)=0;
private:
    pthread_t m_tid;
};

class MyThread:public Thread{
public:
    MyThread(char ch,int ms):m_ch(ch),m_ms(ms){}
private:
    void* run(){
        for(;;){
            cout<<m_ch<<flush;
            usleep(m_ms*1000);
        }
    }
    char m_ch;
    int m_ms;
};

int main(void){
    MyThread t1('+',500),t2('-',100);
    t1.start();
    t2.start();
    getchar();

    return 0;
}

