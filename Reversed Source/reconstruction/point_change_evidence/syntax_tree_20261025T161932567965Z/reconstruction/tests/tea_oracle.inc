/* Test oracle mechanically normalized from retained Ghidra pseudocode; not original execution. */
#include "../../libthecore/include/typedef.h"

void tea_oracle_tea_code(DWORD sz,DWORD sy,const DWORD *key,DWORD *dest)

{
  DWORD DVar1;
  DWORD uVar2;
  DWORD DVar3;
  DWORD DVar4;
  DWORD uVar5;
  DWORD uVar6;
  
                    /* Unresolved local var: DWORD y@[DW_OP_reg2(EDX)]
                       Unresolved local var: DWORD z@[DW_OP_reg1(ECX)]
                       Unresolved local var: DWORD sum@[???] */
  DVar1 = key[1];
  uVar2 = *key;
  uVar6 = ((sz << 4 ^ sz >> 5) + sz ^ uVar2) + sy;
  DVar3 = key[3];
  DVar4 = key[2];
  uVar5 = sz + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar3 + 0x9e3779b9);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0x9e3779b9);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0x3c6ef372);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0x3c6ef372);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0xdaa66d2b);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar3 + 0xdaa66d2b);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar2 + 0x78dde6e4);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar2 + 0x78dde6e4);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar2 + 0x1715609d);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0x1715609d);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar3 + 0xb54cda56);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0xb54cda56);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0x5384540f);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar3 + 0x5384540f);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0xf1bbcdc8);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar2 + 0xf1bbcdc8);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar2 + 0x8ff34781);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0x8ff34781);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar2 + 0x2e2ac13a);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0x2e2ac13a);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar3 + 0xcc623af3);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar3 + 0xcc623af3);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0x6a99b4ac);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar2 + 0x6a99b4ac);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0x8d12e65);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0x8d12e65);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0xa708a81e);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0xa708a81e);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar2 + 0x454021d7);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar3 + 0x454021d7);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar3 + 0xe3779b90);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar2 + 0xe3779b90);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0x81af1549);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0x81af1549);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0x1fe68f02);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0x1fe68f02);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0xbe1e08bb);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar3 + 0xbe1e08bb);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar2 + 0x5c558274);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar2 + 0x5c558274);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar3 + 0xfa8cfc2d);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0xfa8cfc2d);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0x98c475e6);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0x98c475e6);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0x36fbef9f);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar3 + 0x36fbef9f);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0xd5336958);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar2 + 0xd5336958);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar2 + 0x736ae311);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0x736ae311);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar3 + 0x11a25cca);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0x11a25cca);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0xafd9d683);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar3 + 0xafd9d683);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0x4e11503c);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar2 + 0x4e11503c);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0xec48c9f5);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0xec48c9f5);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar2 + 0x8a8043ae);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0x8a8043ae);
  uVar5 = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar3 + 0x28b7bd67);
  uVar6 = uVar6 + ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar3 + 0x28b7bd67);
  *dest = uVar6;
  dest[1] = uVar5 + ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0xc6ef3720);
  return;
}


void tea_oracle_tea_decode(DWORD sz,DWORD sy,const DWORD *key,DWORD *dest)

{
  DWORD DVar1;
  DWORD DVar2;
  DWORD uVar3;
  DWORD DVar4;
  DWORD uVar5;
  DWORD uVar6;
  
                    /* Unresolved local var: DWORD y@[DW_OP_reg2(EDX)]
                       Unresolved local var: DWORD z@[DW_OP_reg1(ECX)]
                       Unresolved local var: DWORD sum@[???] */
  DVar1 = key[2];
  uVar5 = sz - ((sy << 4 ^ sy >> 5) + sy ^ DVar1 + 0xc6ef3720);
  DVar2 = key[3];
  uVar6 = sy - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar2 + 0x28b7bd67);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar2 + 0x28b7bd67);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0x8a8043ae);
  uVar3 = *key;
  DVar4 = key[1];
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar3 + 0x8a8043ae);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0xec48c9f5);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0xec48c9f5);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar3 + 0x4e11503c);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0x4e11503c);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar2 + 0xafd9d683);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0xafd9d683);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0x11a25cca);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar2 + 0x11a25cca);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0x736ae311);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar3 + 0x736ae311);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar3 + 0xd5336958);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0xd5336958);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar2 + 0x36fbef9f);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0x36fbef9f);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0x98c475e6);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0x98c475e6);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0xfa8cfc2d);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar2 + 0xfa8cfc2d);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar3 + 0x5c558274);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar3 + 0x5c558274);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar2 + 0xbe1e08bb);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0xbe1e08bb);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0x1fe68f02);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0x1fe68f02);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0x81af1549);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0x81af1549);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar3 + 0xe3779b90);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar2 + 0xe3779b90);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar2 + 0x454021d7);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar3 + 0x454021d7);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0xa708a81e);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0xa708a81e);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0x8d12e65);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0x8d12e65);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar3 + 0x6a99b4ac);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0x6a99b4ac);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar2 + 0xcc623af3);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar2 + 0xcc623af3);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0x2e2ac13a);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar3 + 0x2e2ac13a);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0x8ff34781);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar3 + 0x8ff34781);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar3 + 0xf1bbcdc8);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0xf1bbcdc8);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar2 + 0x5384540f);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0x5384540f);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0xb54cda56);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar2 + 0xb54cda56);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0x1715609d);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar3 + 0x1715609d);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar3 + 0x78dde6e4);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ uVar3 + 0x78dde6e4);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar2 + 0xdaa66d2b);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar4 + 0xdaa66d2b);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar1 + 0x3c6ef372);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar1 + 0x3c6ef372);
  uVar6 = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ DVar4 + 0x9e3779b9);
  uVar5 = uVar5 - ((uVar6 * 0x10 ^ uVar6 >> 5) + uVar6 ^ DVar2 + 0x9e3779b9);
  *dest = uVar6 - ((uVar5 * 0x10 ^ uVar5 >> 5) + uVar5 ^ uVar3);
  dest[1] = uVar5;
  return;
}


