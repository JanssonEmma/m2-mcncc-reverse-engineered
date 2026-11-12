
/* DWARF original prototype: void Save(CHARACTER * this) */

void __thiscall CHARACTER::Save(CHARACTER *this)

{
  if (this->m_bSkipSave == false) {
    if (singleton<CHARACTER_MANAGER>::ms_singleton == (CHARACTER_MANAGER *)0x0) {
      __assert("instance","../../common/singleton.h",0x1a);
    }
    CHARACTER_MANAGER::DelayedSave(singleton<CHARACTER_MANAGER>::ms_singleton,this);
  }
  return;
}

