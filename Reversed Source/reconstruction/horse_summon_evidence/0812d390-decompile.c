
/* DWARF original prototype: void SetRotation(CHARACTER * this, float fRot) */

void __thiscall CHARACTER::SetRotation(CHARACTER *this,float fRot)

{
  (this->m_pointsInstant).fRot = fRot;
  return;
}

