
/* DWARF original prototype: LPSECTREE Get(SECTREE_MANAGER * this, DWORD dwIndex, DWORD package) */

LPSECTREE __thiscall SECTREE_MANAGER::Get(SECTREE_MANAGER *this,DWORD dwIndex,DWORD package)

{
  SECTREE_MAP *this_00;
  LPSECTREE pSVar1;
  
                    /* Unresolved local var: SECTREE_MAP * pkSectreeMap@[DW_OP_reg0(EAX)] */
  this_00 = GetMap(this,dwIndex);
  if (this_00 != (SECTREE_MAP *)0x0) {
    pSVar1 = SECTREE_MAP::Find(this_00,package);
    return pSVar1;
  }
  return (LPSECTREE)0x0;
}

