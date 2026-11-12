
/* DWARF original prototype: BYTE GetJob(CHARACTER * this) */

BYTE __thiscall CHARACTER::GetJob(CHARACTER *this)

{
  bool bVar1;
  char *pcVar2;
  uint race;
  uint job;
  
                    /* Unresolved local var: uint race@[DW_OP_reg3(EBX)] */
  race = (uint)(this->m_points).job;
  bVar1 = RaceToJob(race,&job);
  if (!bVar1) {
    pcVar2 = GetName(this);
    _sys_err("GetJob",0x621,"CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE",pcVar2,race);
    job._0_1_ = '\0';
  }
  return (BYTE)job;
}

