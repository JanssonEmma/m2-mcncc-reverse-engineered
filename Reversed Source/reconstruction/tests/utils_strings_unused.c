// Non-string dependencies must stay unused in this fixture.
#include <assert.h>
long random(void) { assert(!"unexpected randomness"); return 0; }
void _sys_err(const char* function, int line, const char* format, ...)
{ (void)function; (void)line; (void)format; assert(!"unexpected utility diagnostic"); }
