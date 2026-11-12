
/* DWARF original prototype: BYTE GetEmpireFromMapIndex(SECTREE_MANAGER * this, long lMapIndex) */

BYTE __thiscall SECTREE_MANAGER::GetEmpireFromMapIndex(SECTREE_MANAGER *this,long lMapIndex)

{
  byte bVar1;
  
  bVar1 = 1;
  if ((((0x13 < lMapIndex - 1U) && (bVar1 = 2, 0x13 < lMapIndex - 0x15U)) &&
      (bVar1 = 3, 0x13 < lMapIndex - 0x29U)) &&
     ((bVar1 = 1, 1 < lMapIndex - 0xb8U && (bVar1 = 2, 1 < lMapIndex - 0xbaU)))) {
    bVar1 = -(lMapIndex - 0xbcU < 2) & 3;
  }
  return bVar1;
}

