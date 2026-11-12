
bool NewPlayerTable2(TPlayerTable *table,char *name,BYTE race,BYTE shape,BYTE bEmpire)

{
  short sVar1;
  short sVar2;
  undefined4 uVar3;
  bool bVar4;
  uint race_00;
  int iVar5;
  int local_20;
  int local_1c;
  BYTE bEmpire_local;
  BYTE race_local;
  uint job;
  
  if (race < 8) {
    race_00 = (uint)race;
    bVar4 = RaceToJob(race_00,&job);
    if (bVar4) {
      sys_log(0,"NewPlayerTable2(name=%s, race=%d, job=%d)",name,race_00,job);
      memset(table,0,0x71a);
      strncpy(table->name,name,0x18);
      table->name[0x18] = '\0';
      table->level = '\x01';
      table->voice = '\0';
      table->job = (ushort)race;
      table->part_base = shape;
      iVar5 = job * 0x40;
      uVar3 = *(undefined4 *)(JobInitialPoints + iVar5 + 0x18);
      sVar1 = *(short *)(JobInitialPoints + iVar5 + 0xc);
      table->st = *(short *)(JobInitialPoints + iVar5);
      sVar2 = *(short *)(JobInitialPoints + iVar5 + 8);
      table->iq = sVar1;
      table->dx = sVar2;
      sVar2 = *(short *)(JobInitialPoints + iVar5 + 4);
      table->ht = sVar2;
      table->hp = sVar2 * (short)uVar3 + *(short *)(JobInitialPoints + iVar5 + 0x10);
      sVar2 = *(short *)(JobInitialPoints + iVar5 + 0x30);
      table->sp = sVar1 * (short)*(undefined4 *)(JobInitialPoints + iVar5 + 0x1c) +
                  *(short *)(JobInitialPoints + iVar5 + 0x14);
      table->stamina = sVar2;
      if ((byte)(bEmpire - 1) < 3) {
        local_1c = *(int *)(g_create_position + (uint)bEmpire * 8);
      }
      iVar5 = number_ex(-300,300,"input_login.cpp",0x197);
      table->x = iVar5 + local_1c;
      if ((byte)(bEmpire - 1) < 3) {
        local_20 = *(int *)(g_create_position + (uint)bEmpire * 8 + 4);
      }
      iVar5 = number_ex(-300,300,"input_login.cpp",0x198);
      table->z = 0;
      table->playtime = 0;
      table->dir = '\0';
      table->y = iVar5 + local_20;
      table->gold = 0;
      table->skill_group = '\0';
      return true;
    }
    _sys_err("NewPlayerTable2",0x17f,"NewPlayerTable2.RACE_TO_JOB_ERROR(%d)\n",race_00);
  }
  else {
    _sys_err("NewPlayerTable2",0x177,"NewPlayerTable2.OUT_OF_RACE_RANGE(%d >= max(%d))\n",(uint)race
             ,8);
  }
  return false;
}

