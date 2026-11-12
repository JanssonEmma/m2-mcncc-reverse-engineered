#include "../../libthecore/include/utils.h"
// Complete ORIGINAL independently closed bodies. No destructor/__Destroy/Remove/MoveItem substitute.
#include "safebox.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "desc.h"
#include "desc_client.h"
#include "packet.h"
#include "../../libgame/include/grid.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/memcpy.h"
#include <cassert>
#include <cstdint>
#include <cstring>
CSafebox::CSafebox(LPCHARACTER pkChrOwner,int iSize,DWORD dwGold)
{
    m_iSize=iSize;m_pkChrOwner=pkChrOwner;m_lGold=dwGold;
    assert(pkChrOwner && "CSafebox");
    std::memset(m_pkItems,0,sizeof(m_pkItems));
    if(m_iSize==0)m_pkGrid=0;
    else m_pkGrid=new CGrid(5,m_iSize);
    m_bWindowMode=3;
}
void CSafebox::SetWindowMode(BYTE bMode){m_bWindowMode=bMode;}
bool CSafebox::Add(DWORD dwPos,LPITEM item)
{
    if(!IsValidPosition(dwPos)) {
        _sys_err("Add",58,"SAFEBOX: item on wrong position at %d (size of grid = %d)",static_cast<std::int32_t>(dwPos),static_cast<std::int32_t>(m_pkGrid->GetSize()));
        return false;
    }
    item->SetWindow(m_bWindowMode);
    item->SetCell(m_pkChrOwner,static_cast<BYTE>(dwPos));
    item->Save();
    ITEM_MANAGER::instance().FlushDelayedSave(item);
    m_pkGrid->Put(dwPos,1,item->GetSize());
    m_pkItems[dwPos]=item;
    TPacketGCItemSet packet;
    packet.header=m_bWindowMode==3?85:128;
    packet.vnum=item->GetVnum();
    packet.count=static_cast<BYTE>(item->GetCount());
    packet.pos=static_cast<BYTE>(dwPos);
    for (int i = 0; i < 3; ++i)
        packet.alSockets[i] = static_cast<std::int32_t>(item->GetSockets()[i]);
    (*thecore_memcpy)(packet.aAttr,item->GetAttributes(),21);
    m_pkChrOwner->GetDesc()->Packet(&packet,sizeof(packet));
    DWORD count=item->GetCount();
    const char* name=item->GetName();
    const char* ownerName=m_pkChrOwner->GetName();
    sys_log(1,"SAFEBOX: ADD %s %s count %d",ownerName,name,static_cast<std::int32_t>(count));
    return true;
}
LPITEM CSafebox::Get(DWORD dwPos)
{
    unsigned int size=m_pkGrid->GetSize();
    return dwPos<size?m_pkItems[dwPos]:0;
}
void CSafebox::Save()
{
    TSafeboxTable table;std::memset(&table,0,sizeof(table));
    table.dwID=m_pkChrOwner->GetDesc()->GetAccountTable().id;
    table.dwGold=m_lGold;
    db_clientdesc->DBPacket(13,0,&table,sizeof(table));
    sys_log(1,"SAFEBOX: SAVE %s",m_pkChrOwner->GetName());
}
bool CSafebox::IsEmpty(DWORD dwPos,BYTE size)
{
    return m_pkGrid?m_pkGrid->IsEmpty(dwPos,1,size):false;
}
void CSafebox::ChangeSize(int size)
{
    if(m_iSize<size) {
        m_iSize=size;
        CGrid* old=m_pkGrid;
        if(!old)m_pkGrid=new CGrid(5,m_iSize);
        else m_pkGrid=new CGrid(old,5,m_iSize);
        // ORIGINAL leak: do NOT delete old grid; new rows stay uninitialized in original copy ctor.
    }
}
LPITEM CSafebox::GetItem(BYTE cell)
{
    int maximum=static_cast<int>(static_cast<unsigned int>(m_iSize)*5u);
    if(static_cast<int>(cell)<maximum)return m_pkItems[cell];
    _sys_err("GetItem",159,"CHARACTER::GetItem: invalid item cell %d",static_cast<unsigned int>(cell));
    return 0;
}
bool CSafebox::IsValidPosition(DWORD dwPos)
{
    return m_pkGrid && dwPos<m_pkGrid->GetSize();
}

