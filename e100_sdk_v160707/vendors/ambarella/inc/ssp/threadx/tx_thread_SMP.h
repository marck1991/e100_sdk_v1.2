/**************************************************************************/ 
/*                                                                        */ 
/*            Copyright (c) 1996-2014 by Express Logic Inc.               */ 
/*                                                                        */ 
/*  This software is copyrighted by and is the sole property of Express   */ 
/*  Logic, Inc.  All rights, title, ownership, or other interests         */ 
/*  in the software remain the property of Express Logic, Inc.  This      */ 
/*  software may only be used in accordance with the corresponding        */ 
/*  license agreement.  Any unauthorized use, duplication, transmission,  */ 
/*  distribution, or disclosure of this software is expressly forbidden.  */ 
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */ 
/*  written consent of Express Logic, Inc.                                */ 
/*                                                                        */ 
/*  Express Logic, Inc. reserves the right to modify this software        */ 
/*  without notice.                                                       */ 
/*                                                                        */ 
/*  Express Logic, Inc.                     info@expresslogic.com         */
/*  11423 West Bernardo Court               http://www.expresslogic.com   */
/*  San Diego, CA  92127                                                  */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** ThreadX Component                                                     */
/**                                                                       */
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    tx_thread.h                                        PORTABLE SMP     */ 
/*                                                           5.6.2        */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file defines the ThreadX thread control component, including   */ 
/*    data types and external references.  It is assumed that tx_api.h    */
/*    and tx_port.h have already been included.                           */
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  05-15-2012     William E. Lamie         Initial Version 5.5           */ 
/*  07-15-2012     William E. Lamie         Modified comment(s), and      */ 
/*                                            added SMP optimizations,    */ 
/*                                            resulting in version 5.6    */ 
/*  12-12-2012     William E. Lamie         Modified comment(s), added    */ 
/*                                            macro for redefining the    */ 
/*                                            current state retrieval, and*/ 
/*                                            added function pointer for  */ 
/*                                            releasing any owned mutexes */ 
/*                                            when thread completes or    */ 
/*                                            is terminated,              */ 
/*                                            resulting in version 5.6.1  */ 
/*  05-01-2014     William E. Lamie         Modified comment(s), made     */ 
/*                                            the release cores flag      */ 
/*                                            volatile, and removed       */ 
/*                                            unnecessary prototypes,     */ 
/*                                            resulting in version 5.6.2  */ 
/*                                                                        */ 
/**************************************************************************/ 

#ifndef TX_THREAD_H
#define TX_THREAD_H


/* Define thread control specific data definitions.  */

#define TX_THREAD_ID                            ((ULONG) 0x54485244)
#define TX_THREAD_MAX_BYTE_VALUES               256
#define TX_THREAD_PRIORITY_GROUP_MASK           ((ULONG) 0xFF)
#define TX_THREAD_PRIORITY_GROUP_SIZE           8
#define TX_THREAD_EXECUTE_LOG_SIZE              8


/* Define the MOD32 bit set macro that is used to set/clear a priority bit within a specific
   priority group.  */

#if TX_MAX_PRIORITIES > 32
#define MAP_INDEX                               map_index
#ifndef TX_MOD32_BIT_SET
#define TX_MOD32_BIT_SET(a,b)                   b = (((ULONG) 1) << (a%32));
#endif
#else
#define MAP_INDEX                               0
#ifndef TX_MOD32_BIT_SET
#define TX_MOD32_BIT_SET(a,b)                   b = (((ULONG) 1) << (a));
#endif
#endif


/* Define the DIV32 bit set macro that is used to set/clear a priority group bit and is
   only necessary when using priorities greater than 32.  */

#if TX_MAX_PRIORITIES > 32
#define MAP_INDEX                               map_index
#ifndef TX_DIV32_BIT_SET
#define TX_DIV32_BIT_SET(a,b)                   b = (((ULONG) 1) << (a/32));
#endif
#endif


