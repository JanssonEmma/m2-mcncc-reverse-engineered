#include "utils.h"
#include "db.h"
#include "desc_client.h"
#include "item_manager.h"
#include "locale.hpp"
#include "log.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/main.h"
#include <cassert>
#include <cstdio>
#include "exchange.h"
#include "char.h"
#include "desc.h"
#include "item.h"
#include "packet.h"
#include "../../libgame/include/grid.h"
#include "../../libthecore/include/memcpy.h"
#include <cstring>

void exchange_packet(CHARACTER* ch, BYTE subheader, bool own, DWORD a, DWORD b, DWORD c, void* data)
{
    DESC* desc=ch->GetDesc();
    if (!desc) return;
    TPacketGCExchange packet{};
    packet.header=42;
    packet.sub_header=subheader;
    packet.is_me=own;
    packet.arg1=static_cast<std::uint32_t>(a);
    packet.arg2=static_cast<std::uint32_t>(b);
    packet.arg3=static_cast<std::uint32_t>(c);
    if (subheader==1 && data) {
        CItem* item=static_cast<CItem*>(data);
        const long* sockets = item->GetSockets();
        if (sizeof(long) == sizeof(std::int32_t)) {
            thecore_memcpy(packet.alSockets, sockets, sizeof(packet.alSockets));
        } else {
            const std::int32_t packedSockets[] = {
                static_cast<std::int32_t>(sockets[0]),
                static_cast<std::int32_t>(sockets[1]),
                static_cast<std::int32_t>(sockets[2]),
            };
            thecore_memcpy(packet.alSockets, packedSockets, sizeof(packet.alSockets));
        }
        thecore_memcpy(packet.aAttr,item->GetAttributes(),sizeof(packet.aAttr));
        desc=ch->GetDesc();
    }
    desc->Packet(&packet,sizeof(packet));
}

CExchange::CExchange(CHARACTER* owner)
{
    m_pCompany=0;
    m_bAccept=false;
    for (int slot=0;slot<12;++slot) {
        m_abItemPos[slot]=0;
        m_apItems[slot]=0;
        m_abItemDisplayPos[slot]=0;
    }
    m_pOwner=owner;
    m_lGold=0;
    owner->SetExchange(this);
    m_pGrid=new CGrid(4,3);
}

CExchange::~CExchange()
{
    delete m_pGrid;
}

void CExchange::Cancel()
{
    exchange_packet(m_pOwner,5,false,0,0,0,0);
    m_pOwner->SetExchange(0);
    for (int slot=0;slot<12;++slot)
        if (m_apItems[slot]) m_apItems[slot]->SetExchanging(false);
    if (m_pCompany) {
        m_pCompany->m_pCompany=0;
        m_pCompany->Cancel();
    }
    delete this;
}

bool CExchange::AddGold(long gold)
{
    if (gold<=0) return false;
    if (m_pOwner->GetGold()<gold) {
        exchange_packet(m_pOwner,7,false,0,0,0,0);
        return false;
    }
    Accept(false);
    m_pCompany->Accept(false);
    m_lGold=gold;
    exchange_packet(m_pOwner,3,true,m_lGold,0,0,0);
    exchange_packet(m_pCompany->m_pOwner,3,false,m_lGold,0,0,0);
    return true;
}

bool CExchange::RemoveItem(BYTE slot)
{
    if (slot>=12 || !m_apItems[slot]) return false;
    BYTE cell=m_abItemPos[slot];
    m_apItems[slot]->SetExchanging(false);
    m_pGrid->Get(m_abItemDisplayPos[slot],1,m_apItems[slot]->GetSize());
    exchange_packet(m_pOwner,2,true,slot,cell,0,0);
    exchange_packet(m_pCompany->m_pOwner,2,false,slot,cell,0,0);
    Accept(false);
    m_pCompany->Accept(false);
    m_abItemPos[slot]=0;
    m_apItems[slot]=0;
    m_abItemDisplayPos[slot]=0;
    return true;
}

