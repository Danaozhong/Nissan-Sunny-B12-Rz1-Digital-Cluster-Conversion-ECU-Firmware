#ifndef _POSIX_STUBS_FOR_GCC_
#define _POSIX_STUBS_FOR_GCC_

#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/sys/types.h"

#define _PTHREAD_MUTEX_INITIALIZER ((pthread_mutex_t) 0xFFFFFFFF)
#define PTHREAD_MUTEX_INITIALIZER _PTHREAD_MUTEX_INITIALIZER

#define _PTHREAD_ONCE_INIT  { 1, 0 }  /* is initialized and not run */
#define PTHREAD_ONCE_INIT _PTHREAD_ONCE_INIT

#define PTHREAD_COND_INITIALIZER  ((pthread_cond_t) 0xFFFFFFFF)

typedef __uint32_t pthread_key_t;        /* thread-specific data keys */

typedef struct {
  int   is_initialized;  /* is this structure initialized? */
  int   init_executed;   /* has the initialization routine been run? */
} pthread_once_t;       /* dynamic package initialization */

int	pthread_once(pthread_once_t *__once_control, void (*__init_routine)(void));

int	pthread_setspecific(pthread_key_t __key, const void *__value);

void*	pthread_getspecific(pthread_key_t __key);

int	pthread_detach(pthread_t __pthread);
int	pthread_cancel(pthread_t __pthread);


int	pthread_key_create(pthread_key_t *__key, void (*__destructor)( void * ));
int	pthread_key_delete(pthread_key_t __key);
int	pthread_cancel(pthread_t __pthread);


#endif
