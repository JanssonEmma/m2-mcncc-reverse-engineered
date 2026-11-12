
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

bool LC_IsNewCIBN(void)

{
  eLocalization eVar1;
  
  eVar1 = LC_GetLocalType();
  return eVar1 == LC_NEWCIBN;
}

