
/* DWARF original prototype: bool Show(CHARACTER * this, long lMapIndex, long x, long y, long z,
   bool bShowSpawnMotion) */

bool __thiscall
CHARACTER::Show(CHARACTER *this,long lMapIndex,long x,long y,long z,bool bShowSpawnMotion)

{
  SECTREE *this_00;
  CMobInstance *pCVar1;
  bool bVar2;
  LPSECTREE this_01;
  char *pcVar3;
  long lVar4;
  bool bChangeTree;
  bool bShowSpawnMotion_local;
  
                    /* Unresolved local var: LPSECTREE tree@[DW_OP_reg7(EDI)] */
  if (singleton<SECTREE_MANAGER>::ms_singleton == (SECTREE_MANAGER *)0x0) {
    __assert("instance","../../common/singleton.h",0x1a);
  }
  pcVar3 = (char *)x;
  lVar4 = y;
  this_01 = SECTREE_MANAGER::Get(singleton<SECTREE_MANAGER>::ms_singleton,lMapIndex,x,y);
  if (this_01 == (LPSECTREE)0x0) {
    sys_log(0,"cannot find sectree by %dx%d mapindex %d",x,y,lMapIndex);
    bVar2 = false;
  }
  else {
    (this->super_CEntity).m_lMapIndex = lMapIndex;
    this_00 = (this->super_CEntity).m_pSectree;
    bVar2 = false;
    if ((this_00 == (SECTREE *)0x0) || (this_00 != this_01)) {
      bVar2 = true;
      if (this_00 != (SECTREE *)0x0) {
        SECTREE::RemoveEntity(this_00,&this->super_CEntity);
      }
      CEntity::ViewCleanup(&this->super_CEntity);
    }
    if (this->m_bCharType == '\x06') {
      pcVar3 = GetName(this);
      lVar4 = x;
      sys_log(0,"SHOW: %s %dx%dx%d",pcVar3,x,y,z);
      if ((this->m_points).stamina < (this->m_pointsInstant).iMaxStamina) {
        StartAffectEvent(this);
      }
    }
    else if (this->m_pkMobData != (CMob *)0x0) {
      pCVar1 = this->m_pkMobInst;
      (pCVar1->m_posLastAttacked).x = x;
      (pCVar1->m_posLastAttacked).y = y;
      (pCVar1->m_posLastAttacked).z = z;
    }
    if (bShowSpawnMotion) {
      this->m_bAddChrState = this->m_bAddChrState | 2;
      (this->m_afAffectFlag).bits[0] = (this->m_afAffectFlag).bits[0] | 4;
    }
    (this->super_CEntity).m_pos.x = x;
    (this->super_CEntity).m_pos.y = y;
    (this->super_CEntity).m_pos.z = z;
    (this->m_posDest).x = x;
    (this->m_posDest).y = y;
    (this->m_posDest).z = z;
    if (bVar2) {
      (*(this->super_CEntity)._vptr_CEntity[2])(this,this);
      SECTREE::InsertEntity(this_01,&this->super_CEntity);
      CEntity::UpdateSectree(&this->super_CEntity);
    }
    else {
      CEntity::ViewReencode(&this->super_CEntity);
      sys_log(0,"      in same sectree",pcVar3,lVar4);
    }
    this->m_bAddChrState = this->m_bAddChrState & 0xfd;
    bVar2 = true;
  }
  return bVar2;
}

