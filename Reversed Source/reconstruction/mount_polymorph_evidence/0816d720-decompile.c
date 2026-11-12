
/* DWARF original prototype: bool StopRiding(CHARACTER * this) */

bool __thiscall CHARACTER::StopRiding(CHARACTER *this)

{
  uint pc;
  bool bVar1;
  
  bVar1 = CHorseRider::StopRiding(&this->super_CHorseRider);
  if (bVar1) {
    pc = this->m_dwPlayerID;
    if (singleton<quest::CQuestManager>::ms_singleton == (CQuestManager *)0x0) {
      __assert("instance","../../common/singleton.h",0x1a);
    }
    CQuestManager::Unmount((CQuestManager *)singleton<quest::CQuestManager>::ms_singleton,pc);
    bVar1 = IsDead(this);
    if ((bVar1) || (bVar1 = IsStun(this), bVar1)) {
      this->m_dwMountVnum = 0;
      ComputePoints(this);
      UpdatePacket(this);
    }
    else {
      MountVnum(this,0);
      HorseSummon(this,true,false);
    }
    PointChange(this,'\f',0,false,false);
    PointChange(this,'\x0e',0,false,false);
    PointChange(this,'\r',0,false,false);
    PointChange(this,'\x0f',0,false,false);
    return true;
  }
  return false;
}

