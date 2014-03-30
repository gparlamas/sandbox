#include "executor.h"
#include <exception>
#include <iostream>

#define TERMINATE_ON_EXCEPTION(f) \
    try{    \
        f   \
    }catch(std::exception& ex) {std::cout << ex.what() << std::endl; std::terminate(); }


namespace util
{
    executor::~executor(){}


    loop_executor::loop_executor(){}
    
    loop_executor::~loop_executor(){}

    void loop_executor::add(std::function<void()> closure)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _tasks.emplace_back(closure);
        _cvar.notify_one();
    }

    size_t loop_executor::num_pending_closures()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _tasks.size();
    }

    void loop_executor::make_loop_exit()
    {
        _exit_flag.store(true);
    }

    void loop_executor::loop()
    {
        _exit_flag.store(false);

        while(!_exit_flag.load(std::memory_order_relaxed))
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cvar.wait(lock, [this] { return !_tasks.empty(); } );
            auto begin   =   _tasks.begin();
            auto current =   _tasks.begin();
            auto end     =   _tasks.end(); 
            lock.unlock();

            for(; current != end; ++current)
            {
                TERMINATE_ON_EXCEPTION((*current)();)
                if(_exit_flag.load(std::memory_order_relaxed))
                    break;
            }

            lock.lock();
            _tasks.erase(begin, ++current);
        }
    }
            
    void loop_executor::run_queued_closures()
    {
        _exit_flag.store(false);
        
        std::unique_lock<std::mutex> lock(_mutex);
        if(_tasks.empty()) return;

        auto begin   =   _tasks.begin();
        auto current =   begin;
        auto end     =   _tasks.end();
        lock.unlock();
        
        for(; current != end; ++current)
        {
            (*current)();
            if(_exit_flag.load(std::memory_order_relaxed))
                    break;
        }

         lock.lock();
        _tasks.erase(begin, ++current);
    }
    
    bool loop_executor::try_run_one_closure()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if(_tasks.empty()) return false;
        
        std::function<void() > closure = _tasks.front(); 
        _tasks.pop_front();
        lock.unlock();
        closure();
        return true;
    }
            
    thread_pool_executor::thread_pool_executor(int num_threads)
        :
            _threads(num_threads)
    {
        for(int i=0;i<num_threads;++i)
        {
                _threads[i]=std::thread([this] { execute_tasks(); } );
        }
    }
    
    thread_pool_executor::~thread_pool_executor()
    {
        terminate();

        for(unsigned int i=0;i<_threads.size();++i)
        {
                _threads[i].join();
        }

    }

    void thread_pool_executor::terminate()
    {
        add([this]{ _exit_flag.store(true);});
        for(unsigned int i=1;i<_threads.size();++i)
            add([this] { });//pass an emtpy closure so all threads will pick a job and exit..
    }

    void thread_pool_executor::add(std::function<void()> closure)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _tasks.emplace_back(closure);
        _cvar.notify_one();
    }
    
    size_t thread_pool_executor::num_pending_closures()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _tasks.size();
    }

    void thread_pool_executor::execute_tasks()
    {
        while(!_exit_flag.load(std::memory_order_relaxed))
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cvar.wait(lock, [this] { return !_tasks.empty(); } );
    
            std::function<void()> closure = _tasks.front();
            _tasks.pop_front();
            lock.unlock();

            closure();
        }
    }
            
    inline_executor::inline_executor(){}
    
    inline_executor::~inline_executor(){}
            
    
    void inline_executor::add(std::function<void()> closure)
    {
        closure();
    }
    
    size_t inline_executor::num_pending_closures()
    {
        return 0;
    }

}

