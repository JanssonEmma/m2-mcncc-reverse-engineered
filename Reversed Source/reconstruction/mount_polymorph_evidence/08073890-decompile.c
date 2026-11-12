
/* DWARF original prototype: void StartMuyeongEvent(CHARACTER * this) */

void __thiscall CHARACTER::StartMuyeongEvent(CHARACTER *this)

{
  DWORD dwInfoID;
  LPEVENT pEVar1;
  char_event_info *info;
  
                    /* Unresolved local var: DWORD dwEI@[DW_OP_reg0(EAX)] */
  if (this->m_pkMuyeongEvent == (LPEVENT)0x0) {
    dwInfoID = __AllocEventInfo(4,&info);
    info->ch = this;
    pEVar1 = event_create_ex(skill_muyoung_event,dwInfoID,passes_per_sec,"char_skill.cpp",0xa60);
    this->m_pkMuyeongEvent = pEVar1;
  }
  return;
}

