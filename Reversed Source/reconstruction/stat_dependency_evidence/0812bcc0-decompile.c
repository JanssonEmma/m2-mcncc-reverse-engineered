
/* DWARF original prototype: DWORD GetMonsterDrainSPPoint(CHARACTER * this) */

DWORD __thiscall CHARACTER::GetMonsterDrainSPPoint(CHARACTER *this)

{
  DWORD DVar1;
  
  DVar1 = 0;
  if (this->m_pkMobData != (CMob *)0x0) {
    DVar1 = (this->m_pkMobData->m_table).dwDrainSP;
  }
  return DVar1;
}