/* Define state change macro that can be used by run-mode debug agents to keep track of thread
   state changes. By default, it is mapped to white space.  */
   
#ifndef TX_THREAD_STATE_CHANGE
#define TX_THREAD_STATE_CHANGE(a, b)
#endif


/* Define the macro to set the current thread pointer. This is particularly useful in SMP 
   versions of ThreadX to add additional processing.  The default implementation is to simply
   access the global current thread pointer directly.  */
   
#ifndef TX_THREAD_SET_CURRENT
#define TX_THREAD_SET_CURRENT(a)            TX_MEMSET(&_tx_thread_current_ptr[0], a, sizeof(_tx_thread_current_ptr));
#endif


/* Define the get system state macro. By default, it is mapped to white space.  */
   
#ifndef TX_THREAD_GET_SYSTEM_STATE
#define TX_THREAD_GET_SYSTEM_STATE()        _tx_thread_smp_current_state_get()
#endif


/* Define the lowest bit set macro. Note, that this may be overridden 
   by a port specific definition if there is supporting assembly language
   instructions in the architecture.  */
   
#ifndef TX_LOWEST_SET_BIT_CALCULATE
#define TX_LOWEST_SET_BIT_CALCULATE(m, b)   \
    if (m & (ULONG) 0x1)                    \
    {                                       \
        b = 0;                              \
    }                                       \
    else                                    \
    {                                       \
        m =  m & (ULONG) (-((LONG)m));      \
        if (m < (ULONG) 0x100)              \
        {                                   \
            b = 1;                          \
        }                                   \
        else if (m < (ULONG) 0x10000)       \
        {                                   \
            b =  9;                         \
            m =  m >> 8;                    \
        }                                   \
        else if (m < (ULONG) 0x01000000)    \
        {                                   \
            b = 17;                         \
            m = m >> 16;                    \
        }                                   \
        else                                \
        {                                   \
            b = 25;                         \
            m = m >> 24;                    \
        }                                   \
        if (m >= 0x10)                      \
        {                                   \
            m = m >> 4;                     \
            b = b + 4;                      \
        }                                   \
        if (m >= 0x4)                       \
        {                                   \
            m = m >> 2;                     \
            b = b + 2;                      \
        }                                   \
        b = b - (UINT) (m & 0x1);           \
    }
#endif


/* Define the next priority macro. Note, that this may be overridden 
   by a port specific definition.  */
   
#ifndef TX_NEXT_PRIORITY_FIND
#if TX_MAX_PRIORITIES > 32
#define TX_NEXT_PRIORITY_FIND(p, n)                                     \
{                                                                       \
ULONG           wpriority;                                              \
UINT            temp;                                                   \
UINT            map_index;                                              \
ULONG           local_priority_map_active;                              \
ULONG           local_priority_map;                                     \
    wpriority =  p;                                                     \
    n =  TX_MAX_PRIORITIES;                                             \
    while (wpriority < TX_MAX_PRIORITIES)                               \
    {                                                                   \
        local_priority_map_active =                                     \
            _tx_thread_priority_map_active >> (wpriority / 32);         \
        if (local_priority_map_active == 0)                             \
        {                                                               \
            break;                                                      \
        }                                                               \
        TX_LOWEST_SET_BIT_CALCULATE(local_priority_map_active, temp)    \
        wpriority =  wpriority + (temp * 32);                           \
        if (wpriority >= TX_MAX_PRIORITIES)                             \
        {                                                               \
            break;                                                      \
        }                                                               \
        map_index =  wpriority / 32;                                    \
        local_priority_map =  _tx_thread_priority_maps[map_index];      \
        local_priority_map =  local_priority_map >> (wpriority % 32);   \
        if (local_priority_map == 0)                                    \
        {                                                               \
            wpriority =  wpriority + (32 - (wpriority % 32));           \
            continue;                                                   \
        }                                                               \
        TX_LOWEST_SET_BIT_CALCULATE(local_priority_map, temp)           \
        wpriority =  wpriority + temp;                                  \
        if (wpriority >= TX_MAX_PRIORITIES)                             \
        {                                                               \
            break;                                                      \
        }                                                               \
        else                                                            \
        {                                                               \
            if (_tx_thread_priority_list[wpriority])                    \
            {                                                           \
                n =  wpriority;                                         \
                break;                                                  \
            }                                                           \
            wpriority++;                                                \
        }                                                               \
    }                                                                   \
}
#else
#define TX_NEXT_PRIORITY_FIND(p, n)                                     \
{                                                                       \
ULONG           wpriority;                                              \
UINT            temp;                                                   \
ULONG           local_priority_map;                                     \
    wpriority =  p;                                                     \
    local_priority_map =  _tx_thread_priority_maps[0];                  \
    local_priority_map =  local_priority_map >> wpriority;              \
    if (local_priority_map == 0)                                        \
    {                                                                   \
        n =  TX_MAX_PRIORITIES;                                         \
    }                                                                   \
    else                                                                \
    {                                                                   \
        TX_LOWEST_SET_BIT_CALCULATE(local_priority_map, temp)           \
        n =  wpriority + temp;                                          \
    }                                                                   \
}
#endif
#endif


