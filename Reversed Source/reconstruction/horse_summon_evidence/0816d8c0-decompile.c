
/* DWARF original prototype: void SetRider(CHARACTER * this, LPCHARACTER ch) */

void __thiscall CHARACTER::SetRider(CHARACTER *this,LPCHARACTER ch)

{
  LPCHARACTER pCVar1;
  
  pCVar1 = this->m_chRider;
  if (pCVar1 != (LPCHARACTER)0x0) {
    (*(pCVar1->super_CEntity)._vptr_CEntity[0x12])(pCVar1);
  }
  this->m_chRider = ch;
  if (ch != (LPCHARACTER)0x0) {
    (*(ch->super_CEntity)._vptr_CEntity[0x11])(ch);
  }
  return;
}

