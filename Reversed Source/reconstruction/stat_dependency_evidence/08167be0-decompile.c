
/* DWARF original prototype: long GetValue(CItem * this, DWORD idx) */

long __thiscall CItem::GetValue(CItem *this,DWORD idx)

{
  if (5 < idx) {
    __assert("GetValue","item.cpp",0x323);
  }
  return this->m_pProto->alValues[idx];
}

