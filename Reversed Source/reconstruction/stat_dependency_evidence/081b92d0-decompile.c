
int number_ex(int from,int to,char *file,int line)

{
  DWORD DVar1;
  int iVar2;
  
  iVar2 = to;
  if (to < from) {
                    /* Unresolved local var: int tmp@[DW_OP_reg3(EBX)] */
    _sys_err("number_ex",0x153,
             "number(): first argument is bigger than second argument %d -> %d, %s %d",from,to,file,
             line);
    iVar2 = from;
    from = to;
  }
  DVar1 = thecore_random();
  return from + DVar1 % ((iVar2 - from) + 1U);
}

