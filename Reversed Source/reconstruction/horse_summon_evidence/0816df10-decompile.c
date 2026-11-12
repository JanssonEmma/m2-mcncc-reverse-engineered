
/* DWARF original prototype: DWORD GetMyHorseVnum(CHARACTER * this) */

DWORD __thiscall CHARACTER::GetMyHorseVnum(CHARACTER *this)

{
  int iVar1;
  
                    /* Unresolved local var: int delta@[DW_OP_reg3(EBX)] */
  iVar1 = 0;
  if ((this->m_pGuild != (CGuild *)0x0) &&
     (iVar1 = 1, (this->m_pGuild->m_data).master_pid == this->m_dwPlayerID)) {
    iVar1 = 2;
  }
  return iVar1 + *(int *)(c_aHorseStat + (uint)(this->super_CHorseRider).m_Horse.bLevel * 0x30 + 4);
}

