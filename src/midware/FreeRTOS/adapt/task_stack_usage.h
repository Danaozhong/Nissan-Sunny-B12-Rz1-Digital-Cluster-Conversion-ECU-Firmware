#ifndef TASK_STACK_USAGE_H_
#define TASK_STACK_USAGE_H_
/**
 * task.h
 * <PRE>StackType_t uxTaskGetStackSize( TaskHandle_t xTask );</PRE>
 *
 * INCLUDE_pxTaskGetStackSize must be set to 1 in FreeRTOSConfig.h for
 * this function to be available.
 *
 * Returns the stack size associated with xTask. That is, the stack
 * size (in words, so on a 32 bit machine a value of 1 means 4 bytes) of the task.
 *
 * @param xTask Handle of the task associated with the stack to be checked.
 * Set xTask to NULL to check the stack of the calling task.
 *
 * @return The stack size (in words) associated with xTask.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "include/FreeRTOS.h"
#include "include/task.h"

StackType_t uxTaskGetStackSize( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;

#ifdef __cplusplus
} /* Extern "C" */
#endif

#endif