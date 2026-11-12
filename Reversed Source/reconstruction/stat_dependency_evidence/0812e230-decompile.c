
/* DWARF original prototype: void SetPoint(CHARACTER * this, BYTE type, long val) */

void __thiscall CHARACTER::SetPoint(CHARACTER *this,BYTE type,long val)

{
  DWORD DVar1;
  
  (this->m_pointsInstant).points[type] = val;
  if (type == '\x13') {
    DVar1 = get_dword_time();
    if (DVar1 < this->m_dwMoveDuration + this->m_dwMoveStartTime) {
      CalculateMoveDuration(this);
      return;
    }
  }
  return;
}

