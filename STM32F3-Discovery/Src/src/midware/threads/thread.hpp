/*
 * thread.hpp
 *
 *  Created on: 29.11.2018
 *      Author: Clemens
 */

#ifndef USER_MIDWARE_THREADS_THREAD_HPP_
#define USER_MIDWARE_THREADS_THREAD_HPP_

#include <atomic>
#include <string>
#include <thread>
#include <type_traits>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <functional>


namespace std_ex
{
	void sleep_for(std::chrono::milliseconds milliceconds);

	class thread
	{
	public:
		template<typename _Tuple>
		      struct _Invoker
		      {
			_Tuple _M_t;

			template<size_t _Index>
			  static std::__tuple_element_t<_Index, _Tuple>&&
			  _S_declval();

			template<size_t... _Ind>
			  auto
			  _M_invoke(std::_Index_tuple<_Ind...>)
			  noexcept(noexcept(std::__invoke(_S_declval<_Ind>()...)))
			  -> decltype(std::__invoke(_S_declval<_Ind>()...))
			  { return std::__invoke(std::get<_Ind>(std::move(_M_t))...); }

			using _Indices
			  = typename std::_Build_index_tuple<std::tuple_size<_Tuple>::value>::__type;

			auto
			operator()()
			noexcept(noexcept(std::declval<_Invoker&>()._M_invoke(_Indices())))
			-> decltype(std::declval<_Invoker&>()._M_invoke(_Indices()))
			{ return _M_invoke(_Indices()); }
		      };
#if 0
		    template<typename... _Tp>
		      using __decayed_tuple = std::tuple<typename std::decay<_Tp>::type...>;

		  public:
		    // Returns a call wrapper that stores
		    // tuple{DECAY_COPY(__callable), DECAY_COPY(__args)...}.
		    template<typename _Callable, typename... _Args>
		      static _Invoker<__decayed_tuple<_Callable, _Args...>>
		      __make_invoker(_Callable&& __callable, _Args&&... __args)
		      {
			return { __decayed_tuple<_Callable, _Args...>{
			    std::forward<_Callable>(__callable), std::forward<_Args>(__args)...
			} };
		      }
#endif

	    // Abstract base class for types that wrap arbitrary functors to be
	    // invoked in the new thread of execution.
	    struct _State
	    {
			virtual ~_State();
			virtual void _M_run() = 0;

			// the class which spawned this thread
			thread* _M_owning_thread;
	    };
	    using _State_ptr = std::unique_ptr<_State>;



	    template<typename _Callable, typename... _Args>
	      explicit
	      thread(_Callable&& __f, _Args&&... __args)
	    	: m_task_handle(nullptr)
	      {
	    	//std::bind()
	    	// Create the wrapper object for the arbitrary functors
#if 0
	    	_State_ptr state_ptr = _S_make_state(
	  		      std::__bind_simple(std::forward<_Callable>(__f),
	  					 std::forward<_Args>(__args)...));
#else
	    	_State_ptr state_ptr = _S_make_state(
	  		      __make_invoker(std::forward<_Callable>(__f),
	  					 std::forward<_Args>(__args)...));

#endif

	    	// and start it.
	    	_M_start_thread(std::move(state_ptr));
	      }

		~thread();

	    void join();

	private:
		template<typename _Callable>
		struct _State_impl : public _State
		{
			// The wrapped functor
			_Callable		_M_func;

			/** Constructor. Just take the callable and store it */
			_State_impl(_Callable&& __f)
				: _M_func(std::forward<_Callable>(__f))
			{}

			void _M_run()
			{
				// Call the tasks main itself
				_M_func();

				// Indicate this thread as terminated
				this->_M_owning_thread->_M_bo_thread_terminated = true;

				// return insdie a ask is not possible in FreeRTOS. Need to delete myself from FreeRTOS
				// before being able to terminate
				vTaskDelete(nullptr);
			}
		};

		TaskHandle_t m_task_handle;
		std::string m_task_name;
		size_t m_u_task_priority;
		size_t m_u_stack_size;

		/** Helper to remember if the thread was already terminated */
		std::atomic<bool> _M_bo_thread_terminated;
	private:
		/** Helper function to start the thread */
		void _M_start_thread(_State_ptr);


		template<typename _Callable>
		static _State_ptr _S_make_state(_Callable&& __f)
		{
			using _Impl = _State_impl<_Callable>;
			return _State_ptr(new _Impl(std::forward<_Callable>(__f)));
		}

	};


}

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
