/* Standalone TEST ONLY: actual projected wrapper, external OS random observer. */
#include <assert.h>
#include <limits.h>
#include "../../libthecore/include/typedef.h"
extern DWORD thecore_random(void);
static long next_value;
static unsigned calls;
long random(void) { ++calls; return next_value; }
int main(void) {
 const long values[]={0L,1L,-1L,LONG_MIN,LONG_MAX}; unsigned i;
 assert(sizeof(DWORD)==4);
 for(i=0;i<sizeof(values)/sizeof(values[0]);++i) {
  next_value=values[i];assert(thecore_random()==(DWORD)values[i]);assert(calls==i+1);
 }
 return 0;
}
