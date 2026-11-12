
/* DWARF original prototype: void MonsterChat(CHARACTER * this, BYTE bMonsterChatType) */

void __thiscall CHARACTER::MonsterChat(CHARACTER *this,BYTE bMonsterChatType)

{
  int *piVar1;
  bool bVar2;
  WORD WVar3;
  int iVar4;
  uint uVar5;
  void *data;
  uint uVar6;
  char *__format;
  int line;
  TEMP_BUFFER buf;
  packet_chat pack_chat;
  allocator local_14c [16];
  int local_13c [4];
  string text;
  char sbuf [256];
  
  if ((this->super_CEntity).m_lpDesc != (LPDESC)0x0) {
    return;
  }
  if (this->m_bCharType == '\0') {
    iVar4 = number_ex(0,0x3c,"char.cpp",0x1629);
    if (iVar4 != 0) {
      return;
    }
    iVar4 = number_ex(1,3,"char.cpp",0x162e);
    uVar6 = (uint)bMonsterChatType * 3 + iVar4;
    WVar3 = GetRaceNum(this);
    uVar5 = (uint)WVar3;
    __format = "(locale.monster_chat[%i] and locale.monster_chat[%i][%d] or \'\')";
  }
  else {
    if (bMonsterChatType != '\0') {
      return;
    }
    bVar2 = IsGuardNPC(this);
    if (bVar2) {
      iVar4 = 6;
      line = 0x1637;
    }
    else {
      line = 0x163c;
      iVar4 = 0x1e;
    }
    iVar4 = number_ex(0,iVar4,"char.cpp",line);
    if (iVar4 != 0) {
      return;
    }
    WVar3 = GetRaceNum(this);
    uVar5 = (uint)WVar3;
    __format = 
    "(locale.monster_chat[%i] and locale.monster_chat[%i][number(1, table.getn(locale.monster_chat[%i]))] or \'\')"
    ;
    uVar6 = uVar5;
  }
  snprintf(sbuf,0x100,__format,uVar5,uVar5,uVar6);
  std::string::string((string *)local_13c,sbuf,local_14c);
  ScriptToString((string *)local_13c);
  LOCK();
  piVar1 = (int *)(local_13c[0] + -4);
  iVar4 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar4 < 1) {
    std::string::_Rep::_M_destroy((allocator *)(local_13c[0] + -0xc));
  }
  if (*(int *)((int)text._M_dataplus._M_p + -0xc) == 0) {
    LOCK();
    piVar1 = (int *)((int)text._M_dataplus._M_p + -4);
    iVar4 = *piVar1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
  }
  else {
    pack_chat.header = '\x04';
    pack_chat.type = '\0';
    pack_chat.size = (short)*(undefined4 *)((int)text._M_dataplus._M_p + -0xc) + 10;
    pack_chat.id = (this->m_vid).m_id;
    pack_chat.bEmpire = '\0';
    TEMP_BUFFER::TEMP_BUFFER(&buf);
    TEMP_BUFFER::write(&buf,&pack_chat,9);
                    /* Unresolved local var: size_t __n@[???] */
    iVar4 = *(int *)((int)text._M_dataplus._M_p + -0xc);
    *(undefined1 *)(iVar4 + (int)text._M_dataplus._M_p) = std::string::_Rep::_S_terminal;
    TEMP_BUFFER::write(&buf,(void *)text._M_dataplus._M_p,iVar4 + 1);
    iVar4 = TEMP_BUFFER::size(&buf);
    data = TEMP_BUFFER::read_peek(&buf);
    CEntity::PacketAround(&this->super_CEntity,data,iVar4,(LPENTITY)0x0);
    TEMP_BUFFER::~TEMP_BUFFER(&buf,(int)data);
                    /* Unresolved local var: _Atomic_word __result@[DW_OP_reg2(EDX)] */
    LOCK();
    piVar1 = (int *)((int)text._M_dataplus._M_p + -4);
    iVar4 = *piVar1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
  }
  if (iVar4 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)text._M_dataplus._M_p + -0xc));
  }
  return;
}

