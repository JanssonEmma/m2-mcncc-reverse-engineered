
/* WARNING: Removing unreachable block (ram,0x0812b560) */
/* WARNING: Removing unreachable block (ram,0x0812b6c0) */
/* WARNING: Removing unreachable block (ram,0x0812b820) */
/* WARNING: Removing unreachable block (ram,0x0812b980) */
/* WARNING: Removing unreachable block (ram,0x0812ba30) */
/* WARNING: Removing unreachable block (ram,0x0812b8d0) */
/* WARNING: Removing unreachable block (ram,0x0812b770) */
/* WARNING: Removing unreachable block (ram,0x0812b610) */
/* WARNING: Removing unreachable block (ram,0x0812b4b0) */
/* WARNING: Removing unreachable block (ram,0x0812b480) */
/* WARNING: Removing unreachable block (ram,0x0812b534) */
/* WARNING: Removing unreachable block (ram,0x0812b5e4) */
/* WARNING: Removing unreachable block (ram,0x0812b694) */
/* WARNING: Removing unreachable block (ram,0x0812b744) */
/* WARNING: Removing unreachable block (ram,0x0812b7f4) */
/* WARNING: Removing unreachable block (ram,0x0812b8a4) */
/* WARNING: Removing unreachable block (ram,0x0812b954) */
/* WARNING: Removing unreachable block (ram,0x0812ba04) */
/* WARNING: Removing unreachable block (ram,0x0812b9d8) */
/* WARNING: Removing unreachable block (ram,0x0812b928) */
/* WARNING: Removing unreachable block (ram,0x0812b878) */
/* WARNING: Removing unreachable block (ram,0x0812b7c8) */
/* WARNING: Removing unreachable block (ram,0x0812b718) */
/* WARNING: Removing unreachable block (ram,0x0812b668) */
/* WARNING: Removing unreachable block (ram,0x0812b5b8) */
/* WARNING: Removing unreachable block (ram,0x0812b508) */
/* WARNING: Removing unreachable block (ram,0x0812b450) */
/* DWARF original prototype: void SetProto(CHARACTER * this, CMob * pkMob) */

void __thiscall CHARACTER::SetProto(CHARACTER *this,CMob *pkMob)

