#ifndef SIGNAL_TEST_SHIM_H
#define SIGNAL_TEST_SHIM_H
#define SIGVTALRM 26
#define SIGHUP 1
#define SIGCHLD 20
#define SIGINT 2
#define SIGTERM 15
#define SIGPIPE 13
#define SIGALRM 14
#define SIGUSR1 30
#define SIG_IGN ((void (*)(int))1)
typedef void (*signal_test_handler)(int);
signal_test_handler signal(int sig, signal_test_handler handler);
#endif
