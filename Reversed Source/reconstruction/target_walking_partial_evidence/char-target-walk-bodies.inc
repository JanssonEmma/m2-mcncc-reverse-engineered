// Insert both complete bodies in game/src/char.cpp, original source placement.
// Requires packet.h, common/length.h ECharType, log.h, utils.h, <cstdlib>.
//08132670. Nonempty NPC target sets require valid signed division operands.
void CHARACTER::BroadcastTargetPacket() {
    if(m_set_pkChrTargetedBy.empty()) return;
    TPacketGCTarget packet;
    packet.header=63;
    packet.dwVID=static_cast<DWORD>(GetVID());
    if(GetDesc()) packet.bHPPercent=0;
    else {
        // The original LEA/SHL sequence multiplies in32-bit registers before IDIV.
        int hp100=static_cast<int>(static_cast<unsigned int>(m_points.hp)*100U);
        packet.bHPPercent=static_cast<BYTE>(MINMAX(0,hp100/m_pointsInstant.iMaxHP,100));
    }
    std::set<CHARACTER*>::iterator it=m_set_pkChrTargetedBy.begin();
    while(it!=m_set_pkChrTargetedBy.end()) {
        CHARACTER* recipient=*it++; // Advance BEFORE Packet, preserving current-entry removal.
        DESC* descriptor=recipient->GetDesc();
        if(!descriptor) {
            _sys_err("BroadcastTargetPacket",4801,"%s %p does not have desc",recipient->GetName(),recipient);
            std::abort();
        }
        descriptor->Packet(&packet,sizeof(packet));
    }
}
//081341a0. Repeated unchanged flags still act for raw non-PC char types.
void CHARACTER::SetNowWalking(bool bWalkFlag) {
    if(m_bNowWalking!=bWalkFlag || m_bCharType!=CHAR_TYPE_PC) {
        if(bWalkFlag) {
            m_bNowWalking=true;
            m_dwWalkStartTime=get_dword_time();
        } else m_bNowWalking=false;
        TPacketGCWalkMode packet;
        packet.vid=static_cast<DWORD>(GetVID());
        packet.header=111;
        packet.mode=m_bNowWalking?WALKMODE_WALK:WALKMODE_RUN;
        PacketView(&packet,sizeof(packet));
        if(m_bCharType!=CHAR_TYPE_PC) {
            if(m_bNowWalking) MonsterLog("\xb0\xc8\xb4\xc2\xb4\xd9");
            else MonsterLog("\xb6\xda\xb4\xd9");
        }
    }
}
