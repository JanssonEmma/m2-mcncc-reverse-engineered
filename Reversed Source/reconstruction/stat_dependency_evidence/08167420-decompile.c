
/* DWARF original prototype: void ModifyPoints(CItem * this, bool bAdd) */

void __thiscall CItem::ModifyPoints(CItem *this,bool bAdd)

{
  byte bVar1;
  bool bVar2;
  BYTE BVar3;
  WORD wVal;
  uint uVar4;
  char *pcVar5;
  int iVar6;
  TItemTable *pTVar7;
  CHARACTER *pCVar8;
  CItem *pCVar9;
  int iVar10;
  bool bAdd_local;
  
                    /* Unresolved local var: int i@[DW_OP_reg7(EDI)] */
  bVar2 = IsAccessoryForSocket(this);
  if (bVar2) {
    pTVar7 = this->m_pProto;
  }
  else {
    pTVar7 = this->m_pProto;
    if ((byte)(pTVar7->bType - 1) < 2) {
      iVar10 = 0;
      do {
                    /* Unresolved local var: DWORD dwVnum@[DW_OP_reg3(EBX)]
                       Unresolved local var: TItemTable * p@[DW_OP_reg0(EAX)] */
        uVar4 = this->m_alSockets[iVar10];
        if (2 < uVar4) {
          if (singleton<ITEM_MANAGER>::ms_singleton == (ITEM_MANAGER *)0x0) {
            __assert("instance","../../common/singleton.h",0x1a);
          }
          pTVar7 = ITEM_MANAGER::GetTable(singleton<ITEM_MANAGER>::ms_singleton,uVar4);
          if (pTVar7 == (TItemTable *)0x0) {
            _sys_err("ModifyPoints",0x247,"cannot find table by vnum %u",uVar4);
          }
          else if (pTVar7->bType == '\n') {
            iVar6 = 2;
            do {
                    /* Unresolved local var: int i@[DW_OP_reg6(ESI)] */
              BVar3 = pTVar7->aApplies[0].bType;
              if (BVar3 != '\0') {
                if (BVar3 == '3') {
                  uVar4 = pTVar7->aApplies[0].lValue;
                  if (!bAdd) {
                    uVar4 = uVar4 ^ 0x800000;
                  }
                  BVar3 = pTVar7->aApplies[0].bType;
                  pCVar8 = this->m_pOwner;
                }
                else {
                  uVar4 = pTVar7->aApplies[0].lValue;
                  if (!bAdd) {
                    uVar4 = -uVar4;
                  }
                  BVar3 = pTVar7->aApplies[0].bType;
                  pCVar8 = this->m_pOwner;
                }
                CHARACTER::ApplyPoint(pCVar8,BVar3,uVar4);
              }
              pTVar7 = (TItemTable *)(pTVar7->szName + 1);
              iVar6 = iVar6 + -1;
            } while (-1 < iVar6);
          }
        }
        iVar10 = iVar10 + 1;
      } while (iVar10 < 3);
      pTVar7 = this->m_pProto;
    }
  }
  iVar10 = 0;
  while( true ) {
    if (pTVar7->aApplies[iVar10].bType != '\0') {
      sys_log(2,"%s %d %d",(uint)pTVar7->szLocaleName & (pTVar7 == (TItemTable *)0x0) - 1,
              (uint)this->m_pProto->aApplies[iVar10].bType,pTVar7->aApplies[iVar10].lValue);
      pTVar7 = this->m_pProto;
      if (pTVar7->aApplies[iVar10].bType == '3') {
        uVar4 = pTVar7->aApplies[iVar10].lValue;
        if (!bAdd) {
          uVar4 = uVar4 ^ 0x800000;
        }
        BVar3 = pTVar7->aApplies[iVar10].bType;
        pCVar8 = this->m_pOwner;
      }
      else {
        bVar2 = IsAccessoryForSocket(this);
        if (bVar2) {
                    /* Unresolved local var: long value@[DW_OP_reg6(ESI)]
                       Unresolved local var: int grade@[DW_OP_reg1(ECX)] */
          uVar4 = this->m_pProto->aApplies[iVar10].lValue;
          iVar6 = MINMAX(0,this->m_alSockets[0],3);
          if (iVar6 != 0) {
            iVar6 = MAX(iVar6,(int)(uVar4 * *(int *)(aiAccessorySocketEffectivePct + iVar6 * 4)) /
                              100);
            uVar4 = uVar4 + iVar6;
          }
          if (!bAdd) {
            uVar4 = -uVar4;
          }
          pcVar5 = this->m_pProto->szName + iVar10 * 5 + -4;
        }
        else {
          pcVar5 = this->m_pProto->szName + iVar10 * 5 + -4;
          uVar4 = *(uint *)(pcVar5 + 0x5d);
          if (!bAdd) {
            uVar4 = -uVar4;
          }
        }
        BVar3 = pcVar5[0x5c];
        pCVar8 = this->m_pOwner;
      }
      CHARACTER::ApplyPoint(pCVar8,BVar3,uVar4);
    }
    iVar10 = iVar10 + 1;
    if (2 < iVar10) break;
    pTVar7 = this->m_pProto;
  }
  iVar10 = 6;
  pCVar9 = this;
  do {
    BVar3 = pCVar9->m_aAttr[0].bType;
    if (BVar3 != '\0') {
                    /* Unresolved local var: TPlayerItemAttribute * ia@[???] */
      if (BVar3 == '3') {
        if (bAdd) {
          uVar4 = (uint)pCVar9->m_aAttr[0].sValue;
        }
        else {
          uVar4 = (int)pCVar9->m_aAttr[0].sValue ^ 0x800000;
        }
        BVar3 = pCVar9->m_aAttr[0].bType;
        pCVar8 = this->m_pOwner;
      }
      else {
        if (bAdd) {
          uVar4 = (uint)pCVar9->m_aAttr[0].sValue;
        }
        else {
          uVar4 = -(int)pCVar9->m_aAttr[0].sValue;
        }
        BVar3 = pCVar9->m_aAttr[0].bType;
        pCVar8 = this->m_pOwner;
      }
      CHARACTER::ApplyPoint(pCVar8,BVar3,uVar4);
    }
    pCVar9 = (CItem *)((int)&(pCVar9->super_CEntity)._vptr_CEntity + 3);
    iVar10 = iVar10 + -1;
  } while (-1 < iVar10);
  pTVar7 = this->m_pProto;
  bVar1 = pTVar7->bType;
  if (bVar1 == 2) {
    BVar3 = '\0';
    if (pTVar7 != (TItemTable *)0x0) {
      BVar3 = pTVar7->bSubType;
    }
    if (BVar3 != '\0') {
      BVar3 = '\0';
      if (pTVar7 != (TItemTable *)0x0) {
        BVar3 = pTVar7->bSubType;
      }
      if (BVar3 != '\x01') {
        BVar3 = '\0';
        if (pTVar7 != (TItemTable *)0x0) {
          BVar3 = pTVar7->bSubType;
        }
        if (BVar3 != '\x04') {
          BVar3 = '\0';
          if (pTVar7 != (TItemTable *)0x0) {
            BVar3 = pTVar7->bSubType;
          }
          if (BVar3 != '\x02') {
            return;
          }
        }
      }
    }
    if (bAdd) {
      if (pTVar7->bSubType != '\0') {
        return;
      }
      wVal = (WORD)pTVar7->dwVnum;
    }
    else {
      if (pTVar7->bSubType != '\0') {
        return;
      }
      wVal = CHARACTER::GetOriginalPart(this->m_pOwner,'\0');
    }
    BVar3 = '\0';
    pCVar8 = this->m_pOwner;
    goto LAB_0816753e;
  }
  if (bVar1 < 3) {
    if (bVar1 != 1) {
      return;
    }
    if (bAdd) {
      BVar3 = this->m_bCell;
      goto LAB_0816751e;
    }
LAB_08167548:
    if (this->m_bCell != '^') {
      return;
    }
    wVal = CHARACTER::GetOriginalPart(this->m_pOwner,'\x01');
  }
  else {
    if ((bVar1 != 0xd) && (bVar1 != 0x18)) {
      return;
    }
    if (!bAdd) goto LAB_08167548;
    BVar3 = this->m_bCell;
LAB_0816751e:
    if (BVar3 != '^') {
      return;
    }
    wVal = 0;
    if (pTVar7 != (TItemTable *)0x0) {
      wVal = (WORD)pTVar7->dwVnum;
    }
  }
  BVar3 = '\x01';
  pCVar8 = this->m_pOwner;
LAB_0816753e:
  CHARACTER::SetPart(pCVar8,BVar3,wVal);
  return;
}

