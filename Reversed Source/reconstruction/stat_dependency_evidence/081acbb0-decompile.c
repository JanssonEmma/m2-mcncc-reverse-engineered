
/* DWARF original prototype: void PacketAround(CEntity * this, void * data, int bytes, LPENTITY
   except) */

void __thiscall CEntity::PacketAround(CEntity *this,void *data,int bytes,LPENTITY except)

{
  PacketView(this,data,bytes,except);
  return;
}

