
#ifndef _GLIBCXX_GCC_GHTR_MIOSIX_H
#define _GLIBCXX_GCC_GHTR_MIOSIX_H

#include <pthread.h>
#include <unistd.h>

//Note to self: gthr.h contains useful information
//on how a gthr-xxx.h should look like

#define __GTHREADS 1
#define __GTHREAD_HAS_COND 1
//#define __GTHREADS_CXX0X 1 //Not yet, missing __gthread_*_timed*

//In Miosix, threads are always enabled, period.
#define __gthread_active_p() 1

typedef pthread_t       __gthread_t;
typedef pthread_key_t   __gthread_key_t; //This actually is unimplemented
typedef pthread_once_t  __gthread_once_t;
typedef pthread_mutex_t __gthread_mutex_t;
typedef pthread_mutex_t __gthread_recursive_mutex_t;
typedef pthread_cond_t  __gthread_cond_t;
typedef struct timespec __gthread_time_t;

#define __GTHREAD_MUTEX_INIT           PTHREAD_MUTEX_INITIALIZER
#define __GTHREAD_ONCE_INIT            PTHREAD_ONCE_INIT
#define __GTHREAD_RECURSIVE_MUTEX_INIT PTHREAD_MUTEX_RECURSIVE_INITIALIZER_NP
#define __GTHREAD_COND_INIT            PTHREAD_COND_INITIALIZER
#define __GTHREAD_TIME_INIT            {0,0}

#define __gthread_once                    pthread_once
#define __gthread_mutex_destroy           pthread_mutex_destroy
#define __gthread_mutex_lock              pthread_mutex_lock
#define __gthread_mutex_trylock           pthread_mutex_trylock
#define __gthread_mutex_unlock            pthread_mutex_unlock
#define __gthread_recursive_mutex_lock    pthread_mutex_lock
#define __gthread_recursive_mutex_trylock pthread_mutex_trylock
#define __gthread_recursive_mutex_unlock  pthread_mutex_unlock
#define __gthread_cond_broadcast          pthread_cond_broadcast
#define __gthread_cond_wait               pthread_cond_wait
#define __gthread_cond_wait_recursive     pthread_cond_wait

//These actually aren't implemented in Miosix, so code trying to use these will
//fail to link, and for now it's the "desired" behaviour (better than failing
//at runtime, at least). They are used somewhere in libstdc++ too, but it can
//be patched to remove those uses.
#define __gthread_key_create              pthread_key_create
#define __gthread_key_delete              pthread_key_delete
#define __gthread_getspecific             pthread_getspecific
#define __gthread_setspecific             pthread_setspecific

#endif //_GLIBCXX_GCC_GHTR_MIOSIX_H
