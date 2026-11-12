
/* DWARF original prototype: int size(TEMP_BUFFER * this) */

int __thiscall TEMP_BUFFER::size(TEMP_BUFFER *this)

{
  DWORD DVar1;
  
  DVar1 = buffer_size(this->buf);
  return DVar1;
}

