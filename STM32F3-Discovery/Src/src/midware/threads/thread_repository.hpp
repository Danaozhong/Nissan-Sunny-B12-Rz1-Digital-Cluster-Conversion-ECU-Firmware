/*
 * thread_repository.hpp
 *
 *  Created on: 29.11.2018
 *      Author: Clemens
 */

#ifndef USER_MIDWARE_THREADS_THREAD_REPOSITORY_HPP_
#define USER_MIDWARE_THREADS_THREAD_REPOSITORY_HPP_

#include <vector>
#include <memory>
#include "thread.hpp"


/** A class to store a set of threads */
class ThreadRepository
{
public:
	void start_all_threads();

	void join_all_threads();

	void add_thread(std::shared_ptr<Thread> thread);
private:

	std::vector<std::shared_ptr<Thread>> m_threads;
};



#endif /* USER_MIDWARE_THREADS_THREAD_REPOSITORY_HPP_ */
