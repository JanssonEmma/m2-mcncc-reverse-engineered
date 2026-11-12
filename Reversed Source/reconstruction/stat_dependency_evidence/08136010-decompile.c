
/* DWARF original prototype: void MountVnum(CHARACTER * this, DWORD vnum) */

void __thiscall CHARACTER::MountVnum(CHARACTER *this,DWORD vnum)

{
  if (this->m_dwMountVnum != vnum) {
    this->m_dwMountVnum = vnum;
    CEntity::ViewReencode(&this->super_CEntity);
    ComputePoints(this);
    return;
  }
  return;
}

