


/* System header */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Foreign header files */
#include "midware/trace/trace.h"

#include <functional>   // std::bind

/* Own header files */
#include "cyclic_thread.hpp"



int CyclicThread::start()
{
	DEBUG_PRINTF("Starting cyclic Thread " + this->name + "...");
	this->start_task(4096, 1, [](void* o){ static_cast<CyclicThread*>(o)->main(); });

}

void CyclicThread::initial()
{
	/* Function is virtual, do not do anything if not overriden */
}

void CyclicThread::shutdown()
{
	/* Function is virtual, do not do anything if not overriden */
}


void CyclicThread::set_interval(std::chrono::milliseconds c_interval)
{
	this->interval = c_interval;
}

CyclicThread::CyclicThread(const std::string &name, const std::chrono::milliseconds &interval)
:Thread(name), interval(interval)
{}

void CyclicThread::main()
{
	DEBUG_PRINTF("Cyclic Thread " + this->name + " started!");
	this->initial();

	while (this->terminate == false)
	{
		/* Call the cyclic function... */
		this->run();

		/* and suspend the task again. */
		std_ex::sleep_for(std::chrono::milliseconds(this->interval));
	}

	this->shutdown();
	DEBUG_PRINTF("Cyclic Thread " + this->name + " terminated!");

	this->terminate = false;
	this->task_handle = nullptr;
	vTaskDelete( nullptr );

}


