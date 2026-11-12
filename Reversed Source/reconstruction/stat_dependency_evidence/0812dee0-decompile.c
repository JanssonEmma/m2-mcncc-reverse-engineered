
/* DWARF original prototype: void SetRealPoint(CHARACTER * this, BYTE type, long val) */

void __thiscall CHARACTER::SetRealPoint(CHARACTER *this,BYTE type,long val)

{
  (this->m_points).points[type] = val;
  return;
}

