#include <assert.h>
#include <string.h>
static long draws[] = { 27, 99, 14 };
static unsigned draw_count;
static int diagnostics;
long random(void)
{
    assert(draw_count < sizeof draws / sizeof draws[0]);
    return draws[draw_count++];
}
void _sys_err(const char* function, int line, const char* format, ...)
{
    assert(!strcmp(function, "number_ex") && line == 339);
    assert(!strcmp(format, "number(): first argument is bigger than second argument %d -> %d, %s %d"));
    ++diagnostics;
}
int number_ex(int, int, char*, int);
int main(void)
{
    char file[] = "char.cpp";
    assert(number_ex(0, 13, file, 5962) == 13);
    assert(number_ex(1, 100, file, 5963) == 100);
    assert(number_ex(13, 0, file, 5962) == 0);
    assert(draw_count == 3 && diagnostics == 1);
    return 0;
}
