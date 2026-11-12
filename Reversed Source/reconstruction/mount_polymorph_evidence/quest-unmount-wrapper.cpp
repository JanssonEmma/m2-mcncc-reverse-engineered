// Complete missing wrappers only. Actual event/VM dependencies remain unresolved.
#include "questmanager.h"
#include "../../libthecore/include/log.h"
namespace quest {
//081a4540, old signature unsigned int rather than DWORD.
void CQuestManager::Unmount(unsigned int pid) {
    PC* pc=GetPC(pid);
    if(!pc) {
        _sys_err("Unmount",0x308,"QUEST no such pc id : %d",pid);
        return;
    }
    if(!pc->IsLoaded()) return;
    m_mapNPC[0].OnUnmount(*pc);
}
//081b1a00. Last valid slot of original20-event NPC.
bool NPC::OnUnmount(PC& pc) { return HandleReceiveAllEvent(pc,19); }
}