{
  CMob *table;
  BYTE BVar1;
  bool bVar2;
  WORD WVar3;
  CMobInstance *this_00;
  DWORD DVar4;
  int iVar5;
  LPEVENT pEVar6;
  long val;
  char_event_info *info;
  
                    /* Unresolved local var: TMobTable * t@[DW_OP_reg3(EBX)] */
  if (this->m_pkMobInst != (CMobInstance *)0x0) {
    operator_delete(this->m_pkMobInst);
  }
  this->m_pkMobData = pkMob;
  this_00 = operator_new(0x18);
  CMobInstance::CMobInstance(this_00);
  this->m_pkMobInst = this_00;
  table = this->m_pkMobData;
  this->m_bPKMode = '\x02';
  this->m_bCharType = (table->m_table).bType;
  SetLevel(this,(table->m_table).bLevel);
  SetEmpire(this,(table->m_table).bEmpire);
  (this->m_points).exp = (table->m_table).dwExp;
  SetRealPoint(this,'\f',(uint)(table->m_table).bStr);
  SetRealPoint(this,'\x0e',(uint)(table->m_table).bDex);
  SetRealPoint(this,'\r',(uint)(table->m_table).bCon);
  SetRealPoint(this,'\x0f',(uint)(table->m_table).bInt);
  ComputePoints(this);
  (this->m_points).hp = (this->m_pointsInstant).iMaxHP;
  (this->m_points).sp = (this->m_pointsInstant).iMaxSP;
  (this->m_pointsInstant).dwAIFlag = (table->m_table).dwAIFlag;
  (this->m_pointsInstant).dwImmuneFlag = (table->m_table).dwImmuneFlag;
  AssignTriggers(this,&table->m_table);
  ApplyMobAttribute(this,&table->m_table);
  BVar1 = this->m_bCharType;
  if (BVar1 == '\x02') {
    DetermineDropMetinStone(this);
    BVar1 = this->m_bCharType;
  }
  if ((BVar1 == '\x03') || (BVar1 == '\t')) {
    StartWarpNPCEvent(this);
  }
  if (singleton<CHARACTER_MANAGER>::ms_singleton == (CHARACTER_MANAGER *)0x0) {
    __assert("instance","../../common/singleton.h",0x1a);
  }
  CHARACTER_MANAGER::RegisterRaceNumMap(singleton<CHARACTER_MANAGER>::ms_singleton,this);
  WVar3 = GetRaceNum(this);
  if (WVar3 == 0x4e3f) {
    SetPoint(this,'_',10);
    if (g_iUseLocale == 0) {
      val = 0xf;
    }
    else {
      val = 6;
    }
    SetPoint(this,'`',val);
    DVar4 = get_dword_time();
    this->m_dwPlayStartTime = DVar4 + 30000;
    if (test_server != 0) {
      DVar4 = get_dword_time();
      this->m_dwPlayStartTime = DVar4 + 30000;
    }
    bVar2 = LC_IsJapan();
    if (bVar2) {
      DVar4 = get_dword_time();
      this->m_dwPlayStartTime = DVar4 + 60000;
    }
  }
  WVar3 = GetRaceNum(this);
  if (WVar3 - 0x4e43 < 3) {
    if (this == (CHARACTER *)0x0) {
      __assert("Set","state.h",0x32);
    }
    (this->m_stateIdle).m_pInstance = this;
    (this->m_stateIdle).m_pfnBeginState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x25;
    (this->m_stateIdle).m_pfnBeginState.__delta = 0;
    (this->m_stateIdle).m_pfnState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x1d;
    (this->m_stateIdle).m_pfnState.__delta = 0;
    (this->m_stateIdle).m_pfnEndState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x29;
    (this->m_stateIdle).m_pfnEndState.__delta = 0;
    if (this == (CHARACTER *)0x0) {
      __assert("Set","state.h",0x32);
    }
    (this->m_stateMove).m_pInstance = this;
    (this->m_stateMove).m_pfnBeginState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x25;
    (this->m_stateMove).m_pfnBeginState.__delta = 0;
    (this->m_stateMove).m_pfnState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x1d;
    (this->m_stateMove).m_pfnState.__delta = 0;
    (this->m_stateMove).m_pfnEndState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x29;
    (this->m_stateMove).m_pfnEndState.__delta = 0;
    if (this == (CHARACTER *)0x0) {
      __assert("Set","state.h",0x32);
    }
    (this->m_stateBattle).m_pInstance = this;
    (this->m_stateBattle).m_pfnBeginState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x25;
    (this->m_stateBattle).m_pfnBeginState.__delta = 0;
    (this->m_stateBattle).m_pfnState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x1d;
    (this->m_stateBattle).m_pfnState.__delta = 0;
    (this->m_stateBattle).m_pfnEndState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x29;
    (this->m_stateBattle).m_pfnEndState.__delta = 0;
  }
  WVar3 = GetRaceNum(this);
  if (WVar3 == 0x4e46) {
    if (this == (CHARACTER *)0x0) {
      __assert("Set","state.h",0x32);
    }
    (this->m_stateIdle).m_pInstance = this;
    (this->m_stateIdle).m_pfnBeginState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x25;
    (this->m_stateIdle).m_pfnBeginState.__delta = 0;
    (this->m_stateIdle).m_pfnState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x21;
    (this->m_stateIdle).m_pfnState.__delta = 0;
    (this->m_stateIdle).m_pfnEndState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x29;
    (this->m_stateIdle).m_pfnEndState.__delta = 0;
    if (this == (CHARACTER *)0x0) {
      __assert("Set","state.h",0x32);
    }
    (this->m_stateMove).m_pInstance = this;
    (this->m_stateMove).m_pfnBeginState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x25;
    (this->m_stateMove).m_pfnBeginState.__delta = 0;
    (this->m_stateMove).m_pfnState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x21;
    (this->m_stateMove).m_pfnState.__delta = 0;
    (this->m_stateMove).m_pfnEndState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x29;
    (this->m_stateMove).m_pfnEndState.__delta = 0;
    if (this == (CHARACTER *)0x0) {
      __assert("Set","state.h",0x32);
    }
    (this->m_stateBattle).m_pInstance = this;
    (this->m_stateBattle).m_pfnBeginState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x25;
    (this->m_stateBattle).m_pfnBeginState.__delta = 0;
    (this->m_stateBattle).m_pfnState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x21;
    (this->m_stateBattle).m_pfnState.__delta = 0;
    (this->m_stateBattle).m_pfnEndState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x29;
    (this->m_stateBattle).m_pfnEndState.__delta = 0;
  }
  if ((((((this->m_bCharType == '\b') || (WVar3 = GetRaceNum(this), WVar3 == 0x4e85)) ||
        (WVar3 == 0x4e86)) || ((WVar3 == 0x4e87 || (WVar3 == 0x4e88)))) ||
      ((WVar3 == 0x4e89 || ((WVar3 == 0x4e8a || (WVar3 == 0x4e8b)))))) ||
     ((WVar3 == 0x4e8c || (WVar3 == 0x4e8d)))) {
    if (this == (CHARACTER *)0x0) {
      __assert("Set","state.h",0x32);
    }
    (this->m_stateIdle).m_pInstance = this;
    (this->m_stateIdle).m_pfnBeginState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x25;
    (this->m_stateIdle).m_pfnBeginState.__delta = 0;
    (this->m_stateIdle).m_pfnState.__pfn = StateHorse;
    (this->m_stateIdle).m_pfnState.__delta = 0;
    (this->m_stateIdle).m_pfnEndState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x29;
    (this->m_stateIdle).m_pfnEndState.__delta = 0;
    if (this == (CHARACTER *)0x0) {
      __assert("Set","state.h",0x32);
    }
    (this->m_stateMove).m_pInstance = this;
    (this->m_stateMove).m_pfnBeginState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x25;
    (this->m_stateMove).m_pfnBeginState.__delta = 0;
    (this->m_stateMove).m_pfnState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x11;
    (this->m_stateMove).m_pfnState.__delta = 0;
    (this->m_stateMove).m_pfnEndState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x29;
    (this->m_stateMove).m_pfnEndState.__delta = 0;
    if (this == (CHARACTER *)0x0) {
      __assert("Set","state.h",0x32);
    }
    (this->m_stateBattle).m_pInstance = this;
    (this->m_stateBattle).m_pfnBeginState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x25;
    (this->m_stateBattle).m_pfnBeginState.__delta = 0;
    (this->m_stateBattle).m_pfnState.__pfn = StateHorse;
    (this->m_stateBattle).m_pfnState.__delta = 0;
    (this->m_stateBattle).m_pfnEndState.__pfn = (_func___thiscall_void_CHARACTER_ptr *)0x29;
    (this->m_stateBattle).m_pfnEndState.__delta = 0;
  }
  WVar3 = GetRaceNum(this);
  if ((0x4e4e < WVar3) && (WVar3 < 0x4e5c)) {
                    /* Unresolved local var: DWORD dwEI@[DW_OP_reg3(EBX)] */
    DVar4 = __AllocEventInfo(4,&info);
    info->ch = this;
    iVar5 = number_ex(0x1a4,900,"char.cpp",0x731);
    pEVar6 = event_create_ex(kill_ore_load_event,DVar4,iVar5 * passes_per_sec,"char.cpp",0x731);
    this->m_pkMiningEvent = pEVar6;
  }
  return;
}

