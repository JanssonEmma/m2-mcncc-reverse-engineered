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
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "desc_fixture.h"
#include "../../game/src/desc_client.h"
#include "../../game/src/packet.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <new>
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
class ClientProbe : public CLIENT_DESC {
public: void Output(LPBUFFER buffer) { m_lpOutputBuffer=buffer; }
};
static DWORD Read4(const BYTE* bytes) {
    return static_cast<DWORD>(bytes[0]) | static_cast<DWORD>(bytes[1])<<8 | static_cast<DWORD>(bytes[2])<<16 | static_cast<DWORD>(bytes[3])<<24;
}
static const BYTE* Frame(LPBUFFER buffer,BYTE header,DWORD size) {
    assert(buffer_size(buffer)==size+9);
    const BYTE* bytes=static_cast<const BYTE*>(buffer_read_peek(buffer));
    assert(bytes[0]==header && Read4(bytes+1)==0 && Read4(bytes+5)==size);
    return bytes+9;
}
static TPlayerItem Saved(LPBUFFER buffer) {
    TPlayerItem result; const BYTE* bytes=Frame(buffer,30,52);
    std::memcpy(&result,bytes,sizeof(result)); return result;
}
static unsigned deleted;
class OwnedItem : public CItem {
public:
    virtual ~OwnedItem() { ++deleted; }
};
int main() {
    desc_test_allow_clock();
    quest::CQuestManager quests;
    CItem item; item.Initialize();
    item.SetSkipSave(true); item.Save(); // No singleton lookup on the skip path.
    item.SetSkipSave(false);
    union Storage { long double align; unsigned char bytes[sizeof(ITEM_MANAGER)]; } storage;
    std::memset(storage.bytes,0xa5,sizeof(storage.bytes));
    ITEM_MANAGER* manager=new(storage.bytes) ITEM_MANAGER;
    assert(ITEM_MANAGER::instance_ptr()==manager && !manager->m_dwVIDCount && !manager->m_dwCurrentID);
    const BYTE* top=reinterpret_cast<const BYTE*>(&manager->m_iTopOfTable);
    for(unsigned i=0;i<sizeof(int);++i) assert(top[i]==0xa5);
    assert(manager->m_VIDMap.empty() && manager->m_map_pkItemByID.empty() && manager->m_vec_prototype.empty());
    assert(!manager->Find(7) && !manager->FindByVID(8) && !manager->GetRefineFromVnum(9));
    assert(manager->m_map_pkItemByID.empty() && manager->m_VIDMap.empty() && manager->m_map_ItemRefineFrom.empty());
    manager->m_map_pkItemByID[7]=&item; manager->m_VIDMap[8]=&item; manager->m_map_ItemRefineFrom[9]=10;
    assert(manager->Find(7)==&item && manager->FindByVID(8)==&item && manager->GetRefineFromVnum(9)==10);
    manager->m_VIDMap.clear(); manager->m_map_pkItemByID.clear();
    item.Save(); assert(manager->m_set_pkItemForDelayedSave.empty());
    item.SetID(123); item.Save(); item.Save();
    assert(manager->m_set_pkItemForDelayedSave.size()==1);
    item.SetSkipSave(true); item.SetID(124); item.Save();
    assert(manager->m_set_pkItemForDelayedSave.size()==1); // Skip does not remove prior membership.
    item.SetID(123); item.SetSkipSave(false);
    TItemTable prototype; std::memset(&prototype,0,sizeof(prototype));
    prototype.dwVnum=27001; prototype.dwFlags=16; std::strcpy(prototype.szLocaleName,"saved item");
    item.SetProto(&prototype); item.m_dwLastOwnerPID=0x10203040;
    ClientProbe database; LPBUFFER dboutput=buffer_new(0,8192); database.Output(dboutput); db_clientdesc=&database;
    manager->FlushDelayedSave(&item);
    const BYTE* removed=Frame(dboutput,31,8);
    assert(Read4(removed)==123 && Read4(removed+4)==0x10203040 && manager->m_set_pkItemForDelayedSave.empty());
    buffer_reset(dboutput); manager->FlushDelayedSave(&item); assert(!buffer_size(dboutput));
    CHARACTER character; character.m_dwPlayerID=0x55667788;
    item.m_pOwner=&character; item.m_bCell=107; item.m_dwCount=0x1234;
    item.m_alSockets[0]=-7; item.m_alSockets[1]=0x12345678; item.m_alSockets[2]=99;
    for(int i=0;i<7;++i) { item.m_aAttr[i].bType=static_cast<BYTE>(i+1); item.m_aAttr[i].sValue=static_cast<short>(-20-i); }
    item.SetWindow(2); manager->SaveSingleItem(&item);
    TPlayerItem saved=Saved(dboutput);
    assert(saved.id==123 && saved.window==2 && saved.pos==17 && saved.count==0x1234 && saved.vnum==27001 && saved.owner==0x55667788);
    assert(!std::memcmp(saved.alSockets,item.m_alSockets,sizeof(saved.alSockets)) && !std::memcmp(saved.aAttr,item.m_aAttr,sizeof(saved.aAttr)));
    // Database save preserves DWORD count, while UpdatePacket narrows it to BYTE.
    LPBUFFER visible=buffer_new(0,128); LPFDWATCH watch=fdwatch_new(16); DescOutputProbe recipient;
    recipient.Attach(watch,5,visible); recipient.SetPacketState(5); recipient.GetAccountTable().id=0xaabbccdd;
    character.BindDesc(&recipient); item.UpdatePacket();
    assert(buffer_size(visible)==36);
    const BYTE* shown=static_cast<const BYTE*>(buffer_read_peek(visible));
    assert(shown[0]==25 && shown[1]==107 && shown[2]==0x34 && Read4(shown+3)==0xfffffff9UL);
    assert(!std::memcmp(shown+15,item.m_aAttr,21));
    // Bulk socket replacement changes persisted data but sends no visible update.
    for(int i=0;i<6;++i) prototype.alValues[i]=i%2 ? -1234567-i : 1234567+i;
    for(DWORD i=0;i<6;++i) assert(item.GetValue(i)==prototype.alValues[i]);
    long sockets[3]={-2147483647L-1,0,2147483647L};
    buffer_reset(visible); buffer_reset(dboutput);
    item.SetSkipSave(true); item.SetSockets(sockets);
    assert(!std::memcmp(item.GetSockets(),sockets,sizeof(sockets)));
    assert(manager->m_set_pkItemForDelayedSave.empty() && !buffer_size(visible) && !buffer_size(dboutput));
    item.SetSkipSave(false); sockets[1]=-999;
    item.SetSockets(sockets); item.SetSockets(sockets);
    assert(manager->m_set_pkItemForDelayedSave.size()==1 && !buffer_size(visible) && !buffer_size(dboutput));
    sockets[1]=88; assert(item.GetSockets()[1]==-999); // The input buffer is copied.
    manager->FlushDelayedSave(&item); saved=Saved(dboutput);
    assert(saved.alSockets[0]==(-2147483647L-1) && saved.alSockets[1]==-999 && saved.alSockets[2]==2147483647L);
    assert(manager->m_set_pkItemForDelayedSave.empty() && !buffer_size(visible));
    for(int window=1;window<=5;++window) {
        buffer_reset(dboutput); item.SetWindow(static_cast<BYTE>(window)); manager->SaveSingleItem(&item); saved=Saved(dboutput);
        assert(saved.pos==(window==2?17:107));
        assert(saved.owner==(window==3 || window==4 ? 0xaabbccddUL : 0x55667788UL));
    }
    buffer_reset(dboutput); item.SetSkipSave(true); manager->SaveSingleItem(&item); assert(buffer_size(dboutput)==61);
    item.SetSkipSave(false);
    buffer_reset(dboutput); item.Save(); manager->Update();
    assert(!buffer_size(dboutput) && manager->m_set_pkItemForDelayedSave.count(&item)); // Owned flag16 defers ordinary update.
    manager->FlushDelayedSave(&item); assert(buffer_size(dboutput)==61 && manager->m_set_pkItemForDelayedSave.empty());
    buffer_reset(dboutput); item.Save(); item.m_lFlag=0; manager->Update();
    assert(buffer_size(dboutput)==61 && manager->m_set_pkItemForDelayedSave.empty());
    buffer_reset(dboutput); item.m_lFlag=16; item.m_pOwner=0; item.Save(); manager->Update();
    Frame(dboutput,31,8); assert(manager->m_set_pkItemForDelayedSave.empty());
    buffer_reset(dboutput); item.m_pOwner=&character; item.Save(); manager->GracefulShutdown();
    assert(buffer_size(dboutput)==61 && manager->m_set_pkItemForDelayedSave.empty());
    character.BindDesc(0); recipient.Attach(0,-1,0); buffer_delete(visible); fdwatch_delete(watch);
    database.Output(0); db_clientdesc=0; buffer_delete(dboutput);
    OwnedItem* owned=new OwnedItem; owned->Initialize(); owned->SetID(900);
    manager->m_VIDMap[3]=owned; manager->m_map_pkItemByID[900]=owned; manager->DelayedSave(owned);
    manager->Destroy(); assert(deleted==1 && manager->m_VIDMap.empty());
    assert(manager->m_map_pkItemByID.count(900) && manager->m_set_pkItemForDelayedSave.count(owned)); // Old Destroy retains other registries.
    manager->m_map_pkItemByID.clear(); manager->m_set_pkItemForDelayedSave.clear();
    owned=new OwnedItem; owned->Initialize(); manager->m_VIDMap[4]=owned;
    manager->~ITEM_MANAGER(); assert(deleted==2 && !ITEM_MANAGER::instance_ptr());
    std::puts("item persistence passed: real DB framing, owner/account windows, deferred saves, packets and VID ownership");
}
