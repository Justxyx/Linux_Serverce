//
// Created by xm on 2022/5/19.
//

#ifndef LINUX_TEST04_LOCKER_H
#define LINUX_TEST04_LOCKER_H
#include <exception>
#include <pthread.h>
#include <semaphore.h>  // 信号量包


// 封装信号量
class sem{
public:
    sem(){
        if (sem_init(&m_sem,0,0) != 0){
            throw std::exception();
        }
    }
    ~sem(){
        sem_destroy(&m_sem);
    }
    bool wait(){
        return sem_wait(&m_sem) == 0;
    }
    bool post(){
        return sem_post(&m_sem) == 0;
    }

private:
    sem_t m_sem;
};

//分装互斥锁类
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
    bool lock(){
        return pthread_mutex_lock(&m_mutex);
    }
    bool unlock(){
        return pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};

// 条件变量
class cond{
public:
    cond(){
        if (pthread_mutex_init(&m_mutex,NULL) != 0){
            throw std::exception();
        }
        if (pthread_cond_init(&m_cond,NULL) != 0){
            pthread_mutex_destroy(&m_mutex);
        }
    }
    ~cond(){
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }
    bool wait(){
        int ret = 0;
        pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond,&m_mutex);
        pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    bool signal(){
        return pthread_cond_signal(&m_cond);
    }
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};



#endif //LINUX_TEST04_LOCKER_H
