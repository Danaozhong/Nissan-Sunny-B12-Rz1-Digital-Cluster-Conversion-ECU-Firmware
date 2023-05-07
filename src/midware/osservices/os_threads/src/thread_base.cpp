#include "thread_base.hpp"
#include "trace_if.h"

#include "include/FreeRTOS.h"
#include "include/task.h"

#include "ex_thread.hpp"

#include <cstring>
#include <algorithm>

namespace os_services
{
    CyclicThread::CyclicThread(const char* name, size_t priority, size_t stack_size, std::chrono::milliseconds cycle_time)
        : m_priority(priority), m_stack_size(stack_size), m_is_running(false), m_terminate(false), m_cycle_time(cycle_time), m_thread(nullptr)
    {
        std::strncpy(m_name, name, OS_SERVICES_THREAD_NAME_STR_LENGTH);
        m_name[OS_SERVICES_THREAD_NAME_STR_LENGTH - 1] = '\0';
        
        TRACE_DECLARE_CONTEXT("SCHD");
    }
    
    CyclicThread::~CyclicThread()
    {
        join();
    }
    
    void CyclicThread::start()
    {
        if (m_is_running)
        {
            TRACE_LOG("SCHD", LOGLEVEL_ERROR, "Thread %s is already running, can't be started again.", m_name);
            return;
        }
        
        m_is_running = true;
        m_terminate = false;
        
        if (pdPASS !=xTaskCreate(
              &CyclicThread::callable,  /* Task function. */
              m_name,                   /* name of task. */
              m_stack_size,           /* Stack size of task */
              this,                     /* parameter of the task */
              m_priority,        /* priority of the task */
             (reinterpret_cast<tskTaskControlBlock**>(&m_thread)))           /* Task handle to keep track of created task */
            )
        {
            // task creation has failed!
            TRACE_LOG("SCHD", LOGLEVEL_FATAL, "Thread %s creation failed!", m_name);
            this->m_is_running = false;
        }
    }
        
    void CyclicThread::stop()
    {
        m_terminate = true;
    }
    
    void CyclicThread::join()
    {
        stop();
        
        while (m_terminate != m_is_running)
        {
            std_ex::sleep_for(std::chrono::milliseconds(10));
        }
        TRACE_LOG("SCHD", LOGLEVEL_DEBUG, "Thread %s stopped", m_name);
    }
    
    void CyclicThread::thread_main()
    {
        using namespace std::chrono;
        
        while(!m_terminate)
        {
            auto start_time = system_clock::now();
            
            process();
            
            // calculate the execution time
            const auto delta = duration_cast<std::chrono::milliseconds>(system_clock::now() - start_time);
            auto sleep_delta = std::max(milliseconds(0), duration_cast<milliseconds>(m_cycle_time - delta));
            const auto actual_cycle_time = duration_cast<std::chrono::milliseconds>(start_time - m_ts_last_activation);

            m_ts_last_activation = start_time;
            TRACE_LOG("SCHD", LOGLEVEL_DEBUG, "%s sched. cycle duration %u, sleep time %u, last activation %u\n\r",
                    m_name,
                    static_cast<unsigned int>(delta.count()),
                    static_cast<unsigned int>(sleep_delta.count()),
                    static_cast<unsigned int>(actual_cycle_time.count()));

            std_ex::sleep_for(sleep_delta);
        }
        
        m_is_running = false;
    }
    
    void CyclicThread::callable(void* arg)
    {
        if (nullptr != arg)
        {
            static_cast<CyclicThread*>(arg)->thread_main();
        }
        // return inside a task main is not possible in FreeRTOS. The task
        // explicitly eeeds to delete itself from FreeRTOS
        // before being able to return.
        vTaskDelete(nullptr);
    }
}