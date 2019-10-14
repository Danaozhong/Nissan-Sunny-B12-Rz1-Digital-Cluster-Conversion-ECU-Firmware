#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_


//typedef struct timespec __gthread_time_t;

//#include <FreeRTOS_POSIX/pthread.h>
//#define _GLIBCXX_HAS_GTHREADS
//#include <bits/gthr-posix.h>
// tried imitating boost::signals2
//#include "stdlib_preparations.h"
//#include <bits/gthr-posix.h>
#include <functional>
#include <vector>
#include <memory>


#include <mutex>

#include <mutex>

#include "midware/trace/trace.h"
namespace boost
{
	namespace signals2
	{
		template<typename T>
        class signal
        {
        private:
            typedef std::function< void( T ) > signal_handler_function;

            std::shared_ptr<std::vector<signal_handler_function>> signal_handlers;

            mutable std::mutex signal_handler_mutex;



            //std::vector<U*> signal_handlers;
        public:
            //void connect(void (*handler)(T))
            signal()
                : signal_handlers(new std::vector<signal_handler_function>())
            {}

            void connect(const signal_handler_function &handler)
            {
            	std::lock_guard<std::mutex> lock(this->signal_handler_mutex);
                DEBUG_PRINTF("connect called! size " + std_ex::to_string(signal_handlers->size()));
                signal_handlers->push_back(handler);
            }

            void operator()(const T &arg) const
            {
            	std::lock_guard<std::mutex> lock(this->signal_handler_mutex);
                //DEBUG_PRINTF("Operator() called! size " + helper::to_string(signal_handlers->size()));
                for (auto itr = signal_handlers->begin(); itr != signal_handlers->end(); ++itr)
                {
                    //DEBUG_PRINTF("Calling a function!");
                    (*itr)(arg);
                }
            }

            /*
            template<typename U>
            void connect(void (*handler)(U, T))
            {


            }*/

        };
	}
}

#endif /* _EVENT_HANDLER_H_ */
