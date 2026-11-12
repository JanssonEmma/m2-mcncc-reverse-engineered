
/* DWARF original prototype: LPSECTREE Get(SECTREE_MANAGER * this, DWORD dwIndex, DWORD x, DWORD y)
    */

LPSECTREE __thiscall SECTREE_MANAGER::Get(SECTREE_MANAGER *this,DWORD dwIndex,DWORD x,DWORD y)

{
  LPSECTREE pSVar1;
  
                    /* Unresolved local var: SECTREEID id@[DW_OP_reg1(ECX)] */
  pSVar1 = Get(this,dwIndex,x / 0x1900 & 0xffff | y / 0x1900 << 0x10);
  return pSVar1;
}

