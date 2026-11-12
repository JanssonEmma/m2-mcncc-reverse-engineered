
/* DWARF original prototype: bool StopRiding(CHorseRider * this) */

bool __thiscall CHorseRider::StopRiding(CHorseRider *this)

{
  bool bVar1;
  
  bVar1 = (this->m_Horse).bRiding != '\0';
  if (bVar1) {
    (this->m_Horse).bRiding = '\0';
    StartStaminaRegenEvent(this);
  }
  return bVar1;
}

