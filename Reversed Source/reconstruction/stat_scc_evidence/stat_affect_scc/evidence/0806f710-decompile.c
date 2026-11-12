
/* WARNING: Type propagation algorithm not settling */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* DWARF original prototype: int ComputeSkill(CHARACTER * this, DWORD dwVnum, LPCHARACTER pkVictim,
   BYTE bSkillLevel) */

int __thiscall
CHARACTER::ComputeSkill(CHARACTER *this,DWORD dwVnum,LPCHARACTER pkVictim,BYTE bSkillLevel)

{
  int *piVar1;
  byte bVar2;
  SECTREE *pSVar3;
  _List_node<SECTREE*> *p_Var4;
  LPSECTREE pSVar5;
  long lVar6;
  float fVar7;
  _Rb_tree_iterator<CEntity*,CEntity*_const&,CEntity*_const*> _Var8;
  BYTE BVar9;
  CSkillProto *this_00;
  int iVar10;
  long lVar11;
  long lVar12;
  LPITEM pkWeapon_00;
  int iVar13;
  uint uVar14;
  _List_node<SECTREE*> *p_Var15;
  CPoly *this_01;
  _Rb_tree_base_iterator _Var16;
  int iVar17;
  TSkillUseInfo *pTVar18;
  DWORD DVar19;
  CPoly *pCVar20;
  bool bVar21;
  bool bVar22;
  longdouble lVar23;
  longdouble lVar24;
  float fVar25;
  undefined8 uVar26;
  double dVar27;
  DWORD DVar28;
  bool bAdded;
  int iDur_1;
  int iDur;
  bool bAdded_1;
  int iAmount3;
  int iAmount2;
  int iAmount;
  LPITEM pkWeapon;
  float k;
  CSkillProto *pkSk;
  BYTE bSkillLevel_local;
  _Rb_tree_iterator<CEntity*,CEntity*_const&,CEntity*_const*> it_4;
  _Rb_tree_iterator<CEntity*,CEntity*_const&,CEntity*_const*> it_2;
  _Rb_tree_iterator<CEntity*,CEntity*_const&,CEntity*_const*> it_1;
  _Rb_tree_iterator<CEntity*,CEntity*_const&,CEntity*_const*> it;
  _Rb_tree_iterator<CEntity*,CEntity*_const&,CEntity*_const*> it_3;
  LPITEM pkBow;
  LPITEM pkArrow;
  LPITEM pkBow_1;
  LPITEM pkArrow_1;
  FuncSplashAffect f;
  _List_iterator<SECTREE*,SECTREE*&,SECTREE**> it_tree;
  _List_iterator<SECTREE*,SECTREE*&,SECTREE**> __tmp;
  _Rb_tree_iterator<std::pair<const_int,_TSkillUseInfo>,std::pair<const_int,_TSkillUseInfo>&,std::pair<const_int,_TSkillUseInfo>*>
  __i;
  _Rb_tree<int,std::pair<const_int,_TSkillUseInfo>,std::_Select1st<std::pair<const_int,_TSkillUseInfo>_>,std::less<int>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>_>
  local_bc;
  _Rb_tree_base_iterator local_ac;
  _Rb_tree_iterator<std::pair<const_int,_TSkillUseInfo>,std::pair<const_int,_TSkillUseInfo>&,std::pair<const_int,_TSkillUseInfo>*>
  __i_1;
  FuncSplashDamage f_3;
  allocator local_5c [16];
  allocator local_4c [16];
  string local_3c [7];
  float local_20;
  
                    /* Unresolved local var: bool bOnHorse@[DW_OP_reg3(EBX)] */
  bSkillLevel_local = bSkillLevel;
  bVar21 = (this->super_CHorseRider).m_Horse.bRiding != '\0';
  if (bVar21) {
    BVar9 = CHorseRider::GetHorseGrade(&this->super_CHorseRider);
    bVar22 = BVar9 == '\x03';
  }
  else {
    bVar22 = this->m_dwMountVnum == 0;
  }
  if (!bVar22) {
    return 0;
  }
  if (this->m_dwPolymorphRace != 0) {
    return 0;
  }
  if (g_bSkillDisable != '\0') {
    return 0;
  }
  if (singleton<CSkillManager>::ms_singleton == (CSkillManager *)0x0) {
    __assert("instance","../../common/singleton.h",0x1a);
  }
  this_00 = CSkillManager::Get(singleton<CSkillManager>::ms_singleton,dwVnum);
  if (this_00 == (CSkillProto *)0x0) {
    return 0;
  }
  if (bVar21) {
    if (this_00->bType != 5) {
      return 0;
    }
  }
  else if (this_00->bType == 5) {
    return 0;
  }
  if ((this_00->dwFlag & 8) != 0) {
    pkVictim = this;
  }
  if (pkVictim == (LPCHARACTER)0x0) {
    _sys_err("ComputeSkill",0x775,"Victim is null while using skill %d");
    return 0;
  }
  if (this_00->dwTargetRange != 0) {
    iVar10 = (this->super_CEntity).m_pos.x - (pkVictim->super_CEntity).m_pos.x;
    iVar17 = (this->super_CEntity).m_pos.y - (pkVictim->super_CEntity).m_pos.y;
    dVar27 = sqrt((double)(iVar10 * iVar10 + iVar17 * iVar17));
    local_20 = (float)dVar27;
    if ((float)(this_00->dwTargetRange + 0x32) <= local_20) {
      iVar10 = (this->super_CEntity).m_pos.x - (pkVictim->super_CEntity).m_pos.x;
      iVar17 = (this->super_CEntity).m_pos.y - (pkVictim->super_CEntity).m_pos.y;
      dVar27 = sqrt((double)(iVar10 * iVar10 + iVar17 * iVar17));
      local_20 = (float)dVar27;
      GetName(pkVictim);
      GetName(this);
      _sys_err("ComputeSkill",0x77c,"Victim Too Far skill %d : %s to %s (distance %lu limit %lu)");
      return 0;
    }
  }
  if (bSkillLevel == '\0') {
    iVar10 = GetSkillLevel(this,this_00->dwVnum);
    bSkillLevel_local = (BYTE)iVar10;
    if (bSkillLevel_local == '\0') {
      return 0;
    }
  }
  bVar21 = IsAffectFlag(pkVictim,0x21);
  if ((bVar21) && (bVar21 = IsGoodAffect(pkVictim,(BYTE)dwVnum), bVar21)) {
    return 0;
  }
  iVar10 = GetSkillPower(this,this_00->dwVnum,bSkillLevel_local);
  fVar7 = ((float)this_00->bMaxLevel * (float)iVar10) / (float)_DAT_081f9f48;
  std::string::string((string *)local_3c,"k",local_4c);
  iVar10 = GetSkillPower(this,this_00->dwVnum,bSkillLevel_local);
  CSkillProto::SetPointVar
            (this_00,local_3c,((double)this_00->bMaxLevel * (double)iVar10) / _DAT_081f9f48);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  std::string::string((string *)local_3c,"k",local_5c);
  CPoly::SetVar(&this_00->kSplashAroundDamageAdjustPoly,(string *)local_3c,(double)fVar7);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  if (this_00->bType == 5) {
    iVar10 = GetArrowAndBow(this,&pkBow_1,&pkArrow_1,1);
    if (iVar10 != 1) goto LAB_08071acf;
    std::string::string((string *)local_3c,"atk",local_5c);
    pkBow = pkBow_1;
    pkArrow = pkArrow_1;
LAB_08071a2e:
    iVar10 = CalcArrowDamage(this,pkVictim,pkBow,pkArrow,true);
    CSkillProto::SetPointVar(this_00,local_3c,(double)iVar10);
    LOCK();
    piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
    iVar10 = *piVar1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
joined_r0x08071a70:
    if (iVar10 < 1) {
      std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
    }
  }
  else {
    uVar14 = this_00->dwFlag;
    if ((uVar14 & 2) != 0) {
LAB_08071acf:
      std::string::string((string *)local_3c,"atk",local_5c);
      iVar10 = CalcMeleeDamage(this,pkVictim,true,false);
      CSkillProto::SetPointVar(this_00,local_3c,(double)iVar10);
      LOCK();
      piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
      iVar10 = *piVar1;
      *piVar1 = *piVar1 + -1;
      UNLOCK();
      goto joined_r0x08071a70;
    }
    if ((uVar14 & 0x10) != 0) {
      std::string::string((string *)local_3c,"atk",local_5c);
      iVar10 = CalcMagicDamage(this,pkVictim);
      dVar27 = (double)iVar10;
LAB_080719cc:
      CSkillProto::SetPointVar(this_00,local_3c,dVar27);
      LOCK();
      piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
      iVar10 = *piVar1;
      *piVar1 = *piVar1 + -1;
      UNLOCK();
      goto joined_r0x08071a70;
    }
    if ((uVar14 & 0x200) != 0) {
      iVar10 = GetArrowAndBow(this,&pkBow,&pkArrow,1);
      if (iVar10 == 1) {
        std::string::string((string *)local_3c,"atk",local_5c);
        goto LAB_08071a2e;
      }
      std::string::string((string *)local_3c,"atk",local_5c);
      dVar27 = 0.0;
      goto LAB_080719cc;
    }
  }
  if (this_00->bPointOn == '\x13') {
    std::string::string((string *)local_3c,"maxv",local_5c);
    lVar11 = GetLimitPoint(pkVictim,'\x13');
    CSkillProto::SetPointVar(this_00,local_3c,(double)lVar11);
    LOCK();
    piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
    iVar10 = *piVar1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (iVar10 < 1) {
      std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
    }
  }
  bVar2 = (this->m_points).level;
  std::string::string((string *)local_3c,"lv",local_5c);
  CSkillProto::SetPointVar(this_00,local_3c,(double)bVar2);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  std::string::string((string *)local_3c,"iq",local_5c);
  lVar11 = GetPoint(this,'\x0f');
  CSkillProto::SetPointVar(this_00,local_3c,(double)lVar11);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  std::string::string((string *)local_3c,"str",local_5c);
  lVar11 = GetPoint(this,'\f');
  CSkillProto::SetPointVar(this_00,local_3c,(double)lVar11);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  std::string::string((string *)local_3c,"dex",local_5c);
  lVar11 = GetPoint(this,'\x0e');
  CSkillProto::SetPointVar(this_00,local_3c,(double)lVar11);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  std::string::string((string *)local_3c,"con",local_5c);
  lVar11 = GetPoint(this,'\r');
  CSkillProto::SetPointVar(this_00,local_3c,(double)lVar11);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  iVar10 = (pkVictim->m_pointsInstant).iMaxHP;
  std::string::string((string *)local_3c,"maxhp",local_5c);
  CSkillProto::SetPointVar(this_00,local_3c,(double)iVar10);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  iVar10 = (pkVictim->m_pointsInstant).iMaxSP;
  std::string::string((string *)local_3c,"maxsp",local_5c);
  CSkillProto::SetPointVar(this_00,local_3c,(double)iVar10);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  std::string::string((string *)local_3c,"chain",local_5c);
  CSkillProto::SetPointVar(this_00,local_3c,0.0);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  std::string::string((string *)local_3c,"ar",local_5c);
  fVar25 = CalcAttackRating(this,pkVictim,false);
  CSkillProto::SetPointVar(this_00,local_3c,(double)fVar25);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  std::string::string((string *)local_3c,"def",local_5c);
  lVar11 = GetPoint(this,'\x10');
  CSkillProto::SetPointVar(this_00,local_3c,(double)lVar11);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  std::string::string((string *)local_3c,"odef",local_5c);
  lVar11 = GetPoint(this,'\x10');
  lVar12 = GetPoint(this,'`');
  CSkillProto::SetPointVar(this_00,local_3c,(double)(lVar11 - lVar12));
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  bVar2 = (this->super_CHorseRider).m_Horse.bLevel;
  std::string::string((string *)local_3c,"horse_level",local_5c);
  CSkillProto::SetPointVar(this_00,local_3c,(double)bVar2);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  if (this_00->bSkillAttrType != '\x01') {
    OnMove(this,true);
  }
  pkWeapon_00 = GetWear(this,'\x04');
  pCVar20 = &this_00->kDurationPoly;
  SetPolyVarForAttack(this,this_00,pkWeapon_00);
  std::string::string((string *)local_3c,"k",local_5c);
  CPoly::SetVar(pCVar20,(string *)local_3c,(double)fVar7);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  std::string::string((string *)local_3c,"k",local_5c);
  this_01 = &this_00->kDurationPoly2;
  CPoly::SetVar(this_01,(string *)local_3c,(double)fVar7);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  lVar23 = (longdouble)CPoly::Eval(&this_00->kPointPoly);
  lVar24 = (longdouble)CPoly::Eval(&this_00->kPointPoly2);
  iVar10 = (int)ROUND(lVar24);
  lVar24 = (longdouble)CPoly::Eval(&this_00->kPointPoly3);
  iVar17 = (int)ROUND(lVar24);
  iVar13 = GetUsedSkillMasterType(this,this_00->dwVnum);
  if (1 < iVar13) {
    lVar23 = (longdouble)CPoly::Eval(&this_00->kMasterBonusPoly);
  }
  iAmount = (int)ROUND(lVar23);
  if (((test_server != 0) && (iAmount == 0)) && (this_00->bPointOn != '\0')) {
    ChatPacket(this,'\x01',&DAT_081f93a0);
  }
  uVar14 = this_00->dwFlag;
  if ((uVar14 & 0x20000) != 0) {
    iVar13 = number_ex(1,100,"char_skill.cpp",0x7f9);
    if (iVar10 < iVar13) {
      uVar14 = this_00->dwFlag;
    }
    else {
      RemoveBadAffect(pkVictim);
      uVar14 = this_00->dwFlag;
    }
  }
  if ((((uVar14 & 0x13) != 0) && ((DVar19 = this_00->dwVnum, DVar19 != 0x4e || (pkVictim != this))))
     && (((DVar19 != 5 && (DVar19 != 0x8a)) || (pkVictim != this)))) {
    bAdded_1 = false;
    if ((this_00->bPointOn == '\x05') && (iAmount < 0)) {
                    /* Unresolved local var: int iAG@[???] */
      lVar11 = this_00->lMaxHit;
      lVar12 = (pkVictim->super_CEntity).m_pos.x;
      bVar21 = this->m_bDisableCooltime;
      lVar6 = (pkVictim->super_CEntity).m_pos.y;
      if ((this->super_CEntity).m_lpDesc == (LPDESC)0x0) {
        pTVar18 = (TSkillUseInfo *)0x0;
      }
      else {
        _Rb_tree<int,std::pair<const_int,_TSkillUseInfo>,std::_Select1st<std::pair<const_int,_TSkillUseInfo>_>,std::less<int>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>_>
        ::lower_bound((_Rb_tree<int,std::pair<const_int,_TSkillUseInfo>,std::_Select1st<std::pair<const_int,_TSkillUseInfo>_>,std::less<int>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>_>
                       *)&__i_1,(int *)&this->m_SkillUseInfo);
        local_ac._M_node =
             &((this->m_SkillUseInfo)._M_t.
               super__Rb_tree_base<std::pair<const_int,_TSkillUseInfo>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_TSkillUseInfo>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>,true>
              ._M_header)->super__Rb_tree_node_base;
        if ((__i_1.super__Rb_tree_base_iterator._M_node == (_Rb_tree_base_iterator)local_ac._M_node)
           || ((int)dwVnum < *(int *)((int)__i_1.super__Rb_tree_base_iterator._M_node + 0x10))) {
          __i = __i_1;
          _Rb_tree<int,std::pair<const_int,_TSkillUseInfo>,std::_Select1st<std::pair<const_int,_TSkillUseInfo>_>,std::less<int>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>_>
          ::insert_unique(&local_bc,
                          (_Rb_tree_iterator<std::pair<const_int,_TSkillUseInfo>,std::pair<const_int,_TSkillUseInfo>&,std::pair<const_int,_TSkillUseInfo>*>
                           )&this->m_SkillUseInfo,(pair<const_int,TSkillUseInfo> *)&__i);
          __i_1.super__Rb_tree_base_iterator._M_node =
               (_Rb_tree_base_iterator)
               (_Rb_tree_base_iterator)
               local_bc.
               super__Rb_tree_base<std::pair<const_int,_TSkillUseInfo>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_TSkillUseInfo>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>,true>
               ._M_header;
        }
        pTVar18 = (TSkillUseInfo *)((int)__i_1.super__Rb_tree_base_iterator._M_node + 0x14);
      }
      iVar13 = GetSkillPower(this,dwVnum,bSkillLevel_local);
      f_3.m_iAG = 0;
      f_3.m_iCount = 0;
      f_3.m_pkChr = this;
      f_3.m_iAmount = iAmount;
      f_3.m_bUseSkillPower = (BYTE)iVar13;
      f_3.m_x = lVar12;
      f_3.m_y = lVar6;
      f_3.m_pkSk = this_00;
      f_3.m_iMaxHit = lVar11;
      f_3.m_pkWeapon = pkWeapon_00;
      f_3.m_bDisableCooltime = bVar21;
      f_3.m_pInfo = pTVar18;
      if ((char)this_00->dwFlag < '\0') {
        pSVar3 = (pkVictim->super_CEntity).m_pSectree;
        if ((pSVar3 != (SECTREE *)0x0) &&
           (p_Var4 = (pSVar3->m_neighbor_list).super__List_base<SECTREE*,std::allocator<SECTREE*>_>.
                     super__List_alloc_base<SECTREE*,std::allocator<SECTREE*>,true>._M_node,
           p_Var15 = (_List_node<SECTREE*> *)(p_Var4->super__List_node_base)._M_next,
           p_Var15 != p_Var4)) {
          do {
            pSVar5 = p_Var15->_M_data;
            p_Var15 = (_List_node<SECTREE*> *)(p_Var15->super__List_node_base)._M_next;
            _Var16._M_node =
                 &((pSVar5->m_set_entity)._M_t.
                   super__Rb_tree_base<CEntity*,std::allocator<CEntity*>_>.
                   super__Rb_tree_alloc_base<CEntity*,std::allocator<CEntity*>,true>._M_header)->
                  super__Rb_tree_node_base;
            it_3.super__Rb_tree_base_iterator._M_node =
                 (_Rb_tree_base_iterator)
                 ((_Rb_tree_node_base *)&(_Var16._M_node)->_M_color)->_M_left;
            while (_Var8.super__Rb_tree_base_iterator._M_node =
                        it_3.super__Rb_tree_base_iterator._M_node,
                  it_3.super__Rb_tree_base_iterator._M_node !=
                  (_Rb_tree_base_iterator)_Var16._M_node) {
                    /* Unresolved local var:
                       _Rb_tree_iterator<CEntity*,CEntity*_const&,CEntity*_const*> __tmp@[???] */
              _Rb_tree_base_iterator::_M_increment(&it_3.super__Rb_tree_base_iterator);
              FuncSplashDamage::operator()
                        (&f_3,*(LPENTITY *)((int)_Var8.super__Rb_tree_base_iterator._M_node + 0x10))
              ;
              _Var16._M_node =
                   &((pSVar5->m_set_entity)._M_t.
                     super__Rb_tree_base<CEntity*,std::allocator<CEntity*>_>.
                     super__Rb_tree_alloc_base<CEntity*,std::allocator<CEntity*>,true>._M_header)->
                    super__Rb_tree_node_base;
            }
          } while (p_Var15 !=
                   (pSVar3->m_neighbor_list).super__List_base<SECTREE*,std::allocator<SECTREE*>_>.
                   super__List_alloc_base<SECTREE*,std::allocator<SECTREE*>,true>._M_node);
        }
      }
      else {
        FuncSplashDamage::operator()(&f_3,&pkVictim->super_CEntity);
      }
    }
    else {
      std::string::string((string *)local_3c,"k",local_5c);
      CPoly::SetVar(pCVar20,(string *)local_3c,(double)fVar7);
      LOCK();
      piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
      iVar13 = *piVar1;
      *piVar1 = *piVar1 + -1;
      UNLOCK();
      if (iVar13 < 1) {
        std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
      }
      lVar24 = (longdouble)CPoly::Eval(pCVar20);
      if ((((this->super_CEntity).m_lpDesc != (LPDESC)0x0) && (0xb < dwVnum - 0x97)) &&
         (this->m_bDisableCooltime == false)) {
        _Rb_tree<int,std::pair<const_int,_TSkillUseInfo>,std::_Select1st<std::pair<const_int,_TSkillUseInfo>_>,std::less<int>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>_>
        ::lower_bound((_Rb_tree<int,std::pair<const_int,_TSkillUseInfo>,std::_Select1st<std::pair<const_int,_TSkillUseInfo>_>,std::less<int>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>_>
                       *)&__i,(int *)&this->m_SkillUseInfo);
        if ((__i.super__Rb_tree_base_iterator._M_node ==
             (_Rb_tree_base_iterator)
             &((this->m_SkillUseInfo)._M_t.
               super__Rb_tree_base<std::pair<const_int,_TSkillUseInfo>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_TSkillUseInfo>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>,true>
              ._M_header)->super__Rb_tree_node_base) ||
           ((int)dwVnum < *(int *)((int)__i.super__Rb_tree_base_iterator._M_node + 0x10))) {
          local_ac = __i.super__Rb_tree_base_iterator._M_node;
          __i_1 = __i;
          _Rb_tree<int,std::pair<const_int,_TSkillUseInfo>,std::_Select1st<std::pair<const_int,_TSkillUseInfo>_>,std::less<int>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>_>
          ::insert_unique(&local_bc,
                          (_Rb_tree_iterator<std::pair<const_int,_TSkillUseInfo>,std::pair<const_int,_TSkillUseInfo>&,std::pair<const_int,_TSkillUseInfo>*>
                           )&this->m_SkillUseInfo,(pair<const_int,TSkillUseInfo> *)&__i_1);
          __i.super__Rb_tree_base_iterator._M_node =
               (_Rb_tree_base_iterator)
               (_Rb_tree_base_iterator)
               local_bc.
               super__Rb_tree_base<std::pair<const_int,_TSkillUseInfo>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_TSkillUseInfo>,std::allocator<std::pair<const_int,_TSkillUseInfo>_>,true>
               ._M_header;
        }
        bVar21 = TSkillUseInfo::HitOnce
                           ((TSkillUseInfo *)((int)__i.super__Rb_tree_base_iterator._M_node + 0x14),
                            dwVnum);
        if ((!bVar21) && (dwVnum != 0x4e)) {
          return 0;
        }
      }
      if (0 < (int)ROUND(lVar24)) {
        lVar11 = GetPoint(this,'g');
        iVar13 = (int)ROUND(lVar24) + lVar11;
        if ((char)this_00->dwFlag < '\0') {
          if ((pkVictim->super_CEntity).m_pSectree != (SECTREE *)0x0) {
            f.m_x = (pkVictim->super_CEntity).m_pos.x;
            f.m_y = (pkVictim->super_CEntity).m_pos.y;
            f.m_iDist = this_00->iSplashRange;
            f.m_dwVnum = this_00->dwVnum;
            f.m_bPointOn = this_00->bPointOn;
            f.m_iAmount = iAmount;
            f.m_dwAffectFlag = this_00->dwAffectFlag;
            f.m_bOverride = true;
            f.m_iMaxHit = this_00->lMaxHit;
            f.m_iCount = 0;
            f.m_iSPCost = 0;
            pSVar3 = (pkVictim->super_CEntity).m_pSectree;
            p_Var4 = (pSVar3->m_neighbor_list).super__List_base<SECTREE*,std::allocator<SECTREE*>_>.
                     super__List_alloc_base<SECTREE*,std::allocator<SECTREE*>,true>._M_node;
            f.m_pkChrAttacker = this;
            p_Var15 = (_List_node<SECTREE*> *)(p_Var4->super__List_node_base)._M_next;
            f.m_iDuration = iVar13;
            if (p_Var15 != p_Var4) {
              do {
                pSVar5 = p_Var15->_M_data;
                p_Var15 = (_List_node<SECTREE*> *)(p_Var15->super__List_node_base)._M_next;
                _Var16._M_node =
                     &((pSVar5->m_set_entity)._M_t.
                       super__Rb_tree_base<CEntity*,std::allocator<CEntity*>_>.
                       super__Rb_tree_alloc_base<CEntity*,std::allocator<CEntity*>,true>._M_header)
                      ->super__Rb_tree_node_base;
                it.super__Rb_tree_base_iterator._M_node =
                     (_Rb_tree_base_iterator)
                     ((_Rb_tree_node_base *)&(_Var16._M_node)->_M_color)->_M_left;
                while (_Var8.super__Rb_tree_base_iterator._M_node =
                            it.super__Rb_tree_base_iterator._M_node,
                      it.super__Rb_tree_base_iterator._M_node !=
                      (_Rb_tree_base_iterator)_Var16._M_node) {
                    /* Unresolved local var:
                       _Rb_tree_iterator<CEntity*,CEntity*_const&,CEntity*_const*> __tmp@[???] */
                  _Rb_tree_base_iterator::_M_increment(&it.super__Rb_tree_base_iterator);
                  FuncSplashAffect::operator()
                            (&f,*(LPENTITY *)
                                 ((int)_Var8.super__Rb_tree_base_iterator._M_node + 0x10));
                  _Var16._M_node =
                       &((pSVar5->m_set_entity)._M_t.
                         super__Rb_tree_base<CEntity*,std::allocator<CEntity*>_>.
                         super__Rb_tree_alloc_base<CEntity*,std::allocator<CEntity*>,true>._M_header
                        )->super__Rb_tree_node_base;
                }
              } while (p_Var15 !=
                       (pSVar3->m_neighbor_list).
                       super__List_base<SECTREE*,std::allocator<SECTREE*>_>.
                       super__List_alloc_base<SECTREE*,std::allocator<SECTREE*>,true>._M_node);
            }
          }
        }
        else {
          AddAffect(pkVictim,this_00->dwVnum,this_00->bPointOn,iAmount,this_00->dwAffectFlag,iVar13,
                    0,true,false);
        }
        bAdded_1 = true;
      }
    }
    if (((this_00->bPointOn2 != '\0') && (this_00->dwVnum != 5)) && (this_00->dwVnum != 0x8a)) {
      std::string::string((string *)local_3c,"k",local_5c);
      CPoly::SetVar(this_01,(string *)local_3c,(double)fVar7);
      LOCK();
      piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
      iVar13 = *piVar1;
      *piVar1 = *piVar1 + -1;
      UNLOCK();
      if (iVar13 < 1) {
        std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
      }
      lVar24 = (longdouble)CPoly::Eval(this_01);
      iVar13 = (int)ROUND(lVar24);
      sys_log(0,"try second %lu %d %d",this_00->dwVnum,(uint)this_00->bPointOn2,iVar13);
      if (iVar13 < 1) {
        PointChange(pkVictim,this_00->bPointOn2,iVar10,false,false);
      }
      else {
        lVar11 = GetPoint(this,'g');
        iVar13 = iVar13 + lVar11;
        if ((char)this_00->dwFlag < '\0') {
          if ((pkVictim->super_CEntity).m_pSectree != (SECTREE *)0x0) {
            f.m_x = (pkVictim->super_CEntity).m_pos.x;
            f.m_y = (pkVictim->super_CEntity).m_pos.y;
            f.m_iDist = this_00->iSplashRange;
            f.m_dwVnum = this_00->dwVnum;
            f.m_bPointOn = this_00->bPointOn2;
            f.m_iMaxHit = this_00->lMaxHit;
            f.m_bOverride = (bool)(bAdded_1 ^ 1);
            f.m_dwAffectFlag = this_00->dwAffectFlag2;
            f.m_iCount = 0;
            pSVar3 = (pkVictim->super_CEntity).m_pSectree;
            f.m_pkChrAttacker = this;
            f.m_iSPCost = 0;
            p_Var4 = (pSVar3->m_neighbor_list).super__List_base<SECTREE*,std::allocator<SECTREE*>_>.
                     super__List_alloc_base<SECTREE*,std::allocator<SECTREE*>,true>._M_node;
            p_Var15 = (_List_node<SECTREE*> *)(p_Var4->super__List_node_base)._M_next;
            f.m_iAmount = iVar10;
            f.m_iDuration = iVar13;
            if (p_Var15 != p_Var4) {
              do {
                pSVar5 = p_Var15->_M_data;
                p_Var15 = (_List_node<SECTREE*> *)(p_Var15->super__List_node_base)._M_next;
                _Var16._M_node =
                     &((pSVar5->m_set_entity)._M_t.
                       super__Rb_tree_base<CEntity*,std::allocator<CEntity*>_>.
                       super__Rb_tree_alloc_base<CEntity*,std::allocator<CEntity*>,true>._M_header)
                      ->super__Rb_tree_node_base;
                it_1.super__Rb_tree_base_iterator._M_node =
                     (_Rb_tree_base_iterator)
                     ((_Rb_tree_node_base *)&(_Var16._M_node)->_M_color)->_M_left;
                while (_Var8.super__Rb_tree_base_iterator._M_node =
                            it_1.super__Rb_tree_base_iterator._M_node,
                      it_1.super__Rb_tree_base_iterator._M_node !=
                      (_Rb_tree_base_iterator)_Var16._M_node) {
                    /* Unresolved local var:
                       _Rb_tree_iterator<CEntity*,CEntity*_const&,CEntity*_const*> __tmp@[???] */
                  _Rb_tree_base_iterator::_M_increment(&it_1.super__Rb_tree_base_iterator);
                  FuncSplashAffect::operator()
                            (&f,*(LPENTITY *)
                                 ((int)_Var8.super__Rb_tree_base_iterator._M_node + 0x10));
                  _Var16._M_node =
                       &((pSVar5->m_set_entity)._M_t.
                         super__Rb_tree_base<CEntity*,std::allocator<CEntity*>_>.
                         super__Rb_tree_alloc_base<CEntity*,std::allocator<CEntity*>,true>._M_header
                        )->super__Rb_tree_node_base;
                }
              } while (p_Var15 !=
                       (pSVar3->m_neighbor_list).
                       super__List_base<SECTREE*,std::allocator<SECTREE*>_>.
                       super__List_alloc_base<SECTREE*,std::allocator<SECTREE*>,true>._M_node);
            }
          }
        }
        else {
          AddAffect(pkVictim,this_00->dwVnum,this_00->bPointOn2,iVar10,this_00->dwAffectFlag2,iVar13
                    ,0,(bool)(bAdded_1 ^ 1),false);
        }
        bAdded_1 = true;
      }
    }
    if (((this_00->bPointOn3 != '\0') && (DVar19 = this_00->dwVnum, DVar19 != 5)) &&
       ((DVar19 != 0x8a && (iVar10 = GetUsedSkillMasterType(this,DVar19), 1 < iVar10)))) {
                    /* Unresolved local var: int iDur@[DW_OP_reg7(EDI)] */
      std::string::string((string *)local_3c,"k",local_5c);
      CPoly::SetVar(&this_00->kDurationPoly3,(string *)local_3c,(double)fVar7);
      LOCK();
      piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
      iVar10 = *piVar1;
      *piVar1 = *piVar1 + -1;
      UNLOCK();
      if (iVar10 < 1) {
        std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
      }
      lVar24 = (longdouble)CPoly::Eval(&this_00->kDurationPoly3);
      iVar10 = (int)ROUND(lVar24);
      sys_log(0,"try third %lu %d %d %d 1751",this_00->dwVnum,(uint)this_00->bPointOn3,iVar10,iVar17
             );
      if (iVar10 < 1) {
        PointChange(pkVictim,this_00->bPointOn3,iVar17,false,false);
      }
      else {
        lVar11 = GetPoint(this,'g');
        if ((char)this_00->dwFlag < '\0') {
          if ((pkVictim->super_CEntity).m_pSectree != (SECTREE *)0x0) {
            f.m_x = (pkVictim->super_CEntity).m_pos.x;
            f.m_y = (pkVictim->super_CEntity).m_pos.y;
            f.m_iDist = this_00->iSplashRange;
            f.m_dwVnum = this_00->dwVnum;
            f.m_bPointOn = this_00->bPointOn3;
            f.m_iMaxHit = this_00->lMaxHit;
            f.m_bOverride = (bool)(bAdded_1 ^ 1);
            f.m_iSPCost = 0;
            f.m_pkChrAttacker = this;
            pSVar3 = (pkVictim->super_CEntity).m_pSectree;
            f.m_iCount = 0;
            p_Var4 = (pSVar3->m_neighbor_list).super__List_base<SECTREE*,std::allocator<SECTREE*>_>.
                     super__List_alloc_base<SECTREE*,std::allocator<SECTREE*>,true>._M_node;
            p_Var15 = (_List_node<SECTREE*> *)(p_Var4->super__List_node_base)._M_next;
            f.m_dwAffectFlag = 0;
            if (p_Var15 != p_Var4) {
              f.m_iAmount = iVar17;
              f.m_iDuration = iVar10 + lVar11;
              do {
                pSVar5 = p_Var15->_M_data;
                p_Var15 = (_List_node<SECTREE*> *)(p_Var15->super__List_node_base)._M_next;
                _Var16._M_node =
                     &((pSVar5->m_set_entity)._M_t.
                       super__Rb_tree_base<CEntity*,std::allocator<CEntity*>_>.
                       super__Rb_tree_alloc_base<CEntity*,std::allocator<CEntity*>,true>._M_header)
                      ->super__Rb_tree_node_base;
                it_2.super__Rb_tree_base_iterator._M_node =
                     (_Rb_tree_base_iterator)
                     ((_Rb_tree_node_base *)&(_Var16._M_node)->_M_color)->_M_left;
                while (_Var8.super__Rb_tree_base_iterator._M_node =
                            it_2.super__Rb_tree_base_iterator._M_node,
                      it_2.super__Rb_tree_base_iterator._M_node !=
                      (_Rb_tree_base_iterator)_Var16._M_node) {
                    /* Unresolved local var:
                       _Rb_tree_iterator<CEntity*,CEntity*_const&,CEntity*_const*> __tmp@[???] */
                  _Rb_tree_base_iterator::_M_increment(&it_2.super__Rb_tree_base_iterator);
                  FuncSplashAffect::operator()
                            (&f,*(LPENTITY *)
                                 ((int)_Var8.super__Rb_tree_base_iterator._M_node + 0x10));
                  _Var16._M_node =
                       &((pSVar5->m_set_entity)._M_t.
                         super__Rb_tree_base<CEntity*,std::allocator<CEntity*>_>.
                         super__Rb_tree_alloc_base<CEntity*,std::allocator<CEntity*>,true>._M_header
                        )->super__Rb_tree_node_base;
                }
              } while (p_Var15 !=
                       (pSVar3->m_neighbor_list).
                       super__List_base<SECTREE*,std::allocator<SECTREE*>_>.
                       super__List_alloc_base<SECTREE*,std::allocator<SECTREE*>,true>._M_node);
            }
          }
        }
        else {
          AddAffect(pkVictim,this_00->dwVnum,this_00->bPointOn3,iVar17,0,iVar10 + lVar11,0,
                    (bool)(bAdded_1 ^ 1),false);
        }
      }
    }
    return 1;
  }
  if (dwVnum == 0x4e) {
                    /* Unresolved local var: int iDur@[DW_OP_reg3(EBX)] */
    std::string::string((string *)local_3c,"k",local_5c);
    CPoly::SetVar(pCVar20,(string *)local_3c,(double)fVar7);
    LOCK();
    piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
    iVar10 = *piVar1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (iVar10 < 1) {
      std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
    }
    std::string::string((string *)local_3c,"k",local_5c);
    CPoly::SetVar(&this_00->kDurationSPCostPoly,(string *)local_3c,(double)fVar7);
    LOCK();
    piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
    iVar10 = *piVar1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (iVar10 < 1) {
      std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
    }
    lVar24 = (longdouble)CPoly::Eval(pCVar20);
    lVar11 = GetPoint(this,'g');
    f.m_iDuration = (int)ROUND(lVar24) + lVar11;
    if (pkVictim != this) {
      return 0;
    }
    lVar24 = (longdouble)CPoly::Eval(&this_00->kDurationSPCostPoly);
    bVar21 = true;
    DVar28 = 0x1b;
    DVar19 = 0x4e;
    iVar10 = (int)ROUND(lVar24);
    uVar26 = 0;
    goto LAB_0807126a;
  }
  bAdded = false;
  std::string::string((string *)local_3c,"k",local_5c);
  CPoly::SetVar(pCVar20,(string *)local_3c,(double)fVar7);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar13 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar13 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  lVar24 = (longdouble)CPoly::Eval(pCVar20);
  if ((int)ROUND(lVar24) < 1) {
    if ((this_00->dwVnum != 5) && (this_00->dwVnum != 0x8a)) {
      PointChange(pkVictim,this_00->bPointOn,iAmount,false,false);
    }
    if (this_00->bPointOn2 != '\0') {
                    /* Unresolved local var: int iDur2@[DW_OP_reg3(EBX)] */
      RemoveAffect(pkVictim,this_00->dwVnum);
      lVar24 = (longdouble)CPoly::Eval(this_01);
      if ((int)ROUND(lVar24) < 1) {
        PointChange(pkVictim,this_00->bPointOn2,iVar10,false,false);
      }
      else {
        lVar11 = GetPoint(this,'g');
        if ((this_00->dwVnum == 5) || (this_00->dwVnum == 0x8a)) {
          DVar19 = 0x20;
          BVar9 = this_00->bPointOn2;
          DVar28 = this_00->dwVnum;
        }
        else {
          DVar19 = this_00->dwAffectFlag2;
          BVar9 = this_00->bPointOn2;
          DVar28 = this_00->dwVnum;
        }
        AddAffect(pkVictim,DVar28,BVar9,iVar10,DVar19,(int)ROUND(lVar24) + lVar11,0,false,false);
      }
    }
  }
  else {
    lVar11 = GetPoint(this,'g');
    std::string::string((string *)local_3c,"k",local_5c);
    pCVar20 = &this_00->kDurationSPCostPoly;
    CPoly::SetVar(pCVar20,(string *)local_3c,(double)fVar7);
    LOCK();
    piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
    iVar13 = *piVar1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (iVar13 < 1) {
      std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
    }
    if (this_00->bPointOn2 == '\0') {
      lVar23 = (longdouble)CPoly::Eval(pCVar20);
      bVar21 = true;
      DVar19 = this_00->dwAffectFlag;
      BVar9 = this_00->bPointOn;
      DVar28 = this_00->dwVnum;
    }
    else {
                    /* Unresolved local var: int iDur2@[DW_OP_reg3(EBX)]
                       Unresolved local var: DWORD affact_flag@[DW_OP_reg3(EBX)] */
      RemoveAffect(pkVictim,this_00->dwVnum);
      lVar23 = (longdouble)CPoly::Eval(this_01);
      if ((int)ROUND(lVar23) < 1) {
        PointChange(pkVictim,this_00->bPointOn2,iVar10,false,false);
      }
      else {
        lVar12 = GetPoint(this,'g');
        AddAffect(pkVictim,this_00->dwVnum,this_00->bPointOn2,iVar10,this_00->dwAffectFlag2,
                  (int)ROUND(lVar23) + lVar12,0,false,false);
      }
      DVar19 = this_00->dwAffectFlag;
      bVar21 = LC_IsYMIR();
      if (bVar21) {
        if ((this_00->dwVnum == 0x13) && (iVar10 = GetUsedSkillMasterType(this,0x13), iVar10 < 1))
        goto LAB_080712eb;
      }
      else if ((this_00->dwVnum == 0x13) && (iVar10 = GetUsedSkillMasterType(this,0x13), iVar10 < 2)
              ) {
LAB_080712eb:
        DVar19 = 0x22;
      }
      lVar23 = (longdouble)CPoly::Eval(pCVar20);
      bVar21 = false;
      BVar9 = this_00->bPointOn;
      DVar28 = this_00->dwVnum;
    }
    AddAffect(pkVictim,DVar28,BVar9,iAmount,DVar19,(int)ROUND(lVar24) + lVar11,(int)ROUND(lVar23),
              bVar21,false);
    bAdded = true;
  }
  if (this_00->bPointOn3 == '\0') {
    return 0;
  }
  DVar19 = this_00->dwVnum;
  if (DVar19 == 5) {
    return 0;
  }
  if (DVar19 == 0x8a) {
    return 0;
  }
  iVar10 = GetUsedSkillMasterType(this,DVar19);
  if (iVar10 < 2) {
    return 0;
  }
                    /* Unresolved local var: int iDur@[DW_OP_reg7(EDI)] */
  std::string::string((string *)local_3c,"k",local_5c);
  CPoly::SetVar(&this_00->kDurationPoly3,(string *)local_3c,(double)fVar7);
  LOCK();
  piVar1 = (int *)((int)local_3c[0]._M_dataplus._M_p + -4);
  iVar10 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar10 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)local_3c[0]._M_dataplus._M_p + -0xc));
  }
  lVar24 = (longdouble)CPoly::Eval(&this_00->kDurationPoly3);
  iVar10 = (int)ROUND(lVar24);
  sys_log(0,"try third %lu %d %d %d 1894",this_00->dwVnum,(uint)this_00->bPointOn3,iVar10,iVar17);
  if (iVar10 < 1) {
    PointChange(pkVictim,this_00->bPointOn3,iVar17,false,false);
    return 0;
  }
  lVar11 = GetPoint(this,'g');
  f.m_iDuration = iVar10 + lVar11;
  if ((char)this_00->dwFlag < '\0') {
    if ((pkVictim->super_CEntity).m_pSectree == (SECTREE *)0x0) {
      return 0;
    }
    f.m_x = (pkVictim->super_CEntity).m_pos.x;
    f.m_y = (pkVictim->super_CEntity).m_pos.y;
    f.m_iDist = this_00->iSplashRange;
    f.m_dwVnum = this_00->dwVnum;
    f.m_bPointOn = this_00->bPointOn3;
    f.m_iMaxHit = this_00->lMaxHit;
    f.m_bOverride = (bool)(bAdded ^ 1);
    f.m_dwAffectFlag = 0;
    f.m_iSPCost = 0;
    pSVar3 = (pkVictim->super_CEntity).m_pSectree;
    f.m_pkChrAttacker = this;
    f.m_iCount = 0;
    p_Var4 = (pSVar3->m_neighbor_list).super__List_base<SECTREE*,std::allocator<SECTREE*>_>.
             super__List_alloc_base<SECTREE*,std::allocator<SECTREE*>,true>._M_node;
    p_Var15 = (_List_node<SECTREE*> *)(p_Var4->super__List_node_base)._M_next;
    if (p_Var15 == p_Var4) {
      return 0;
    }
    f.m_iAmount = iVar17;
    do {
      pSVar5 = p_Var15->_M_data;
      p_Var15 = (_List_node<SECTREE*> *)(p_Var15->super__List_node_base)._M_next;
      _Var16._M_node =
           &((pSVar5->m_set_entity)._M_t.super__Rb_tree_base<CEntity*,std::allocator<CEntity*>_>.
             super__Rb_tree_alloc_base<CEntity*,std::allocator<CEntity*>,true>._M_header)->
            super__Rb_tree_node_base;
      it_4.super__Rb_tree_base_iterator._M_node =
           (_Rb_tree_base_iterator)((_Rb_tree_node_base *)&(_Var16._M_node)->_M_color)->_M_left;
      while (_Var8.super__Rb_tree_base_iterator._M_node = it_4.super__Rb_tree_base_iterator._M_node,
            it_4.super__Rb_tree_base_iterator._M_node != (_Rb_tree_base_iterator)_Var16._M_node) {
                    /* Unresolved local var:
                       _Rb_tree_iterator<CEntity*,CEntity*_const&,CEntity*_const*> __tmp@[???] */
        _Rb_tree_base_iterator::_M_increment(&it_4.super__Rb_tree_base_iterator);
        FuncSplashAffect::operator()
                  (&f,*(LPENTITY *)((int)_Var8.super__Rb_tree_base_iterator._M_node + 0x10));
        _Var16._M_node =
             &((pSVar5->m_set_entity)._M_t.super__Rb_tree_base<CEntity*,std::allocator<CEntity*>_>.
               super__Rb_tree_alloc_base<CEntity*,std::allocator<CEntity*>,true>._M_header)->
              super__Rb_tree_node_base;
      }
    } while (p_Var15 !=
             (pSVar3->m_neighbor_list).super__List_base<SECTREE*,std::allocator<SECTREE*>_>.
             super__List_alloc_base<SECTREE*,std::allocator<SECTREE*>,true>._M_node);
    return 0;
  }
  bVar21 = (bool)(bAdded ^ 1);
  DVar28 = 0;
  iVar10 = 0;
  uVar26 = CONCAT44(iVar17,(uint)this_00->bPointOn3);
  DVar19 = this_00->dwVnum;
LAB_0807126a:
  AddAffect(pkVictim,DVar19,(BYTE)uVar26,(long)((ulonglong)uVar26 >> 0x20),DVar28,f.m_iDuration,
            iVar10,bVar21,false);
                    /* Unresolved local var: int iDur@[DW_OP_reg6(ESI)] */
  return 0;
}

