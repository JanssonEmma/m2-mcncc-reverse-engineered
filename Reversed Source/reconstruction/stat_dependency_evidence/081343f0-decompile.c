
/* DWARF original prototype: void ResetPoint(CHARACTER * this, int iLv) */

void __thiscall CHARACTER::ResetPoint(CHARACTER *this,int iLv)

{
  DWORD y;
  DWORD x;
  DWORD dwPID;
  byte bVar1;
  byte bVar2;
  long lVar3;
  long lVar4;
  int iVar5;
  
                    /* Unresolved local var: BYTE bJob@[DW_OP_reg3(EBX)] */
  bVar1 = GetJob(this);
  iVar5 = (uint)bVar1 * 0x40;
  PointChange(this,'\x01',iLv - (uint)(this->m_points).level,false,false);
  SetRealPoint(this,'\f',*(long *)(JobInitialPoints + iVar5));
  lVar3 = GetRealPoint(this,'\f');
  SetPoint(this,'\f',lVar3);
  SetRealPoint(this,'\r',*(long *)(JobInitialPoints + iVar5 + 4));
  lVar3 = GetRealPoint(this,'\r');
  SetPoint(this,'\r',lVar3);
  SetRealPoint(this,'\x0e',*(long *)(JobInitialPoints + iVar5 + 8));
  lVar3 = GetRealPoint(this,'\x0e');
  SetPoint(this,'\x0e',lVar3);
  SetRealPoint(this,'\x0f',*(long *)(JobInitialPoints + iVar5 + 0xc));
  lVar3 = GetRealPoint(this,'\x0f');
  SetPoint(this,'\x0f',lVar3);
  bVar1 = GetJob(this);
  bVar2 = GetJob(this);
  iVar5 = number_ex(*(int *)(JobInitialPoints + (uint)bVar2 * 0x40 + 0x20),
                    *(int *)(JobInitialPoints + (uint)bVar1 * 0x40 + 0x24),"char.cpp",0x159c);
  (this->m_points).iRandomHP = iVar5 * (iLv + -1);
  bVar1 = GetJob(this);
  bVar2 = GetJob(this);
  iVar5 = number_ex(*(int *)(JobInitialPoints + (uint)bVar2 * 0x40 + 0x28),
                    *(int *)(JobInitialPoints + (uint)bVar1 * 0x40 + 0x2c),"char.cpp",0x159d);
  (this->m_points).iRandomSP = (iLv + -1) * iVar5;
  lVar3 = GetPoint(this,'\x19');
  lVar4 = GetPoint(this,'\x1a');
  PointChange(this,'\x1a',((lVar3 + iLv * 3) - lVar4) + -3,false,false);
  ComputePoints(this);
  PointChange(this,'\x05',(this->m_pointsInstant).iMaxHP - (this->m_points).hp,false,false);
  PointChange(this,'\a',(this->m_pointsInstant).iMaxSP - (this->m_points).sp,false,false);
  PointsPacket(this);
  y = (this->super_CEntity).m_pos.y;
  x = (this->super_CEntity).m_pos.x;
  dwPID = this->m_dwPlayerID;
  if (singleton<LogManager>::ms_singleton == (LogManager *)0x0) {
    __assert("instance","../../common/singleton.h",0x1a);
  }
  LogManager::CharLog(singleton<LogManager>::ms_singleton,dwPID,x,y,0,"RESET_POINT","");
  return;
}

