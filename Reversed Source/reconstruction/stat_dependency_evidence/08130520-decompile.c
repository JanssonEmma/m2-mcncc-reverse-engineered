
/* DWARF original prototype: void SetPart(CHARACTER * this, BYTE bPartPos, WORD wVal) */

void __thiscall CHARACTER::SetPart(CHARACTER *this,BYTE bPartPos,WORD wVal)

{
  if (3 < bPartPos) {
    __assert("SetPart","char.cpp",0xf37);
  }
  (this->m_pointsInstant).parts[bPartPos] = wVal;
  return;
}

