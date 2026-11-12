// Only linked into older isolated fixtures. New utils dependencies must not run.
#include <assert.h>
int fork(void) { assert(!"unexpected fork in isolated utility fixture"); return -1; }
long random(void) { assert(!"unexpected random in isolated utility fixture"); return 0; }
