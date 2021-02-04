
#include <cstring>
#include <algorithm>
#include "trace_if.h"
#include "stm32fxxx.h"
#include "hwheader.h"
#include "FreeRTOS.h"
#include "task.h"

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

        char print_buffer[100];
        const uint32_t u32_length =  std::min(len, 100 - 1);
        std::memcpy(print_buffer, ptr, u32_length);
        print_buffer[u32_length] = '\0';

        DEBUG_PRINTF(print_buffer);
        return u32_length;
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