/* Define thread control function prototypes.  */

VOID        _tx_thread_context_save(VOID);
VOID        _tx_thread_context_restore(VOID);
UINT        _tx_thread_create(TX_THREAD *thread_ptr, CHAR *name_ptr, 
                VOID (*entry_function)(ULONG), ULONG entry_input,
                VOID *stack_start, ULONG stack_size, 
                UINT priority, UINT preempt_threshold, 
                ULONG time_slice, UINT auto_start);
UINT        _tx_thread_delete(TX_THREAD *thread_ptr);
UINT        _tx_thread_entry_exit_notify(TX_THREAD *thread_ptr, VOID (*thread_entry_exit_notify)(TX_THREAD *, UINT));
TX_THREAD  *_tx_thread_identify(VOID);
UINT        _tx_thread_info_get(TX_THREAD *thread_ptr, CHAR **name, UINT *state, ULONG *run_count, 
                UINT *priority, UINT *preemption_threshold, ULONG *time_slice, 
                TX_THREAD **next_thread, TX_THREAD **next_suspended_thread);
VOID        _tx_thread_initialize(VOID);
UINT        _tx_thread_interrupt_control(UINT new_posture);
UINT        _tx_thread_performance_info_get(TX_THREAD *thread_ptr, ULONG *resumptions, ULONG *suspensions, 
                ULONG *solicited_preemptions, ULONG *interrupt_preemptions, ULONG *priority_inversions,
                ULONG *time_slices, ULONG *relinquishes, ULONG *timeouts, ULONG *wait_aborts, TX_THREAD **last_preempted_by);
UINT        _tx_thread_performance_system_info_get(ULONG *resumptions, ULONG *suspensions,
                ULONG *solicited_preemptions, ULONG *interrupt_preemptions, ULONG *priority_inversions,
                ULONG *time_slices, ULONG *relinquishes, ULONG *timeouts, ULONG *wait_aborts,
                ULONG *non_idle_returns, ULONG *idle_returns);
UINT        _tx_thread_preemption_change(TX_THREAD *thread_ptr, UINT new_threshold,
                        UINT *old_threshold);
UINT        _tx_thread_priority_change(TX_THREAD *thread_ptr, UINT new_priority,
                        UINT *old_priority);
