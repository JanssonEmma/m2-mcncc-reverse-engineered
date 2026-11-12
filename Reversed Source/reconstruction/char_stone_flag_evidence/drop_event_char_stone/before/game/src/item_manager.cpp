#include "config.h"
#include "../../libthecore/include/main.h"
#include <cassert>
// Old item manager lifecycle, registries and persistence scheduling.
#include "item_manager.h"
#include "item.h"
#include "char.h"
#include "desc_client.h"
#include "../../libthecore/include/log.h"
#include <cstring>

// 08190630/08190a40: m_iTopOfTable is deliberately not initialized here.
ITEM_MANAGER::ITEM_MANAGER() : m_dwVIDCount(0),m_dwCurrentID(0) {}
ITEM_MANAGER::~ITEM_MANAGER() { Destroy(); }
// 08191c50: wipe_second owns VID entries; other pointer maps are not wiped here.
void ITEM_MANAGER::Destroy() {
    for(std::map<DWORD,CItem*>::iterator it=m_VIDMap.begin();it!=m_VIDMap.end();++it) delete it->second;
    m_VIDMap.clear();
}
CItem* ITEM_MANAGER::Find(DWORD id) {
    std::map<DWORD,CItem*>::iterator it=m_map_pkItemByID.find(id);
    return it==m_map_pkItemByID.end() ? 0 : it->second;
}
CItem* ITEM_MANAGER::FindByVID(DWORD vid) {
    std::map<DWORD,CItem*>::iterator it=m_VIDMap.find(vid);
    return it==m_VIDMap.end() ? 0 : it->second;
}
DWORD ITEM_MANAGER::GetRefineFromVnum(DWORD vnum) {
    std::map<DWORD,DWORD>::iterator it=m_map_ItemRefineFrom.find(vnum);
    return it==m_map_ItemRefineFrom.end() ? 0 : it->second;
}
void ITEM_MANAGER::DelayedSave(CItem* item) {
    if(item->GetID()) m_set_pkItemForDelayedSave.insert(item);
}
void ITEM_MANAGER::FlushDelayedSave(CItem* item) {
    std::set<CItem*>::iterator it=m_set_pkItemForDelayedSave.find(item);
    if(it!=m_set_pkItemForDelayedSave.end()) {
        m_set_pkItemForDelayedSave.erase(it);
        SaveSingleItem(item);
    }
}
void ITEM_MANAGER::SaveSingleItem(CItem* item) {
    if(!item->GetOwner()) {
        DWORD id=item->GetID(),owner=item->GetLastOwnerPID();
        db_clientdesc->DBPacketHeader(31,0,8);
        db_clientdesc->Packet(&id,sizeof(id));
        db_clientdesc->Packet(&owner,sizeof(owner));
        sys_log(0,"ITEM_DELETE %s:%lu",item->GetName(),id);
    } else {
        sys_log(0,"ITEM_SAVE %s:%d in %s window %d",item->GetName(),item->GetID(),item->GetOwner()->GetName(),static_cast<unsigned>(item->GetWindow()));
        TPlayerItem packet;
        packet.id=item->GetID(); packet.window=item->GetWindow();
        packet.pos=packet.window==2 ? item->GetCell()-90 : static_cast<WORD>(item->GetCell());
        packet.count=item->GetCount(); packet.vnum=item->GetVnum();
        if(packet.window==3 || packet.window==4) packet.owner=item->GetOwner()->GetDesc()->GetAccountTable().id;
        else packet.owner=item->GetOwner()->GetPlayerID();
        std::memcpy(packet.alSockets,item->GetSockets(),sizeof(packet.alSockets));
        std::memcpy(packet.aAttr,item->GetAttributes(),sizeof(packet.aAttr));
        db_clientdesc->DBPacketHeader(30,0,sizeof(packet));
        db_clientdesc->Packet(&packet,sizeof(packet));
    }
}
void ITEM_MANAGER::Update() {
    std::set<CItem*>::iterator it=m_set_pkItemForDelayedSave.begin();
    while(it!=m_set_pkItemForDelayedSave.end()) {
        std::set<CItem*>::iterator current=it++;
        CItem* item=*current;
        if(!item->GetOwner() || !(item->GetFlag() & 0x10)) {
            SaveSingleItem(item);
            m_set_pkItemForDelayedSave.erase(current);
        }
    }
}
void ITEM_MANAGER::GracefulShutdown() {
    std::set<CItem*>::iterator it=m_set_pkItemForDelayedSave.begin();
    while(it!=m_set_pkItemForDelayedSave.end()) SaveSingleItem(*it++);
    m_set_pkItemForDelayedSave.clear();
}

