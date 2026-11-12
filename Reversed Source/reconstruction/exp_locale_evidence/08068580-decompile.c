
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void LocaleService_TransferDefaultSetting(void)

{
  bool bVar1;
  int iVar2;
  
  if (check_name == (code *)0x0) {
    check_name = check_name_euckr;
  }
  if (is_twobyte == (code *)0x0) {
    is_twobyte = is_twobyte_euckr;
  }
  if (exp_table == (undefined1 *)0x0) {
    exp_table = exp_table_euckr;
  }
                    /* Unresolved local var: int job@[DW_OP_reg3(EBX)] */
  iVar2 = 0;
  while( true ) {
    if (singleton<CTableBySkill>::ms_singleton == (CTableBySkill *)0x0) {
      __assert("instance","../../common/singleton.h",0x1a);
    }
    bVar1 = CTableBySkill::Check(singleton<CTableBySkill>::ms_singleton);
    if (!bVar1) break;
    iVar2 = iVar2 + 1;
    if (7 < iVar2) {
      if (aiPercentByDeltaLevForBoss == (undefined1 *)0x0) {
        aiPercentByDeltaLevForBoss = aiPercentByDeltaLevForBoss_euckr;
      }
      if (aiPercentByDeltaLev == (undefined1 *)0x0) {
        aiPercentByDeltaLev = aiPercentByDeltaLev_euckr;
      }
      if (aiChainLightningCountBySkillLevel == (undefined1 *)0x0) {
        aiChainLightningCountBySkillLevel = aiChainLightningCountBySkillLevel_euckr;
      }
      return;
    }
  }
  fprintf(__stderrp,"[NO SETTING SKILL] aiSkillPowerByLevelFromType[%d]",iVar2);
                    /* WARNING: Subroutine does not return */
  exit(1);
}

