
/* DWARF original prototype: void StartAffectEvent(CHARACTER * this) */

void __thiscall CHARACTER::StartAffectEvent(CHARACTER *this)

{
  DWORD dwInfoID;
  LPEVENT pEVar1;
  char *pcVar2;
  char_event_info *info;
  
                    /* Unresolved local var: DWORD dwEI@[DW_OP_reg0(EAX)] */
  if (this->m_pkAffectEvent == (LPEVENT)0x0) {
    dwInfoID = __AllocEventInfo(4,&info);
    info->ch = this;
    pEVar1 = event_create_ex(affect_event,dwInfoID,passes_per_sec,"char_affect.cpp",0x9d);
    this->m_pkAffectEvent = pEVar1;
    pcVar2 = GetName(this);
    sys_log(1,"StartAffectEvent %s %p %p",pcVar2,this,this->m_pkAffectEvent);
  }
  return;
}

