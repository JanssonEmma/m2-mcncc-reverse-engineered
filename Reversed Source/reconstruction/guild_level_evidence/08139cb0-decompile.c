
/* DWARF original prototype: void BufferedPacket(DESC * this, void * c_pvData, int iSize) */

void __thiscall DESC::BufferedPacket(DESC *this,void *c_pvData,int iSize)

{
  LPBUFFER buffer;
  
  buffer = this->m_lpBufferedOutputBuffer;
  if (buffer == (LPBUFFER)0x0) {
    buffer = buffer_new((void *)0x0,0x400);
    this->m_lpBufferedOutputBuffer = buffer;
  }
  buffer_write(buffer,c_pvData,iSize);
  return;
}

