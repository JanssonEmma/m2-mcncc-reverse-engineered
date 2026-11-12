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
#include "../../game/src/packet.h"
#include "../../libthecore/include/memcpy.h"
#include <cassert>
#include <cstring>
#include <cstdio>
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
extern "C" int desc_test_inventory_errors(void);
extern void (*desc_test_cancel_event)(LPEVENT*);
static CHARACTER* observed_character;
static CItem* observed_item;
static BYTE observed_cell;
static unsigned observed_packets,cancel_calls;
static void* ObserveCopy(void* destination,const void* source,size_t size) {
    if(observed_item && (size==40 || size==44)) {
        const BYTE* bytes=static_cast<const BYTE*>(source);
        assert((bytes[0]==20 || bytes[0]==21) && bytes[1]==observed_cell);
        assert(observed_character->GetItem(observed_cell)==observed_item);
        assert(!observed_item->GetOwner() && !observed_item->GetCell() && !observed_item->GetWindow());
        ++observed_packets;
    }
    return std::memcpy(destination,source,size);
}
static void OnCancel(LPEVENT* handle) {
    assert(observed_item && handle==&observed_item->m_pkDestroyEvent && *handle==reinterpret_cast<LPEVENT>(7));
    assert(!observed_item->GetOwner() && !observed_character->GetItem(observed_cell));
    assert(observed_item->GetLastOwnerPID()==observed_character->GetPlayerID());
    *handle=0; ++cancel_calls;
}
static DWORD Read4(const BYTE* bytes) { return static_cast<DWORD>(bytes[0]) | static_cast<DWORD>(bytes[1])<<8 | static_cast<DWORD>(bytes[2])<<16 | static_cast<DWORD>(bytes[3])<<24; }
static void ResetOwner(CItem& item) { item.SetCell(0,0); item.SetWindow(0); }
int main() {
    desc_test_allow_clock(); quest::CQuestManager quests; ITEM_MANAGER manager;
    CHARACTER character;
    TItemTable prototype; std::memset(&prototype,0,sizeof(prototype)); prototype.dwVnum=123; prototype.bSize=3;
    CItem item; item.Initialize(); item.SetProto(&prototype);
    TItemTable small=prototype; small.bSize=1;
    CItem other; other.Initialize(); other.SetProto(&small);
    character.SetItem(3,&item);
    assert(character.GetItem(3)==&item && !character.GetItem(8));
    assert(character.m_pointsInstant.bItemGrid[3]==4 && character.m_pointsInstant.bItemGrid[8]==4 && character.m_pointsInstant.bItemGrid[13]==4);
    assert(item.GetOwner()==&character && item.GetCell()==3 && item.GetWindow()==1 && manager.m_set_pkItemForDelayedSave.empty());
    character.SetItem(8,&other); character.SetItem(3,0);
    assert(!character.m_pointsInstant.bItemGrid[3] && !character.m_pointsInstant.bItemGrid[13] && character.m_pointsInstant.bItemGrid[8]==9);
    assert(item.GetOwner()==&character && item.GetCell()==3); // Removing a slot does not clear old item ownership.
    character.SetItem(8,0); ResetOwner(other); ResetOwner(item);
    character.m_pointsInstant.bItemGrid[94]=77;
    character.SetItem(89,&item); assert(character.m_pointsInstant.bItemGrid[89]==90 && character.m_pointsInstant.bItemGrid[94]==77);
    character.SetItem(89,0); ResetOwner(item);
    prototype.bSize=0; character.m_pointsInstant.bItemGrid[20]=55;
    character.SetItem(20,&item); assert(character.m_pointsInstant.bItemGrid[20]==55);
    character.SetItem(20,0); ResetOwner(item);
    character.SetItem(108,&item); assert(character.GetWear(18)==&item && character.m_pointsInstant.bItemGrid[108]==109 && item.GetWindow()==2);
    assert(!character.FindSpecifyItem(123)); // Equipment slots are excluded.
    character.SetItem(108,0); ResetOwner(item);
    character.m_pointsInstant.bItemGrid[30]=42; character.SetItem(30,0); assert(character.m_pointsInstant.bItemGrid[30]==42);
    prototype.bSize=1; character.SetItem(10,&item); character.SetItem(2,&other);
    assert(character.FindSpecifyItem(123)==&other && !character.FindSpecifyItem(999));
    character.SetItem(2,0); character.SetItem(10,0); ResetOwner(item); ResetOwner(other);
    CItem no_prototype; no_prototype.Initialize(); character.SetItem(5,&no_prototype); assert(character.FindSpecifyItem(0)==&no_prototype);
    character.SetItem(5,0); ResetOwner(no_prototype);
    int errors=desc_test_inventory_errors();
    assert(!character.GetItem(109) && !character.GetItem(255) && !character.GetWear(19) && !character.GetWear(255));
    character.SetItem(109,0); character.SetItem(255,0); assert(desc_test_inventory_errors()==errors+6);
    LPFDWATCH watch=fdwatch_new(16); LPBUFFER output=buffer_new(0,8192); DescOutputProbe descriptor;
    descriptor.Attach(watch,5,output); descriptor.SetPacketState(5); character.BindDesc(&descriptor); character.m_dwPlayerID=456;
    item.m_dwCount=0x1234; item.m_alSockets[0]=-7; item.m_alSockets[1]=8; item.m_alSockets[2]=9;
    for(int i=0;i<7;++i) { item.m_aAttr[i].bType=static_cast<BYTE>(i+1); item.m_aAttr[i].sValue=static_cast<short>(-100-i); }
    observed_character=&character; observed_item=&item; observed_cell=12; thecore_memcpy=ObserveCopy;
    const long flags[]={0x3fff,0x4000,0x7fffffff,-1};
    for(unsigned i=0;i<4;++i) {
        item.m_lFlag=flags[i]; character.SetItem(12,&item);
        bool extended=flags[i]>=0x4000; assert(buffer_size(output)==(extended?44:40));
        const BYTE* bytes=static_cast<const BYTE*>(buffer_read_peek(output));
        assert(bytes[0]==(extended?21:20) && bytes[1]==12 && Read4(bytes+2)==123 && bytes[6]==0x34);
        if(extended) assert(Read4(bytes+7)==static_cast<DWORD>(flags[i]));
        int sockets=extended?11:7; assert(Read4(bytes+sockets)==0xfffffff9UL);
        assert(!std::memcmp(bytes+sockets+12,item.m_aAttr,21));
        assert(item.GetOwner()==&character && item.GetCell()==12 && item.GetWindow()==1);
        observed_item=0; buffer_reset(output); character.SetItem(12,0);
        assert(buffer_size(output)==40); bytes=static_cast<const BYTE*>(buffer_read_peek(output));
        assert(bytes[0]==20 && bytes[1]==12); for(int k=2;k<40;++k) assert(!bytes[k]);
        buffer_reset(output); ResetOwner(item); observed_item=&item;
    }
    assert(observed_packets==4);
    item.SetID(900); item.m_pkDestroyEvent=reinterpret_cast<LPEVENT>(7); observed_cell=22;
    desc_test_cancel_event=OnCancel;
    assert(item.AddToCharacter(&character,22));
    assert(cancel_calls==1 && observed_packets==5 && !item.m_pkDestroyEvent && item.GetLastOwnerPID()==456);
    assert(character.GetItem(22)==&item && item.GetOwner()==&character && manager.m_set_pkItemForDelayedSave.count(&item));
    observed_item=0; character.SetItem(22,0); ResetOwner(item); buffer_reset(output);
    item.m_bCell=90; assert(!item.AddToCharacter(&character,4) && !character.GetItem(4) && !item.GetOwner());
    item.m_bCell=0;
    // The original checks current cell; invalid argument logs in SetItem but AddToCharacter still assigns owner/enqueues.
    errors=desc_test_inventory_errors(); assert(item.AddToCharacter(&character,109));
    assert(desc_test_inventory_errors()==errors+1 && item.GetOwner()==&character && !item.GetCell() && !item.GetWindow());
    assert(!buffer_size(output)); ResetOwner(item);
    character.BindDesc(0); item.m_dwLastOwnerPID=77; assert(item.AddToCharacter(&character,4));
    assert(item.GetLastOwnerPID()==77); character.SetItem(4,0); ResetOwner(item);
    manager.m_set_pkItemForDelayedSave.clear(); desc_test_cancel_event=0; thecore_memcpy=std::memcpy;
    descriptor.Attach(0,-1,0); buffer_delete(output); fdwatch_delete(watch);
    std::puts("character inventory passed: grid footprints, signed packet threshold, ownership order and old transfer guards");
}
