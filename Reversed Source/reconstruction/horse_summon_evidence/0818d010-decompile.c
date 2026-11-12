
/* DWARF original prototype: LPCHARACTER SpawnMob(CHARACTER_MANAGER * this, DWORD dwVnum, long
   lMapIndex, long x, long y, long z, bool bSpawnMotion, int iRot, bool bShow) */

LPCHARACTER __thiscall
CHARACTER_MANAGER::SpawnMob
          (CHARACTER_MANAGER *this,DWORD dwVnum,long lMapIndex,long x,long y,long z,
          bool bSpawnMotion,int iRot,bool bShow)

{
  int *piVar1;
  bool bVar2;
  BYTE BVar3;
  CMob *pkMob;
  LPSECTREE this_00;
  DWORD DVar4;
  LPCHARACTER this_01;
  char *format;
  uint uVar5;
  int iVar6;
  time_t tVar7;
  char *pcVar8;
  bool bShow_local;
  bool bSpawnMotion_local;
  allocator local_4c [16];
  allocator local_3c [16];
  int local_2c [7];
  
                    /* Unresolved local var: CMob * pkMob@[DW_OP_reg6(ESI)]
                       Unresolved local var: LPCHARACTER ch@[DW_OP_reg3(EBX)] */
  if (singleton<CMobManager>::ms_singleton == (CMobManager *)0x0) {
    __assert("instance","../../common/singleton.h",0x1a);
  }
  pkMob = CMobManager::Get(singleton<CMobManager>::ms_singleton,dwVnum);
  if (pkMob == (CMob *)0x0) {
    _sys_err("SpawnMob",0x132,"SpawnMob: no mob data for vnum %lu",dwVnum);
    return (LPCHARACTER)0x0;
  }
  BVar3 = (pkMob->m_table).bType;
  if ((BVar3 == '\x01') || (BVar3 == '\x03')) {
LAB_0818d0b6:
    if (singleton<CHARACTER_MANAGER>::ms_singleton == (CHARACTER_MANAGER *)0x0) {
      __assert("instance","../../common/singleton.h",0x1a);
    }
    this_01 = CreateCharacter(singleton<CHARACTER_MANAGER>::ms_singleton,
                              (pkMob->m_table).szLocaleName,0);
    if (this_01 == (LPCHARACTER)0x0) {
      pcVar8 = "SpawnMob: cannot create new character";
    }
    else {
      if (iRot == -1) {
        iRot = number_ex(0,0x168,"char_manager.cpp",0x167);
      }
      CHARACTER::SetProto(this_01,pkMob);
      if (((pkMob->m_table).bType == '\x01') && (this_01->m_bEmpire == '\0')) {
        if (singleton<SECTREE_MANAGER>::ms_singleton == (SECTREE_MANAGER *)0x0) {
          __assert("instance","../../common/singleton.h",0x1a);
        }
        BVar3 = SECTREE_MANAGER::GetEmpireFromMapIndex
                          (singleton<SECTREE_MANAGER>::ms_singleton,lMapIndex);
        CHARACTER::SetEmpire(this_01,BVar3);
      }
      CHARACTER::SetRotation(this_01,(float)iRot);
      if (!bShow) {
        return this_01;
      }
      bVar2 = CHARACTER::Show(this_01,lMapIndex,x,y,z,bSpawnMotion);
      if (bVar2) {
        return this_01;
      }
      if (singleton<CHARACTER_MANAGER>::ms_singleton == (CHARACTER_MANAGER *)0x0) {
        __assert("instance","../../common/singleton.h",0x1a);
      }
      DestroyCharacter(singleton<CHARACTER_MANAGER>::ms_singleton,this_01);
      pcVar8 = "SpawnMob: cannot show monster";
    }
    sys_log(0,pcVar8);
  }
  else {
                    /* Unresolved local var: LPSECTREE tree@[DW_OP_reg0(EAX)]
                       Unresolved local var: DWORD dwAttr@[DW_OP_reg3(EBX)] */
    if (singleton<SECTREE_MANAGER>::ms_singleton == (SECTREE_MANAGER *)0x0) {
      __assert("instance","../../common/singleton.h",0x1a);
    }
    this_00 = SECTREE_MANAGER::Get(singleton<SECTREE_MANAGER>::ms_singleton,lMapIndex,x,y);
    if (this_00 == (LPSECTREE)0x0) {
      format = "no sectree for spawn at %d %d mobvnum %d mapindex %d";
      pcVar8 = (char *)x;
      DVar4 = y;
      x = dwVnum;
      y = lMapIndex;
    }
    else {
      DVar4 = SECTREE::GetAttribute(this_00,x,y);
      if ((DVar4 & 0x81) != 0) {
                    /* Unresolved local var: DWORD curTime@[DW_OP_reg0(EAX)] */
        if ((char)SpawnMob(unsigned_long,long,long,long,long,bool,int,bool)::s_isLog == '\0') {
          std::string::string((string *)local_2c,"spawn_block_log",local_3c);
          if (singleton<quest::CQuestManager>::ms_singleton == (CQuestManager *)0x0) {
            __assert("instance","../../common/singleton.h",0x1a);
          }
          iVar6 = CQuestManager::GetEventFlag
                            ((CQuestManager *)singleton<quest::CQuestManager>::ms_singleton,
                             (string *)local_2c);
          SpawnMob(unsigned_long,long,long,long,long,bool,int,bool)::s_isLog._0_1_ = '\x01';
          SpawnMob::lexical_block_5::s_isLog = iVar6 != 0;
          LOCK();
          piVar1 = (int *)(local_2c[0] + -4);
          iVar6 = *piVar1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (iVar6 < 1) {
            std::string::_Rep::_M_destroy((allocator *)(local_2c[0] + -0xc));
          }
        }
        if ((char)SpawnMob(unsigned_long,long,long,long,long,bool,int,bool)::s_nextTime == '\0') {
          tVar7 = get_global_time();
          SpawnMob(unsigned_long,long,long,long,long,bool,int,bool)::s_nextTime._0_1_ = '\x01';
          SpawnMob::lexical_block_5::s_nextTime = tVar7 + 10000;
        }
        uVar5 = get_global_time();
        if (SpawnMob::lexical_block_5::s_nextTime < uVar5) {
          SpawnMob::lexical_block_5::s_nextTime = uVar5;
          std::string::string((string *)local_2c,"spawn_block_log",local_4c);
          if (singleton<quest::CQuestManager>::ms_singleton == (CQuestManager *)0x0) {
            __assert("instance","../../common/singleton.h",0x1a);
          }
          iVar6 = CQuestManager::GetEventFlag
                            ((CQuestManager *)singleton<quest::CQuestManager>::ms_singleton,
                             (string *)local_2c);
          SpawnMob::lexical_block_5::s_isLog = iVar6 != 0;
          LOCK();
          piVar1 = (int *)(local_2c[0] + -4);
          iVar6 = *piVar1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (iVar6 < 1) {
            std::string::_Rep::_M_destroy((allocator *)(local_2c[0] + -0xc));
          }
        }
        if (SpawnMob::lexical_block_5::s_isLog == false) {
          return (LPCHARACTER)0x0;
        }
        sys_log(0,"SpawnMob: BLOCKED position for spawn %s %lu at %d %d (attr %lu)",
                (pkMob->m_table).szName,dwVnum,x,y,DVar4);
        return (LPCHARACTER)0x0;
      }
      if ((DVar4 & 4) == 0) goto LAB_0818d0b6;
      format = "SpawnMob: BAN_PK position for mob spawn %s %lu at %d %d";
      pcVar8 = (pkMob->m_table).szName;
      DVar4 = dwVnum;
    }
    sys_log(0,format,pcVar8,DVar4,x,y);
  }
  return (LPCHARACTER)0x0;
}

