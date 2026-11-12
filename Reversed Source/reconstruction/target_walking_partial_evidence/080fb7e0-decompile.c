
/* DWARF original prototype: void * read_peek(TEMP_BUFFER * this) */

void * __thiscall TEMP_BUFFER::read_peek(TEMP_BUFFER *this)

{
  void *pvVar1;
  
  pvVar1 = buffer_read_peek(this->buf);
  return pvVar1;
}

