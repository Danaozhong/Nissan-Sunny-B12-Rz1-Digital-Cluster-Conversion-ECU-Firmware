/*
 * thread.hpp
 *
 *  Created on: 29.11.2018
 *      Author: Clemens
 */

#ifndef USER_MIDWARE_THREADS_THREAD_HPP_
#define USER_MIDWARE_THREADS_THREAD_HPP_

#include <atomic>
#include <thread>
#include <type_traits>

#include "include/FreeRTOS.h"
#include "include/task.h"
#include <functional>


//#define STD_EX_USE_SIMPLIFIED_THREAD

namespace std_ex
{
    void sleep_for(std::chrono::milliseconds milliceconds);

    uint32_t get_timestamp_in_ms();


#ifdef STD_EX_USE_SIMPLIFIED_THREAD
    class thread
    {
    public:
        /** Base class of a functor */
        struct _State
        {
            virtual ~_State();
            virtual void _M_run() = 0;
            thread* _M_owning_thread;
        };


        /** A simplified function wich has the signature void(f)(void)
         */
        struct simple_functor : public _State
        {
            /** Constructor */
            simple_functor(void (m_p_main_function)(void));
            virtual ~simple_functor();

            void operator()();

            virtual void _M_run();

            // The wrapped functor
            void (*m_p_main_function)(void);

        };

        thread(simple_functor __f);

        ~thread();
        void detach();
        void join();
        using _State_ptr = _State*;

        _State_ptr m_p_functor;

    private:
        bool m_bo_thread_detached;
        bool m_bo_thread_terminated;

        TaskHandle_t m_task_handle;
        char m_task_name[30];
        size_t m_u_task_priority;
        size_t m_u_stack_size;

        /** Helper function to start the thread */
        void _M_start_thread(_State_ptr);
    };

#else
    class thread
    {
    public:
        template<typename _Tuple>
            struct _Invoker
            {
                _Tuple _M_t;

                template<size_t _Index>
                static std::__tuple_element_t<_Index, _Tuple>&& _S_declval();

                template<size_t... _Ind>
                auto _M_invoke(std::_Index_tuple<_Ind...>)
                    noexcept(noexcept(std::__invoke(_S_declval<_Ind>()...)))
                    -> decltype(std::__invoke(_S_declval<_Ind>()...))
                {
                    return std::__invoke(std::get<_Ind>(std::move(_M_t))...);
                }

                using _Indices
                  = typename std::_Build_index_tuple<std::tuple_size<_Tuple>::value>::__type;

                auto operator()()
                    noexcept(noexcept(std::declval<_Invoker&>()._M_invoke(_Indices())))
                    -> decltype(std::declval<_Invoker&>()._M_invoke(_Indices()))
                {
                    return _M_invoke(_Indices());
                }
            };

            template<typename... _Tp>
              using __decayed_tuple = std::tuple<typename std::decay<_Tp>::type...>;

        public:
            // Returns a call wrapper that stores
            // tuple{DECAY_COPY(__callable), DECAY_COPY(__args)...}.
            template<typename _Callable, typename... _Args>
            static _Invoker<__decayed_tuple<_Callable, _Args...>>
                __make_invoker(_Callable&& __callable, _Args&&... __args)
            {
                return
                {
                    __decayed_tuple<_Callable, _Args...>
                    {
                        std::forward<_Callable>(__callable), std::forward<_Args>(__args)...
                    }
                };
            }

        // Abstract base class for types that wrap arbitrary functors to be
        // invoked in the new thread of execution.
        struct _State
        {
            virtual ~_State();
            virtual void _M_run() = 0;

            // the class which spawned this thread
            thread* _M_owning_thread;
        };

#ifdef REDUCE_CODE_FLASH_SIZE
        // shared ptrs take up a lot of code flash space, in case you have no flash at all, switch
        // to raw pointers instead.

        using _State_ptr = _State*;
#else
        using _State_ptr = std::unique_ptr<_State>;
#endif


        template<typename _Callable, typename... _Args>
          explicit
          thread(_Callable&& __f, const char ai8_task_name[],
                  uint32_t u32_priority, uint32_t u32_stack_size, _Args&&... __args)
            : m_task_handle(nullptr)
          {
            _State_ptr state_ptr = _S_make_state(
                    __make_invoker(std::forward<_Callable>(__f),
                           std::forward<_Args>(__args)...));

            // and start it.
            _M_start_thread(std::move(state_ptr), ai8_task_name, u32_priority, u32_stack_size);
          }

        ~thread();

        void detach();
        void join();

    private:
        bool m_bo_thread_detached;

        template<typename _Callable>
        struct _State_impl : public _State
        {
            // The wrapped functor
            _Callable        _M_func;

            /** Constructor. Just take the callable and store it */
            _State_impl(_Callable&& __f)
                : _M_func(std::forward<_Callable>(__f))
            {}

            void _M_run()
            {
                // Call the tasks main itself
                _M_func();

                // Indicate this thread as terminated
                if (nullptr != _M_owning_thread)
                {
                    this->_M_owning_thread->m_bo_thread_terminated = true;
                }

                // return inside a ask is not possible in FreeRTOS. Need to delete myself from FreeRTOS
                // before being able to terminate
                vTaskDelete(nullptr);
            }
        };

        TaskHandle_t m_task_handle;
        char m_task_name[30];
        size_t m_u_task_priority;
        size_t m_u_stack_size;

        _State_ptr m_p_state_ptr;

        /** Helper to remember if the thread was already terminated */
        std::atomic<bool> m_bo_thread_terminated;
    private:
        /** Helper function to start the thread */
        void _M_start_thread(_State_ptr, const char ai8_tsk_name[],
                uint32_t u32_priority,
                uint32_t u32_stack_size);


        template<typename _Callable>
        static _State_ptr _S_make_state(_Callable&& __f)
        {
            using _Impl = _State_impl<_Callable>;
            return _State_ptr(new _Impl(std::forward<_Callable>(__f)));
        }

    };
#endif

}
#if 0
namespace OSManager
{

    void list_tasks();


    class ThreadRepository
    {
    public:
        static ThreadRepository& get_instance()
        {
            static ThreadRepository o_thread_repository;
            return o_thread_repository;
        }

        void add_thread(std_ex::thread &thread);
    private:


    };

}
#endif

// UGLY FROM HERE BELOW - use std::thread in the future
/** C++ wrapper for the RTOS tasks */
class Thread
{
public:
    Thread(const std::string &name);

    virtual ~Thread();

    virtual int start();
    int start_task(size_t stack_size, size_t priority, void (*fp)(void*));
    virtual int stop();
    virtual int join();
    virtual void run() = 0;
protected:
    std::string name;
    std::atomic<bool> terminate;
    volatile TaskHandle_t task_handle;
};
#endif /* USER_MIDWARE_THREADS_THREAD_HPP_ */
