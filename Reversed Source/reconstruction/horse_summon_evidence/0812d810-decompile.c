
/* DWARF original prototype: bool Goto(CHARACTER * this, long x, long y) */

bool __thiscall CHARACTER::Goto(CHARACTER *this,long x,long y)

{
  bool bVar1;
  LPCHARACTER pCVar2;
  undefined *puVar3;
  
  if (((this->super_CEntity).m_pos.x != x) || ((this->super_CEntity).m_pos.y != y)) {
    if (((this->m_posDest).x != x) || ((this->m_posDest).y != y)) {
      (this->m_posDest).x = x;
      (this->m_posDest).y = y;
      CalculateMoveDuration(this);
      this->m_dwStateDuration = 4;
      bVar1 = CFSM::IsState(&this->super_CFSM,&(this->m_stateMove).super_CState);
      if (!bVar1) {
        pCVar2 = GetVictim(this);
        puVar3 = &DAT_082267f2;
        if (pCVar2 == (LPCHARACTER)0x0) {
          puVar3 = &DAT_082267fb;
        }
        MonsterLog(this,"[MOVE] %s",puVar3);
        pCVar2 = GetVictim(this);
        if (pCVar2 != (LPCHARACTER)0x0) {
          MonsterChat(this,'\x01');
        }
      }
      CFSM::GotoState(&this->super_CFSM,&(this->m_stateMove).super_CState);
      return true;
    }
    bVar1 = CFSM::IsState(&this->super_CFSM,&(this->m_stateMove).super_CState);
    if (!bVar1) {
      this->m_dwStateDuration = 4;
      CFSM::GotoState(&this->super_CFSM,&(this->m_stateMove).super_CState);
    }
  }
  return false;
}

