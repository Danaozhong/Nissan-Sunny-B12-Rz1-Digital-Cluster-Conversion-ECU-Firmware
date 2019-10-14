#ifndef _CYCLIC_THREAD_H_
#define _CYCLIC_THREAD_H_

#include <atomic>
#include <chrono>
#include <string>
#include "thread.hpp"
#include <memory>
#include <vector>



namespace std_ex
{

}

/** Cyclic threads are called every x ms */
class CyclicThread : public Thread
{
public:
	CyclicThread(const std::string &name, const std::chrono::milliseconds &interval);
	virtual int start();

protected:
	/** Function called when the thread is created */
	virtual void initial();
	virtual void shutdown();

	/** Function to set the cycle time of the thread. */
	void set_interval(std::chrono::milliseconds c_interval);

private:
	void main();
	std::chrono::milliseconds interval;
};

#endif /* _CYCLIC_THREAD_H_ */
