#include "char.h"
#include "item.h"
#include "desc.h"
#include "packet.h"

// Full old quickslot helpers; full CHARACTER data and actual DESC::Packet required.
bool CHARACTER::GetQuickslot(BYTE pos,TQuickslot** slot)
{
    if(pos>=36) return false;
    *slot=&m_quickslot[pos];
    return true;
}
bool CHARACTER::DelQuickslot(BYTE pos)
{
    if(pos<36) {
        m_quickslot[pos].type=0;
        m_quickslot[pos].pos=0;
        packet_quickslot_del packet;
        packet.pos=pos;packet.header=29;
        GetDesc()->Packet(&packet,2); // Original has no descriptor guard.
    }
    return pos<36;
}
bool CHARACTER::SwapQuickslot(BYTE first, BYTE second)
{
    if (first >= 36 || second >= 36)
        return false;

    const TQuickslot slot = m_quickslot[first];
    m_quickslot[first] = m_quickslot[second];
    m_quickslot[second] = slot;
    const BYTE packet[] = {0x1e, first, second};
    if (DESC* desc = GetDesc())
        desc->Packet(packet, sizeof(packet));
    return true;
}

bool CHARACTER::SetQuickslot(BYTE pos,TQuickslot& slot)
{
    if(pos>=36) return false;
    BYTE type=slot.type;
    if(type>3) return false;
    for(int i=0;i<36;++i) {
        if(type!=0 && m_quickslot[i].type==type && m_quickslot[i].pos==slot.pos) {
            DelQuickslot((BYTE)i);
            type=slot.type; // Input may alias deleted actual slot.
        }
    }
    if(!((type==1 && slot.pos<90) || (type==2 && slot.pos!=255) || type==3)) return false;
    m_quickslot[pos]=slot;
    if(GetDesc()) {
        packet_quickslot_add packet;
        packet.pos=pos;packet.header=28;packet.slot=m_quickslot[pos];
        GetDesc()->Packet(&packet,4);
    }
    return true;
}

// Complete old helpers08050660/08050940. Full CHARACTER/CItem data and actual
// SetQuickslot/DelQuickslot are already production, no substitute app methods.
void CHARACTER::SyncQuickslot(BYTE type,BYTE oldPos,BYTE newPos)
{
    if(oldPos==newPos) return;
    for(int i=0;i<36;++i) {
        if(m_quickslot[i].type==type && m_quickslot[i].pos==oldPos) {
            if(newPos==255) DelQuickslot((BYTE)i);
            else {
                TQuickslot slot;slot.pos=newPos;slot.type=type;
                SetQuickslot((BYTE)i,slot);
            }
        }
    }
}
void CHARACTER::ChainQuickslotItem(LPITEM item,BYTE type,BYTE oldPos)
{
    for(int i=0;i<36;++i) {
        if(m_quickslot[i].type==type && m_quickslot[i].pos==oldPos) {
            TQuickslot slot;slot.pos=item->GetCell();slot.type=type;
            SetQuickslot((BYTE)i,slot);
            return;
        }
    }
}
