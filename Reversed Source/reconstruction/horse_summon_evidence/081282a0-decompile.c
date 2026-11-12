
void EncodeMovePacket(TPacketGCMove *pack,DWORD dwVID,BYTE bFunc,BYTE bArg,DWORD x,DWORD y,
                     DWORD dwDuration,DWORD dwTime,BYTE bRot)

{
  pack->bHeader = '\x03';
  pack->bFunc = bFunc;
  pack->bArg = bArg;
  pack->dwVID = dwVID;
  if (dwTime == 0) {
    dwTime = get_dword_time();
  }
  pack->dwTime = dwTime;
  pack->bRot = bRot;
  pack->lX = x;
  pack->lY = y;
  pack->dwDuration = dwDuration;
  return;
}

