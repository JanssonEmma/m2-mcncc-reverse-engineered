// Original item_attribute.cpp bodies; complete methods.
#include "item.h"
#include "constants.h"
#include "../../libthecore/include/utils.h"
#include <vector>
#include "log.h"
#include "../../libthecore/include/log.h"
#include <cassert>
// 08186cf0
int CItem::GetAttributeSetIndex() {
    if(GetType()==1) return GetSubType()==6 ? -1 : 0;
    if(GetType()==2) {
        switch(GetSubType()) {
        case 0:return 1; case 1:return 5; case 2:return 6;
        case 3:return 2; case 4:return 3; case 5:return 4; case 6:return 7;
        }
    }
    return -1;
}
// 08186d90. Prototype required; rare slots are excluded.
bool CItem::HasAttr(BYTE bApply) {
    for(int i=0;i<3;++i) if(m_pProto->aApplies[i].bType==bApply) return true;
    for(int i=0;i<5;++i) if(m_aAttr[i].bType==bApply) return true;
    return false;
}
// 081876a0. First hole, not population count.
int CItem::GetAttributeCount() {
    for(int i=0;i<5;++i) if(m_aAttr[i].bType==0) return i;
    return 5;
}
// 081876c0. Does not inspect prototype applies.
int CItem::FindAttribute(BYTE bType) {
    for(int i=0;i<5;++i) if(m_aAttr[i].bType==bType) return i;
    return -1;
}
// 08187800. Original assertion checks only the upper bound.
void CItem::SetAttribute(int i,BYTE bType,short sValue) {
    assert(i<5);
    m_aAttr[i].bType=bType;
    m_aAttr[i].sValue=sValue;
    UpdatePacket();
    Save();
    if(bType) LogManager::instance().ItemLog(i,bType,sValue,m_dwID,"SET_ATTR","");
}
// 08186e30
void CItem::AddAttribute(BYTE bApply,short sValue) {
    if(!HasAttr(bApply)) {
        int i=GetAttributeCount();
        if(i>4) {
            _sys_err("AddAttribute",82,"item attribute overflow!");
            return;
        }
        if(sValue) SetAttribute(i,bApply,sValue);
    }
}

// 08187770. Deliberate ORIGINAL recursion, NOT removal by index.
// Instruction081877a0=e8cbffffff calls08187770 itself. Do not repair.
bool CItem::RemoveAttributeType(BYTE bType) {
    int index=FindAttribute(bType);
    if(index!=-1 && RemoveAttributeType(static_cast<BYTE>(index))) return true;
    return false;
}

// Exact old signed32 ADD/SUB register semantics, avoiding C++ signed overflow.
static int AttrAdd32(int a,DWORD b) {
    return static_cast<int>(static_cast<unsigned int>(a)+static_cast<unsigned int>(b));
}
static int AttrSub32(int a,int b) {
    return static_cast<int>(static_cast<unsigned int>(a)-static_cast<unsigned int>(b));
}
// 08186ec0. Prototype required by HasAttr even when level0.
void CItem::AddAttr(BYTE bApply,BYTE bLevel) {
    if(HasAttr(bApply) || bLevel==0) return;
    int i=GetAttributeCount();
    if(i==5) {
        _sys_err("AddAttr",101,"item attribute overflow!");
    } else {
        const TItemAttrTable& r=g_map_itemAttr[bApply];
        long value=r.lValues[MIN(4,static_cast<int>(bLevel)-1)];
        if(value) SetAttribute(i,bApply,static_cast<short>(value));
    }
}
// 08187060. Operator[] side effects, level0 and zero/overflowed totals retained.
void CItem::PutAttributeWithLevel(BYTE bLevel) {
    int set=GetAttributeSetIndex();
    if(set<0 || bLevel>5) return;
    std::vector<int> avail;
    int total=0;
    for(int i=0;i<82;++i) {
        const TItemAttrTable& r=g_map_itemAttr[i];
        if(r.bMaxLevelBySet[set] && !HasAttr(static_cast<BYTE>(i))) {
            avail.push_back(i);
            total=AttrAdd32(total,r.dwProb);
        }
    }
    unsigned int prob=static_cast<unsigned int>(number_ex(1,total,const_cast<char*>("item_attribute.cpp"),139));
    int attr_idx=0;
    for(DWORD i=0;i<avail.size();++i) {
        const auto weight = static_cast<unsigned int>(g_map_itemAttr[avail[i]].dwProb);
        if (prob <= weight) {
            attr_idx = avail[i];
            break;
        }
        prob -= weight;
    }
    if(!attr_idx) {
        _sys_err("PutAttributeWithLevel",157,"Cannot put item attribute %d %d",set,static_cast<int>(bLevel));
        return;
    }
    const TItemAttrTable& r=g_map_itemAttr[attr_idx];
    const BYTE cap = r.bMaxLevelBySet[set];
    if (bLevel > cap)
        bLevel = cap;
    AddAttr(static_cast<BYTE>(attr_idx),bLevel);
}
// 081874d0. Supplied weights need not sum100; leftoverlevel6 goes through guard.
void CItem::PutAttribute(const int* aiAttrPercentTable) {
    int percent=number_ex(1,100,const_cast<char*>("item_attribute.cpp"),172);
    int i;
    for(i=0;i<5;++i) {
        const int weight = aiAttrPercentTable[i];
        if (percent <= weight)
            break;
        percent = AttrSub32(percent, weight);
    }
    PutAttributeWithLevel(static_cast<BYTE>(i+1));
}

