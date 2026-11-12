
/* DWARF original prototype: long GetPolymorphPoint(CHARACTER * this, BYTE type) */

long __thiscall CHARACTER::GetPolymorphPoint(CHARACTER *this,BYTE type)

{
  byte bVar1;
  DWORD dwVnum;
  BYTE BVar2;
  long lVar3;
  CMob *pCVar4;
  int iVar5;
  int iVar6;
  BYTE type_local;
  
  dwVnum = this->m_dwPolymorphRace;
  if ((dwVnum == 0) || (this->m_bPolyMaintainStat != false)) {
LAB_0812df2c:
    lVar3 = GetPoint(this,type);
    return lVar3;
  }
                    /* Unresolved local var: DWORD dwMobVnum@[???]
                       Unresolved local var: CMob * pMob@[DW_OP_reg3(EBX)]
                       Unresolved local var: int iPower@[DW_OP_reg7(EDI)] */
  if (singleton<CMobManager>::ms_singleton == (CMobManager *)0x0) {
    __assert("instance","../../common/singleton.h",0x1a);
  }
  pCVar4 = CMobManager::Get(singleton<CMobManager>::ms_singleton,dwVnum);
  iVar5 = GetPolymorphPower(this);
  if (pCVar4 == (CMob *)0x0) goto LAB_0812df2c;
  if (type == '\r') {
    iVar5 = (uint)(pCVar4->m_table).bCon * iVar5;
    iVar6 = iVar5 / 100 + (iVar5 >> 0x1f);
    BVar2 = '\r';
LAB_0812dfc6:
    iVar6 = iVar6 - (iVar5 >> 0x1f);
    goto LAB_0812dfc8;
  }
  if (type < 0xe) {
    if (type != '\f') goto LAB_0812df2c;
    BVar2 = GetJob(this);
    if (BVar2 != '\x03') {
      BVar2 = GetJob(this);
      if ((BVar2 != '\x02') || ((this->m_points).skill_group != '\x02')) {
        iVar5 = (uint)(pCVar4->m_table).bStr * iVar5;
        iVar6 = iVar5 / 100 + (iVar5 >> 0x1f);
        BVar2 = '\f';
        goto LAB_0812dfc6;
      }
    }
    bVar1 = (pCVar4->m_table).bStr;
LAB_0812dfed:
    BVar2 = '\x0f';
  }
  else {
    if (type != '\x0e') {
      if (type != '\x0f') goto LAB_0812df2c;
      bVar1 = (pCVar4->m_table).bInt;
      goto LAB_0812dfed;
    }
    bVar1 = (pCVar4->m_table).bDex;
    BVar2 = '\x0e';
  }
  iVar6 = (int)((uint)bVar1 * iVar5) / 100;
LAB_0812dfc8:
  lVar3 = GetPoint(this,BVar2);
  return lVar3 + iVar6;
}

