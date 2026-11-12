
int MINMAX(int min,int value,int max)

{
                    /* Unresolved local var: int tv@[DW_OP_reg0(EAX)] */
  if (value < min) {
    value = min;
  }
  if (max < value) {
    value = max;
  }
  return value;
}