bool CExchange::Check(int* count)
{
    if (m_pOwner->GetGold()<m_lGold) return false;
    int items=0;
    for (int slot=0;slot<12;++slot) {
        if (!m_apItems[slot]) continue;
        BYTE cell=m_abItemPos[slot];
        if (cell>=90 || m_pOwner->GetItem(cell)!=m_apItems[slot]) return false;
        ++items;
    }
    *count=items;
    return true;
}

bool CExchange::AddItem(BYTE cell, BYTE display)
{
    assert(m_pOwner && m_pCompany);
    if (cell>=90) return false;
    CItem* item=m_pOwner->GetItem(cell);
    if (!item) return false;
    if (item->GetProto() && (item->GetProto()->dwAntiFlags&0x2000)) {
        m_pOwner->ChatPacket(1,locale_find("\276\306\300\314\305\333\300\273\040\260\307\263\327\301\331\040\274\366\040\276\370\275\300\264\317\264\331\056",g_iUseLocale));
        return false;
    }
    if (item->IsExchanging()) {
        sys_log(0,"EXCHANGE under exchanging");
        return false;
    }
    if (!m_pGrid->IsEmpty(display,1,item->GetSize())) {
        sys_log(0,"EXCHANGE not empty item_pos %d %d %d",display,1,item->GetSize());
        return false;
    }
    Accept(false);
    m_pCompany->Accept(false);
    for (int slot=0;slot<12;++slot) {
        if (m_apItems[slot]) continue;
        m_apItems[slot]=item;
        m_abItemPos[slot]=cell;
        m_abItemDisplayPos[slot]=display;
        m_pGrid->Put(display,1,item->GetSize());
        item->SetExchanging(true);
        exchange_packet(m_pOwner,1,true,item->GetVnum(),display,item->GetCount(),item);
        exchange_packet(m_pCompany->m_pOwner,1,false,item->GetVnum(),display,item->GetCount(),item);
        sys_log(0,"EXCHANGE AddItem success %s %d %d",item->GetName(),cell,display);
        return true;
    }
    return false;
}

bool CExchange::CheckSpace()
{
    static CGrid grid(5,18);
    grid.Clear();
    CHARACTER* recipient=m_pCompany->m_pOwner;
    for (int cell=0;cell<90;++cell) {
        CItem* item=recipient->GetItem(cell);
        if (item) grid.Put(cell,1,item->GetSize());
    }
    for (int slot=0;slot<12;++slot) {
        CItem* item=m_apItems[slot];
        if (!item) continue;
        int cell=grid.FindBlank(1,item->GetSize());
        if (cell<0) return false;
        grid.Put(cell,1,item->GetSize());
    }
    return true;
}

