/*
 * thread_repository.cpp
 *
 *  Created on: 29.11.2018
 *      Author: Clemens
 */


#include "thread_repository.hpp"
#include "trace.h"

void ThreadRepository::start_all_threads()
{
	for (auto itr = this->m_threads.begin(); itr != this->m_threads.end(); ++itr)
	{
		(*itr)->start();
	}
}

void ThreadRepository::join_all_threads()
{
	DEBUG_PRINTF("Thread repository will now join all threads!");
	/* Request all threads to terminate*/
	for (auto itr = this->m_threads.begin(); itr != this->m_threads.end(); ++itr)
	{
		(*itr)->stop();
	}

	/* Wait for all threads to have finished */
	for (auto itr = this->m_threads.begin(); itr != this->m_threads.end(); ++itr)
	{
		(*itr)->join();
	}

}

void ThreadRepository::add_thread(std::shared_ptr<Thread> p_thread)
{
	this->m_threads.push_back(p_thread);
}
