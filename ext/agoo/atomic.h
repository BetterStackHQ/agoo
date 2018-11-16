// Copyright (c) 2018, Peter Ohler, All rights reserved.

#ifndef AGOO_ATOMIC_H
#define AGOO_ATOMIC_H

#if HAVE_STDATOMIC_H

#include <stdatomic.h>

#else

// This is a poor attempt to implement the stdatomic calls needed for this
// project. No attempt was made to make it either fast or memory efficient. It
// works for older compilers and it turns out the overall impact on
// performance is small.

#include <pthread.h>

typedef struct _agooAtom {
    volatile void	*value;
    pthread_mutex_t	lock;
} *agooAtom;

#define	_agooAtomic(T) struct _agooAtom

typedef struct _agooAtom	atomic_flag;
typedef struct _agooAtom	atomic_int;

static inline void
atomic_init(agooAtom a, void *value) {
    a->value = value;
    pthread_mutex_init(&a->lock, 0);
}

#define atomic_store(a, v) _atomic_store((a), (void*)(v))

static inline void
_atomic_store(agooAtom a, void *value) {
    pthread_mutex_lock(&a->lock);
    a->value = value;
    pthread_mutex_unlock(&a->lock);
}

static inline volatile void*
atomic_load(agooAtom a) {
    volatile void	*value;
    
    pthread_mutex_lock(&a->lock);
    value = a->value;
    pthread_mutex_unlock(&a->lock);

    return value;
}

static inline int
atomic_fetch_add(agooAtom a, int delta) {
    int	before;
    
    pthread_mutex_lock(&a->lock);
    before = (int)(long)a->value;
    a->value = (void*)(long)(before + delta);
    pthread_mutex_unlock(&a->lock);

    return before;
}

static inline int
atomic_fetch_sub(agooAtom a, int delta) {
    int	before;
    
    pthread_mutex_lock(&a->lock);
    before = (int)a->value;
    a->value = (void*)(long)(before - delta);
    pthread_mutex_unlock(&a->lock);

    return before;
}

static inline void
atomic_flag_clear(agooAtom a) {
    pthread_mutex_lock(&a->lock);
    a->value = NULL;
    pthread_mutex_unlock(&a->lock);
}

static inline bool
atomic_flag_test_and_set(agooAtom a) {
    volatile void	*prev;
    
    pthread_mutex_lock(&a->lock);
    if (NULL == (prev = a->value)) {
	a->value = (void*)1;
    }
    pthread_mutex_unlock(&a->lock);

    return (NULL != prev);
}

#endif

#endif // AGOO_ATOMIC_H
