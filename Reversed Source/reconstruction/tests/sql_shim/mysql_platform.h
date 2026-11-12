// Test-only MySQL socket type and OS-call redirects, after host STL headers.
#ifndef MCNCC_TEST_SQL_MYSQL_PLATFORM_H
#define MCNCC_TEST_SQL_MYSQL_PLATFORM_H
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <pthread.h>
#include <sys/time.h>
extern "C" int sql_test_mutex_lock(pthread_mutex_t*);
extern "C" int sql_test_mutex_unlock(pthread_mutex_t*);
extern "C" int sql_test_mutex_destroy(pthread_mutex_t*);
extern "C" int sql_test_thread_join(pthread_t,void**);
extern "C" int sql_test_mutex_init(pthread_mutex_t*,const pthread_mutexattr_t*);
extern "C" int sql_test_thread_create(pthread_t*,const pthread_attr_t*,void*(*)(void*),void*);
#define pthread_mutex_lock sql_test_mutex_lock
#define pthread_mutex_unlock sql_test_mutex_unlock
#define pthread_mutex_destroy sql_test_mutex_destroy
#define pthread_join sql_test_thread_join
#define pthread_mutex_init sql_test_mutex_init
#define pthread_create sql_test_thread_create
extern "C" int sql_test_gettimeofday(timeval*,void*);
#define gettimeofday sql_test_gettimeofday
#define my_socket_defined
typedef int my_socket;
#endif
