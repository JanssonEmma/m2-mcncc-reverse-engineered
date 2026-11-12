
/* DWARF original prototype: void SendMovePacket(CHARACTER * this, BYTE bFunc, BYTE bArg, DWORD x,
   DWORD y, DWORD dwDuration, DWORD dwTime, int iRot) */

void __thiscall
CHARACTER::SendMovePacket
          (CHARACTER *this,BYTE bFunc,BYTE bArg,DWORD x,DWORD y,DWORD dwDuration,DWORD dwTime,
          int iRot)

{
  BYTE bArg_local;
  TPacketGCMove pack;
  
  if (bFunc == '\0') {
    x = (this->m_posDest).x;
    y = (this->m_posDest).y;
    dwDuration = this->m_dwMoveDuration;
  }
  if (iRot == -1) {
    iRot = (int)ROUND((this->m_pointsInstant).fRot) / 5;
  }
  EncodeMovePacket(&pack,(this->m_vid).m_id,bFunc,bArg,x,y,dwDuration,dwTime,(BYTE)iRot);
  CEntity::PacketView(&this->super_CEntity,&pack,0x18,&this->super_CEntity);
  return;
}

