
/* DWARF original prototype: void SetNowWalking(CHARACTER * this, bool bWalkFlag) */

void __thiscall CHARACTER::SetNowWalking(CHARACTER *this,bool bWalkFlag)

{
  char *format;
  DWORD DVar1;
  TPacketGCWalkMode p;
  
  if ((this->m_bNowWalking != bWalkFlag) || (this->m_bCharType != '\x06')) {
    if (bWalkFlag) {
      this->m_bNowWalking = true;
      DVar1 = get_dword_time();
      this->m_dwWalkStartTime = DVar1;
    }
    else {
      this->m_bNowWalking = false;
    }
    p.vid = (this->m_vid).m_id;
    p.header = 'o';
    p.mode = this->m_bNowWalking != false;
    CEntity::PacketView(&this->super_CEntity,&p,6,(LPENTITY)0x0);
    if (this->m_bCharType != '\x06') {
      if (this->m_bNowWalking == false) {
        format = &DAT_08226bcc;
      }
      else {
        format = &DAT_08226bc5;
      }
      MonsterLog(this,format);
    }
  }
  return;
}

