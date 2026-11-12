
/* DWARF original prototype: void ClearHorseInfo(CHARACTER * this) */

void __thiscall CHARACTER::ClearHorseInfo(CHARACTER *this)

{
  if ((this->super_CHorseRider).m_Horse.bRiding == '\0') {
    ChatPacket(this,'\x05',"hide_horse_state");
    this->m_bSendHorseLevel = '\0';
    this->m_bSendHorseHealthGrade = '\0';
    this->m_bSendHorseStaminaGrade = '\0';
  }
  this->m_chHorse = (LPCHARACTER)0x0;
  return;
}

