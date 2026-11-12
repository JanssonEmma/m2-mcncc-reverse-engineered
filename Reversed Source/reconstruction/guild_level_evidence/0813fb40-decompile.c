
/* DWARF original prototype: void DBPacketHeader(CLIENT_DESC * this, BYTE bHeader, DWORD dwHandle,
   DWORD dwSize) */

void __thiscall
CLIENT_DESC::DBPacketHeader(CLIENT_DESC *this,BYTE bHeader,DWORD dwHandle,DWORD dwSize)

{
  encode_byte(char)::a._0_1_ = bHeader;
  buffer_write((this->super_DESC).m_lpOutputBuffer,&encode_byte(char)::a,1);
  encode_4bytes(int)::a._0_4_ = dwHandle;
  buffer_write((this->super_DESC).m_lpOutputBuffer,&encode_4bytes(int)::a,4);
  encode_4bytes(int)::a._0_4_ = dwSize;
  buffer_write((this->super_DESC).m_lpOutputBuffer,&encode_4bytes(int)::a,4);
  return;
}

