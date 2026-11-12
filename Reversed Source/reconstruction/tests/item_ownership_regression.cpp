#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/item.h"
#include "../../game/src/item_manager.h"
#include "../../game/src/char.h"
#include "../../game/src/sectree.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "desc_fixture.h"
#include "../../game/src/packet.h"
#include "../../libthecore/include/main.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>
extern "C" void desc_test_allow_clock(void);
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError() { std::abort(); }
static int Advance(int pulse) { thecore_heart->pulse=pulse; return event_process(pulse); }
static DWORD Read4(const BYTE* bytes) { return static_cast<DWORD>(bytes[0]) | static_cast<DWORD>(bytes[1])<<8 | static_cast<DWORD>(bytes[2])<<16 | static_cast<DWORD>(bytes[3])<<24; }
static void Ownership(LPBUFFER output,DWORD vid,const char* name) {
    assert(buffer_size(output)==30);
    const BYTE* bytes=static_cast<const BYTE*>(buffer_read_peek(output));
    assert(bytes[0]==31 && Read4(bytes+1)==vid);
    if(name) { assert(!std::strncmp(reinterpret_cast<const char*>(bytes+5),name,24) && !bytes[29]); }
    else assert(!bytes[5]); // Remaining clear-name bytes are unspecified in the target.
}
int main() {
    desc_test_allow_clock(); quest::CQuestManager quests; ITEM_MANAGER manager;
    thecore_heart->pulse=100;
    CHARACTER owner,other; owner.m_dwPlayerID=101; other.m_dwPlayerID=202;
    owner.m_stName="abcdefghijklmnopqrstuvwx trailing"; other.m_stName="other";
    LPBUFFER output=buffer_new(0,8192); LPFDWATCH watch=fdwatch_new(16); DescOutputProbe descriptor;
    descriptor.Attach(watch,5,output); descriptor.SetPacketState(5); owner.BindDesc(&descriptor);
    SECTREE sector;
    TItemTable prototype; std::memset(&prototype,0,sizeof(prototype)); std::strcpy(prototype.szLocaleName,"ownership");
    CItem item; item.Initialize(); item.SetProto(&prototype); item.SetVID(0x12345678); item.SetID(9);
    item.SetMapIndex(55); item.SetXYZ(11,22,33); item.SetWindow(5); sector.InsertEntity(&item);
    item.m_map_view[&owner]=0;
    assert(item.IsOwnership(0) && !event_count());
    item.SetOwnership(&owner,2);
    Ownership(output,0x12345678,"abcdefghijklmnopqrstuvwx");
    LPEVENT timer=item.m_pkOwnershipEvent; DWORD id=timer->dwInfoID;
    assert(event_time(timer)==50 && event_count()==1 && !std::strcmp(timer->file,"item.cpp") && timer->line==934);
    assert(*static_cast<CItem**>(event_info(timer))==&item && item.IsOwnership(&owner) && !item.IsOwnership(&other));
    buffer_reset(output); item.SetOwnership(&other,20);
    assert(item.m_pkOwnershipEvent==timer && item.m_dwOwnershipPID==101 && event_time(timer)==50 && !buffer_size(output));
    assert(!Advance(149) && item.m_pkOwnershipEvent && !buffer_size(output));
    assert(Advance(150)==1 && !item.m_pkOwnershipEvent && !FindEventInfo(id) && !event_count());
    Ownership(output,0x12345678,0); assert(item.m_dwOwnershipPID==101 && item.IsOwnership(&other));
    buffer_reset(output); item.SetOwnership(0); assert(!buffer_size(output) && item.m_dwOwnershipPID==101);
    item.SetOwnership(&other,10); Ownership(output,0x12345678,"other");
    id=item.m_pkOwnershipEvent->dwInfoID; buffer_reset(output); item.SetOwnership(0);
    Ownership(output,0x12345678,0);
    assert(!item.m_pkOwnershipEvent && !item.m_dwOwnershipPID && !FindEventInfo(id));
    buffer_reset(output); assert(!Advance(400) && !event_count() && !buffer_size(output)); // Cancelled queue entry never calls back.
    for(int seconds=-1;seconds<=0;++seconds) {
        item.SetOwnership(&owner,seconds); buffer_reset(output);
        assert(event_time(item.m_pkOwnershipEvent)==1); int next=thecore_heart->pulse+1;
        assert(Advance(next)==1 && !item.m_pkOwnershipEvent); Ownership(output,0x12345678,0); buffer_reset(output);
    }
    // The setter only replaces the handle; it does not cancel the queued event or reset PID.
    item.SetOwnership(&owner,1); timer=item.m_pkOwnershipEvent; id=timer->dwInfoID;
    item.SetOwnershipEvent(0); assert(FindEventInfo(id) && item.m_dwOwnershipPID==101);
    event_cancel(&timer); assert(!FindEventInfo(id)); buffer_reset(output);
    assert(!Advance(thecore_heart->pulse+25) && !event_count());
    // Ground removal cancels ownership before sector removal, then cleans reciprocal views and queues persistence.
    owner.m_map_view[&item]=0;
    item.SetOwnership(&owner,10); id=item.m_pkOwnershipEvent->dwInfoID; buffer_reset(output);
    assert(item.RemoveFromGround()==&item);
    assert(buffer_size(output)==35);
    const BYTE* bytes=static_cast<const BYTE*>(buffer_read_peek(output));
    assert(bytes[0]==31 && !bytes[5] && bytes[30]==27 && Read4(bytes+31)==0x12345678);
    assert(!item.GetSectree() && sector.m_set_entity.empty() && item.m_map_view.empty() && owner.m_map_view.empty());
    assert(!item.m_pkOwnershipEvent && !item.m_dwOwnershipPID && !FindEventInfo(id) && manager.m_set_pkItemForDelayedSave.count(&item));
    assert(item.GetWindow()==5 && item.GetMapIndex()==55 && item.GetX()==11 && item.GetY()==22 && item.GetZ()==33);
    manager.m_set_pkItemForDelayedSave.clear(); buffer_reset(output);
    item.RemoveFromGround(); assert(!buffer_size(output) && manager.m_set_pkItemForDelayedSave.empty());
    item.SetOwnership(&owner,10); timer=item.m_pkOwnershipEvent;
    assert(timer && !buffer_size(output)); // No sector means ownership changes have no packet recipients.
    item.RemoveFromGround(); assert(item.m_pkOwnershipEvent==timer);
    item.SetOwnership(0); assert(!item.m_pkOwnershipEvent);
    assert(!Advance(thecore_heart->pulse+250) && !event_count());
    // Real item destruction cancels and frees active ownership event info.
    CItem* doomed=new CItem; doomed->Initialize(); doomed->SetOwnership(&other,10);
    id=doomed->m_pkOwnershipEvent->dwInfoID; delete doomed; assert(!FindEventInfo(id));
    assert(!Advance(thecore_heart->pulse+250) && !event_count());
    owner.BindDesc(0); descriptor.Attach(0,-1,0); buffer_delete(output); fdwatch_delete(watch);
    event_destroy();
    std::puts("item ownership passed: real event expiry/cancellation, stale PID, fanout and ground removal/save order");
}
