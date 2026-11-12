
/* DWARF original prototype: void ComputePoints(CHARACTER * this) */

void __thiscall CHARACTER::ComputePoints(CHARACTER *this)

{
  DWORD *pDVar1;
  int iVar2;
  byte bVar3;
  byte bVar4;
  WORD WVar5;
  long lVar6;
  long val;
  long val_00;
  long val_01;
  long val_02;
  long val_03;
  long val_04;
  long val_05;
  long val_06;
  long val_07;
  long val_08;
  long val_09;
  long val_10;
  long val_11;
  uint uVar7;
  LPITEM pCVar8;
  CHARACTER_POINT_INSTANT *pCVar9;
  DWORD DVar10;
  int iVar11;
  BYTE bCell;
  CHARACTER *this_local;
  int iMaxSP;
  int iMaxHP;
  long lSPRecovery;
  long lHPRecovery;
  long lDefenderBonus;
  long lHasteBonus;
  long lSkillMasterBonus;
  long lBufferBonus;
  long lTankerBonus;
  long lAttackerBonus;
  long lLevelStep;
  long lSkillHorse;
  long lStatResetCount;
  long lStat;
  
                    /* Unresolved local var: long lSkillActive@[DW_OP_reg3(EBX)]
                       Unresolved local var: long lSkillSub@[DW_OP_reg6(ESI)]
                       Unresolved local var: int iMaxStamina@[DW_OP_reg6(ESI)] */
  lVar6 = GetPoint(this,'\x1a');
  val = GetPoint(this,'p');
  val_00 = GetPoint(this,'\x1c');
  val_01 = GetPoint(this,'\x1b');
  val_02 = GetPoint(this,'q');
  val_03 = GetPoint(this,'\x19');
  val_04 = GetPoint(this,'[');
  val_05 = GetPoint(this,'\\');
  val_06 = GetPoint(this,'g');
  val_07 = GetPoint(this,'h');
  val_08 = GetPoint(this,'n');
  val_09 = GetPoint(this,'o');
  val_10 = GetPoint(this,'#');
  val_11 = GetPoint(this,'$');
  pCVar9 = &this->m_pointsInstant;
  uVar7 = 0x3fc;
  if (((uint)pCVar9 & 4) != 0) {
    pCVar9 = (CHARACTER_POINT_INSTANT *)((this->m_pointsInstant).points + 1);
    (this->m_pointsInstant).points[0] = 0;
    uVar7 = 0x3f8;
  }
  for (uVar7 = uVar7 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
    pCVar9->points[0] = 0;
    pCVar9 = (CHARACTER_POINT_INSTANT *)(pCVar9->points + 1);
  }
  SetPoint(this,'\x1a',lVar6);
  SetPoint(this,'\x1c',val_00);
  SetPoint(this,'\x1b',val_01);
  SetPoint(this,'q',val_02);
  SetPoint(this,'\x19',val_03);
  SetPoint(this,'p',val);
  lVar6 = GetRealPoint(this,'\f');
  SetPoint(this,'\f',lVar6);
  lVar6 = GetRealPoint(this,'\r');
  SetPoint(this,'\r',lVar6);
  lVar6 = GetRealPoint(this,'\x0e');
  SetPoint(this,'\x0e',lVar6);
  lVar6 = GetRealPoint(this,'\x0f');
  SetPoint(this,'\x0f',lVar6);
  WVar5 = GetOriginalPart(this,'\0');
  SetPart(this,'\0',WVar5);
  WVar5 = GetOriginalPart(this,'\x01');
  SetPart(this,'\x01',WVar5);
  WVar5 = GetOriginalPart(this,'\x02');
  SetPart(this,'\x02',WVar5);
  WVar5 = GetOriginalPart(this,'\x03');
  SetPart(this,'\x03',WVar5);
  SetPoint(this,'[',val_04);
  SetPoint(this,'\\',val_05);
  SetPoint(this,'g',val_06);
  SetPoint(this,'h',val_07);
  SetPoint(this,'n',val_08);
  SetPoint(this,'o',val_09);
  SetPoint(this,'#',val_10);
  SetPoint(this,'$',val_11);
  SetPoint(this,'}',0);
  SetPoint(this,'~',0);
  if ((this->super_CEntity).m_lpDesc == (LPDESC)0x0) {
    iMaxSP = 0;
    iVar11 = 0;
    iMaxHP = (this->m_pkMobData->m_table).dwMaxHP;
    SetPoint(this,'\x11',(int)(this->m_pkMobData->m_table).sAttackSpeed);
    SetPoint(this,'\x13',(int)(this->m_pkMobData->m_table).sMovingSpeed);
    lVar6 = (long)(this->m_pkMobData->m_table).sAttackSpeed;
  }
  else {
    bVar3 = GetJob(this);
    iVar11 = (this->m_points).iRandomHP;
    iVar2 = *(int *)(JobInitialPoints + (uint)bVar3 * 0x40 + 0x10);
    lVar6 = GetPoint(this,'\r');
    bVar3 = GetJob(this);
    iMaxHP = iVar11 + iVar2 + lVar6 * *(int *)(JobInitialPoints + (uint)bVar3 * 0x40 + 0x18);
    bVar3 = GetJob(this);
    iVar11 = (this->m_points).iRandomSP;
    iVar2 = *(int *)(JobInitialPoints + (uint)bVar3 * 0x40 + 0x14);
    lVar6 = GetPoint(this,'\x0f');
    bVar3 = GetJob(this);
    iMaxSP = iVar11 + iVar2 + lVar6 * *(int *)(JobInitialPoints + (uint)bVar3 * 0x40 + 0x1c);
    bVar3 = GetJob(this);
    lVar6 = GetPoint(this,'\r');
    bVar4 = GetJob(this);
    iVar11 = lVar6 * *(int *)(JobInitialPoints + (uint)bVar4 * 0x40 + 0x34) +
             *(int *)(JobInitialPoints + (uint)bVar3 * 0x40 + 0x30);
    SetPoint(this,'\x13',100);
    SetPoint(this,'\x11',100);
    lVar6 = GetPoint(this,'n');
    PointChange(this,'\x11',lVar6,false,false);
    lVar6 = 100;
  }
  SetPoint(this,'\x15',lVar6);
  if (((this->super_CEntity).m_lpDesc != (LPDESC)0x0) && (this->m_dwMountVnum != 0)) {
    iVar2 = *(int *)(c_aHorseStat + (uint)(this->super_CHorseRider).m_Horse.bLevel * 0x30 + 0x10);
    lVar6 = GetPoint(this,'\f');
    if (lVar6 < iVar2) {
      iVar2 = *(int *)(c_aHorseStat + (uint)(this->super_CHorseRider).m_Horse.bLevel * 0x30 + 0x10);
      lVar6 = GetPoint(this,'\f');
      PointChange(this,'\f',iVar2 - lVar6,false,false);
    }
    iVar2 = *(int *)(c_aHorseStat + (uint)(this->super_CHorseRider).m_Horse.bLevel * 0x30 + 0x14);
    lVar6 = GetPoint(this,'\x0e');
    if (lVar6 < iVar2) {
      iVar2 = *(int *)(c_aHorseStat + (uint)(this->super_CHorseRider).m_Horse.bLevel * 0x30 + 0x14);
      lVar6 = GetPoint(this,'\x0e');
      PointChange(this,'\x0e',iVar2 - lVar6,false,false);
    }
    iVar2 = *(int *)(c_aHorseStat + (uint)(this->super_CHorseRider).m_Horse.bLevel * 0x30 + 0x18);
    lVar6 = GetPoint(this,'\r');
    if (lVar6 < iVar2) {
      iVar2 = *(int *)(c_aHorseStat + (uint)(this->super_CHorseRider).m_Horse.bLevel * 0x30 + 0x18);
      lVar6 = GetPoint(this,'\r');
      PointChange(this,'\r',iVar2 - lVar6,false,false);
    }
    iVar2 = *(int *)(c_aHorseStat + (uint)(this->super_CHorseRider).m_Horse.bLevel * 0x30 + 0x1c);
    lVar6 = GetPoint(this,'\x0f');
    if (lVar6 < iVar2) {
      iVar2 = *(int *)(c_aHorseStat + (uint)(this->super_CHorseRider).m_Horse.bLevel * 0x30 + 0x1c);
      lVar6 = GetPoint(this,'\x0f');
      PointChange(this,'\x0f',iVar2 - lVar6,false,false);
    }
  }
  ComputeBattlePoints(this);
  if (iMaxHP != (this->m_pointsInstant).iMaxHP) {
    SetRealPoint(this,'\x06',iMaxHP);
  }
  PointChange(this,'\x06',0,false,false);
  if (iMaxSP != (this->m_pointsInstant).iMaxSP) {
    SetRealPoint(this,'\b',iMaxSP);
  }
  PointChange(this,'\b',0,false,false);
  (this->m_pointsInstant).iMaxStamina = iVar11;
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)] */
  iVar11 = 0;
  (this->m_pointsInstant).dwImmuneFlag = 0;
  do {
    bCell = (BYTE)iVar11;
    pCVar8 = GetWear(this,bCell);
    if (pCVar8 != (LPITEM)0x0) {
      pCVar8 = GetWear(this,bCell);
      CItem::ModifyPoints(pCVar8,true);
      pCVar8 = GetWear(this,bCell);
      DVar10 = 0;
      if (pCVar8->m_pProto != (TItemTable *)0x0) {
        DVar10 = pCVar8->m_pProto->dwImmuneFlag;
      }
      pDVar1 = &(this->m_pointsInstant).dwImmuneFlag;
      *pDVar1 = *pDVar1 | DVar10;
    }
    iVar11 = iVar11 + 1;
  } while (iVar11 < 0x13);
  iVar11 = (this->m_points).hp;
  iVar2 = (this->m_pointsInstant).iMaxHP;
  if (iVar2 < iVar11) {
    PointChange(this,'\x05',iVar2 - iVar11,false,false);
  }
  iVar11 = (this->m_points).sp;
  iVar2 = (this->m_pointsInstant).iMaxSP;
  if (iVar2 < iVar11) {
    PointChange(this,'\a',iVar2 - iVar11,false,false);
  }
  UpdatePacket(this);
  ComputeSkillPoints(this);
  RefreshAffect(this);
  return;
}

