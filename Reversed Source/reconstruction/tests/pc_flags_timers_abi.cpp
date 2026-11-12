#include <string>
#include <map>
#include <vector>
#define private public
#include "../../game/src/questpc.h"
#undef private
void CheckPCStorageAPI() {
    void (quest::PC::*set)(const std::string&,int,bool)=&quest::PC::SetFlag;
    int (quest::PC::*get)(const std::string&)=&quest::PC::GetFlag;
    bool (quest::PC::*del)(const std::string&)=&quest::PC::DeleteFlag;
    void (quest::PC::*save)(const std::string&,int)=&quest::PC::SaveFlag;
    void (quest::PC::*add)(const std::string&,LPEVENT)=&quest::PC::AddTimer;
    void (quest::PC::*remove)(const std::string&)=&quest::PC::RemoveTimer;
    void (quest::PC::*detach)(const std::string&)=&quest::PC::RemoveTimerNotCancel;
    (void)set;(void)get;(void)del;(void)save;(void)add;(void)remove;(void)detach;
}
typedef char pc_storage_int32[sizeof(int)==4?1:-1];
// No old full PC/STL size assertions.
