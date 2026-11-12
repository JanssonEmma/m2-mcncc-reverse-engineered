
/* DWARF original prototype: void ChatPacket(CHARACTER * this, BYTE type, char * format, ...) */

void __thiscall CHARACTER::ChatPacket(CHARACTER *this,BYTE type,char *format,...)

{
  int iVar1;
  void *c_pvData;
  char *pcVar2;
  int len;
  BYTE type_local;
  TEMP_BUFFER buf;
  packet_chat pack_chat;
  char chatbuf [86];
  
                    /* Unresolved local var: va_list args@[???] */
  if ((this->super_CEntity).m_lpDesc != (LPDESC)0x0) {
    iVar1 = vsnprintf(chatbuf,0x55,format,&stack0x00000010);
    pack_chat.header = '\x04';
    pack_chat.size = (short)iVar1 + 10;
    pack_chat.type = type;
    pack_chat.id = 0;
    pack_chat.bEmpire = DESC::GetEmpire((this->super_CEntity).m_lpDesc);
    if ((type == '\x05') && (test_server != 0)) {
      pcVar2 = GetName(this);
      sys_log(0,"SNED COMMAND %s %s",pcVar2,chatbuf);
    }
    TEMP_BUFFER::TEMP_BUFFER(&buf);
    TEMP_BUFFER::write(&buf,&pack_chat,9);
    TEMP_BUFFER::write(&buf,chatbuf,iVar1 + 1);
    iVar1 = TEMP_BUFFER::size(&buf);
    c_pvData = TEMP_BUFFER::read_peek(&buf);
    DESC::Packet((this->super_CEntity).m_lpDesc,c_pvData,iVar1);
    TEMP_BUFFER::~TEMP_BUFFER(&buf,(int)c_pvData);
  }
  return;
}

