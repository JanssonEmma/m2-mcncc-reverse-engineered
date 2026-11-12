
/* DWARF original prototype: void UpdatePacket(CHARACTER * this) */

void __thiscall CHARACTER::UpdatePacket(CHARACTER *this)

{
  long lVar1;
  TPacketGCCharacterUpdate pack;
  
  pack.header = '\x13';
  pack.dwVID = (this->m_vid).m_id;
  pack.awPart[0] = GetPart(this,'\0');
  pack.awPart[1] = GetPart(this,'\x01');
  pack.awPart[2] = GetPart(this,'\x02');
  pack.awPart[3] = GetPart(this,'\x03');
  lVar1 = GetLimitPoint(this,'\x13');
  pack.bMovingSpeed = (BYTE)lVar1;
  lVar1 = GetLimitPoint(this,'\x11');
  pack.bAttackSpeed = (BYTE)lVar1;
  pack.bStateFlag = this->m_bAddChrState;
  pack.dwAffectFlag[0] = (this->m_afAffectFlag).bits[0];
  pack.dwAffectFlag[1] = (this->m_afAffectFlag).bits[1];
  pack.dwGuildID = 0;
  pack.sAlignment = (short)(this->m_iAlignment / 10);
  pack.bPKMode = this->m_bPKMode;
  if (this->m_pGuild != (CGuild *)0x0) {
    pack.dwGuildID = (this->m_pGuild->m_data).guild_id;
  }
  pack.dwMountVnum = this->m_dwMountVnum;
  CEntity::PacketAround(&this->super_CEntity,&pack,0x23,(LPENTITY)0x0);
  return;
}

