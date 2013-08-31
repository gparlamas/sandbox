#include "executor.h"

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
        while(!_exit_flag.load(std::memory_order_relaxed))
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cvar.wait(lock, [this] { return _tasks.empty(); } );
            auto begin   =   _tasks.begin();
            auto current =   begin;
            auto end     =   _tasks.end(); 
            lock.unlock();

            for(; current != end; ++current)
            {
                *current;
                if(_exit_flag.load(std::memory_order_relaxed))
                    break;
            }

            --current;
            lock.lock();
            _tasks.erase(begin, current);
            
        }
    }
            
    void loop_executor::run_queued_closures()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if(_tasks.empty()) return;

        auto begin   =   _tasks.begin();
        auto current =   begin;
        auto end     =   _tasks.end();
        lock.unlock();
        
        for(; current != end; ++current)
        {
            *current;
            if(_exit_flag.load(std::memory_order_relaxed))
                    break;
        }

        --current;
         lock.lock();
        _tasks.erase(begin, current);

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
}
