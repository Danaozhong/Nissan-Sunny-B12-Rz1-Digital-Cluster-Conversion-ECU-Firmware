#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

#include <functional>
#include <vector>
#include <memory>
#include <mutex>

#include "trace.h"

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

#ifdef HAS_STD_MUTEX
            mutable std::mutex signal_handler_mutex;
#endif


            //std::vector<U*> signal_handlers;
        public:
            //void connect(void (*handler)(T))
            signal()
                : signal_handlers(new std::vector<signal_handler_function>())
            {}

            void connect(const signal_handler_function &handler)
            {
#ifdef HAS_STD_MUTEX
            	std::lock_guard<std::mutex> lock(this->signal_handler_mutex);
#endif
                DEBUG_PRINTF("connect called! size %u", signal_handlers->size());
                signal_handlers->push_back(handler);
            }

            void operator()(const T &arg) const
            {
#ifdef HAS_STD_MUTEX
            	std::lock_guard<std::mutex> lock(this->signal_handler_mutex);
#endif
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
