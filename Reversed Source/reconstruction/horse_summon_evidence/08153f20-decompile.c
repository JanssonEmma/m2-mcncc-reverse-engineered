
/* DWARF original prototype: LPCHARACTER GetVictim(CHARACTER * this) */

LPCHARACTER __thiscall CHARACTER::GetVictim(CHARACTER *this)

{
  LPCHARACTER pCVar1;
  
  if (singleton<CHARACTER_MANAGER>::ms_singleton == (CHARACTER_MANAGER *)0x0) {
    __assert("instance","../../common/singleton.h",0x1a);
  }
  pCVar1 = CHARACTER_MANAGER::Find(singleton<CHARACTER_MANAGER>::ms_singleton,&this->m_kVIDVictim);
  return pCVar1;
}

