
/* DWARF original prototype: void StopMuyeongEvent(CHARACTER * this) */

void __thiscall CHARACTER::StopMuyeongEvent(CHARACTER *this)

{
  event_cancel(&this->m_pkMuyeongEvent);
  return;
}

