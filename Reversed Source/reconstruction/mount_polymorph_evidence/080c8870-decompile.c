
/* DWARF original prototype: void StartStaminaRegenEvent(CHorseRider * this) */

void __thiscall CHorseRider::StartStaminaRegenEvent(CHorseRider *this)

{
  DWORD dwInfoID;
  LPEVENT pEVar1;
  horserider_info *info;
  
                    /* Unresolved local var: DWORD dwEI@[DW_OP_reg0(EAX)] */
  if (((this->m_Horse).bLevel != '\0') && (0 < (this->m_Horse).sHealth)) {
    sys_log(0,"HORSE STAMINA CONSUME EVENT CANCEL %p",this->m_eventStaminaConsume);
    event_cancel(&this->m_eventStaminaConsume);
    if (this->m_eventStaminaRegen == (LPEVENT)0x0) {
      dwInfoID = __AllocEventInfo(4,&info);
      info->hr = this;
      pEVar1 = event_create_ex(horse_stamina_regen_event,dwInfoID,passes_per_sec * 0x2d0,
                               "horse_rider.cpp",0x130);
      this->m_eventStaminaRegen = pEVar1;
      sys_log(0,"HORSE STAMINA REGEN EVENT CREATE %p",pEVar1);
    }
  }
  return;
}

