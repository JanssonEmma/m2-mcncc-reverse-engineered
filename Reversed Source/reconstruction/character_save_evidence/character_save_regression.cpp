
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/char_manager.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include <cassert>
#include <cstring>
#include <cstdio>
// Reuse established CHARACTER initialization/virtual-lifecycle and clock fixtures.
// Save and DelayedSave must come from actual production/candidate source, never fixtures.
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
static std::vector<unsigned char> Snapshot(const CHARACTER& ch) {
    const unsigned char* begin=reinterpret_cast<const unsigned char*>(&ch);
    return std::vector<unsigned char>(begin,begin+sizeof ch);
}
static void Unchanged(const CHARACTER& ch,const std::vector<unsigned char>& bytes) {
    assert(bytes.size()==sizeof ch && !std::memcmp(&ch,&bytes[0],sizeof ch));
}
int main() {
    desc_test_allow_clock();
    quest::CQuestManager quests;
    CHARACTER first,second;
    assert(!CHARACTER_MANAGER::instance_ptr());
    // Skip must return before even trying to acquire the manager singleton.
    first.m_bSkipSave=true;
    std::vector<unsigned char> snapshot=Snapshot(first);
    first.Save();
    Unchanged(first,snapshot);
    assert(!CHARACTER_MANAGER::instance_ptr());
    {
        CHARACTER_MANAGER manager;
        assert(manager.m_set_pkChrForDelayedSave.empty());
        first.Save();
        assert(manager.m_set_pkChrForDelayedSave.empty());
        Unchanged(first,snapshot);
        // No PID, descriptor, IsPC, or connection filter exists in Save.
        first.m_dwPlayerID=0; first.BindDesc(0); first.m_bSkipSave=false;
        snapshot=Snapshot(first);
        first.Save();
        assert(manager.m_set_pkChrForDelayedSave.size()==1);
        assert(manager.m_set_pkChrForDelayedSave.count(&first)==1);
        Unchanged(first,snapshot);
        for(int i=0;i<5;++i) first.Save();
        assert(manager.m_set_pkChrForDelayedSave.size()==1);
        Unchanged(first,snapshot);
        // Distinct objects with equal PID are distinct pending saves.
        second.m_dwPlayerID=first.m_dwPlayerID;second.m_bSkipSave=false;
        std::vector<unsigned char> second_snapshot=Snapshot(second);
        second.Save();
        assert(manager.m_set_pkChrForDelayedSave.size()==2);
        assert(manager.m_set_pkChrForDelayedSave.count(&first) &&
               manager.m_set_pkChrForDelayedSave.count(&second));
        Unchanged(second,second_snapshot);
        // A later skipped call does not cancel an existing request.
        first.m_bSkipSave=true;snapshot=Snapshot(first);
        first.Save();
        assert(manager.m_set_pkChrForDelayedSave.size()==2);
        assert(manager.m_set_pkChrForDelayedSave.count(&first));
        Unchanged(first,snapshot);
        // Existing manager behavior accepts null and retains other entries.
        manager.DelayedSave(0);manager.DelayedSave(0);
        assert(manager.m_set_pkChrForDelayedSave.size()==3 &&
               manager.m_set_pkChrForDelayedSave.count(0));
        second.Save();
        assert(manager.m_set_pkChrForDelayedSave.size()==3);
        // Erasing here is explicit test cleanup; Save itself does not process/delete.
        manager.m_set_pkChrForDelayedSave.clear();
        first.Save();assert(manager.m_set_pkChrForDelayedSave.empty());
        first.m_bSkipSave=false;snapshot=Snapshot(first);first.Save();
        assert(manager.m_set_pkChrForDelayedSave.size()==1 &&
               manager.m_set_pkChrForDelayedSave.count(&first));
        Unchanged(first,snapshot);
        manager.m_set_pkChrForDelayedSave.clear();
    }
    assert(!CHARACTER_MANAGER::instance_ptr());
    std::puts("CHARACTER::Save passed: skip-before-singleton, actual unique pointer queue, no PC filter, no object mutation or cancellation");
}
