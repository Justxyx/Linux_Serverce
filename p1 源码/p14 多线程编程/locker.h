//
// Created by xm on 2022/4/20.
//

#ifndef LINUX_LOCKER_H
#define LINUX_LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>  // 信号量包

/*
 * 封装信号量类
 */
class sem{
public:
    // 初始化信号量
    sem(){
        if (sem_init(&m_sem,0,0) != 0){
            throw std::exception();
        }
    }
    // 销毁信号量
    ~sem(){
        sem_destroy(&m_sem);
    }
    // 等待信号量
    bool wait(){
        //如果m_sem为0  阻塞等待
        return sem_wait(&m_sem) == 0;
    }
    // 新增信号量
    bool  post(){
        return sem_post(&m_sem) == 0;
    }
private:
    sem_t m_sem;
};



// 封装的互斥锁类
class locker{
public:
    locker(){
        if (pthread_mutex_init(&m_mutex,NULL) != 0){
            throw std::exception();
        }
    }
    ~locker(){
        pthread_mutex_destroy(&m_mutex);
    }
    // 获得互斥锁
    bool lock(){
        return pthread_mutex_lock(&m_mutex);
    }
    // 释放互斥锁
    bool unlock(){
        return pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};

// 封装条件变量类
class cond{
public:
    cond(){
        if (pthread_mutex_init(&m_mutex,NULL) != 0)
            throw std::exception();
        if (pthread_cond_init(&m_cond,NULL) != 0){
            pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
    }
    ~cond(){
        pthread_cond_destroy(&m_cond);
        pthread_mutex_destroy(&m_mutex);
    }

    // 等待条件变量
    bool wait(){
        int ret = 0;
        pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond,&m_mutex);
        pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    // 唤醒等待条件变量的线程
    bool signal(){
        return pthread_cond_signal(&m_cond);
    }
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};


#endif //LINUX_LOCKER_H
