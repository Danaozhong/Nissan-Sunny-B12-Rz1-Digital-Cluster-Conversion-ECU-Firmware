/*
 * thread.cpp
 *
 *  Created on: 29.11.2018
 *      Author: Clemens
 */
#include <cstdio>
#include <cstring>

#include "ex_thread.hpp"
#include "trace.h"


namespace std_ex
{
	void sleep_for(std::chrono::milliseconds milliseconds)
	{
		 vTaskDelay(milliseconds.count() / portTICK_PERIOD_MS);
	}

#ifdef STD_EX_USE_SIMPLIFIED_THREAD
	thread::simple_functor::simple_functor(void (m_p_main_function)(void))
		: m_p_main_function(m_p_main_function)
	{


	}

	void thread::simple_functor::_M_run()
	{
		// Call the tasks main itself
		m_p_main_function();

		// Indicate this thread as terminated
		if (nullptr != _M_owning_thread)
		{
			this->_M_owning_thread->m_bo_thread_terminated = true;
		}

		// return inside a ask is not possible in FreeRTOS. Need to delete myself from FreeRTOS
		// before being able to terminate
		vTaskDelete(nullptr);

	}

	void thread::simple_functor::operator()()
	{
		_M_run();
	}



	thread::thread(simple_functor __f)
	{
		m_p_functor = new simple_functor(__f);
		_M_start_thread(m_p_functor);
	}

	thread::~thread()
	{
		if (false == m_bo_thread_detached)
		{
			this->join();
		}
	}


	void thread::detach()
	{
		m_bo_thread_detached = true;
	}

	void thread::join()
	{
		while(this->m_bo_thread_terminated == false)
		{
			sleep_for(std::chrono::milliseconds(10));
		}
	}



#else
	thread::~thread()
	{
		if (false == m_bo_thread_detached)
		{
			this->join();
		}
	}

	void thread::detach()
	{
		m_bo_thread_detached = true;
	}
	void thread::join()
	{
		while(this->m_bo_thread_terminated == false)
		{
			sleep_for(std::chrono::milliseconds(10));
		}
	}

    thread::_State::~_State() {}
#endif

    //void task_main_loop_wrapper(void* o)
    //{
   //	static_cast<thread::_State*>(o)->_M_run();
    //}

    void thread::_M_start_thread(_State_ptr ptr)
    {
    	static uint32_t u32I = 0;

    	// save a reference to the owning thread object insdie the functor
    	ptr->_M_owning_thread = this;
    	memset(m_task_name, 0x0, sizeof(m_task_name));

    	snprintf(m_task_name, sizeof(m_task_name) - 1, "Task%u", u32I++);
    	m_u_task_priority = 2u;
    	m_u_stack_size = 1024;
    	m_bo_thread_detached = false;

        m_bo_thread_terminated = false;

		void (*fp)(void*) = [](void* o)
		{
			if (nullptr != o)
			{
				static_cast<_State*>(o)->_M_run();
			}
			// this should not happen, report an error here

		};

    	if (pdPASS !=xTaskCreate(
    		  fp,                       /* Task function. */
			  m_task_name,   			/* name of task. */
			  m_u_stack_size,           /* Stack size of task */
    	      ptr,                /* parameter of the task */
			  m_u_task_priority,        /* priority of the task */
    	      &m_task_handle)          /* Task handle to keep track of created task */
    			)
    	{
    		// task creation has failed!
    		this->m_bo_thread_terminated = true;
    	}
    }

}
