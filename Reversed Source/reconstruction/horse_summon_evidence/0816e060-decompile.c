
/* DWARF original prototype: void SendHorseInfo(CHARACTER * this) */

void __thiscall CHARACTER::SendHorseInfo(CHARACTER *this)

{
  short sVar1;
  short sVar2;
  CHorseRider *this_00;
  uint uVar3;
  int iHealthGrade;
  
  if ((this->m_chHorse != (LPCHARACTER)0x0) || ((this->super_CHorseRider).m_Horse.bRiding != '\0'))
  {
    this_00 = &this->super_CHorseRider;
                    /* Unresolved local var: int iStaminaGrade@[DW_OP_reg7(EDI)] */
    sVar1 = (this->super_CHorseRider).m_Horse.sHealth;
    iHealthGrade = 0;
    if (sVar1 != 0) {
      sVar2 = CHorseRider::GetHorseMaxHealth(this_00);
      iHealthGrade = 1;
      if (sVar2 * 3 < sVar1 * 10) {
        sVar1 = (this->super_CHorseRider).m_Horse.sHealth;
        sVar2 = CHorseRider::GetHorseMaxHealth(this_00);
        iHealthGrade = (sVar2 * 7 < sVar1 * 10) + 2;
      }
    }
    sVar1 = (this->super_CHorseRider).m_Horse.sStamina;
    uVar3 = 0;
    sVar2 = CHorseRider::GetHorseMaxStamina(this_00);
    if ((int)sVar2 < sVar1 * 10) {
      sVar1 = (this->super_CHorseRider).m_Horse.sStamina;
      uVar3 = 1;
      sVar2 = CHorseRider::GetHorseMaxStamina(this_00);
      if (sVar2 * 3 < sVar1 * 10) {
        sVar1 = (this->super_CHorseRider).m_Horse.sStamina;
        sVar2 = CHorseRider::GetHorseMaxStamina(this_00);
        uVar3 = (sVar2 * 7 < sVar1 * 10) + 2;
      }
    }
    if ((((this->super_CHorseRider).m_Horse.bLevel != this->m_bSendHorseLevel) ||
        ((uint)this->m_bSendHorseHealthGrade != iHealthGrade)) ||
       (this->m_bSendHorseStaminaGrade != uVar3)) {
      ChatPacket(this,'\x05',"horse_state %d %d %d",(uint)(this->super_CHorseRider).m_Horse.bLevel,
                 iHealthGrade,uVar3);
      this->m_bSendHorseLevel = (this->super_CHorseRider).m_Horse.bLevel;
      this->m_bSendHorseHealthGrade = (BYTE)iHealthGrade;
      this->m_bSendHorseStaminaGrade = (BYTE)uVar3;
    }
  }
  return;
}

