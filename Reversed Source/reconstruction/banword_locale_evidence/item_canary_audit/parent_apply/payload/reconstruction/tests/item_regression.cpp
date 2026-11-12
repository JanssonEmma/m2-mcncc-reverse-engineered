#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/item.h"
#include "../../game/src/char.h"
#include "../../game/src/sectree.h"
#include "../../game/src/desc.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../game/src/packet.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <new>
extern void (*isolated_packet_hook)(const void*,int);
extern void (*isolated_cancel_hook)(LPEVENT*);
extern unsigned isolated_item_packet_logs;
static std::vector<unsigned char> encoded;
static std::vector<LPEVENT*> cancelled;
static void Capture(const void* data,int size) {
    const unsigned char* first=static_cast<const unsigned char*>(data);
    encoded.assign(first,first+size);
}
static void Cancel(LPEVENT* handle) { cancelled.push_back(handle); *handle=0; }
int main() {
    quest::CQuestManager quests;
    isolated_packet_hook=Capture;
    isolated_cancel_hook=Cancel;
    // Placement construction establishes that the constructor leaves item scalars untouched.
    union Storage { long double align; unsigned char bytes[sizeof(CItem)]; } storage;
    // Observable raw-byte seed survives placement-new lifetime DSE under O2/LTO.
    volatile unsigned char* const raw=storage.bytes;
    for(size_t i=0;i<sizeof(storage.bytes);++i) raw[i]=0xa5;
    CItem* item=new(storage.bytes) CItem;
    size_t first=reinterpret_cast<unsigned char*>(&item->m_pProto)-storage.bytes;
    size_t last=reinterpret_cast<unsigned char*>(&item->m_bSkipSave)-storage.bytes;
    const volatile unsigned char* const observed=reinterpret_cast<const volatile unsigned char*>(item);
    for(size_t i=first;i<=last;++i) assert(observed[i]==0xa5);
    assert(item->CEntity::GetType()==-1);
    item->Initialize();
    assert(item->CEntity::GetType()==1 && !item->m_pProto && !item->m_pOwner);
    assert(!item->m_bWindow && !item->m_dwID && !item->m_bEquipped && !item->m_dwVID);
    assert(!item->m_bCell && !item->m_dwCount && !item->m_lFlag && !item->m_dwLastOwnerPID);
    assert(!item->m_bExchanging && !item->m_bSkipSave && !item->m_dwOwnershipPID);
    for(int i=0;i<3;++i) assert(!item->m_alSockets[i]);
    for(int i=0;i<7;++i) assert(!item->m_aAttr[i].bType && !item->m_aAttr[i].sValue);
    assert(!item->m_pkDestroyEvent && !item->m_pkOwnershipEvent && !item->m_pkUniqueExpireEvent && !item->m_pkAccessorySocketExpireEvent);
    TItemTable table; std::memset(&table,0,sizeof(table));
    table.dwVnum=12345; table.dwFlags=0xdeadbeefUL; std::strcpy(table.szLocaleName,"ground item");
    item->SetProto(&table); assert(item->m_pProto==&table && static_cast<DWORD>(item->m_lFlag)==table.dwFlags);
    item->m_dwVID=987; item->SetXYZ(-17,123456,-9);
    CHARACTER recipient;
    item->EncodeInsertPacket(&recipient); item->EncodeRemovePacket(&recipient);
    assert(encoded.empty() && !isolated_item_packet_logs);
    // Only DESC::Packet is captured; no DESC object is dereferenced by this fixture.
    recipient.BindDesc(reinterpret_cast<DESC*>(1));
    item->EncodeInsertPacket(&recipient);
    assert(encoded.size()==sizeof(TPacketGCItemGroundAdd));
    TPacketGCItemGroundAdd added; std::memcpy(&added,&encoded[0],sizeof(added));
    assert(added.bHeader==26 && added.x==-17 && added.y==123456 && added.z==-9 && added.dwVnum==12345 && added.dwVID==987);
    item->EncodeRemovePacket(&recipient);
    assert(encoded.size()==sizeof(TPacketGCItemGroundDel));
    TPacketGCItemGroundDel removed; std::memcpy(&removed,&encoded[0],sizeof(removed));
    assert(removed.bHeader==27 && removed.dwVID==987 && isolated_item_packet_logs==2);
    recipient.BindDesc(0);
    SECTREE sector; sector.InsertEntity(item);
    item->m_pkDestroyEvent=reinterpret_cast<LPEVENT>(1);
    item->m_pkOwnershipEvent=reinterpret_cast<LPEVENT>(2);
    item->m_pkUniqueExpireEvent=reinterpret_cast<LPEVENT>(3);
    item->m_pkAccessorySocketExpireEvent=reinterpret_cast<LPEVENT>(4);
    cancelled.clear(); item->Destroy();
    assert(cancelled.size()==4);
    assert(cancelled[0]==&item->m_pkDestroyEvent && cancelled[1]==&item->m_pkOwnershipEvent && cancelled[2]==&item->m_pkUniqueExpireEvent && cancelled[3]==&item->m_pkAccessorySocketExpireEvent);
    assert(!item->GetSectree() && sector.m_set_entity.empty() && item->m_bIsDestroyed);
    assert(item->m_pProto==&table && item->m_dwVID==987); // Destroy retains item identity.
    cancelled.clear(); item->~CItem(); assert(cancelled.size()==4);
    isolated_cancel_hook=0; isolated_packet_hook=0;
    std::puts("item lifecycle/ground packet regression passed");
}