VOID        _tx_thread_relinquish(VOID);
UINT        _tx_thread_reset(TX_THREAD *thread_ptr);
UINT        _tx_thread_resume(TX_THREAD *thread_ptr);
VOID        _tx_thread_schedule(VOID);
VOID        _tx_thread_shell_entry(VOID);
UINT        _tx_thread_sleep(ULONG timer_ticks);
VOID        _tx_thread_stack_analyze(TX_THREAD *thread_ptr);
VOID        _tx_thread_stack_build(TX_THREAD *thread_ptr, VOID (*function_ptr)(VOID));
VOID        _tx_thread_stack_error(TX_THREAD *thread_ptr);
VOID        _tx_thread_stack_error_handler(TX_THREAD *thread_ptr);
UINT        _tx_thread_stack_error_notify(VOID (*stack_error_handler)(TX_THREAD *));
VOID        _tx_thread_system_preempt_check(VOID);
VOID        _tx_thread_system_resume(TX_THREAD *thread_ptr);
VOID        _tx_thread_system_ni_resume(TX_THREAD *thread_ptr);
VOID        _tx_thread_system_return(VOID);
VOID        _tx_thread_system_suspend(TX_THREAD *thread_ptr);
VOID        _tx_thread_system_ni_suspend(TX_THREAD *thread_ptr, ULONG timeout);
UINT        _tx_thread_suspend(TX_THREAD *thread_ptr);
UINT        _tx_thread_terminate(TX_THREAD *thread_ptr);
UINT        _tx_thread_time_slice_change(TX_THREAD *thread_ptr, ULONG new_time_slice, ULONG *old_time_slice);
VOID        _tx_thread_time_slice(VOID);
VOID        _tx_thread_timeout(ULONG timeout_input);
UINT        _tx_thread_wait_abort(TX_THREAD *thread_ptr);


/* Define error checking shells for API services.  These are only referenced by the 
   application.  */

UINT        _txe_thread_create(TX_THREAD *thread_ptr, CHAR *name_ptr, 
                VOID (*entry_function)(ULONG), ULONG entry_input,
                VOID *stack_start, ULONG stack_size, 
                UINT priority, UINT preempt_threshold, 
                ULONG time_slice, UINT auto_start, UINT thread_control_block_size);
UINT        _txe_thread_delete(TX_THREAD *thread_ptr);
UINT        _txe_thread_entry_exit_notify(TX_THREAD *thread_ptr, VOID (*thread_entry_exit_notify)(TX_THREAD *, UINT));
TX_THREAD  *_tx_thread_identify(VOID);
UINT        _txe_thread_info_get(TX_THREAD *thread_ptr, CHAR **name, UINT *state, ULONG *run_count, 
                UINT *priority, UINT *preemption_threshold, ULONG *time_slice, 
                TX_THREAD **next_thread, TX_THREAD **next_suspended_thread);
UINT        _txe_thread_preemption_change(TX_THREAD *thread_ptr, UINT new_threshold,
                        UINT *old_threshold);
UINT        _txe_thread_priority_change(TX_THREAD *thread_ptr, UINT new_priority,
                        UINT *old_priority);
VOID        _txe_thread_relinquish(VOID);
UINT        _txe_thread_reset(TX_THREAD *thread_ptr);
UINT        _txe_thread_resume(TX_THREAD *thread_ptr);
UINT        _txe_thread_suspend(TX_THREAD *thread_ptr);
UINT        _txe_thread_terminate(TX_THREAD *thread_ptr);
UINT        _txe_thread_time_slice_change(TX_THREAD *thread_ptr, ULONG new_time_slice, ULONG *old_time_slice);
UINT        _txe_thread_wait_abort(TX_THREAD *thread_ptr);


/* Define all SMP prototypes for calling from C.  */

void        _tx_thread_smp_current_state_set(ULONG new_state);
UINT        _tx_thread_smp_find_next_priority(UINT priority);
void        _tx_thread_smp_high_level_initialize(void);
void        _tx_thread_smp_rebalance_execute_list(UINT core_index);


/* Define ThreadX SMP low-level assembly routines.   */

