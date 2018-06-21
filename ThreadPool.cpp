#include<ThreadPool.h>


ThreadPool::ThreadPool(size_t poolSize, size_t queueSize)
    : _mutex(),
      _notEmpty(_mutex),
      _notFull(_mutex),
      _maxQueueSize(queueSize),
      _poolSize(poolSize),
      _running(true)
{
    for (size_t i = 0; i < _poolSize; ++ i)
    {
        _threads.push_back(std::unique_ptr<Thread>
                           (new Thread(std::bind(&ThreadPool::runInThread, this))));
        _threads[i]->start();
    }

}

ThreadPool::~ThreadPool()
{
    if (_running)
    {
        shutdown();
    }
}



ThreadPool::Task ThreadPool::take()
{
    MutexLock lock(_mutex);
    while (_queue.empty() && _running)
        _notEmpty.wait();
    Task task;
    if (!_queue.empty())
    {
        task = _queue.front();
        _queue.pop_front();
        if (_maxQueueSize > 0)
            _notFull.notify();
    }
    return task;
}

void ThreadPool::shutdown()
{
    {
        MutexLock lock(_mutex);
        _running = false;
        _notEmpty.notifyAll();
    }
    for (size_t i = 0; i < _threads.size(); ++ i)
        _threads[i]->join();

}

bool ThreadPool::isFull() const
{
    return _maxQueueSize > 0 && _queue.size() >= _maxQueueSize;
}

void ThreadPool::runInThread()
{
    try
    {
        while (_running)
        {
            Task task = take();
            if (task)
                task();

        }

    }
    catch(...)
    {
        std::cerr << "Unknow Fault";
        abort();
    }
}
