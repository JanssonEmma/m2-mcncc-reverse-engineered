#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
static long next_random;
static int random_calls, log_calls;
long random(void) { ++random_calls; return next_random; }
void _sys_err(const char *func, int line, const char *fmt, ...)
{
    va_list ap;
    assert(strcmp(func, "number_ex") == 0);
    assert(line == 339);
    assert(strcmp(fmt, "number(): first argument is bigger than second argument %d -> %d, %s %d") == 0);
    va_start(ap, fmt);
    assert(va_arg(ap,int) == 17);
    assert(va_arg(ap,int) == -9);
    assert(strcmp(va_arg(ap,char *), "fixture.c") == 0);
    assert(va_arg(ap,int) == 77);
    va_end(ap);
    ++log_calls;
}
#ifdef NUMERIC_STANDALONE
#include "fragment.c"
#else
#include "../../libthecore/include/utils.h"
#endif
int main(void)
{
    char file[] = "fixture.c";
    assert(sizeof(DWORD) == 4 && sizeof(int) == 4);
    assert(MIN(INT_MIN, INT_MAX) == INT_MIN);
    assert(MAX(INT_MIN, INT_MAX) == INT_MAX);
    assert(MIN(7,7)==7 && MAX(-5,-5)==-5);
    assert(MINMAX(-9, -10, 17)==-9);
    assert(MINMAX(-9, 5, 17)==5);
    assert(MINMAX(-9, 18, 17)==17);
    assert(MINMAX(17, 5, -9)==-9);
    next_random=2147483647L;
    assert(thecore_random()==2147483647UL && random_calls==1);
    next_random=0;
    assert(number_ex(-9,17,file,77)==-9);
    next_random=26;
    assert(number_ex(-9,17,file,77)==17);
    next_random=27;
    assert(number_ex(-9,17,file,77)==-9);
    next_random=41;
    assert(number_ex(17,-9,file,77)==5 && log_calls==1);
    next_random=INT_MAX;
    assert(number_ex(5,5,file,77)==5);
    assert(number_ex(INT_MIN,0,file,77)==-1);
    assert(number_ex(-1,INT_MAX,file,77)==INT_MAX-1);
    assert(number_ex(INT_MIN,INT_MAX-1,file,77)==-1);
    assert(number_ex(INT_MAX-1,INT_MAX,file,77)==INT_MAX);
    assert(number_ex(INT_MIN,INT_MIN+1,file,77)==INT_MIN+1);
    assert(log_calls==1 && random_calls==11);
    puts("numeric regression passed");
    return 0;
}