UINT        _tx_thread_smp_core_get(void);
VOID        _tx_thread_smp_core_wait(void);
TX_THREAD * _tx_thread_smp_current_thread_get(void);
void        _tx_thread_smp_initialize_wait(void);
void        _tx_thread_smp_low_level_initialize(UINT number_of_cores);
void        _tx_thread_smp_core_preempt(UINT core);
UINT        _tx_thread_smp_protect(void);
void        _tx_thread_smp_unprotect(UINT interrupt_save);
ULONG       _tx_thread_smp_current_state_get(void);
ULONG       _tx_thread_smp_time_get(void);


/* Thread control component data declarations follow.  */

/* Determine if the initialization function of this component is including
   this file.  If so, make the data definitions really happen.  Otherwise,
   make them extern so other functions in the component can access them.  */

#ifdef TX_THREAD_INIT
#define THREAD_DECLARE 
#else
#define THREAD_DECLARE extern
#endif


/* Define the pointer that contains the system stack pointer.  This is
   utilized when control returns from a thread to the system to reset the
   current stack.  This is setup in the low-level initialization function. */

THREAD_DECLARE  VOID *                      _tx_thread_system_stack_ptr[TX_THREAD_SMP_MAX_CORES];


/* Define the current thread pointer.  This variable points to the currently
   executing thread.  If this variable is NULL, no thread is executing.  */

THREAD_DECLARE  TX_THREAD *                 _tx_thread_current_ptr[TX_THREAD_SMP_MAX_CORES];


/* Define the variable that holds the next thread to execute.  It is important
   to remember that this is not necessarily equal to the current thread 
   pointer.  */

THREAD_DECLARE  TX_THREAD *                 _tx_thread_execute_ptr[TX_THREAD_SMP_MAX_CORES];


/* Define the ThreadX SMP scheduling and mapping data structures.  */

THREAD_DECLARE  TX_THREAD *                 _tx_thread_smp_schedule_list[TX_THREAD_SMP_MAX_CORES];
THREAD_DECLARE  ULONG                       _tx_thread_smp_reschedule_pending;
THREAD_DECLARE  TX_THREAD_SMP_PROTECT       _tx_thread_smp_protection;
THREAD_DECLARE  volatile ULONG              _tx_thread_smp_release_cores_flag;
THREAD_DECLARE  ULONG                       _tx_thread_smp_system_error;
THREAD_DECLARE  ULONG                       _tx_thread_smp_inter_core_interrupts[TX_THREAD_SMP_MAX_CORES];


/* Define logic for conditional dynamic maximum number of cores.  */

#ifdef TX_THREAD_SMP_DYNAMIC_CORE_MAX

THREAD_DECLARE  ULONG                       _tx_thread_smp_max_cores;
THREAD_DECLARE  ULONG                       _tx_thread_smp_detected_cores;

#endif



/* Define the head pointer of the created thread list.  */

THREAD_DECLARE  TX_THREAD *                 _tx_thread_created_ptr;


/* Define the variable that holds the number of created threads. */

THREAD_DECLARE  ULONG                       _tx_thread_created_count;


/* Define the current state variable.  When this value is 0, a thread
   is executing or the system is idle.  Other values indicate that 
   interrupt or initialization processing is active.  This variable is
   initialized to TX_INITIALIZE_IN_PROGRESS to indicate initialization is
   active.  */

THREAD_DECLARE  volatile ULONG              _tx_thread_system_state[TX_THREAD_SMP_MAX_CORES];


/* Determine if we need to remap system state to a function call.  */

#ifndef TX_THREAD_SMP_SOURCE_CODE


/* Yes, remap system state to a function call so we can get the system state for the current core.  */

#define _tx_thread_system_state             _tx_thread_smp_current_state_get()


/* Yes, remap get current thread to a function call so we can get the current thread for the current core.  */

#define _tx_thread_current_ptr              _tx_thread_smp_current_thread_get()

#endif


