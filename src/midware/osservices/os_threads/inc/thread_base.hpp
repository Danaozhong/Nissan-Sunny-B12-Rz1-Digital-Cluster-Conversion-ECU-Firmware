#ifndef _THREAD_BASE_HPP_
#define _THREAD_BASE_HPP_

#include <chrono>

#define OS_SERVICES_THREAD_NAME_STR_LENGTH  (15u)

namespace os_services
{
    class CyclicThread
    {
    public:
        CyclicThread(const char* name, size_t priority, size_t stack_size, std::chrono::milliseconds cycle_time_in_ms);
        
        /** Destructor */
        virtual ~CyclicThread();
        
    private:
        char m_name[OS_SERVICES_THREAD_NAME_STR_LENGTH];
        
        const size_t m_priority;
        
        const size_t m_stack_size;
        
        bool m_is_running;
        
        bool m_terminate;
        
        const std::chrono::milliseconds m_cycle_time;
        
        void* m_thread;
        
        // timestamp when this task was last activated.
        std::chrono::time_point<std::chrono::system_clock> m_ts_last_activation;
public:
        /** starts the thread */
        void start();
        
        /** Sends the thread a signal to stop */
        void stop();
        
        /** stops the thread, and waits for it to terminate */
        void join();
        
        void thread_main();
    protected:
        virtual void process() = 0;
    private:
        static void callable(void* arg);
    };
}

#endif