bool CExchange::Done()
{
    CHARACTER* recipient=m_pCompany->m_pOwner;
    char hint[51];
    for (int slot=0;slot<12;++slot) {
        CItem* item=m_apItems[slot];
        if (!item) continue;
        int cell=recipient->GetEmptyInventory(item->GetSize());
        if (cell<0) {
            _sys_err("Done",0x14b,"Exchange::Done : Cannot find blank position in inventory %s <-> %s item %s",
                     m_pOwner->GetName(),recipient->GetName(),item->GetName());
            continue;
        }
        if (item->GetVnum()==90008 || item->GetVnum()==90009) {
            VCardUse(m_pOwner,recipient,item);
            continue;
        }
        m_pOwner->SyncQuickslot(1,item->GetCell(),255);
        item->RemoveFromCharacter();
        item->AddToCharacter(recipient,static_cast<BYTE>(cell));
        ITEM_MANAGER::instance().FlushDelayedSave(item);
        item->SetExchanging(false);

        std::snprintf(hint,sizeof(hint),"%s %lu %lu",item->GetName(),m_pOwner->GetPlayerID(),item->GetCount());
        LogManager::instance().ItemLog(recipient->GetPlayerID(),recipient->GetX(),recipient->GetY(),
                                       item->GetID(),"EXCHANGE_TAKE",hint);
        std::snprintf(hint,sizeof(hint),"%s %lu %lu",item->GetName(),recipient->GetPlayerID(),item->GetCount());
        LogManager::instance().ItemLog(m_pOwner->GetPlayerID(),m_pOwner->GetX(),m_pOwner->GetY(),
                                       item->GetID(),"EXCHANGE_GIVE",hint);
        if (item->GetVnum()>80002 && item->GetVnum()<80008) {
            LogManager::instance().GoldBarLog(recipient->GetPlayerID(),item->GetID(),EXCHANGE_TAKE,"");
            LogManager::instance().GoldBarLog(m_pOwner->GetPlayerID(),item->GetID(),EXCHANGE_GIVE,"");
        }
        m_apItems[slot]=0;
    }
    if (m_lGold) {
        m_pOwner->PointChange(11,-m_lGold,true,false);
        recipient->PointChange(11,m_lGold,true,false);
        if (m_lGold>1000) {
            std::snprintf(hint,sizeof(hint),"%lu %s",m_pOwner->GetPlayerID(),m_pOwner->GetName());
            LogManager::instance().CharLog(recipient->GetPlayerID(),recipient->GetX(),recipient->GetY(),
                                           m_lGold,"EXCHANGE_GOLD_TAKE",hint);
            std::snprintf(hint,sizeof(hint),"%lu %s",recipient->GetPlayerID(),recipient->GetName());
            LogManager::instance().CharLog(m_pOwner->GetPlayerID(),m_pOwner->GetX(),m_pOwner->GetY(),
                                           m_lGold,"EXCHANGE_GOLD_GIVE",hint);
        }
    }
    m_pGrid->Clear();
    return true;
}

bool CExchange::Accept(bool accept)
{
    if (m_bAccept==accept) return true;
    m_bAccept=accept;
    if (!accept || !m_pCompany->m_bAccept) {
        exchange_packet(m_pOwner,4,true,m_bAccept,0,0,0);
        exchange_packet(m_pCompany->m_pOwner,4,false,m_bAccept,0,0,0);
        return true;
    }

    CHARACTER* owner=m_pOwner;
    CHARACTER* recipient=m_pCompany->m_pOwner;
    owner->SetExchangeTime();
    recipient->SetExchangeTime();
    int count;
    if (!Check(&count)) {
        owner->ChatPacket(1,locale_find("\265\267\300\314\040\272\316\301\267\307\317\260\305\263\252\040\276\306\300\314\305\333\300\314\040\301\246\300\332\270\256\277\241\040\276\370\275\300\264\317\264\331\056",g_iUseLocale));
        recipient->ChatPacket(1,locale_find("\273\363\264\353\271\346\300\307\040\265\267\300\314\040\272\316\301\267\307\317\260\305\263\252\040\276\306\300\314\305\333\300\314\040\301\246\300\332\270\256\277\241\040\276\370\275\300\264\317\264\331\056",g_iUseLocale));
    } else if (!CheckSpace()) {
        owner->ChatPacket(1,locale_find("\273\363\264\353\271\346\300\307\040\274\322\301\366\307\260\277\241\040\272\363\040\260\370\260\243\300\314\040\276\370\275\300\264\317\264\331\056",g_iUseLocale));
        recipient->ChatPacket(1,locale_find("\274\322\301\366\307\260\277\241\040\272\363\040\260\370\260\243\300\314\040\276\370\275\300\264\317\264\331\056",g_iUseLocale));
    } else if (!m_pCompany->Check(&count)) {
        recipient->ChatPacket(1,locale_find("\265\267\300\314\040\272\316\301\267\307\317\260\305\263\252\040\276\306\300\314\305\333\300\314\040\301\246\300\332\270\256\277\241\040\276\370\275\300\264\317\264\331\056",g_iUseLocale));
        owner->ChatPacket(1,locale_find("\273\363\264\353\271\346\300\307\040\265\267\300\314\040\272\316\301\267\307\317\260\305\263\252\040\276\306\300\314\305\333\300\314\040\301\246\300\332\270\256\277\241\040\276\370\275\300\264\317\264\331\056",g_iUseLocale));
    } else if (!m_pCompany->CheckSpace()) {
        recipient->ChatPacket(1,locale_find("\273\363\264\353\271\346\300\307\040\274\322\301\366\307\260\277\241\040\272\363\040\260\370\260\243\300\314\040\276\370\275\300\264\317\264\331\056",g_iUseLocale));
        owner->ChatPacket(1,locale_find("\274\322\301\366\307\260\277\241\040\272\363\040\260\370\260\243\300\314\040\276\370\275\300\264\317\264\331\056",g_iUseLocale));
    } else if (db_clientdesc->GetSocket()==-1) {
        _sys_err("Accept",0x1bd,"Cannot use exchange feature while DB cache connection is dead.");
        recipient->ChatPacket(1,"Unknown error");
        owner->ChatPacket(1,"Unknown error");
    } else if (Done()) {
        if (m_lGold) m_pOwner->Save();
        if (m_pCompany->Done()) {
            if (m_pCompany->m_lGold) recipient->Save();
            m_pOwner->ChatPacket(1,locale_find("\045\163\040\264\324\260\372\300\307\040\261\263\310\257\300\314\040\274\272\273\347\040\265\307\276\372\275\300\264\317\264\331\056",g_iUseLocale),recipient->GetName());
            recipient->ChatPacket(1,locale_find("\045\163\040\264\324\260\372\300\307\040\261\263\310\257\300\314\040\274\272\273\347\040\265\307\276\372\275\300\264\317\264\331\056",g_iUseLocale),m_pOwner->GetName());
        }
    }
    Cancel();
    return false;
}