/* Define the 32-bit priority bit-maps. There is one priority bit map for each
   32 priority levels supported. If only 32 priorities are supported there is 
   only one bit map. Each bit within a priority bit map represents that one 
   or more threads at the associated thread priority are ready.  */ 

THREAD_DECLARE  ULONG                       _tx_thread_priority_maps[TX_MAX_PRIORITIES/32];


/* Define the priority map active bit map that specifies which of the previously 
   defined priority maps have something set. This is only necessary if more than 
   32 priorities are supported.  */

#if TX_MAX_PRIORITIES > 32
THREAD_DECLARE  ULONG                       _tx_thread_priority_map_active;
#endif


#ifndef TX_DISABLE_PREEMPTION_THRESHOLD

/* Define the 32-bit preempt priority bit maps.  There is one preempt bit map 
   for each 32 priority levels supported. If only 32 priorities are supported 
   there is only one bit map. Each set set bit corresponds to a preempted priority 
   level that had preemption-threshold active to protect against preemption of a 
   range of relatively higher priority threads.  */

THREAD_DECLARE  ULONG                       _tx_thread_preempted_maps[TX_MAX_PRIORITIES/32];


/* Define the preempt map active bit map that specifies which of the previously 
   defined preempt maps have something set. This is only necessary if more than 
   32 priorities are supported.  */

#if TX_MAX_PRIORITIES > 32
THREAD_DECLARE  ULONG                       _tx_thread_preempted_map_active;
#endif
#endif


/* Define the last thread scheduled with preemption-threshold. When preemption-threshold is
   disabled, a thread with preemption-threshold set disables all other threads from running.  
   Effectively, its preemption-threshold is 0.  */
   
THREAD_DECLARE  TX_THREAD                   *_tx_thread_preemption_threshold_scheduled;


/* Define the array of thread pointers.  Each entry represents the threads that
   are ready at that priority group.  For example, index 10 in this array
   represents the first thread ready at priority 10.  If this entry is NULL,
   no threads are ready at that priority.  */

THREAD_DECLARE  TX_THREAD *                 _tx_thread_priority_list[TX_MAX_PRIORITIES];


/* Define the global preempt disable variable.  If this is non-zero, preemption is
   disabled.  It is used internally by ThreadX to prevent preemption of a thread in 
   the middle of a service that is resuming or suspending another thread.  */

THREAD_DECLARE  volatile UINT               _tx_thread_preempt_disable;


/* Define the global function pointer for mutex cleanup on thread completion or 
   termination. This pointer is setup during mutex initialization.  */

THREAD_DECLARE  VOID            (*_tx_thread_mutex_release)(TX_THREAD *);


/* Define the global build options variable.  This contains a bit map representing
   how the ThreadX library was built. The following are the bit field definitions:

                    Bit(s)                   Meaning

                    31                  Reserved
                    30                  TX_NOT_INTERRUPTABLE defined
                    29-24               Priority groups 1  -> 32 priorities
                                                        2  -> 64 priorities
                                                        3  -> 96 priorities

                                                        ...

                                                        32 -> 1024 priorities
                    23                  TX_TIMER_PROCESS_IN_ISR defined
                    22                  TX_REACTIVATE_INLINE defined
                    21                  TX_DISABLE_STACK_FILLING defined
                    20                  TX_ENABLE_STACK_CHECKING defined
                    19                  TX_DISABLE_PREEMPTION_THRESHOLD defined
                    18                  TX_DISABLE_REDUNDANT_CLEARING defined
                    17                  TX_DISABLE_NOTIFY_CALLBACKS defined
                    16                  TX_BLOCK_POOL_ENABLE_PERFORMANCE_INFO defined
                    15                  TX_BYTE_POOL_ENABLE_PERFORMANCE_INFO defined
                    14                  TX_EVENT_FLAGS_ENABLE_PERFORMANCE_INFO defined
                    13                  TX_MUTEX_ENABLE_PERFORMANCE_INFO defined
                    12                  TX_QUEUE_ENABLE_PERFORMANCE_INFO defined
                    11                  TX_SEMAPHORE_ENABLE_PERFORMANCE_INFO defined
                    10                  TX_THREAD_ENABLE_PERFORMANCE_INFO defined
                    9                   TX_TIMER_ENABLE_PERFORMANCE_INFO defined
                    8                   TX_ENABLE_EVENT_TRACE | TX_ENABLE_EVENT_LOGGING defined
                    7                   Reserved
                    6                   Reserved
                    5                   Reserved
                    4                   Reserved
                    3                   Reserved
                    2                   Reserved
                    1                   64-bit FPU Enabled
                    0                   Reserved  */