namespace item_lookup_detail {
inline int add(int a,int b) { return static_cast<int>(static_cast<unsigned int>(a)+static_cast<unsigned int>(b)); }
inline int sub(int a,int b) { return static_cast<int>(static_cast<unsigned int>(a)-static_cast<unsigned int>(b)); }
inline int half_arithmetic(int value) {
    unsigned int bits=static_cast<unsigned int>(value);
    return static_cast<int>((bits>>1)|(bits&0x80000000U));
}
}
//08193080: negative RealNumber -> NULL, otherwise address of full prototype.
TItemTable* ITEM_MANAGER::GetTable(DWORD vnum) {
    int rnum=RealNumber(vnum);
    return rnum<0?0:&m_vec_prototype[rnum];
}
//081930c0: unsigned prototype-vnum compare, signed interval and SAR midpoint.
int ITEM_MANAGER::RealNumber(DWORD vnum) {
    int bottom=0;
    int top=static_cast<int>(m_vec_prototype.size());
    for(;;) {
        int middle=item_lookup_detail::half_arithmetic(item_lookup_detail::add(top,bottom));
        DWORD current=m_vec_prototype[middle].dwVnum;
        if(current==vnum) return middle;
        if(bottom>=top) return -1;
        if(current>vnum) top=item_lookup_detail::sub(middle,1);
        else bottom=item_lookup_detail::add(middle,1);
    }
}

// Add to FULL existing ITEM_MANAGER public API (both nonconst):
// DWORD GetNewID();
// bool SetMaxItemID(DWORD dwID);
// Config ownership: extern DWORD g_dwItemIDRange[2];
// Define ONCE in config.cpp: DWORD g_dwItemIDRange[2]={0,0};
// g_dwItemIDRange[1] is original address083231a8, NOT a second symbol.
DWORD ITEM_MANAGER::GetNewID()
{
    assert(m_dwCurrentID != 0);
    // Original FreeBSD assertion ABI at08192239 has four arguments:
    // __assert("GetNewID","item_manager.cpp",199,"m_dwCurrentID != 0").
    // Standard assert here preserves fatal source semantics; it does not claim
    // identical host-platform diagnostic spelling or original file/line.
    DWORD value=m_dwCurrentID;
    if(value<g_dwItemIDRange[1]) m_dwCurrentID=value+1;
    else {
        _sys_err("GetNewID",203,"FATAL ERROR! ITEM ID Overflow, contact your administrator!");
        thecore_shutdown(); // Existing REAL libthecore implementation.
        value=0;
    }
    return value;
}
bool ITEM_MANAGER::SetMaxItemID(DWORD id)
{
    if(id==0) return false;
    if(g_dwItemIDRange[0]<id && id<g_dwItemIDRange[1]) {
        m_dwCurrentID=id;
        sys_log(0,"ITEM: ID start at %lu",id);
        return true;
    }
    _sys_err("SetMaxItemID",218,"FATAL ERROR! item id out of range! id %lu, range %lu ~ %lu",id,g_dwItemIDRange[0],g_dwItemIDRange[1]);
    thecore_shutdown();
    return false;
}
