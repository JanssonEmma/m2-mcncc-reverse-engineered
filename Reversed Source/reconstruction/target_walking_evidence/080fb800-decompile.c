
/* DWARF original prototype: void write(TEMP_BUFFER * this, void * data, int size) */

void __thiscall TEMP_BUFFER::write(TEMP_BUFFER *this,void *data,int size)

{
  buffer_write(this->buf,data,size);
  return;
}

