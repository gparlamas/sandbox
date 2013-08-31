#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <deque>

namespace util
{
    class executor
    {
        public:
            virtual ~executor();
            virtual void add(std::function<void()> closure) = 0;
            virtual size_t num_pending_closures() = 0;
    };

    class loop_executor : public executor
    {
        public:
            loop_executor();
            ~loop_executor();
            
            void loop();

            void run_queued_closures();

            bool try_run_one_closure();

            void make_loop_exit();

            virtual void add(std::function<void()> closure);
            virtual size_t num_pending_closures();

        private:
            std::deque<std::function<void()> >  _tasks;
            std::mutex                          _mutex;
            std::condition_variable             _cvar;
            std::atomic_bool                    _exit_flag;
    };
}

