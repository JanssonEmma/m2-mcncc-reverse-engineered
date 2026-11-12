
/* DWARF original prototype: bool IsDead(CHARACTER * this) */

bool __thiscall CHARACTER::IsDead(CHARACTER *this)

{
  return (this->m_pointsInstant).position == 0;
}

