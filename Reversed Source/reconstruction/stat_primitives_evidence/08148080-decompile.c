
/* DWARF original prototype: bool IsStun(CHARACTER * this) */

bool __thiscall CHARACTER::IsStun(CHARACTER *this)

{
  return ((this->m_pointsInstant).instant_flag & 8) != 0;
}

