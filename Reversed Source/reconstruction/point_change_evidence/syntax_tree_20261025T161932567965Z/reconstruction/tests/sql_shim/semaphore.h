// Test-only FreeBSD opaque handle types and scripted OS calls.
#ifndef MCNCC_TEST_SQL_SEMAPHORE_H
#define MCNCC_TEST_SQL_SEMAPHORE_H
struct sem;
typedef sem* sem_t;
extern "C" int sem_init(sem_t*,int,unsigned int);
extern "C" int sem_destroy(sem_t*);
extern "C" int sem_wait(sem_t*);
extern "C" int sem_post(sem_t*);
#endif
