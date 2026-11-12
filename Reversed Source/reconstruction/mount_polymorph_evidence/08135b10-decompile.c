
/* DWARF original prototype: void SetPolymorph(CHARACTER * this, DWORD dwRaceNum, bool
   bMaintainStat) */

void __thiscall CHARACTER::SetPolymorph(CHARACTER *this,DWORD dwRaceNum,bool bMaintainStat)

{
  char *pcVar1;
  bool bMaintainStat_local;
  
  bMaintainStat_local = bMaintainStat;
  if (dwRaceNum < 4) {
    bMaintainStat_local = false;
    dwRaceNum = 0;
  }
  if (this->m_dwPolymorphRace != dwRaceNum) {
    this->m_dwPolymorphRace = dwRaceNum;
    this->m_bPolyMaintainStat = bMaintainStat_local;
    pcVar1 = GetName(this);
    sys_log(0,"POLYMORPH: %s race %lu ",pcVar1,dwRaceNum);
    if (dwRaceNum != 0) {
      (*(this->super_CEntity)._vptr_CEntity[0xd])(this);
    }
    this->m_bAddChrState = this->m_bAddChrState | 2;
    (this->m_afAffectFlag).bits[0] = (this->m_afAffectFlag).bits[0] | 4;
    CEntity::ViewReencode(&this->super_CEntity);
    this->m_bAddChrState = this->m_bAddChrState & 0xfd;
    if (bMaintainStat_local == false) {
      PointChange(this,'\f',0,false,false);
      PointChange(this,'\x0e',0,false,false);
      PointChange(this,'\x0f',0,false,false);
      PointChange(this,'\r',0,false,false);
    }
    ComputeBattlePoints(this);
    return;
  }
  return;
}

