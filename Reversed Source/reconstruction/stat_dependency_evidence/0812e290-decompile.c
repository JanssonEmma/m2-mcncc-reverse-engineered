
/* DWARF original prototype: long GetAllowedGold(CHARACTER * this) */

long __thiscall CHARACTER::GetAllowedGold(CHARACTER *this)

{
  byte bVar1;
  long lVar2;
  
  bVar1 = (this->m_points).level;
  lVar2 = 100000;
  if (10 < bVar1) {
    lVar2 = ((0x14 < bVar1) - 1 & 0xfd0cb0a0) + 50000000;
  }
  return lVar2;
}

