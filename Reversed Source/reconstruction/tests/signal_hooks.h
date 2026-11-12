#ifndef SIGNAL_TEST_HOOKS_H
#define SIGNAL_TEST_HOOKS_H
#include <stdlib.h>
void signal_test_abort(void);
#define abort signal_test_abort
#endif
