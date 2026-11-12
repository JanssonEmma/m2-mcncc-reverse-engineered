#include <assert.h>
#include <string.h>
#include <stddef.h>
#include "libgame/include/grid.h"
extern "C" { void* (*thecore_memcpy)(void*,const void*,size_t)=memcpy; }
int main() {
 CGrid g(4,3); assert(g.GetSize()==12); assert(g.FindBlank(2,2)==0);
 assert(g.Put(0,2,2)); assert(!g.Put(1,2,1)); assert(g.FindBlank(2,2)==2);
 assert(!g.IsEmpty(-1,1,1)); assert(!g.IsEmpty(3,2,1)); assert(!g.IsEmpty(8,1,2));
 assert(g.FindBlank(5,1)==-1); assert(g.FindBlank(1,4)==-1);
 g.Get(-1,1,1); g.Get(12,1,1); assert(!g.IsEmpty(0,1,1));
 CGrid same(&g,4,3); CGrid shrunk(&g,4,2); CGrid wide(&g,6,2);
 assert(!same.IsEmpty(5,1,1)); assert(!shrunk.IsEmpty(5,1,1));
 assert(!wide.IsEmpty(5,1,1)); assert(wide.IsEmpty(6,1,1)); // linear copy, no row repacking
 CGrid grown(&g,4,4); assert(!grown.IsEmpty(5,1,1)); // uninitialized extension is deliberately unread
 g.Get(0,2,2); assert(g.FindBlank(4,3)==0); assert(!same.IsEmpty(0,1,1));
 same.Clear(); assert(same.FindBlank(4,3)==0);
 assert(g.Put(0,0,1)); assert(!g.IsEmpty(0,1,1)); // original width-zero still touches first cell
 g.Clear(); assert(g.Put(0,1,0)); assert(g.IsEmpty(0,1,1));
 return 0;
}
