
/* DWARF original prototype: void ~TEMP_BUFFER(TEMP_BUFFER * this, int __in_chrg) */

void __thiscall TEMP_BUFFER::~TEMP_BUFFER(TEMP_BUFFER *this,int __in_chrg)

{
  if (singleton<BUFFER_MANAGER>::ms_singleton == (BUFFER_MANAGER *)0x0) {
    __assert("instance","../../common/singleton.h",0x1a);
  }
  BUFFER_MANAGER::release(singleton<BUFFER_MANAGER>::ms_singleton,this->buf);
  return;
}

