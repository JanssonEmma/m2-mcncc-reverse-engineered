
/* DWARF original prototype: void CheckMaximumPoints(CHARACTER * this) */

void __thiscall CHARACTER::CheckMaximumPoints(CHARACTER *this)

{
  int iVar1;
  int iVar2;
  
  iVar1 = (this->m_pointsInstant).iMaxHP;
  iVar2 = (this->m_points).hp;
  if (iVar1 < iVar2) {
    PointChange(this,'\x05',iVar1 - iVar2,false,false);
  }
  iVar1 = (this->m_pointsInstant).iMaxSP;
  iVar2 = (this->m_points).sp;
  if (iVar1 < iVar2) {
    PointChange(this,'\a',iVar1 - iVar2,false,false);
  }
  return;
}