#include <cstring>
namespace safebox_detail {
inline int bits(unsigned int value){int result;std::memcpy(&result,&value,4);return result;}
inline int mul(int a,int b){return bits(static_cast<unsigned int>(a)*static_cast<unsigned int>(b));}
}

CSafebox::~CSafebox(){__Destroy();}
void CSafebox::__Destroy()
{
    for(int i=0;i<135;++i) {
        if(m_pkItems[i]) {
            m_pkItems[i]->SetSkipSave(true);
            ITEM_MANAGER::instance().FlushDelayedSave(m_pkItems[i]);
            LPITEM item=m_pkItems[i]->RemoveFromCharacter();
            ITEM_MANAGER::instance().DestroyItem(item);
            m_pkItems[i]=0;
        }
    }
    if(m_pkGrid){delete m_pkGrid;m_pkGrid=0;}
}
LPITEM CSafebox::Remove(DWORD position)
{
    LPITEM item=Get(position);
    if(!item)return 0;
    if(!m_pkGrid)_sys_err("Remove",100,"Safebox::Remove : nil grid");
    else m_pkGrid->Get(position,1,item->GetSize());
    item->RemoveFromCharacter();
    BYTE mode=m_bWindowMode;
    m_pkItems[position]=0;
    TPacketGCItemDel packet;packet.header=mode==3?86:129;packet.pos=static_cast<BYTE>(position);
    m_pkChrOwner->GetDesc()->Packet(&packet,2);
    DWORD count=item->GetCount();const char* name=item->GetName();
    const char* ownerName=m_pkChrOwner->GetName();
    sys_log(1,"SAFEBOX: REMOVE %s %s count %d",ownerName,name,static_cast<std::int32_t>(count));
    return item;
}
bool CSafebox::MoveItem(BYTE cell,BYTE destination,BYTE count)
{
    int maximum=safebox_detail::mul(m_iSize,5);
    if(maximum<=static_cast<int>(cell) || maximum<=static_cast<int>(destination))return false;
    LPITEM item=GetItem(cell);
    if(!item || item->IsExchanging() || item->GetCount()<static_cast<DWORD>(count))return false;
    LPITEM other=GetItem(destination);
    if(other && item!=other && ((static_cast<unsigned long>(other->GetFlag())>>2)&1u)) {
        if(!(other->GetProto() && (static_cast<WORD>(other->GetProto()->dwAntiFlags)&0x8000))) {
            if(other->GetVnum()==item->GetVnum()) {
                for(int i=0;i<3;++i)if(other->GetSockets()[i]!=item->GetSockets()[i])return false;
                BYTE requested=count?count:static_cast<BYTE>(item->GetCount());
                BYTE moved=static_cast<BYTE>(MIN(safebox_detail::bits(200u-static_cast<unsigned int>(other->GetCount())),static_cast<int>(requested)));
                if(static_cast<DWORD>(moved)<=item->GetCount())Remove(cell);
                item->SetCount(static_cast<std::uint32_t>(item->GetCount()) - moved);
                other->SetCount(static_cast<std::uint32_t>(other->GetCount()) + moved);
                DWORD total=other->GetCount();const char* name=other->GetName();const char* owner=m_pkChrOwner->GetName();
                sys_log(1,"SAFEBOX: STACK %s %d -> %d %s count %d",owner,static_cast<int>(cell),static_cast<int>(destination),name,static_cast<std::int32_t>(total));
                return true;
            }
        }
    }
    if(!IsEmpty(destination,item->GetSize()))return false;
    m_pkGrid->Get(cell,1,item->GetSize());
    if(!m_pkGrid->Put(destination,1,item->GetSize())) {
        m_pkGrid->Put(cell,1,item->GetSize());return false;
    }
    m_pkGrid->Get(destination,1,item->GetSize());
    m_pkGrid->Put(cell,1,item->GetSize());
    DWORD total=item->GetCount();const char* name=item->GetName();const char* owner=m_pkChrOwner->GetName();
    sys_log(1,"SAFEBOX: MOVE %s %d -> %d %s count %d",owner,static_cast<int>(cell),static_cast<int>(destination),name,static_cast<std::int32_t>(total));
    Remove(cell);Add(destination,item);
    return true;
}
