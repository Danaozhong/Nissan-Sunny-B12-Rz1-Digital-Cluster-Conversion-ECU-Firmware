
#include <cstring>
#include <algorithm>
#include "trace_if.h"
#include "stm32fxxx.h"
#include "hwheader.h"
#include "FreeRTOS.h"
#include "task.h"
#include "excp_handler_if.h"

#include <sys/time.h> // for timeval

extern "C"
{
    extern int _end;

    void *_sbrk(int incr)
    {
        static unsigned char *heap = NULL;
        unsigned char *prev_heap;

        if (heap == NULL) 
        {
            heap = (unsigned char *)&_end;
        }
        prev_heap = heap;

        heap += incr;

        return prev_heap;
    }

    int _close(int file)
    {
        return -1;
    }

    int _fstat(int file, struct stat *st)
    {
        //st->st_mode = S_IFCHR;
        return 0;
    }

    int _isatty(int file)
    {
        return 1;
    }

    int _lseek(int file, int ptr, int dir)
    {
        return 0;
    }

    void _exit(int status)
    {
        __asm("BKPT #0");
    }

    void _kill(int pid, int sig)
    {
        return;
    }

    extern void *pxCurrentTCB;

    int _getpid(void)
    {
        return -1;
    }

    int _write (int file, char * ptr, int len)
    {
        if ((file != 1) && (file != 2) && (file != 3)) 
        {
            return -1;
        }

        //char* print_buffer = (char*)malloc(len + 1);
        //const uint32_t u32_length =  len; //std::min(len, 100 - 1);
        //std::memcpy(print_buffer, ptr, u32_length);
        //print_buffer[u32_length] = '\0';

        DEBUG_PRINTF_NON_TERMINATED(ptr, len);
        return len;
    }

    int _read (int file, char * ptr, int len)
    {
    #if 0
      int read = 0;

      if (file != 0) {
        return -1;
      }

      for (; len > 0; --len) {
        usart_serial_getchar(&stdio_uart_module, (uint8_t *)ptr++);
        read++;
      }
      return read;
    #endif
          return 0;
    }

    int _gettimeofday( struct timeval *tv, void *tzvp )
    {
        if (nullptr == tv)
        {
            return -1;
        }

        const uint64_t tickFrequency = static_cast<uint64_t>(HAL_GetTickFreq()); /* unit is Hz */

        uint64_t t = (static_cast<uint64_t>(HAL_GetTick()) * 1000 * 1000) / tickFrequency;  // get uptime in nanoseconds
        tv->tv_sec = t / 1'000'000'000;  // convert to seconds
        tv->tv_usec = ( t % 1'000'000'000 ) / 1000;  // get remaining microseconds
        return 0;  // return non-zero for error
    } // end _gettimeofday()

    void* malloc (size_t size)
    {
        /* Call the FreeRTOS version of malloc. */
        return pvPortMalloc(size);
    }
    void free (void* ptr)
    {
        /* Call the FreeRTOS version of free. */
        vPortFree( ptr );
    }
    
    void vApplicationMallocFailedHook( void )
    {
        /* vApplicationMallocFailedHook() will only be called if
        configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
        function that will get called if a call to pvPortMalloc() fails.
        pvPortMalloc() is called internally by the kernel whenever a task, queue,
        timer or semaphore is created.  It is also called by various parts of the
        demo application.  If heap_1.c or heap_2.c are used, then the size of the
        heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
        FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
        to query the size of free heap space that remains (although it does not
        provide information on how the remaining heap might be fragmented). */
        taskDISABLE_INTERRUPTS();
        
        ExceptionHandler_handle_exception(EXCP_MODULE_OS, EXCP_TYPE_OS_MALLOC_FAILED, true, __FILE__, __LINE__, 0u);
        
        for( ;; );
    }

    void vApplicationIdleHook( void )
    {
        /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
        to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
        task.  It is essential that code added to this hook function never attempts
        to block in any way (for example, call xQueueReceive() with a block time
        specified, or call vTaskDelay()).  If the application makes use of the
        vTaskDelete() API function (as this demo application does) then it is also
        important that vApplicationIdleHook() is permitted to return to its calling
        function, because it is the responsibility of the idle task to clean up
        memory allocated by the kernel to any task that has since been deleted. */
    }

    /** see https://www.cnblogs.com/shangdawei/p/4684798.html */
    void vApplicationTickHook(void)
    {
        HAL_IncTick();
    }


    void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
    {
        ( void ) pcTaskName;
        ( void ) pxTask;

        /* Run time stack overflow checking is performed if
        configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
        function is called if a stack overflow is detected. */
        taskDISABLE_INTERRUPTS();
        
        ExceptionHandler_handle_exception(EXCP_MODULE_OS, EXCP_TYPE_OS_STACK_OVERFLOW, true, __FILE__, __LINE__, 0u);
        
        for( ;; );
    }


    /**
      * @brief  This function handles Hard Fault exception.
      * @param  None
      * @retval None
      */
    void HardFault_Handler(void)
    {
        /* Go to infinite loop when Hard Fault exception occurs */
        ExceptionHandler_handle_exception(EXCP_MODULE_OS, EXCP_TYPE_OS_HARD_FAULT, true, __FILE__, __LINE__, 0u);
        while (1)
        {
        }
    }

    /**
      * @brief  This function handles Memory Manage exception.
      * @param  None
      * @retval None
      */
    void MemManage_Handler(void)
    {
        /* Go to infinite loop when Memory Manage exception occurs */
        ExceptionHandler_handle_exception(EXCP_MODULE_OS, EXCP_TYPE_OS_MEM_MANAGER_FAULT, true, __FILE__, __LINE__, 0u);
        while (1)
        {
        }
    }

    /**
      * @brief  This function handles Bus Fault exception.
      * @param  None
      * @retval None
      */
    void BusFault_Handler(void)
    {
        /* Go to infinite loop when Bus Fault exception occurs */
        ExceptionHandler_handle_exception(EXCP_MODULE_OS, EXCP_TYPE_OS_BUS_FAULT, true, __FILE__, __LINE__, 0u);
        while (1)
      {
      }
    }

    /**
      * @brief  This function handles Usage Fault exception.
      * @param  None
      * @retval None
      */
    void UsageFault_Handler(void)
    {
      /* Go to infinite loop when Usage Fault exception occurs */
      while (1)
      {
      }
    }

}

void* operator new( size_t size )
{
    return pvPortMalloc( size );
}

void* operator new[]( size_t size )
{
    return pvPortMalloc(size);
}

void operator delete( void * ptr )
{
    vPortFree(ptr);
}

void operator delete[]( void * ptr )
{
    vPortFree(ptr);
}
