#include <assert.h>
#include <string.h>
#include <stddef.h>
#include "libgame/include/attribute.h"
extern "C" { void* (*thecore_memcpy)(void*,const void*,size_t)=memcpy; }
static void check(DWORD* values,int type) {
 CAttribute a(values,2,2); DWORD row[2]; assert(a.GetDataType()==type);
 assert(a.GetDataPtr()!=0); a.CopyRow(1,row); assert(row[0]==values[2] && row[1]==values[3]);
 a.Set(0,0,0x10001); DWORD mask=type==D_BYTE?0xff:type==D_WORD?0xffff:0xffffffffUL;
 assert(a.Get(0,0)==((values[0]|0x10001)&mask));
 a.Remove(0,0,1); assert(a.Get(0,0)==((values[0]|0x10001)&mask&~1UL));
 a.Set(3,0,1); a.Remove(0,3,1); assert(a.Get(3,0)==0 && a.Get(0,3)==0);
}
int main() {
 typedef char target_DWORD_width[(sizeof(DWORD)==4) ? 1 : -1];
 (void)sizeof(target_DWORD_width);
 DWORD b[]={1,2,3,255}, w[]={1,256,65535,2}, d[]={1,65536,0x80000000UL,2};
 check(b,D_BYTE); check(w,D_WORD); check(d,D_DWORD);
 DWORD u[]={7,7,7,7}; CAttribute uniform(u,2,2); DWORD row[2];
 assert(uniform.GetDataType()==D_DWORD && uniform.GetDataPtr()==0);
 uniform.Remove(0,0,7); assert(uniform.Get(0,0)==7 && uniform.GetDataPtr()==0);
 assert(uniform.Get(2,2)==7 && uniform.Get(-1,-1)==7); // signed strict-greater bound, safe without storage
 uniform.CopyRow(99,row); assert(row[0]==7 && row[1]==7);
 uniform.Set(1,1,8); assert(uniform.GetDataPtr()!=0 && uniform.Get(1,1)==15 && uniform.Get(0,0)==7);
 CAttribute zero(2,2); assert(zero.GetDataType()==D_DWORD && zero.GetDataPtr()!=0);
 assert(zero.Get(0,0)==0); zero.Set(0,0,0x80000001UL); zero.Remove(0,0,1); assert(zero.Get(0,0)==0x80000000UL);
 return 0;
}
