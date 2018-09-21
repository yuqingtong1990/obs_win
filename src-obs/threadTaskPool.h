#pragma once

/*
*fuc:C++ 11 �����̳߳ط�װ��
*date:2017.09.14
*author:yuqingtong
*email:yuqingtong1990@gmail.com
*/

#include <thread>
#include <mutex>

#include <list>
#include <atomic>
#include <deque>
#include <stack>
#include <queue>
#include <iostream>
#include <condition_variable>

typedef void* TASKARG;

class ITask
{
public:
    enum Priority
    {
        Idle = 0,       //����
        Normal = 5,     //��ͨ
        High = 10,       //��
        Realtime = 50,   //ʵʱ
    };
    //����Ԥ���
    friend bool operator<(ITask  &lv, ITask &rv)
    {
        return lv.priority_ < rv.priority_;
    }
    ITask();
    virtual ~ITask();
    void setArg(TASKARG arg);
    void setName(const std::string& strName);
    virtual int run() = 0;
    void SetPriority(int priority);
    int GetPriority() const;
    void SetAutoRelease(bool bAutoRelease);
    bool IsAutoRelease();
    virtual void release(){ delete this; }
    
protected:
    int   priority_ = Normal;        //�������ȼ�
    TASKARG     arg_;                //�������
    std::string taskName_;           //��������
    bool bAutoRelease_ = true;       //�Ƿ��Զ�ɾ��*
};

struct PCmp
{
    bool operator () (ITask const *x, ITask const *y)
    {
        return x->GetPriority() < y->GetPriority();
    }
};

//��������
class TaskContainer
{
public:
    TaskContainer(){};
    ~TaskContainer(){};
    void push(ITask *);
    ITask* top();
    void pop();
    bool empty();
    std::priority_queue<ITask*, std::vector<ITask*>, PCmp >::size_type size();
private:
    std::priority_queue<ITask*, std::vector<ITask*>, PCmp > task_container_;
};

class ThreadTaskPool;

class CThreadTask
{
public:
    ~CThreadTask();
public:
    CThreadTask(ThreadTaskPool *pool);
    void Assign(ITask *task);
    void Run();
    void Start();
    void setDetach(bool isdetach);
private:
    static int  s_threadnumber;
    ThreadTaskPool *threadpool_;
    int threadid_ = 0;
    bool isdetach_;
    ITask *task_;
    std::thread thread_;
};



class ThreadTaskPool
{
public:
    explicit ThreadTaskPool(int threadNum = 0, int wait_seconds = 1);
    ~ThreadTaskPool();
public:
    void start();
    void run();
    void addTask(ITask* task);
    size_t taskNum();
    size_t idlThreadNum();
    void stop();
    void release();
    void SetThreadidl(CThreadTask *myThread);
private:
    ThreadTaskPool& operator=(const ThreadTaskPool&);
    ThreadTaskPool(const ThreadTaskPool&);
private:
    std::atomic<bool>           stop_ = false;  // �Ƿ�ر��ύ
    std::mutex                  task_mutex_;    // ���������
    std::mutex                  thread_mutex_;  // �̶߳�����
    std::condition_variable     cond_;          // ��������
    TaskContainer               tasks_;         // �������
    std::thread                 thread_;        // �����߳�
    std::stack<CThreadTask*>    idel_threads_;  // �����̶߳���

    typedef std::chrono::duration<int> seconds_type;
    seconds_type seconds_wait;
};

