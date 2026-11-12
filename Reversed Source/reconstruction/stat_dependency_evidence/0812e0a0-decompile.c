
/* DWARF original prototype: long GetPoint(CHARACTER * this, BYTE type) */

long __thiscall CHARACTER::GetPoint(CHARACTER *this,BYTE type)

{
  int iVar1;
  char *pcVar2;
  int iVar3;
  
                    /* Unresolved local var: long val@[DW_OP_reg3(EBX)]
                       Unresolved local var: long max_val@[DW_OP_reg6(ESI)] */
  iVar3 = 0x7fffffff;
  iVar1 = (this->m_pointsInstant).points[type];
  if (type - 0x3f < 2) {
    iVar3 = 0x32;
  }
  if (iVar3 < iVar1) {
    pcVar2 = GetName(this);
    _sys_err("GetPoint",0xab2,"POINT_ERROR: %s type %d val %d (max: %d)",pcVar2,iVar1,iVar3);
  }
  return iVar1;
}

