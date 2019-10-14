/*
 * thread.cpp
 *
 *  Created on: 29.11.2018
 *      Author: Clemens
 */
#include "thread.hpp"
#include "trace.h"





namespace std_ex
{
	void sleep_for(std::chrono::milliseconds milliseconds)
	{
		 vTaskDelay(milliseconds.count() / portTICK_PERIOD_MS);
	}


	thread::~thread()
	{
		this->join();
	}

	void thread::join()
	{
		while(this->_M_bo_thread_terminated == false)
		{
			sleep_for(std::chrono::milliseconds(10));
		}
	}

    thread::_State::~_State() {}

    void thread::_M_start_thread(_State_ptr ptr)
    {
    	static int i = 0;

    	// save a reference to the owning thread object insdie the functor
    	ptr->_M_owning_thread = this;
    	m_task_name = "TASK" + std_ex::to_string(i++);
    	m_u_task_priority = 1u;
    	m_u_stack_size = 0x2000u;

    	_M_bo_thread_terminated = false;

    	void (*fp)(void*) = [](void* o){ static_cast<_State*>(o)->_M_run(); };

    	if (pdPASS !=xTaskCreate(
    		  fp,                       /* Task function. */
			  m_task_name.c_str(),   	/* name of task. */
			  m_u_stack_size,           /* Stack size of task */
    	      ptr.get(),                /* parameter of the task */
			  m_u_task_priority,        /* priority of the task */
    	      &m_task_handle)          /* Task handle to keep track of created task */
    			)
    	{
    		// task creation has failed!
    		this->_M_bo_thread_terminated = true;
    	}
    }
}



Thread::Thread(const std::string &name)
: name(name), terminate(false), task_handle(nullptr)
{}

Thread::~Thread()
{
	if (this->task_handle != nullptr)
	{
		this->join();
	}
}

int Thread::start()
{
	DEBUG_PRINTF("Task " + this->name + " started!");

	/* TODO create a table to define the task priorities and sizes */
	this->start_task(4096, 1, [](void* o){ static_cast<Thread*>(o)->run(); });
}

int Thread::start_task(size_t stack_size, size_t priority, void (*fp)(void*))
{
	TaskHandle_t task_handle;
	  xTaskCreate(
		  fp,           			/* Task function. */
		  name.c_str(),        				/* name of task. */
		  stack_size,               /* Stack size of task */
	      this,                     /* parameter of the task */
		  priority,                /* priority of the task */
	      &task_handle);           /* Task handle to keep track of created task */
	return 0;
}


int Thread::stop()
{
	DEBUG_PRINTF("Sending thread " + this->name + " to stop!");
	this->terminate = true;
	return 0;
}


int Thread::join()
{
	if (this->terminate == false)
	{
		this->stop();
	}
	while (this->task_handle != nullptr)
	{
		 vTaskDelay(100 / portTICK_PERIOD_MS);
		//delay(100);
	}
	DEBUG_PRINTF("Thread " + this->name + " stopped!");
	return 0;
}




