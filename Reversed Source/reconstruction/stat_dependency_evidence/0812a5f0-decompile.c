
/* DWARF original prototype: void SetLevel(CHARACTER * this, BYTE level) */

void __thiscall CHARACTER::SetLevel(CHARACTER *this,BYTE level)

{
  BYTE BVar1;
  
  (this->m_points).level = level;
  if ((this->super_CEntity).m_lpDesc == (LPDESC)0x0) {
    return;
  }
  if ((level < PK_PROTECT_LEVEL) || (BVar1 = GetGMLevel(this), BVar1 != '\0')) {
    level = '\x03';
  }
  else {
    if (this->m_bPKMode != '\x03') {
      return;
    }
    level = '\0';
  }
  SetPKMode(this,level);
  return;
}

