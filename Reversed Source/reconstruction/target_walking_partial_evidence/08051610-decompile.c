
int DISTANCE_APPROX(int dx,int dy)

{
  int iVar1;
  
                    /* Unresolved local var: int min@[DW_OP_reg3(EBX)]
                       Unresolved local var: int max@[DW_OP_reg1(ECX)] */
  if (dx < 0) {
    dx = -dx;
  }
  if (dy < 0) {
    dy = -dy;
  }
  iVar1 = dx;
  if (dx < dy) {
    iVar1 = dy;
    dy = dx;
  }
  return iVar1 * 0xf6 + dy * 0x66 >> 8;
}

