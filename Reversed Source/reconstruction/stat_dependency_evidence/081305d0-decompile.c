
/* DWARF original prototype: WORD GetOriginalPart(CHARACTER * this, BYTE bPartPos) */

WORD __thiscall CHARACTER::GetOriginalPart(CHARACTER *this,BYTE bPartPos)

{
  WORD WVar1;
  BYTE bPartPos_00;
  
  if (bPartPos == '\0') {
    if ((this->super_CEntity).m_lpDesc != (LPDESC)0x0) {
      return (ushort)(this->m_pointsInstant).bBasePart;
    }
    bPartPos_00 = '\0';
  }
  else {
    if (bPartPos != '\x03') {
      return 0;
    }
    bPartPos_00 = '\x03';
  }
  WVar1 = GetPart(this,bPartPos_00);
  return WVar1;
}

