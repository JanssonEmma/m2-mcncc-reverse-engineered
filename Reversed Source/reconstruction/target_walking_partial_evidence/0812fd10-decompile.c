
/* DWARF original prototype: void MonsterLog(CHARACTER * this, char * format, ...) */

void __thiscall CHARACTER::MonsterLog(CHARACTER *this,char *format,...)

{
  int iVar1;
  int iVar2;
  void *buf_00;
  TEMP_BUFFER buf;
  packet_chat pack_chat;
  char chatbuf [86];
  
                    /* Unresolved local var: int len2@[DW_OP_reg3(EBX)]
                       Unresolved local var: va_list args@[???]
                       Unresolved local var: int len@[DW_OP_reg3(EBX)] */
  if ((test_server != 0) && ((this->super_CEntity).m_lpDesc == (LPDESC)0x0)) {
    iVar1 = sprintf(chatbuf,"%lu)",(this->m_vid).m_id);
    iVar2 = vsnprintf(chatbuf + iVar1,0x55,format,&stack0x0000000c);
    pack_chat.header = '\x04';
    pack_chat.type = '\0';
    pack_chat.size = (short)(iVar2 + iVar1) + 10;
    pack_chat.id = (this->m_vid).m_id;
    pack_chat.bEmpire = '\0';
    TEMP_BUFFER::TEMP_BUFFER(&buf);
    TEMP_BUFFER::write(&buf,&pack_chat,9);
    TEMP_BUFFER::write(&buf,chatbuf,iVar2 + iVar1 + 1);
    iVar1 = TEMP_BUFFER::size(&buf);
    buf_00 = TEMP_BUFFER::read_peek(&buf);
    if (singleton<CHARACTER_MANAGER>::ms_singleton == (CHARACTER_MANAGER *)0x0) {
      __assert("instance","../../common/singleton.h",0x1a);
    }
    CHARACTER_MANAGER::PacketMonsterLog
              (singleton<CHARACTER_MANAGER>::ms_singleton,this,buf_00,iVar1);
    TEMP_BUFFER::~TEMP_BUFFER(&buf,(int)this);
  }
  return;
}

