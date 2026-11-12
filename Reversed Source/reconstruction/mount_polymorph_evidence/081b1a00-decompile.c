
/* DWARF original prototype: bool OnUnmount(NPC * this, PC * pc) */

bool __thiscall NPC::OnUnmount(NPC *this,PC *pc)

{
  bool bVar1;
  
  bVar1 = HandleReceiveAllEvent(this,pc,0x13);
  return bVar1;
}

