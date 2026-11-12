
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* DWARF original prototype: void ComputeBattlePoints(CHARACTER * this) */

void __thiscall CHARACTER::ComputeBattlePoints(CHARACTER *this)

{
  byte bVar1;
  TItemTable *pTVar2;
  float fVar3;
  byte bVar4;
  bool bVar5;
  BYTE BVar6;
  CMob *pCVar7;
  DWORD DVar8;
  LPITEM this_00;
  long lVar9;
  TMobTable *pTVar10;
  int iVar11;
  long lVar12;
  int iVar13;
  int iVar14;
  int iArmor;
  int iDef;
  int iShowDef;
  int iAtt;
  
  DVar8 = this->m_dwPolymorphRace;
  if (DVar8 != 0) {
                    /* Unresolved local var: DWORD dwMobVnum@[???]
                       Unresolved local var: CMob * pMob@[DW_OP_reg6(ESI)]
                       Unresolved local var: int iDef@[DW_OP_reg3(EBX)] */
    if (singleton<CMobManager>::ms_singleton == (CMobManager *)0x0) {
      __assert("instance","../../common/singleton.h",0x1a);
    }
    lVar12 = 0;
    pCVar7 = CMobManager::Get(singleton<CMobManager>::ms_singleton,DVar8);
    iAtt = 0;
    if (pCVar7 != (CMob *)0x0) {
      bVar1 = (this->m_points).level;
      lVar12 = GetPolymorphPoint(this,'\f');
      iAtt = ((uint)bVar1 + lVar12) * 2;
      bVar1 = (this->m_points).level;
      lVar12 = GetPolymorphPoint(this,'\r');
      lVar12 = (uint)bVar1 + lVar12 + (uint)(pCVar7->m_table).wDef;
    }
LAB_0812be28:
                    /* Unresolved local var: int iAtt@[???]
                       Unresolved local var: int iDef@[DW_OP_reg3(EBX)] */
    SetPoint(this,'\x12',iAtt);
    SetPoint(this,'\x10',lVar12);
    lVar12 = GetPoint(this,'\x12');
    SetPoint(this,'\x16',lVar12);
    lVar12 = GetPoint(this,'\x10');
    SetPoint(this,'\x17',lVar12);
    return;
  }
  if ((this->super_CEntity).m_lpDesc == (LPDESC)0x0) {
    bVar1 = (this->m_points).level;
    lVar12 = GetPoint(this,'\f');
    bVar4 = (this->m_points).level;
    iAtt = ((uint)bVar1 + lVar12) * 2;
    lVar12 = GetPoint(this,'\r');
    pTVar10 = GetMobTable(this);
    lVar12 = (uint)bVar4 + lVar12 + (uint)pTVar10->wDef;
    goto LAB_0812be28;
  }
                    /* Unresolved local var: int iAtk@[DW_OP_reg3(EBX)]
                       Unresolved local var: int iStatAtk@[DW_OP_reg3(EBX)]
                       Unresolved local var: LPITEM pkItem@[DW_OP_reg6(ESI)] */
  SetPoint(this,'\x12',0);
  SetPoint(this,'\x10',0);
  SetPoint(this,'\x14',0);
  lVar12 = GetPoint(this,'\x12');
  SetPoint(this,'\x16',lVar12);
  lVar12 = GetPoint(this,'\x10');
  SetPoint(this,'\x17',lVar12);
  bVar1 = (this->m_points).level;
  bVar4 = GetJob(this);
  if (bVar4 == 1) {
    iVar13 = GetPoint(this,'\f');
    BVar6 = '\x0e';
LAB_0812c479:
    lVar12 = GetPoint(this,BVar6);
    iVar13 = ((lVar12 + iVar13 * 2) * 2) / 3;
    goto LAB_0812bfe5;
  }
  if (bVar4 < 2) {
    if (bVar4 != 0) goto LAB_0812bfa4;
  }
  else if (bVar4 != 2) {
    if (bVar4 == 3) {
      iVar13 = GetPoint(this,'\f');
      BVar6 = '\x0f';
      goto LAB_0812c479;
    }
LAB_0812bfa4:
    bVar4 = GetJob(this);
    _sys_err("ComputeBattlePoints",0x7c8,"invalid job %d",(uint)bVar4);
  }
  lVar12 = GetPoint(this,'\f');
  iVar13 = lVar12 * 2;
LAB_0812bfe5:
  DVar8 = 0;
  if (this->m_dwMountVnum != 0) {
    lVar12 = GetPoint(this,'\f');
    if (iVar13 < lVar12 * 2) {
      lVar12 = GetPoint(this,'\f');
      iVar13 = lVar12 * 2;
    }
    DVar8 = this->m_dwMountVnum;
  }
  iVar13 = iVar13 + (uint)bVar1 * 2;
  if (DVar8 != 0) {
    sys_log(0,"HORSE ATK BONUS Before: %d",iVar13);
    BVar6 = GetJob(this);
    if ((BVar6 == '\x02') && ((this->m_points).skill_group == '\x01')) {
      iVar14 = (uint)(this->super_CHorseRider).m_Horse.bLevel * iVar13;
      iVar11 = iVar14 / 0x3c + (iVar14 >> 0x1f);
    }
    else {
      iVar14 = (uint)(this->super_CHorseRider).m_Horse.bLevel * iVar13;
      iVar11 = iVar14 / 0x1e + (iVar14 >> 0x1f);
    }
    iVar13 = iVar13 + (iVar11 - (iVar14 >> 0x1f));
    sys_log(0,"HORSE ATK BONUS After: %d",iVar13);
  }
  PointChange(this,'\x12',iVar13,false,false);
  bVar1 = (this->m_points).level;
  lVar12 = GetPoint(this,'\r');
  iVar13 = (uint)bVar1 + lVar12;
  bVar1 = (this->m_points).level;
  lVar12 = GetPoint(this,'\r');
  fVar3 = (float)lVar12 / _DAT_08226cd8;
  iArmor = 0;
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
  iVar14 = 0;
  do {
    this_00 = GetWear(this,(BYTE)iVar14);
    if (this_00 != (LPITEM)0x0) {
      pTVar2 = this_00->m_pProto;
      BVar6 = '\0';
      if (pTVar2 != (TItemTable *)0x0) {
        BVar6 = pTVar2->bType;
      }
      if (BVar6 == '\x02') {
        BVar6 = '\0';
        if (pTVar2 != (TItemTable *)0x0) {
          BVar6 = pTVar2->bSubType;
        }
        if (BVar6 != '\0') {
          BVar6 = '\0';
          if (pTVar2 != (TItemTable *)0x0) {
            BVar6 = pTVar2->bSubType;
          }
          if (BVar6 != '\x01') {
            BVar6 = '\0';
            if (pTVar2 != (TItemTable *)0x0) {
              BVar6 = pTVar2->bSubType;
            }
            if (BVar6 != '\x04') {
              BVar6 = '\0';
              if (pTVar2 != (TItemTable *)0x0) {
                BVar6 = pTVar2->bSubType;
              }
              if (BVar6 != '\x02') goto LAB_0812c0bb;
            }
          }
        }
        lVar12 = CItem::GetValue(this_00,1);
        lVar9 = CItem::GetValue(this_00,5);
        iArmor = iArmor + lVar12 + lVar9 * 2;
      }
    }
LAB_0812c0bb:
    iVar14 = iVar14 + 1;
    if (0x12 < iVar14) {
      if ((this->m_dwMountVnum != 0) &&
         (iArmor < *(int *)(c_aHorseStat +
                           (uint)(this->super_CHorseRider).m_Horse.bLevel * 0x30 + 0x2c))) {
        iArmor = *(int *)(c_aHorseStat +
                         (uint)(this->super_CHorseRider).m_Horse.bLevel * 0x30 + 0x2c);
      }
      bVar5 = LC_IsYMIR();
      if (bVar5) {
        PointChange(this,'\x10',iVar13 + iArmor,false,false);
      }
      else {
        PointChange(this,'\x10',(uint)bVar1 + (int)ROUND(fVar3) + iArmor,false,false);
      }
      lVar12 = GetPoint(this,'\x10');
      PointChange(this,'\x14',(iVar13 + iArmor) - lVar12,false,false);
      bVar1 = (this->m_points).level;
      lVar12 = GetPoint(this,'\x0f');
      PointChange(this,'\x16',((uint)bVar1 + lVar12) * 2,false,false);
      bVar1 = (this->m_points).level;
      lVar12 = GetPoint(this,'\x0f');
      lVar9 = GetPoint(this,'\r');
      PointChange(this,'\x17',(uint)bVar1 + (lVar9 + lVar12 * 3) / 3 + iArmor / 2,false,false);
      lVar12 = GetPoint(this,'_');
      PointChange(this,'\x12',lVar12,false,false);
      lVar12 = GetPoint(this,'`');
      PointChange(this,'\x10',lVar12,false,false);
      lVar12 = GetPoint(this,'o');
      PointChange(this,'\x10',lVar12,false,false);
      lVar12 = GetPoint(this,'a');
      PointChange(this,'\x16',lVar12,false,false);
      lVar12 = GetPoint(this,'b');
      PointChange(this,'\x17',lVar12,false,false);
      return;
    }
  } while( true );
}

