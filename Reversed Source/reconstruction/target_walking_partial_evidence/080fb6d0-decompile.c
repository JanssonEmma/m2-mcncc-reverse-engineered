
/* DWARF original prototype: void TEMP_BUFFER(TEMP_BUFFER * this) */

void __thiscall TEMP_BUFFER::TEMP_BUFFER(TEMP_BUFFER *this)

{
  LPBUFFER pbVar1;
  
  if (singleton<BUFFER_MANAGER>::ms_singleton == (BUFFER_MANAGER *)0x0) {
    __assert("instance","../../common/singleton.h",0x1a);
  }
  pbVar1 = BUFFER_MANAGER::get(singleton<BUFFER_MANAGER>::ms_singleton);
  this->buf = pbVar1;
  return;
}

