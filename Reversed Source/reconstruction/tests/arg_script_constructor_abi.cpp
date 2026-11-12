#include "../../game/src/quest.h"
typedef char ArgScriptUnsigned32[sizeof(unsigned int)==4?1:-1];
typedef char ArgScriptSigned32[sizeof(int)==4?1:-1];
void ArgScriptFieldTypes(){
 std::string quest::AArgScript::*a=&quest::AArgScript::arg;
 std::vector<char> quest::AArgScript::*w=&quest::AArgScript::when_condition;
 quest::AStateScriptType quest::AArgScript::*s=&quest::AArgScript::script;
 unsigned int quest::AArgScript::*q=&quest::AArgScript::quest_index;
 int quest::AArgScript::*st=&quest::AArgScript::state_index;
 (void)a;(void)w;(void)s;(void)q;(void)st;
}
// Original record36bytes/string4/vector12 are evidence only, not current nativeSTL size promises.
