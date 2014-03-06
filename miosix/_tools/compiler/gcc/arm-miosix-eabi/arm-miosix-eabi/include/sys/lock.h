#ifndef __SYS_LOCK_H__
#define __SYS_LOCK_H__

#include <_ansi.h>

/*
 * The type of pthread_mutex_t, has been moved here from sys/types.h, because
 * sys/types.h #includes sys/_types.h which in turn #includes sys/lock.h,
 * and sys/lock.h actually needs to know the type of pthread_mutex_t.
 * Unfortunately simply adding an #include sys/types.h into sys/lock.h didn't
 * work because it caused a cyclic dependency between headers
 */

struct WaitingList
{
    void *thread; /* Actually, a Thread * but C doesn't know about C++ classes */
    struct WaitingList *next;
};

typedef struct
{
    void *owner;  /* Actually, a Thread * but C doesn't know about C++ classes */
    struct WaitingList *first;
    struct WaitingList *last;
    int recursive; /* -1 = special value for non recursive */
} pthread_mutex_t;

/*
 * Finished declaring pthread stuff, now starting real content of lock.h
 */

typedef pthread_mutex_t _LOCK_T;
typedef pthread_mutex_t _LOCK_RECURSIVE_T;

#define __LOCK_INIT(clazz,lock) clazz pthread_mutex_t lock = {0,0,0,-1}
#define __LOCK_INIT_RECURSIVE(clazz,lock) clazz pthread_mutex_t lock = {0,0,0,0}
#define __lock_init(lock) pthread_mutex_init(&lock,NULL) 
#define __lock_init_recursive(lock) \
do { \
    lock.owner=0; \
    lock.first=0; \
    lock.last=0; \
    lock.recursive=0; \
} while(0)
#define __lock_close(lock) pthread_mutex_destroy(&lock)
#define __lock_close_recursive(lock) pthread_mutex_destroy(&lock)
#define __lock_acquire(lock) pthread_mutex_lock(&lock)
#define __lock_acquire_recursive(lock) pthread_mutex_lock(&lock)
#define __lock_try_acquire(lock) pthread_mutex_trylock(&lock)
#define __lock_try_acquire_recursive(lock) pthread_mutex_trylock(&lock)
#define __lock_release(lock) pthread_mutex_unlock(&lock)
#define __lock_release_recursive(lock) pthread_mutex_unlock(&lock)

#endif /* __SYS_LOCK_H__ */
