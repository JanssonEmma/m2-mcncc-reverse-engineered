
/* DWARF original prototype: void PointsPacket(CHARACTER * this) */

void __thiscall CHARACTER::PointsPacket(CHARACTER *this)

{
  long lVar1;
  uint uVar2;
  int iVar3;
  TPacketGCPoints pack;
  
  if ((this->super_CEntity).m_lpDesc != (LPDESC)0x0) {
    pack.header = '\x10';
    pack.points[1] = (long)(this->m_points).level;
    pack.points[3] = (this->m_points).exp;
    pack.points[4] = 2000000000;
    uVar2 = (uint)(this->m_points).level;
    if (uVar2 < 99) {
      pack.points[4] = *(long *)(exp_table + uVar2 * 4);
    }
                    /* Unresolved local var: int i@[DW_OP_reg3(EBX)] */
    iVar3 = 0xc;
    pack.points[5] = (this->m_points).hp;
    pack.points[6] = (this->m_pointsInstant).iMaxHP;
    pack.points[7] = (this->m_points).sp;
    pack.points[8] = (this->m_pointsInstant).iMaxSP;
    pack.points[0xb] = (this->m_points).gold;
    pack.points[9] = (this->m_points).stamina;
    pack.points[10] = (this->m_pointsInstant).iMaxStamina;
    do {
      lVar1 = GetPoint(this,(BYTE)iVar3);
      pack.points[iVar3] = lVar1;
      iVar3 = iVar3 + 1;
    } while (iVar3 < 0xff);
    DESC::Packet((this->super_CEntity).m_lpDesc,&pack,0x3fd);
  }
  return;
}

