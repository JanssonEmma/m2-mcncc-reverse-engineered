// Prepared bounded actual AArgScript/NPC container regression. No fake application methods.
#include <map>
#include <string>
#include <vector>
#define protected public
#include "../../game/src/questnpc.h"
#undef protected
#include <cassert>
#include <cstdlib>
#include <climits>
#include <cstdio>
static void Seed(quest::AArgScript& s,unsigned int q,int state){
 // Never read/copy indeterminate original scalar defaults: initialize BEFORE any copy.
 s.quest_index=q;s.state_index=state;
 const char arg[]={'a',0,'z'};s.arg.assign(arg,3);
 const char cond[]={'c',0,'d'};s.when_condition.assign(cond,cond+3);
 const char code[]={'s',0,'t'};s.script.m_code.assign(code,code+3);
}
static void Check(const quest::AArgScript& s,unsigned int q,int state){
 assert(s.quest_index==q&&s.state_index==state);
 assert(s.arg.size()==3&&s.arg[0]=='a'&&!s.arg[1]&&s.arg[2]=='z');
 assert(s.when_condition.size()==3&&s.when_condition[0]=='c'&&!s.when_condition[1]&&s.when_condition[2]=='d');
 assert(s.script.m_code.size()==3&&s.script.m_code[0]=='s'&&!s.script.m_code[1]&&s.script.m_code[2]=='t');
}
// Link the existing descriptor test boundary for the expanded NPC dependency graph.
int max_bytes_written, current_bytes_written, total_bytes_written;
void ShutdownOnFatalError() { std::abort(); }

int main(){
 quest::AArgScript first;assert(first.arg.empty()&&first.when_condition.empty()&&first.script.m_code.empty());
 Seed(first,UINT_MAX,INT_MIN);quest::AArgScript second(first);Check(second,UINT_MAX,INT_MIN);
 first.arg[0]='X';first.when_condition[0]='Y';first.script.m_code[0]='Z';Check(second,UINT_MAX,INT_MIN);
 quest::AArgScript assigned;assigned=second;Check(assigned,UINT_MAX,INT_MIN);
 std::vector<quest::AArgScript> many;many.reserve(4);many.push_back(second);many.push_back(assigned);
 for(unsigned i=0;i<many.size();++i)Check(many[i],UINT_MAX,INT_MIN);
 quest::NPC npc;for(unsigned i=0;i<20;++i){
  quest::AArgScript value;assert(value.arg.empty()&&value.when_condition.empty()&&value.script.m_code.empty());
  Seed(value,i,i%2?INT_MAX:INT_MIN);npc.m_mapOwnArgQuest[i][300+i][-1].push_back(value);
 }
 quest::NPC copied(npc);npc.m_mapOwnArgQuest[19].clear();
 for(unsigned i=0;i<20;++i)Check(copied.m_mapOwnArgQuest[i].find(300+i)->second.find(-1)->second[0],i,i%2?INT_MAX:INT_MIN);
 quest::NPC* heap=new quest::NPC(copied);delete heap;
 std::puts("actual AArgScript default STL construction plus initialized-scalar copy/assignment and full NPC populated ownership passed");
 return 0;
}
