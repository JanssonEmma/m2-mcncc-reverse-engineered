
/* DWARF original prototype: LPITEM GetWear(CHARACTER * this, BYTE bCell) */

LPITEM __thiscall CHARACTER::GetWear(CHARACTER *this,BYTE bCell)

{
  LPITEM pCVar1;
  
  if (bCell < 0x13) {
    pCVar1 = (this->m_pointsInstant).pItems[bCell + 0x5a];
  }
  else {
    _sys_err("GetWear",0x15e,"CHARACTER::GetWear: invalid wear cell %d",(uint)bCell);
    pCVar1 = (LPITEM)0x0;
  }
  return pCVar1;
}