#include "../../libthecore/include/memcpy.h"

// Complete original item_attribute.cpp tail. Unsafe original domains retained.
// 08187540. The original PutAttribute takes const int* weights.
void CItem::ChangeAttribute(const int* aiChangeProb) {
    int count=GetAttributeCount();
    ClearAttribute();
    if(count==0) return;
    const TItemTable* proto=m_pProto;
    const int addonType = proto ? proto->sAddonType : 0;
    if (addonType) ApplyAddon(addonType);
    int tmp[5]={0,10,40,35,15};
    if(!aiChangeProb) aiChangeProb=tmp;
    for(int i=GetAttributeCount();i<count;++i) PutAttribute(aiChangeProb);
}
// 08187610. Local original array, not an invented global probability table.
void CItem::AddAttribute() {
    int percent[5]={40,50,10,0,0};
    if(GetAttributeCount()<5) PutAttribute(percent);
}
// 08187670. No packet, save or audit; rare slots untouched.
void CItem::ClearAttribute() {
    for(int i=0;i<5;++i) {m_aAttr[i].bType=0;m_aAttr[i].sValue=0;}
}
// 081876f0. Only upper-count check, no new negative-index guard.
bool CItem::RemoveAttributeAt(int index) {
    if(GetAttributeCount()<=index) return false;
    for (int i = index; i < 4; ++i) {
        const short value = m_aAttr[i + 1].sValue;
        const BYTE type = m_aAttr[i + 1].bType;
        SetAttribute(i, type, value);
    }
    SetAttribute(4,0,0);
    return true;
}
// 081877c0. Indirect original copy hook, exact21-byte full seven-slot array.
void CItem::SetAttributes(const TPlayerItemAttribute* c_pAttribute) {
    static_assert(sizeof(m_aAttr) == 21, "2009 item attribute array");
    (*thecore_memcpy)(m_aAttr, c_pAttribute, sizeof(m_aAttr));
    Save();
}
// 08187900. Only destination saves; neither side emits item-update packet.
void CItem::CopyAttributeTo(CItem* pItem) {pItem->SetAttributes(m_aAttr);}
// 08186e00. Only rare slots, including matching zero; no prototype requirement.
bool CItem::HasRareAttr(BYTE bApply) {
    for(int i=5;i<7;++i) if(m_aAttr[i].bType==bApply) return true;
    return false;
}
// 08187920. Population count, not first-hole index.
int CItem::GetRareAttrCount() {
    int count=m_aAttr[5].bType!=0;
    if(m_aAttr[6].bType!=0) ++count;
    return count;
}
// 08187940. Captured count; clears prefix, audit first, ignores add return values.
bool CItem::ChangeRareAttribute() {
    int count=GetRareAttrCount();
    if(count==0) return false;
    for(int i=0;i<count;++i) {m_aAttr[i+5].bType=0;m_aAttr[i+5].sValue=0;}
    LogManager::instance().ItemLog(0,0,0,m_dwID,"SET_RARE_CHANGE","");
    for(int i=0;i<count;++i) AddRareAttribute();
    return true;
}
// 08187a20. No invalid-set or empty-vector guard: original preconditions remain.
bool CItem::AddRareAttribute() {
    int count=GetRareAttrCount();
    if(count==2) {sys_log(0,"RARE: over 2");return false;}
    int pos=count+5;
    TPlayerItemAttribute& attr=m_aAttr[pos];
    int set=GetAttributeSetIndex();
    std::vector<int> avail;
    for(int i=0;i<82;++i) {
        const TItemAttrTable& r=g_map_itemRare[i];
        if(r.dwApplyIndex && r.bMaxLevelBySet[set] && !HasRareAttr(static_cast<BYTE>(i))) avail.push_back(i);
    }
    const TItemAttrTable& r=g_map_itemRare[avail[number_ex(0,static_cast<int>(avail.size())-1,const_cast<char*>("item_attribute.cpp"),366)]];
    int level=5;
    const BYTE cap = r.bMaxLevelBySet[set];
    if (level > cap)
        level = cap;
    attr.bType=static_cast<BYTE>(r.dwApplyIndex);
    attr.sValue=static_cast<short>(r.lValues[level-1]);
    UpdatePacket();
    Save();
    auto& logs = LogManager::instance();
    const DWORD itemId = m_dwID;
    const short value = attr.sValue;
    const BYTE type = attr.bType;
    logs.ItemLog(pos, type, value, itemId, "SET_RARE", "");
    return true;
}
