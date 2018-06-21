#ifndef THREADPOOL_H
#define THREADPOOL_H

#include<Base.h>
#include<Mutex.h>
#include<Condition.h>
#include<pthread.h>
#include<deque>
#include<functional>
#include<string>
#include<vector>
#include<limits>
#include<algorithm>
#include<iostream>
#include<memory>
#include<Thread.h>
#include<assert.h>


class ThreadPool : Noncopyable
{
public:
    typedef std::function<void()> Task;

    explicit ThreadPool(size_t poolSize, size_t queueSize = INT32_MAX);
    ~ThreadPool();

    void setMaxQueueSize(int maxSize)
    {
        _maxQueueSize = maxSize;
    }


    size_t queueSize()
    {
        return _maxQueueSize;
    }

    //将excute模板化，使得任务构造能够以多种形式完成
    template<typename T>
    void execute(T& ttask);
    void shutdown();

    bool isShutdown()
    {
        return !_running;
    }



private:
    bool isFull() const;
    void runInThread();
    Task take();

    mutable Mutex _mutex;
    Condition _notEmpty;
    Condition _notFull;
    size_t _maxQueueSize;
    size_t _poolSize;
    std::vector<std::unique_ptr<Thread> > _threads;
    std::deque<Task> _queue;
    bool _running;



};



//模板函数必须放在头文件页
template<typename T>
void ThreadPool::execute(T& ttask)
{
    Task task = static_cast<Task>(ttask);
    if (_threads.empty())
        task();
    else
    {
        MutexLock lock(_mutex);
        while(isFull())
            _notFull.wait();
        assert(!isFull());
        _queue.push_back(task);
        _notEmpty.notify();
    }
}




#endif // THREADPOOL_H

