#include <cstdlib>
extern "C" int pong_test_rand();
extern "C" void pong_test_exit(int status);
#define rand pong_test_rand
#define exit pong_test_exit
