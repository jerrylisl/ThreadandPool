#include<iostream>
#include<Thread.h>
#include<Mutex.h>
#include<Condition.h>
#include<ThreadPool.h>
#include<functional>

using namespace std;

Mutex mutex;
Condition c(mutex);
int a = 0;

static void func1()
{
    MutexLock lock(mutex);
    while (a != 0)
        c.wait();
    ++ a;
    cout << a;
    c.notify();
}

static void func2()
{
    MutexLock lock(mutex);
    while (a == 0)
        c.wait();
    -- a;
    cout << a;
    c.notify();
}

class E
{
public:
    void operator() ()
    {
        cout << "OO" << endl;
    }
};



int main()
{
    cout << "Thread Part" << endl;
    //交替输出0和1
    for (int i = 0; i < 10; ++ i)
    {
        Thread a(func1);
        Thread b(func2);
        a.start();
        b.start();
        a.join();
        b.join();
    }
    cout << endl << "Pool Part" << endl;
    ThreadPool pool(10);
    E a;
    pool.execute(a);
    pool.execute(func1);
    pool.execute(func2);


    if (!pool.isShutdown())
        pool.shutdown();

    return 0;
}