bool CHARACTER::ExchangeStart(CHARACTER* victim)
{
    if (this==victim) return false;
    if (m_bIsObserver) {
        ChatPacket(1,locale_find("\260\374\300\374\040\273\363\305\302\277\241\274\255\264\302\040\261\263\310\257\300\273\040\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056",g_iUseLocale));
        return false;
    }
    if (victim->m_bCharType!=6) return false;
    if (m_isOpenSafebox || m_pkChrShopOwner || m_pkMyShop || m_pointsInstant.pCubeNpc) {
        ChatPacket(1,locale_find("\264\331\270\245\040\260\305\267\241\303\242\300\314\040\277\255\267\301\300\326\300\273\260\346\277\354\040\260\305\267\241\270\246\040\307\322\274\366\040\276\370\275\300\264\317\264\331\056",g_iUseLocale));
        return false;
    }
    if (victim->m_isOpenSafebox || victim->m_pkChrShopOwner || victim->m_pkMyShop ||
        victim->m_pointsInstant.pCubeNpc) {
        ChatPacket(1,locale_find("\273\363\264\353\271\346\300\314\040\264\331\270\245\040\260\305\267\241\301\337\300\314\266\363\040\260\305\267\241\270\246\040\307\322\274\366\040\276\370\275\300\264\317\264\331\056",g_iUseLocale));
        return false;
    }
    if (DISTANCE_APPROX(GetX()-victim->GetX(),GetY()-victim->GetY())>=1000 || m_pkExchange)
        return false;
    if (victim->m_pkExchange) {
        exchange_packet(this,6,false,0,0,0,0);
        return false;
    }
    if (victim->IsBlockMode(1)) {
        ChatPacket(1,locale_find("\273\363\264\353\271\346\300\314\040\261\263\310\257\040\260\305\272\316\040\273\363\305\302\300\324\264\317\264\331\056",g_iUseLocale));
        return false;
    }
    SetExchange(new CExchange(this));
    victim->SetExchange(new CExchange(victim));
    victim->m_pkExchange->SetCompany(m_pkExchange);
    m_pkExchange->SetCompany(victim->m_pkExchange);
    SetExchangeTime();
    victim->SetExchangeTime();
    exchange_packet(victim,0,false,GetVID(),0,0,0);
    exchange_packet(this,0,false,victim->GetVID(),0,0,0);
    return true;
}
