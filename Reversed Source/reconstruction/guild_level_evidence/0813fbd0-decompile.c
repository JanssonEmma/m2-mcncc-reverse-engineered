
/* DWARF original prototype: void DBPacket(CLIENT_DESC * this, BYTE bHeader, DWORD dwHandle, void *
   c_pvData, DWORD dwSize) */

void __thiscall
CLIENT_DESC::DBPacket(CLIENT_DESC *this,BYTE bHeader,DWORD dwHandle,void *c_pvData,DWORD dwSize)

{
  DWORD DVar1;
  void *c_pvData_local;
  CLIENT_DESC *this_local;
  
  DVar1 = buffer_size((this->super_DESC).m_lpOutputBuffer);
  sys_log(1,"DB_PACKET: header %d handle %d size %d buffer_size %d",(uint)bHeader,dwHandle,dwSize,
          DVar1);
  DBPacketHeader(this,bHeader,dwHandle,dwSize);
  if (c_pvData != (void *)0x0) {
    buffer_write((this->super_DESC).m_lpOutputBuffer,c_pvData,dwSize);
    return;
  }
  return;
}