THREAD_DECLARE  ULONG                       _tx_build_options;


#ifdef TX_ENABLE_STACK_CHECKING

/* Define the global function pointer for stack error handling. If a stack error is 
   detected and the application has registered a stack error handler, it will be 
   called via this function pointer.  */

THREAD_DECLARE  VOID                        (*_tx_thread_application_stack_error_handler)(TX_THREAD *);

#endif

#ifdef TX_THREAD_ENABLE_PERFORMANCE_INFO

/* Define the total number of thread resumptions. Each time a thread enters the
   ready state this variable is incremented.  */

THREAD_DECLARE  ULONG                       _tx_thread_performance_resume_count;


/* Define the total number of thread suspensions. Each time a thread enters a 
   suspended state this variable is incremented.  */

THREAD_DECLARE  ULONG                       _tx_thread_performance_suspend_count;


/* Define the total number of solicited thread preemptions. Each time a thread is 
   preempted by directly calling a ThreadX service, this variable is incremented.  */

THREAD_DECLARE  ULONG                       _tx_thread_performance_solicited_preemption_count;


/* Define the total number of interrupt thread preemptions. Each time a thread is 
   preempted as a result of an ISR calling a ThreadX service, this variable is 
   incremented.  */

THREAD_DECLARE  ULONG                       _tx_thread_performance_interrupt_preemption_count;


/* Define the total number of priority inversions. Each time a thread is blocked by
   a mutex owned by a lower-priority thread, this variable is incremented.  */

THREAD_DECLARE  ULONG                       _tx_thread_performance_priority_inversion_count;


/* Define the total number of time-slices.  Each time a time-slice operation is 
   actually performed (another thread is setup for running) this variable is 
   incremented.  */

THREAD_DECLARE  ULONG                       _tx_thread_performance_time_slice_count;


/* Define the total number of thread relinquish operations.  Each time a thread 
   relinquish operation is actually performed (another thread is setup for running)
   this variable is incremented.  */

THREAD_DECLARE  ULONG                       _tx_thread_performance_relinquish_count;


/* Define the total number of thread timeouts. Each time a thread has a 
   timeout this variable is incremented.  */

THREAD_DECLARE  ULONG                       _tx_thread_performance_timeout_count;


/* Define the total number of thread wait aborts. Each time a thread's suspension 
   is lifted by the tx_thread_wait_abort call this variable is incremented.  */

THREAD_DECLARE  ULONG                       _tx_thread_performance_wait_abort_count;


/* Define the total number of idle system thread returns. Each time a thread returns to 
   an idle system (no other thread is ready to run) this variable is incremented.  */

THREAD_DECLARE  ULONG                       _tx_thread_performance_idle_return_count;


/* Define the total number of non-idle system thread returns. Each time a thread returns to 
   a non-idle system (another thread is ready to run) this variable is incremented.  */

THREAD_DECLARE  ULONG                       _tx_thread_performance_non_idle_return_count;

#endif


#ifdef TX_THREAD_INIT
const CHAR _tx_thread_special_string[] = 
  "G-ML-EL-ML-BL-DL-BL-GB-GL-M-D-DL-GZ-KH-EL-CM-NH-HA-GF-DD-AT-DW-USA-CA-SD-SDSU";
#endif

#endif
