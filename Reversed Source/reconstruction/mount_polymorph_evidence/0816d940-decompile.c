
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* DWARF original prototype: void HorseSummon(CHARACTER * this, bool bSummon, bool bFromFar) */

void __thiscall CHARACTER::HorseSummon(CHARACTER *this,bool bSummon,bool bFromFar)

{
  int iVar1;
  int iVar2;
  long z;
  long lMapIndex;
  CHARACTER *this_00;
  bool bVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  DWORD DVar7;
  LPCHARACTER pCVar8;
  char *pcVar9;
  LPEVENT pEVar10;
  float fVar11;
  long y;
  long x;
  float fx;
  float fy;
  char_event_info *info;
  TPacketGCDead pack;
  
  if (bSummon) {
    if ((((this->super_CHorseRider).m_Horse.bLevel != '\0') &&
        ((this->super_CHorseRider).m_Horse.bRiding == '\0')) &&
       (this->m_chHorse == (LPCHARACTER)0x0)) {
      iVar1 = (this->super_CEntity).m_pos.x;
      iVar2 = (this->super_CEntity).m_pos.y;
      if ((this->super_CHorseRider).m_Horse.sHealth < 1) {
        bFromFar = false;
      }
      if (bFromFar == false) {
        iVar5 = number_ex(-100,100,"char_horse.cpp",0x87);
        iVar4 = number_ex(-100,100,"char_horse.cpp",0x88);
      }
      else {
        iVar4 = number_ex(0,1,"char_horse.cpp",0x82);
        iVar5 = number_ex(2000,0x9c4,"char_horse.cpp",0x82);
        iVar5 = (iVar4 * 2 + -1) * iVar5;
        iVar6 = number_ex(0,1,"char_horse.cpp",0x83);
        iVar4 = number_ex(2000,0x9c4,"char_horse.cpp",0x83);
        iVar4 = (iVar6 * 2 + -1) * iVar4;
      }
      x = iVar1 + iVar5;
      y = iVar2 + iVar4;
      fVar11 = _DAT_081f9f70 + (this->m_pointsInstant).fRot;
      z = (this->super_CEntity).m_pos.z;
      lMapIndex = (this->super_CEntity).m_lMapIndex;
      DVar7 = (*(this->super_CEntity)._vptr_CEntity[0xe])(this);
      if (singleton<CHARACTER_MANAGER>::ms_singleton == (CHARACTER_MANAGER *)0x0) {
        __assert("instance","../../common/singleton.h",0x1a);
      }
      pCVar8 = CHARACTER_MANAGER::SpawnMob
                         (singleton<CHARACTER_MANAGER>::ms_singleton,DVar7,lMapIndex,x,y,z,false,
                          (int)ROUND(fVar11),false);
      this->m_chHorse = pCVar8;
      if (pCVar8 == (LPCHARACTER)0x0) {
        pcVar9 = locale_find(&DAT_0822c7b9,g_iUseLocale);
        ChatPacket(this,'\x01',pcVar9);
      }
      else {
        if ((this->super_CHorseRider).m_Horse.sHealth < 1) {
                    /* Unresolved local var: DWORD dwEI@[DW_OP_reg0(EAX)] */
          SetPosition(this->m_chHorse,0);
          DVar7 = __AllocEventInfo(4,&info);
          iVar1 = passes_per_sec;
          info->ch = this;
          pCVar8 = this->m_chHorse;
          pEVar10 = event_create_ex(horse_dead_event,DVar7,iVar1 * 0x3c,"char_horse.cpp",0x9f);
          pCVar8->m_pkDeadEvent = pEVar10;
        }
        SetLevel(this->m_chHorse,(this->super_CHorseRider).m_Horse.bLevel);
        pCVar8 = this->m_chHorse;
        pcVar9 = GetName(this);
        strlen(pcVar9);
        std::string::assign((char *)&pCVar8->m_stName,(uint)pcVar9);
        pCVar8 = this->m_chHorse;
        pcVar9 = locale_find(&DAT_0822c7b1,g_iUseLocale);
        strlen(pcVar9);
        std::string::append((char *)&pCVar8->m_stName,(uint)pcVar9);
        bVar3 = Show(this->m_chHorse,(this->super_CEntity).m_lMapIndex,x,y,
                     (this->super_CEntity).m_pos.z,false);
        if (bVar3) {
          if ((this->super_CHorseRider).m_Horse.sHealth < 1) {
            pack.header = '\x0e';
            pack.vid = (this->m_chHorse->m_vid).m_id;
            CEntity::PacketAround(&this->super_CEntity,&pack,5,(LPENTITY)0x0);
          }
          SetRider(this->m_chHorse,this);
        }
        else {
          if (singleton<CHARACTER_MANAGER>::ms_singleton == (CHARACTER_MANAGER *)0x0) {
            y = 0x81f8943;
            __assert("instance","../../common/singleton.h",0x1a);
          }
          CHARACTER_MANAGER::DestroyCharacter
                    (singleton<CHARACTER_MANAGER>::ms_singleton,this->m_chHorse);
          _sys_err("HorseSummon",0xa9,"cannot show monster",y);
          this->m_chHorse = (LPCHARACTER)0x0;
        }
      }
    }
  }
  else {
                    /* Unresolved local var: LPCHARACTER chHorse@[???] */
    this_00 = this->m_chHorse;
    if (this_00 != (CHARACTER *)0x0) {
      SetRider(this_00,(LPCHARACTER)0x0);
      if (bFromFar) {
        SetNowWalking(this_00,false);
        fVar11 = GetDegreeFromPositionXY
                           ((this_00->super_CEntity).m_pos.x,(this_00->super_CEntity).m_pos.y,
                            (this->super_CEntity).m_pos.x,(this->super_CEntity).m_pos.y);
        SetRotation(this_00,fVar11 + _DAT_081f9f70);
        GetDeltaByDegree((this_00->m_pointsInstant).fRot,3500.0,&fx,&fy);
        Goto(this_00,(int)ROUND((float)(this_00->super_CEntity).m_pos.x + fx),
             (int)ROUND((float)(this_00->super_CEntity).m_pos.y + fy));
        SendMovePacket(this_00,'\0','\0',0,0,0,0,-1);
      }
      else {
        if (singleton<CHARACTER_MANAGER>::ms_singleton == (CHARACTER_MANAGER *)0x0) {
          __assert("instance","../../common/singleton.h",0x1a);
        }
        CHARACTER_MANAGER::DestroyCharacter(singleton<CHARACTER_MANAGER>::ms_singleton,this_00);
      }
    }
  }
  return;
}

