
/* DWARF original prototype: long GetLimitPoint(CHARACTER * this, BYTE type) */

long __thiscall CHARACTER::GetLimitPoint(CHARACTER *this,BYTE type)

{
  uint uVar1;
  char *pcVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  long min_limit;
  
                    /* Unresolved local var: long val@[DW_OP_reg3(EBX)]
                       Unresolved local var: long max_val@[DW_OP_reg7(EDI)]
                       Unresolved local var: long limit@[DW_OP_reg6(ESI)] */
  uVar1 = (uint)type;
  iVar4 = 0x7fffffff;
  iVar5 = 0x7fffffff;
  iVar3 = (this->m_pointsInstant).points[uVar1];
  min_limit = -0x7fffffff;
  if (uVar1 < 0x41) {
    if (uVar1 < 0x3f) {
      if (uVar1 == 0x11) {
        uVar1 = ((this->super_CEntity).m_lpDesc == (LPDESC)0x0) - 1 & 0xffffffb0;
      }
      else {
        if (uVar1 != 0x13) {
          iVar4 = 0x7fffffff;
          goto LAB_0812e170;
        }
        uVar1 = ((this->super_CEntity).m_lpDesc == (LPDESC)0x0) - 1 & 0xffffffce;
      }
      min_limit = 0;
      iVar4 = uVar1 + 0xfa;
      goto LAB_0812e170;
    }
    iVar4 = 0x32;
  }
  else {
    if (1 < uVar1 - 0x72) goto LAB_0812e170;
    iVar4 = 0xd;
  }
  iVar5 = 0x32;
LAB_0812e170:
  if (iVar5 < iVar3) {
    pcVar2 = GetName(this);
    _sys_err("GetLimitPoint",0xae0,"POINT_ERROR: %s type %d val %d (max: %d)",pcVar2,iVar3,iVar5);
  }
  if (iVar4 < iVar3) {
    iVar3 = iVar4;
  }
  if (iVar3 < min_limit) {
    iVar3 = min_limit;
  }
  return iVar3;
}

