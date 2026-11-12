
/* DWARF original prototype: long GetRealPoint(CHARACTER * this, BYTE type) */

long __thiscall CHARACTER::GetRealPoint(CHARACTER *this,BYTE type)

{
  return (this->m_points).points[type];
}

