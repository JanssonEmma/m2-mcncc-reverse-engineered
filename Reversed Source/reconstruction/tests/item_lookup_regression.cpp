#include <map>
#include <set>
#include <vector>
#define private public
#define protected public
#include "../../game/src/item_manager.h"
#undef protected
#undef private
#include <cassert>
#include <cstring>
#include <cstdio>
int max_bytes_written,current_bytes_written,total_bytes_written;
static void Fill(ITEM_MANAGER& m,const std::vector<DWORD>& ids) {
 m.m_vec_prototype.clear();
 for(unsigned i=0;i<ids.size();++i){TItemTable row;std::memset(&row,0,sizeof row);row.dwVnum=ids[i];row.bType=static_cast<BYTE>(i+1);m.m_vec_prototype.push_back(row);}
}
static void Check(ITEM_MANAGER& m,DWORD value) {
 int expected=-1;
 for(unsigned i=0;i<m.m_vec_prototype.size();++i)if(m.m_vec_prototype[i].dwVnum==value)expected=static_cast<int>(i);
 const std::vector<TItemTable> before=m.m_vec_prototype;
 assert(m.RealNumber(value)==expected);
 TItemTable* table=m.GetTable(value);
 assert(table==(expected<0?0:&m.m_vec_prototype[expected]));
 assert(m.m_vec_prototype.size()==before.size());
 assert(!std::memcmp(&before[0],&m.m_vec_prototype[0],before.size()*sizeof(TItemTable)));
}
int main(){
 assert(sizeof(DWORD)==4&&sizeof(TItemTable)==154);ITEM_MANAGER manager;
 const unsigned counts[]={3,7,15,31};
 for(unsigned c=0;c<sizeof(counts)/sizeof(counts[0]);++c){
  const unsigned count=counts[c];
  std::vector<DWORD> ids;for(unsigned i=0;i<count;++i)ids.push_back(9+17*i);Fill(manager,ids);
  for(DWORD value=0;value<=ids.back();++value)Check(manager,value);
 }
 const DWORD raw[]={0,0x80000000UL,0xffffffffUL};
 Fill(manager,std::vector<DWORD>(raw,raw+3));
 const DWORD values[]={0,1,0x7ffffffeUL,0x7fffffffUL,0x80000000UL,0x80000001UL,0xfffffffeUL,0xffffffffUL};
 for(unsigned i=0;i<sizeof(values)/sizeof(values[0]);++i)Check(manager,values[i]);
 std::puts("item lookup passed: actual full manager/prototypes, identity, safe-domain matches/misses and unsigned DWORD ordering");
}
