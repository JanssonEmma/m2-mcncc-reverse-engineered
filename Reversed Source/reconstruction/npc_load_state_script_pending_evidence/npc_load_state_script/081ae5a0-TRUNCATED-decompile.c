
/* WARNING: Removing unreachable block (ram,0x081b09b6) */
/* WARNING: Removing unreachable block (ram,0x081b09b8) */
/* WARNING: Removing unreachable block (ram,0x081b0a62) */
/* WARNING: Removing unreachable block (ram,0x081b09da) */
/* WARNING: Removing unreachable block (ram,0x081b0a4e) */
/* WARNING: Removing unreachable block (ram,0x081b09ed) */
/* WARNING: Removing unreachable block (ram,0x081b06bf) */
/* WARNING: Removing unreachable block (ram,0x081b06c1) */
/* WARNING: Removing unreachable block (ram,0x081b0aa2) */
/* WARNING: Removing unreachable block (ram,0x081b06e3) */
/* WARNING: Removing unreachable block (ram,0x081b0a8b) */
/* WARNING: Removing unreachable block (ram,0x081b06fa) */
/* WARNING: Removing unreachable block (ram,0x081b02f6) */
/* WARNING: Removing unreachable block (ram,0x081b02f8) */
/* WARNING: Removing unreachable block (ram,0x081b0af6) */
/* WARNING: Removing unreachable block (ram,0x081b031a) */
/* WARNING: Removing unreachable block (ram,0x081b0adf) */
/* WARNING: Removing unreachable block (ram,0x081b0331) */
/* WARNING: Removing unreachable block (ram,0x081afeae) */
/* WARNING: Removing unreachable block (ram,0x081afeb0) */
/* WARNING: Removing unreachable block (ram,0x081aff87) */
/* WARNING: Removing unreachable block (ram,0x081afed2) */
/* WARNING: Removing unreachable block (ram,0x081aff70) */
/* WARNING: Removing unreachable block (ram,0x081afee9) */
/* WARNING: Removing unreachable block (ram,0x081afff6) */
/* WARNING: Removing unreachable block (ram,0x081b0b28) */
/* WARNING: Removing unreachable block (ram,0x081b0b68) */
/* WARNING: Removing unreachable block (ram,0x081aeefd) */
/* WARNING: Removing unreachable block (ram,0x081aef00) */
/* WARNING: Removing unreachable block (ram,0x081b0b97) */
/* WARNING: Removing unreachable block (ram,0x081aef22) */
/* WARNING: Removing unreachable block (ram,0x081b0b80) */
/* WARNING: Removing unreachable block (ram,0x081aef39) */
/* WARNING: Removing unreachable block (ram,0x081af1e4) */
/* WARNING: Removing unreachable block (ram,0x081af1f0) */
/* WARNING: Removing unreachable block (ram,0x081b0b57) */
/* WARNING: Removing unreachable block (ram,0x081af212) */
/* WARNING: Removing unreachable block (ram,0x081b0b40) */
/* WARNING: Removing unreachable block (ram,0x081af229) */
/* WARNING: Removing unreachable block (ram,0x081af82a) */
/* WARNING: Removing unreachable block (ram,0x081afb4e) */
/* WARNING: Removing unreachable block (ram,0x081afb50) */
/* WARNING: Removing unreachable block (ram,0x081b0025) */
/* WARNING: Removing unreachable block (ram,0x081afb72) */
/* WARNING: Removing unreachable block (ram,0x081b000e) */
/* WARNING: Removing unreachable block (ram,0x081afb89) */
/* WARNING: Removing unreachable block (ram,0x081aff56) */
/* WARNING: Removing unreachable block (ram,0x081b0ac7) */
/* WARNING: Removing unreachable block (ram,0x081b0a73) */
/* WARNING: Removing unreachable block (ram,0x081b0a39) */
/* WARNING: Removing unreachable block (ram,0x081af5c5) */
/* WARNING: Removing unreachable block (ram,0x081af5d0) */
/* WARNING: Removing unreachable block (ram,0x081af859) */
/* WARNING: Removing unreachable block (ram,0x081af5f2) */
/* WARNING: Removing unreachable block (ram,0x081af842) */
/* WARNING: Removing unreachable block (ram,0x081af609) */
/* WARNING: Type propagation algorithm not settling */
/* DWARF original prototype: void LoadStateScript(NPC * this, int event_index, char * filename, char
   * script_name) */

void __thiscall NPC::LoadStateScript(NPC *this,int event_index,char *filename,char *script_name)

{
  uint uVar1;
  vector<char,std::allocator<char>_> *this_00;
  basic_streambuf<char,std::char_traits<char>_> *pbVar2;
  AArgScript *pAVar3;
  byte *pbVar4;
  bool bVar5;
  _Rb_tree_node_base *p_Var6;
  CQuestManager *this_01;
  basic_streambuf<char,std::char_traits<char>_> *pbVar7;
  _Rb_tree_iterator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>&,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>*>
  _Var8;
  _Rb_tree_iterator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>&,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>*>
  _Var9;
  _Rb_tree_iterator<std::pair<const_int,_quest::AStateScriptType>,std::pair<const_int,_quest::AStateScriptType>&,std::pair<const_int,_quest::AStateScriptType>*>
  _Var10;
  _Rb_tree_iterator<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>,std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>&,std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>*>
  _Var11;
  char cVar12;
  int iVar13;
  uint uVar14;
  char *pcVar15;
  long lVar16;
  int iVar17;
  int iVar18;
  _Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
  *p_Var19;
  _Rb_tree_node_base *p_Var20;
  _Rb_tree_node<std::pair<const_int,_quest::AStateScriptType>_> *p_Var21;
  vector<quest::AArgScript,std::allocator<quest::AArgScript>_> *this_02;
  int *piVar22;
  __normal_iterator<quest::AArgScript*,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
  __first;
  __normal_iterator<quest::AArgScript*,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
  __position;
  _Rb_tree_base_iterator _Var23;
  __normal_iterator<char*,std::vector<char,_std::allocator<char>_>_> _Var24;
  pair<const_int,quest::AStateScriptType> *__v;
  __normal_iterator<char*,std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>
  it;
  int index;
  int quest_index;
  istreambuf_iterator<char,std::char_traits<char>_> local_68c;
  char local_67d;
  istreambuf_iterator<char,std::char_traits<char>_> local_67c;
  char *local_670;
  char *local_66c;
  char *local_668;
  istreambuf_iterator<char,std::char_traits<char>_> local_664;
  char local_655;
  istreambuf_iterator<char,std::char_traits<char>_> local_654;
  char *local_64c;
  char *local_648;
  int state_index;
  char *local_640;
  istreambuf_iterator<char,std::char_traits<char>_> ie;
  istreambuf_iterator<char,std::char_traits<char>_> ib;
  istreambuf_iterator<char,std::char_traits<char>_> local_62c;
  undefined1 local_61d [5];
  uint local_618;
  undefined1 local_614 [8];
  istreambuf_iterator<char,std::char_traits<char>_> ie_1;
  istreambuf_iterator<char,std::char_traits<char>_> ib_1;
  pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_> local_5fc;
  char *local_5ec;
  _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
  local_5e8;
  pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_> local_5dc;
  char *local_5cc;
  _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
  local_5c8;
  pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_> local_5bc;
  char *local_5ac;
  _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
  local_5a8;
  pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_> local_59c;
  char *local_58c;
  _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
  local_588;
  pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_> local_57c;
  char *local_56c;
  _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
  local_568;
  string s_1;
  pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_> local_54c;
  char *local_53c;
  _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
  local_538;
  AArgScript local_52c;
  pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_> local_4fc;
  char *local_4ec;
  _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
  local_4e8;
  _Rb_tree_iterator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>&,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>*>
  __i_3;
  pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_> local_49c;
  _Rb_tree_base_iterator local_48c;
  _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
  local_47c;
  _Rb_tree_base_iterator local_46c;
  _Rb_tree_iterator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>&,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>*>
  __i_1;
  _Rb_tree_iterator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>&,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>*>
  __i_2;
  _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
  local_42c;
  pair<const_unsigned_int,std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>
  local_41c;
  _Rb_tree_base_iterator local_40c;
  _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
  local_3fc;
  _Rb_tree_base_iterator local_3ec;
  _Rb_tree_iterator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>&,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>*>
  __i;
  pair<const_int,quest::AStateScriptType> local_3cc;
  void *local_3bc;
  undefined4 local_3b8;
  int local_3b4;
  pair<const_int,quest::AStateScriptType> local_3ac;
  _Rb_tree_base_iterator local_39c;
  _Rb_tree<int,std::pair<const_int,_quest::AStateScriptType>,std::_Select1st<std::pair<const_int,_quest::AStateScriptType>_>,std::less<int>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
  local_38c;
  _Rb_tree_node_base *local_37c;
  _Rb_tree_iterator<std::pair<const_int,_quest::AStateScriptType>,std::pair<const_int,_quest::AStateScriptType>&,std::pair<const_int,_quest::AStateScriptType>*>
  __i_17;
  pair<const_unsigned_int,std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>
  local_35c [2];
  _Rb_tree<int,std::pair<const_int,_quest::AStateScriptType>,std::_Select1st<std::pair<const_int,_quest::AStateScriptType>_>,std::less<int>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
  local_33c;
  pair<const_unsigned_int,std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>
  local_32c;
  _Rb_tree_base_iterator local_31c;
  _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>_>_>
  local_30c;
  _Rb_tree_node<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>_>
  *local_2fc;
  _Rb_tree_iterator<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>,std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>&,std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>*>
  __i_16;
  string type_name;
  string stStateName;
  string stQuestName;
  allocator local_2ac [16];
  string s;
  ifstream inf;
  undefined4 local_284 [2];
  undefined4 local_27c;
  uint local_278;
  undefined1 local_274;
  undefined4 local_258;
  locale local_254 [64];
  int aiStack_214 [23];
  __basic_file<char> local_1b8 [265];
  undefined1 local_af;
  undefined *local_a8;
  ios_base local_a4 [148];
  
                    /* Unresolved local var: uint i@[DW_OP_reg6(ESI)]
                       Unresolved local var: CQuestManager * q@[???]
                       Unresolved local var: int j@[DW_OP_reg3(EBX)] */
  std::ios_base::ios_base(local_a4);
  local_a8 = (undefined *)0x8322470;
  _inf = (undefined *)std::ifstream::VTT._4_4_;
  *(undefined4 *)(&inf + *(int *)(std::ifstream::VTT._4_4_ + -0xc)) = std::ifstream::VTT._8_4_;
  std::ios::init((streambuf *)(&inf + *(int *)(std::ifstream::VTT._4_4_ + -0xc)));
  local_a8 = &DAT_08321748;
  _inf = &DAT_08321734;
  std::filebuf::filebuf((filebuf *)local_284);
  std::ios::init((streambuf *)&local_a8);
  iVar13 = std::filebuf::open(local_284,filename,8);
  if (iVar13 == 0) {
    std::ios::clear(&inf + *(int *)(_inf + -0xc),
                    *(uint *)((int)&local_278 + *(int *)(_inf + -0xc)) | 4);
  }
  std::string::string((string *)&s,script_name,local_2ac);
  uVar14 = std::string::find((char)&s,0x2e);
  if (singleton<quest::CQuestManager>::ms_singleton == (CQuestManager *)0x0) {
    __assert("instance","../../common/singleton.h",0x1a);
  }
  this_01 = singleton<quest::CQuestManager>::ms_singleton;
  std::string::string((string *)&stQuestName,(string *)&s,0,uVar14);
  pcVar15 = (char *)CQuestManager::GetQuestIndexByName((CQuestManager *)this_01,&stQuestName);
  if (pcVar15 == (char *)0x0) {
                    /* Unresolved local var: size_t __n@[???] */
    *(undefined1 *)
     ((int)stQuestName._M_dataplus._M_p + *(int *)((int)stQuestName._M_dataplus._M_p + -0xc)) =
         std::string::_Rep::_S_terminal;
    fprintf(__stderrp,"cannot find quest index for %s\n",stQuestName._M_dataplus._M_p);
    __assert("LoadStateScript","questnpc.cpp",0x60);
    LOCK();
    piVar22 = (int *)((int)stQuestName._M_dataplus._M_p + -4);
    iVar13 = *piVar22;
    *piVar22 = *piVar22 + -1;
    UNLOCK();
    if (iVar13 < 1) {
      std::string::_Rep::_M_destroy((allocator *)((int)stQuestName._M_dataplus._M_p + -0xc));
    }
    LOCK();
    piVar22 = (int *)((int)s._M_dataplus._M_p + -4);
    iVar13 = *piVar22;
    *piVar22 = *piVar22 + -1;
    UNLOCK();
    if (iVar13 < 1) {
      std::string::_Rep::_M_destroy((allocator *)((int)s._M_dataplus._M_p + -0xc));
    }
    _inf = &DAT_08321734;
    local_a8 = &DAT_08321748;
    local_284[0] = 0x83216b8;
    std::filebuf::close();
    local_af = 0;
    std::__basic_file<char>::~__basic_file(local_1b8);
    local_284[0] = 0x8322430;
    local_258 = 0;
    local_278 = 0;
    local_274 = 0;
    local_27c = 0;
    std::locale::~locale(local_254);
    *(undefined4 *)(&inf + *(int *)(std::ifstream::VTT._4_4_ + -0xc)) = std::ifstream::VTT._8_4_;
    goto LAB_081ae847;
  }
  LOCK();
  DAT_083223c8 = DAT_083223c8 + 1;
  UNLOCK();
  uVar1 = uVar14 + 1;
  stStateName._M_dataplus._M_p = (_Alloc_hider)&DAT_083223cc;
  iVar13 = std::string::find((char)&s,0x2e);
  if (iVar13 == -1) {
    if (*(uint *)((int)s._M_dataplus._M_p + -0xc) < uVar1) {
      std::__throw_out_of_range("basic_string::substr");
    }
    std::string::string((string *)&type_name,(string *)&s,uVar1,0xffffffff);
    std::string::assign((string *)&stStateName);
    LOCK();
    piVar22 = (int *)((int)type_name._M_dataplus._M_p + -4);
    iVar17 = *piVar22;
    *piVar22 = *piVar22 + -1;
    UNLOCK();
  }
  else {
    if (*(uint *)((int)s._M_dataplus._M_p + -0xc) < uVar1) {
      std::__throw_out_of_range("basic_string::substr");
    }
    std::string::string((string *)&type_name,(string *)&s,uVar1,(iVar13 - uVar14) - 1);
    std::string::assign((string *)&stStateName);
    LOCK();
    piVar22 = (int *)((int)type_name._M_dataplus._M_p + -4);
    iVar17 = *piVar22;
    *piVar22 = *piVar22 + -1;
    UNLOCK();
  }
  if (iVar17 < 1) {
    std::string::_Rep::_M_destroy((allocator *)((int)type_name._M_dataplus._M_p + -0xc));
  }
  state_index = CQuestManager::GetQuestStateIndex
                          ((CQuestManager *)this_01,&stQuestName,&stStateName);
                    /* Unresolved local var: size_t __n@[???] */
  *(undefined1 *)
   ((int)stStateName._M_dataplus._M_p + *(int *)((int)stStateName._M_dataplus._M_p + -0xc)) =
       std::string::_Rep::_S_terminal;
                    /* Unresolved local var: size_t __n@[???] */
  *(undefined1 *)
   ((int)stQuestName._M_dataplus._M_p + *(int *)((int)stQuestName._M_dataplus._M_p + -0xc)) =
       std::string::_Rep::_S_terminal;
  sys_log(0,"QUEST loading %s : %s [STATE] %s",filename,stQuestName._M_dataplus._M_p,
          stStateName._M_dataplus._M_p);
  if (iVar13 == -1) {
    piVar22 = *(int **)((int)aiStack_214 + *(int *)(_inf + -0xc));
    ie_1._M_sbuf = (basic_streambuf<char,std::char_traits<char>_> *)0x0;
    ie_1._M_c = -1;
    _Var23._M_node = (_Rb_tree_node_base *)(this->m_mapOwnQuest + event_index);
    local_614._4_4_ = pcVar15;
    _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>_>_>
    ::lower_bound((_Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>_>_>
                   *)&__i_16,(uint *)_Var23._M_node);
    local_2fc = this->m_mapOwnQuest[event_index]._M_t.
                super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>_>_>
                .
                super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>_>,true>
                ._M_header;
    if ((__i_16.super__Rb_tree_base_iterator._M_node == (_Rb_tree_base_iterator)local_2fc) ||
       ((uint)local_614._4_4_ < *(char **)((int)__i_16.super__Rb_tree_base_iterator._M_node + 0x10))
       ) {
      local_31c = __i_16.super__Rb_tree_base_iterator._M_node;
      local_33c.
      super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
      .
      super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
      ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
                    )(_Rb_tree_node<std::pair<const_int,_quest::AStateScriptType>_> *)0x0;
                    /* Unresolved local var:
                       _Rb_tree_node<std::pair<const_int,_quest::AStateScriptType>_> *
                       __ret@[DW_OP_reg0(EAX)] */
      local_33c.
      super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
      .
      super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
      ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
                    )std::__default_alloc_template<true,0>::allocate(0x20);
      (((_Rb_tree_node<std::pair<const_int,_quest::AStateScriptType>_> *)
       local_33c.
       super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
       .
       super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
       ._M_header)->super__Rb_tree_node_base)._M_parent = (_Rb_tree_node_base *)0x0;
      (((_Rb_tree_node<std::pair<const_int,_quest::AStateScriptType>_> *)
       local_33c.
       super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
       .
       super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
       ._M_header)->super__Rb_tree_node_base)._M_color = _M_red;
      local_33c._M_node_count = 0;
      (((_Rb_tree_node<std::pair<const_int,_quest::AStateScriptType>_> *)
       local_33c.
       super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
       .
       super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
       ._M_header)->super__Rb_tree_node_base)._M_left =
           (_Rb_tree_node_base *)
           local_33c.
           super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
           .
           super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
           ._M_header;
      (((_Rb_tree_node<std::pair<const_int,_quest::AStateScriptType>_> *)
       local_33c.
       super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
       .
       super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
       ._M_header)->super__Rb_tree_node_base)._M_right =
           (_Rb_tree_node_base *)
           local_33c.
           super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
           .
           super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
           ._M_header;
      local_32c.second._M_t.
      super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
      .
      super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
      ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
                    )(_Rb_tree_node<std::pair<const_int,_quest::AStateScriptType>_> *)0x0;
      local_32c.first = local_614._4_4_;
      local_32c.second._M_t.
      super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
      .
      super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
      ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
                    )__simple_alloc<std::_Rb_tree_node<std::pair<const_int,_quest::AStateScriptType>_>,std::__default_alloc_template<true,_0>_>
                     ::allocate(1);
      local_32c.second._M_t._M_node_count = 0;
      if (*(_Rb_tree_node_base **)
           ((int)local_33c.
                 super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
                 .
                 super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
                 ._M_header + 4) == (_Rb_tree_node_base *)0x0) {
        _Rb_tree<int,std::pair<const_int,_quest::AStateScriptType>,std::_Select1st<std::pair<const_int,_quest::AStateScriptType>_>,std::less<int>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
        ::_M_empty_initialize(&local_32c.second._M_t);
      }
      else {
        (((_Rb_tree_node<std::pair<const_int,_quest::AStateScriptType>_> *)
         local_32c.second._M_t.
         super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
         .
         super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
         ._M_header)->super__Rb_tree_node_base)._M_color = _M_red;
        p_Var21 = _Rb_tree<int,std::pair<const_int,_quest::AStateScriptType>,std::_Select1st<std::pair<const_int,_quest::AStateScriptType>_>,std::less<int>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
                  ::_M_copy(&local_32c.second._M_t,
                            *(_Rb_tree_node<std::pair<const_int,_quest::AStateScriptType>_> **)
                             ((int)local_33c.
                                   super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
                                   .
                                   super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
                                   ._M_header + 4),
                            (_Rb_tree_node<std::pair<const_int,_quest::AStateScriptType>_> *)
                            local_32c.second._M_t.
                            super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
                            .
                            super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
                            ._M_header);
        *(_Rb_tree_node<std::pair<const_int,_quest::AStateScriptType>_> **)
         ((int)local_32c.second._M_t.
               super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
               ._M_header + 4) = p_Var21;
        p_Var20 = *(_Rb_tree_node_base **)
                   ((int)local_32c.second._M_t.
                         super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
                         .
                         super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
                         ._M_header + 4);
        for (p_Var6 = (*(_Rb_tree_node_base **)
                        ((int)local_32c.second._M_t.
                              super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
                              .
                              super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
                              ._M_header + 4))->_M_left; p_Var6 != (_Rb_tree_node_base *)0x0;
            p_Var6 = p_Var6->_M_left) {
          p_Var20 = p_Var6;
        }
        *(_Rb_tree_node_base **)
         ((int)local_32c.second._M_t.
               super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
               ._M_header + 8) = p_Var20;
        p_Var20 = *(_Rb_tree_node_base **)
                   ((int)local_32c.second._M_t.
                         super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
                         .
                         super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
                         ._M_header + 4);
        for (p_Var6 = (*(_Rb_tree_node_base **)
                        ((int)local_32c.second._M_t.
                              super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
                              .
                              super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
                              ._M_header + 4))->_M_right; p_Var6 != (_Rb_tree_node_base *)0x0;
            p_Var6 = p_Var6->_M_right) {
          p_Var20 = p_Var6;
        }
        *(_Rb_tree_node_base **)
         ((int)local_32c.second._M_t.
               super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
               ._M_header + 0xc) = p_Var20;
      }
      local_32c.second._M_t._M_node_count = local_33c._M_node_count;
      local_35c[0].first = (uint)local_31c._M_node;
      _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>_>_>
      ::insert_unique(&local_30c,
                      (_Rb_tree_iterator<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>,std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>&,std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>*>
                       )_Var23._M_node,local_35c);
      __i_16.super__Rb_tree_base_iterator._M_node =
           (_Rb_tree_base_iterator)
           (_Rb_tree_base_iterator)
           local_30c.
           super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>_>_>
           .
           super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>_>,true>
           ._M_header;
      pair<const_unsigned_int,std::map<int,_quest::AStateScriptType,_std::less<int>,_std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>_>
      ::~pair(&local_32c,(int)_Var23._M_node);
      _Rb_tree<int,std::pair<const_int,_quest::AStateScriptType>,std::_Select1st<std::pair<const_int,_quest::AStateScriptType>_>,std::less<int>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
      ::clear(&local_33c);
      std::__default_alloc_template<true,0>::deallocate
                ((void *)local_33c.
                         super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
                         .
                         super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
                         ._M_header,0x20);
    }
    _Var11.super__Rb_tree_base_iterator._M_node = __i_16.super__Rb_tree_base_iterator._M_node;
    _Var23 = __i_16.super__Rb_tree_base_iterator._M_node + 0x14;
    local_614._0_4_ =
         CQuestManager::GetQuestStateIndex((CQuestManager *)this_01,&stQuestName,&stStateName);
    __v = (pair<const_int,quest::AStateScriptType> *)local_614;
    _Var24._M_current = (char *)_Var23._M_node;
    _Rb_tree<int,std::pair<const_int,_quest::AStateScriptType>,std::_Select1st<std::pair<const_int,_quest::AStateScriptType>_>,std::less<int>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
    ::lower_bound((_Rb_tree<int,std::pair<const_int,_quest::AStateScriptType>,std::_Select1st<std::pair<const_int,_quest::AStateScriptType>_>,std::less<int>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
                   *)&__i_17,(int *)_Var23._M_node);
    local_37c = *(_Rb_tree_node_base **)((int)_Var11.super__Rb_tree_base_iterator._M_node + 0x14);
    if ((__i_17.super__Rb_tree_base_iterator._M_node == (_Rb_tree_base_iterator)local_37c) ||
       ((int)local_614._0_4_ < *(int *)((int)__i_17.super__Rb_tree_base_iterator._M_node + 0x10))) {
      local_39c = __i_17.super__Rb_tree_base_iterator._M_node;
      local_3bc = (void *)0x0;
      local_3b4 = 0;
      local_3ac.second.m_code.super__Vector_base<char,std::allocator<char>_>.
      super__Vector_alloc_base<char,std::allocator<char>,true>._M_start = (char *)0x0;
      local_3ac.first = local_614._0_4_;
      local_3ac.second.m_code.super__Vector_base<char,std::allocator<char>_>.
      super__Vector_alloc_base<char,std::allocator<char>,true>._M_finish = (char *)0x0;
      local_3ac.second.m_code.super__Vector_base<char,std::allocator<char>_>.
      super__Vector_alloc_base<char,std::allocator<char>,true>._M_end_of_storage = (char *)0x0;
      local_3b8 = 0;
      memmove((void *)0x0,(void *)0x0,0);
      __v = &local_3cc;
      local_3ac.second.m_code.super__Vector_base<char,std::allocator<char>_>.
      super__Vector_alloc_base<char,std::allocator<char>,true>._M_finish = (char *)0x0;
      local_3cc.first = (int)local_39c._M_node;
      _Rb_tree<int,std::pair<const_int,_quest::AStateScriptType>,std::_Select1st<std::pair<const_int,_quest::AStateScriptType>_>,std::less<int>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
      ::insert_unique(&local_38c,
                      (_Rb_tree_iterator<std::pair<const_int,_quest::AStateScriptType>,std::pair<const_int,_quest::AStateScriptType>&,std::pair<const_int,_quest::AStateScriptType>*>
                       )_Var23._M_node,__v);
      __i_17.super__Rb_tree_base_iterator._M_node =
           (_Rb_tree_base_iterator)
           (_Rb_tree_base_iterator)
           local_38c.
           super__Rb_tree_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>_>
           .
           super__Rb_tree_alloc_base<std::pair<const_int,_quest::AStateScriptType>,std::allocator<std::pair<const_int,_quest::AStateScriptType>_>,true>
           ._M_header;
      pair<const_int,quest::AStateScriptType>::~pair(&local_3ac,(int)_Var23._M_node);
      p_Var20 = (_Rb_tree_node_base *)(local_3b4 - (int)local_3bc);
      _Var24._M_current = (char *)_Var23._M_node;
      if (p_Var20 != (_Rb_tree_node_base *)0x0) {
        std::__default_alloc_template<true,0>::deallocate(local_3bc,(uint)p_Var20);
        _Var24._M_current = (char *)p_Var20;
      }
    }
    _Var10.super__Rb_tree_base_iterator._M_node = __i_17.super__Rb_tree_base_iterator._M_node;
    this_00 = (vector<char,std::allocator<char>_> *)
              ((int)__i_17.super__Rb_tree_base_iterator._M_node + 0x14);
    local_618 = 0xffffffff;
    local_62c._M_c = -1;
    local_62c._M_sbuf = (basic_streambuf<char,std::char_traits<char>_> *)0x0;
    local_61d._1_4_ = piVar22;
LAB_081b1020:
                    /* Unresolved local var: int __eof@[???]
                       Unresolved local var: bool __thiseof@[DW_OP_reg3(EBX)]
                       Unresolved local var: bool __beof@[DW_OP_reg0(EAX)]
                       Unresolved local var: int __ret@[DW_OP_reg0(EAX)] */
    uVar14 = 0xffffffff;
    p_Var20 = (_Rb_tree_node_base *)_Var24._M_current;
    if (((int *)local_61d._1_4_ != (int *)0x0) && (uVar14 = local_618, local_618 == 0xffffffff)) {
                    /* Unresolved local var: int __ret@[DW_OP_reg0(EAX)] */
      pbVar4 = *(byte **)(local_61d._1_4_ + 0x18);
      if ((pbVar4 == (byte *)0x0) || (*(byte **)(local_61d._1_4_ + 0x1c) <= pbVar4)) {
        uVar14 = (**(code **)(*(int *)local_61d._1_4_ + 0x24))
                           (local_61d._1_4_,_Var24._M_current,__v);
      }
      else {
        uVar14 = (uint)*pbVar4;
      }
      p_Var20 = (_Rb_tree_node_base *)_Var24._M_current;
      if (uVar14 == 0xffffffff) {
        local_61d._1_4_ = (int *)0x0;
      }
    }
    iVar13 = istreambuf_iterator<char,std::char_traits<char>_>::_M_get(&local_62c);
    bVar5 = false;
    if (uVar14 == 0xffffffff) {
      if (iVar13 == -1) {
LAB_081b1120:
        bVar5 = true;
      }
    }
    else if (iVar13 != -1) goto LAB_081b1120;
    if (!bVar5) {
      iVar13 = istreambuf_iterator<char,std::char_traits<char>_>::_M_get
                         ((istreambuf_iterator<char,std::char_traits<char>_> *)(local_61d + 1));
      local_61d[0] = (char)iVar13;
      _Var24._M_current = *(char **)((int)_Var10.super__Rb_tree_base_iterator._M_node + 0x18);
      if (_Var24._M_current == *(char **)((int)_Var10.super__Rb_tree_base_iterator._M_node + 0x1c))
      {
        __v = (pair<const_int,quest::AStateScriptType> *)local_61d;
        vector<char,std::allocator<char>_>::_M_insert_aux(this_00,_Var24,(char *)__v);
      }
      else {
        iVar13 = 0;
        if (_Var24._M_current != (char *)0x0) {
          *_Var24._M_current = local_61d[0];
          iVar13 = *(int *)((int)_Var10.super__Rb_tree_base_iterator._M_node + 0x18);
        }
        *(int *)((int)_Var10.super__Rb_tree_base_iterator._M_node + 0x18) = iVar13 + 1;
        _Var24._M_current = (char *)p_Var20;
      }
                    /* Unresolved local var: int __eof@[???] */
      if (((int *)local_61d._1_4_ == (int *)0x0) ||
         (iVar13 = std::streambuf::sbumpc(), iVar13 != -1)) {
        local_618 = 0xffffffff;
      }
      else {
        local_61d._1_4_ = (int *)0x0;
      }
      goto LAB_081b1020;
    }
LAB_081af74d:
                    /* Unresolved local var: _Atomic_word __result@[DW_OP_reg2(EDX)] */
    LOCK();
    piVar22 = (int *)((int)stStateName._M_dataplus._M_p + -4);
    iVar13 = *piVar22;
    *piVar22 = *piVar22 + -1;
    UNLOCK();
    if (iVar13 < 1) {
      std::string::_Rep::_M_destroy((allocator *)((int)stStateName._M_dataplus._M_p + -0xc));
    }
                    /* Unresolved local var: _Atomic_word __result@[DW_OP_reg2(EDX)] */
    LOCK();
    piVar22 = (int *)((int)stQuestName._M_dataplus._M_p + -4);
    iVar13 = *piVar22;
    *piVar22 = *piVar22 + -1;
    UNLOCK();
    if (iVar13 < 1) {
      std::string::_Rep::_M_destroy((allocator *)((int)stQuestName._M_dataplus._M_p + -0xc));
    }
                    /* Unresolved local var: _Atomic_word __result@[DW_OP_reg2(EDX)] */
    LOCK();
    piVar22 = (int *)((int)s._M_dataplus._M_p + -4);
    iVar13 = *piVar22;
    *piVar22 = *piVar22 + -1;
    UNLOCK();
    if (iVar13 < 1) {
      std::string::_Rep::_M_destroy((allocator *)((int)s._M_dataplus._M_p + -0xc));
    }
  }
  else {
                    /* Unresolved local var: size_t __n@[???] */
    uVar14 = std::string::find((char)&s,0x2e);
    if (uVar14 == 0xffffffff) {
      _sys_err("LoadStateScript",0x8a,"invalid QUEST STATE index [%s] [%s]",filename,script_name);
      LOCK();
      piVar22 = (int *)((int)stStateName._M_dataplus._M_p + -4);
      iVar13 = *piVar22;
      *piVar22 = *piVar22 + -1;
      UNLOCK();
      if (iVar13 < 1) {
        std::string::_Rep::_M_destroy((allocator *)((int)stStateName._M_dataplus._M_p + -0xc));
      }
      LOCK();
      piVar22 = (int *)((int)stQuestName._M_dataplus._M_p + -4);
      iVar13 = *piVar22;
      *piVar22 = *piVar22 + -1;
      UNLOCK();
      if (iVar13 < 1) {
        std::string::_Rep::_M_destroy((allocator *)((int)stQuestName._M_dataplus._M_p + -0xc));
      }
    }
    else {
      if (*(uint *)((int)s._M_dataplus._M_p + -0xc) < iVar13 + 1U) {
        std::__throw_out_of_range("basic_string::substr");
      }
      std::string::string((string *)&type_name,(string *)&s,iVar13 + 1U,(uVar14 - iVar13) - 1);
      *(undefined1 *)
       ((int)type_name._M_dataplus._M_p + *(int *)((int)type_name._M_dataplus._M_p + -0xc)) =
           std::string::_Rep::_S_terminal;
      lVar16 = strtol((char *)type_name._M_dataplus._M_p,(char **)0x0,10);
      LOCK();
      piVar22 = (int *)((int)type_name._M_dataplus._M_p + -4);
      iVar13 = *piVar22;
      *piVar22 = *piVar22 + -1;
      UNLOCK();
      if (iVar13 < 1) {
        std::string::_Rep::_M_destroy((allocator *)((int)type_name._M_dataplus._M_p + -0xc));
      }
      iVar13 = std::string::find((char)&s,0x2e);
      if (iVar13 == -1) {
        if (*(uint *)((int)s._M_dataplus._M_p + -0xc) < uVar14 + 1) {
          std::__throw_out_of_range("basic_string::substr");
        }
        std::string::string((string *)&type_name,(string *)&s,uVar14 + 1,~uVar14 - 1);
        pbVar2 = *(basic_streambuf<char,std::char_traits<char>_> **)
                  ((int)aiStack_214 + *(int *)(_inf + -0xc));
        ie._M_sbuf = (basic_streambuf<char,std::char_traits<char>_> *)0x0;
        ie._M_c = -1;
        _Var23._M_node = (_Rb_tree_node_base *)(this->m_mapOwnArgQuest + event_index);
        local_640 = pcVar15;
        _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
        ::lower_bound((_Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                       *)&__i,(uint *)_Var23._M_node);
        local_3ec._M_node =
             &(this->m_mapOwnArgQuest[event_index]._M_t.
               super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,true>
              ._M_header)->super__Rb_tree_node_base;
        if ((__i.super__Rb_tree_base_iterator._M_node == (_Rb_tree_base_iterator)local_3ec._M_node)
           || (local_640 < *(char **)((int)__i.super__Rb_tree_base_iterator._M_node + 0x10))) {
          local_40c = __i.super__Rb_tree_base_iterator._M_node;
          local_42c.
          super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
          .
          super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
          ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                        )(_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                          *)0x0;
                    /* Unresolved local var:
                       _Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                       * __ret@[DW_OP_reg0(EAX)] */
          local_42c.
          super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
          .
          super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
          ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                        )std::__default_alloc_template<true,0>::allocate(0x20);
          (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
             *)local_42c.
               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
               ._M_header)->super__Rb_tree_node_base)._M_parent = (_Rb_tree_node_base *)0x0;
          (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
             *)local_42c.
               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
               ._M_header)->super__Rb_tree_node_base)._M_color = _M_red;
          local_42c._M_node_count = 0;
          (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
             *)local_42c.
               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
               ._M_header)->super__Rb_tree_node_base)._M_left =
               (_Rb_tree_node_base *)
               local_42c.
               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
               ._M_header;
          (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
             *)local_42c.
               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
               ._M_header)->super__Rb_tree_node_base)._M_right =
               (_Rb_tree_node_base *)
               local_42c.
               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
               ._M_header;
          local_41c.second._M_t.
          super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
          .
          super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
          ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                        )(_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                          *)0x0;
          local_41c.first = (uint)local_640;
          local_41c.second._M_t.
          super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
          .
          super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
          ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                        )__simple_alloc<std::_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::__default_alloc_template<true,_0>_>
                         ::allocate(1);
          local_41c.second._M_t._M_node_count = 0;
          if (*(_Rb_tree_node_base **)
               ((int)local_42c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header + 4) == (_Rb_tree_node_base *)0x0) {
            _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
            ::_M_empty_initialize(&local_41c.second._M_t);
          }
          else {
            (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
               *)local_41c.second._M_t.
                 super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                 .
                 super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                 ._M_header)->super__Rb_tree_node_base)._M_color = _M_red;
            p_Var19 = _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                      ::_M_copy(&local_41c.second._M_t,
                                *(_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                                  **)((int)local_42c.
                                           super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                                           .
                                           super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                                           ._M_header + 4),
                                (_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                                 *)local_41c.second._M_t.
                                   super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                                   .
                                   super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                                   ._M_header);
            *(_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
              **)((int)local_41c.second._M_t.
                       super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                       .
                       super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                       ._M_header + 4) = p_Var19;
            p_Var20 = *(_Rb_tree_node_base **)
                       ((int)local_41c.second._M_t.
                             super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                             .
                             super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                             ._M_header + 4);
            for (p_Var6 = (*(_Rb_tree_node_base **)
                            ((int)local_41c.second._M_t.
                                  super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                                  .
                                  super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                                  ._M_header + 4))->_M_left; p_Var6 != (_Rb_tree_node_base *)0x0;
                p_Var6 = p_Var6->_M_left) {
              p_Var20 = p_Var6;
            }
            *(_Rb_tree_node_base **)
             ((int)local_41c.second._M_t.
                   super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                   .
                   super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                   ._M_header + 8) = p_Var20;
            p_Var20 = *(_Rb_tree_node_base **)
                       ((int)local_41c.second._M_t.
                             super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                             .
                             super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                             ._M_header + 4);
            for (p_Var6 = (*(_Rb_tree_node_base **)
                            ((int)local_41c.second._M_t.
                                  super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                                  .
                                  super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                                  ._M_header + 4))->_M_right; p_Var6 != (_Rb_tree_node_base *)0x0;
                p_Var6 = p_Var6->_M_right) {
              p_Var20 = p_Var6;
            }
            *(_Rb_tree_node_base **)
             ((int)local_41c.second._M_t.
                   super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                   .
                   super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                   ._M_header + 0xc) = p_Var20;
          }
          local_41c.second._M_t._M_node_count = local_42c._M_node_count;
          __i_2.super__Rb_tree_base_iterator._M_node =
               (_Rb_tree_base_iterator)(_Rb_tree_base_iterator)local_40c._M_node;
          _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
          ::insert_unique(&local_3fc,
                          (_Rb_tree_iterator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>&,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>*>
                           )_Var23._M_node,
                          (pair<const_unsigned_int,std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>
                           *)&__i_2);
          __i.super__Rb_tree_base_iterator._M_node =
               (_Rb_tree_base_iterator)
               (_Rb_tree_base_iterator)
               local_3fc.
               super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,true>
               ._M_header;
          pair<const_unsigned_int,std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>
          ::~pair(&local_41c,(int)_Var23._M_node);
          _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
          ::clear(&local_42c);
          std::__default_alloc_template<true,0>::deallocate
                    ((void *)local_42c.
                             super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                             .
                             super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                             ._M_header,0x20);
        }
        _Var9.super__Rb_tree_base_iterator._M_node = __i.super__Rb_tree_base_iterator._M_node;
        _Var23 = __i.super__Rb_tree_base_iterator._M_node + 0x14;
        _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
        ::lower_bound((_Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                       *)&__i_1,(int *)_Var23._M_node);
        local_46c._M_node =
             *(_Rb_tree_node_base **)((int)_Var9.super__Rb_tree_base_iterator._M_node + 0x14);
        if ((__i_1.super__Rb_tree_base_iterator._M_node == (_Rb_tree_base_iterator)local_46c._M_node
            ) || (state_index < *(int *)((int)__i_1.super__Rb_tree_base_iterator._M_node + 0x10))) {
          local_48c = __i_1.super__Rb_tree_base_iterator._M_node;
          local_49c.first = state_index;
          local_49c.second.super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
          super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
          _M_start = (AArgScript *)0x0;
          local_49c.second.super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
          super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
          _M_finish = (AArgScript *)0x0;
          local_49c.second.super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
          super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
          _M_end_of_storage = (AArgScript *)0x0;
          local_49c.second.super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
          super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
          _M_finish = __uninitialized_copy_aux<__gnu_cxx::__normal_iterator<const_quest::AArgScript*,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,_quest::AArgScript*>
                                ((__normal_iterator<const_quest::AArgScript*,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                                  )0x0,(__normal_iterator<const_quest::AArgScript*,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                                        )0x0,(AArgScript *)0x0,(__false_type)0x0);
          __i_3.super__Rb_tree_base_iterator._M_node =
               (_Rb_tree_base_iterator)(_Rb_tree_base_iterator)local_48c._M_node;
          _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
          ::insert_unique(&local_47c,
                          (_Rb_tree_iterator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>&,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>*>
                           )_Var23._M_node,
                          (pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                           *)&__i_3);
          __i_1.super__Rb_tree_base_iterator._M_node =
               (_Rb_tree_base_iterator)
               (_Rb_tree_base_iterator)
               local_47c.
               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
               ._M_header;
          pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>::~pair
                    (&local_49c,(int)_Var23._M_node);
        }
        _Var8.super__Rb_tree_base_iterator._M_node = __i_1.super__Rb_tree_base_iterator._M_node;
        this_02 = (vector<quest::AArgScript,std::allocator<quest::AArgScript>_> *)
                  ((int)__i_1.super__Rb_tree_base_iterator._M_node + 0x14);
        _Var23._M_node = (_Rb_tree_node_base *)(this->m_mapOwnArgQuest + event_index);
        local_648 = pcVar15;
        _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
        ::lower_bound((_Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                       *)&__i_2,(uint *)_Var23._M_node);
        local_40c._M_node =
             &(this->m_mapOwnArgQuest[event_index]._M_t.
               super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,true>
              ._M_header)->super__Rb_tree_node_base;
        if ((__i_2.super__Rb_tree_base_iterator._M_node == (_Rb_tree_base_iterator)local_40c._M_node
            ) || (local_648 < *(char **)((int)__i_2.super__Rb_tree_base_iterator._M_node + 0x10))) {
          local_3ec = __i_2.super__Rb_tree_base_iterator._M_node;
          local_42c.
          super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
          .
          super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
          ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                        )(_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                          *)0x0;
                    /* Unresolved local var:
                       _Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                       * __ret@[DW_OP_reg0(EAX)] */
          local_42c.
          super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
          .
          super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
          ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                        )std::__default_alloc_template<true,0>::allocate(0x20);
          (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
             *)local_42c.
               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
               ._M_header)->super__Rb_tree_node_base)._M_parent = (_Rb_tree_node_base *)0x0;
          (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
             *)local_42c.
               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
               ._M_header)->super__Rb_tree_node_base)._M_color = _M_red;
          local_42c._M_node_count = 0;
          (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
             *)local_42c.
               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
               ._M_header)->super__Rb_tree_node_base)._M_left =
               (_Rb_tree_node_base *)
               local_42c.
               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
               ._M_header;
          (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
             *)local_42c.
               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
               ._M_header)->super__Rb_tree_node_base)._M_right =
               (_Rb_tree_node_base *)
               local_42c.
               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
               ._M_header;
          local_4ec = local_648;
          _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
          ::_Rb_tree(&local_4e8,&local_42c);
          __i.super__Rb_tree_base_iterator._M_node =
               (_Rb_tree_base_iterator)(_Rb_tree_base_iterator)local_3ec._M_node;
          _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
          ::insert_unique(&local_3fc,
                          (_Rb_tree_iterator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>&,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>*>
                           )_Var23._M_node,
                          (pair<const_unsigned_int,std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>
                           *)&__i);
          __i_2.super__Rb_tree_base_iterator._M_node =
               (_Rb_tree_base_iterator)
               (_Rb_tree_base_iterator)
               local_3fc.
               super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,true>
               ._M_header;
          _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
          ::~_Rb_tree(&local_4e8,(int)_Var23._M_node);
          _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
          ::clear(&local_42c);
          std::__default_alloc_template<true,0>::deallocate
                    ((void *)local_42c.
                             super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                             .
                             super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                             ._M_header,0x20);
        }
        _Var9.super__Rb_tree_base_iterator._M_node = __i_2.super__Rb_tree_base_iterator._M_node;
        _Var23 = __i_2.super__Rb_tree_base_iterator._M_node + 0x14;
        _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
        ::lower_bound((_Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                       *)&__i_3,(int *)_Var23._M_node);
        local_48c._M_node =
             *(_Rb_tree_node_base **)((int)_Var9.super__Rb_tree_base_iterator._M_node + 0x14);
        if ((__i_3.super__Rb_tree_base_iterator._M_node == (_Rb_tree_base_iterator)local_48c._M_node
            ) || (state_index < *(int *)((int)__i_3.super__Rb_tree_base_iterator._M_node + 0x10))) {
          local_46c = __i_3.super__Rb_tree_base_iterator._M_node;
          local_4fc.second.super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
          super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
          _M_end_of_storage = (AArgScript *)0x0;
          local_4fc.first = state_index;
          local_4fc.second.super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
          super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
          _M_start = (AArgScript *)0x0;
          local_4fc.second.super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
          super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
          _M_finish = (AArgScript *)0x0;
          local_4fc.second.super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
          super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
          _M_start = __simple_alloc<quest::AArgScript,std::__default_alloc_template<true,_0>_>::
                     allocate(0);
          local_4fc.second.super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
          super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
          _M_finish = local_4fc.second.
                      super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                      super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>
                      ._M_start;
          local_4fc.second.super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
          super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
          _M_end_of_storage =
               local_4fc.second.
               super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
               super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
               _M_start;
          local_4fc.second.super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
          super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
          _M_finish = __uninitialized_copy_aux<__gnu_cxx::__normal_iterator<const_quest::AArgScript*,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,_quest::AArgScript*>
                                ((__normal_iterator<const_quest::AArgScript*,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                                  )0x0,(__normal_iterator<const_quest::AArgScript*,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                                        )0x0,
                                 local_4fc.second.
                                 super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>
                                 .
                                 super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>
                                 ._M_start,(__false_type)0x0);
          __i_1.super__Rb_tree_base_iterator._M_node =
               (_Rb_tree_base_iterator)(_Rb_tree_base_iterator)local_46c._M_node;
          _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
          ::insert_unique(&local_47c,
                          (_Rb_tree_iterator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>&,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>*>
                           )_Var23._M_node,
                          (pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                           *)&__i_1);
          __i_3.super__Rb_tree_base_iterator._M_node =
               (_Rb_tree_base_iterator)
               (_Rb_tree_base_iterator)
               local_47c.
               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
               ._M_header;
          pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>::~pair
                    (&local_4fc,(int)_Var23._M_node);
        }
        uVar14 = MAX(lVar16 + 1,
                     (*(int *)((int)__i_3.super__Rb_tree_base_iterator._M_node + 0x18) -
                      *(int *)((int)__i_3.super__Rb_tree_base_iterator._M_node + 0x14) >> 2) *
                     0x38e38e39);
        LOCK();
        DAT_083223c8 = DAT_083223c8 + 1;
        UNLOCK();
        local_52c.when_condition.super__Vector_base<char,std::allocator<char>_>.
        super__Vector_alloc_base<char,std::allocator<char>,true>._M_start = (char *)0x0;
        local_52c.script.m_code.super__Vector_base<char,std::allocator<char>_>.
        super__Vector_alloc_base<char,std::allocator<char>,true>._M_finish = (char *)0x0;
        local_52c.arg._M_dataplus._M_p = (_Alloc_hider)&DAT_083223cc;
        local_52c.script.m_code.super__Vector_base<char,std::allocator<char>_>.
        super__Vector_alloc_base<char,std::allocator<char>,true>._M_start = (char *)0x0;
        __position._M_current =
             *(AArgScript **)((int)_Var8.super__Rb_tree_base_iterator._M_node + 0x18);
        local_52c.when_condition.super__Vector_base<char,std::allocator<char>_>.
        super__Vector_alloc_base<char,std::allocator<char>,true>._M_end_of_storage = (char *)0x0;
        pAVar3 = (this_02->super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>)
                 .super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>
                 ._M_start;
        local_52c.when_condition.super__Vector_base<char,std::allocator<char>_>.
        super__Vector_alloc_base<char,std::allocator<char>,true>._M_finish = (char *)0x0;
        local_52c.script.m_code.super__Vector_base<char,std::allocator<char>_>.
        super__Vector_alloc_base<char,std::allocator<char>,true>._M_end_of_storage = (char *)0x0;
        iVar13 = (int)__position._M_current - (int)pAVar3 >> 2;
        if (uVar14 < (uint)(iVar13 * 0x38e38e39)) {
          __first._M_current = pAVar3 + uVar14;
          vector<quest::AArgScript,std::allocator<quest::AArgScript>_>::erase
                    (this_02,__first,__position);
          __position._M_current = __first._M_current;
        }
        else {
          vector<quest::AArgScript,std::allocator<quest::AArgScript>_>::_M_fill_insert
                    (this_02,__position,uVar14 + iVar13 * -0x38e38e39,&local_52c);
        }
        AArgScript::~AArgScript(&local_52c,(int)__position._M_current);
        iVar17 = std::string::compare((char *)&type_name);
        iVar13 = ie._M_c;
        pbVar7 = ie._M_sbuf;
        if (iVar17 == 0) {
          _Var23._M_node = (_Rb_tree_node_base *)(this->m_mapOwnArgQuest + event_index);
          local_64c = pcVar15;
          _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
          ::lower_bound((_Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                         *)&__i_2,(uint *)_Var23._M_node);
          local_40c._M_node =
               &(this->m_mapOwnArgQuest[event_index]._M_t.
                 super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                 .
                 super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,true>
                ._M_header)->super__Rb_tree_node_base;
          if ((__i_2.super__Rb_tree_base_iterator._M_node ==
               (_Rb_tree_base_iterator)local_40c._M_node) ||
             (local_64c < *(char **)((int)__i_2.super__Rb_tree_base_iterator._M_node + 0x10))) {
            local_3ec = __i_2.super__Rb_tree_base_iterator._M_node;
            local_42c.
            super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
            .
            super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
            ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                          )(_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                            *)0x0;
                    /* Unresolved local var:
                       _Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                       * __ret@[DW_OP_reg0(EAX)] */
            local_42c.
            super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
            .
            super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
            ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                          )std::__default_alloc_template<true,0>::allocate(0x20);
            (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
               *)local_42c.
                 super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                 .
                 super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                 ._M_header)->super__Rb_tree_node_base)._M_parent = (_Rb_tree_node_base *)0x0;
            (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
               *)local_42c.
                 super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                 .
                 super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                 ._M_header)->super__Rb_tree_node_base)._M_color = _M_red;
            local_42c._M_node_count = 0;
            (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
               *)local_42c.
                 super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                 .
                 super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                 ._M_header)->super__Rb_tree_node_base)._M_left =
                 (_Rb_tree_node_base *)
                 local_42c.
                 super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                 .
                 super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                 ._M_header;
            (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
               *)local_42c.
                 super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                 .
                 super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                 ._M_header)->super__Rb_tree_node_base)._M_right =
                 (_Rb_tree_node_base *)
                 local_42c.
                 super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                 .
                 super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                 ._M_header;
            local_53c = local_64c;
            _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
            ::_Rb_tree(&local_538,&local_42c);
            __i.super__Rb_tree_base_iterator._M_node =
                 (_Rb_tree_base_iterator)(_Rb_tree_base_iterator)local_3ec._M_node;
            _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
            ::insert_unique(&local_3fc,
                            (_Rb_tree_iterator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>&,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>*>
                             )_Var23._M_node,
                            (pair<const_unsigned_int,std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>
                             *)&__i);
            __i_2.super__Rb_tree_base_iterator._M_node =
                 (_Rb_tree_base_iterator)
                 (_Rb_tree_base_iterator)
                 local_3fc.
                 super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                 .
                 super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,true>
                 ._M_header;
            _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
            ::~_Rb_tree(&local_538,(int)_Var23._M_node);
            _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
            ::clear(&local_42c);
            std::__default_alloc_template<true,0>::deallocate
                      ((void *)local_42c.
                               super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                               .
                               super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                               ._M_header,0x20);
          }
          _Var9.super__Rb_tree_base_iterator._M_node = __i_2.super__Rb_tree_base_iterator._M_node;
          _Var23 = __i_2.super__Rb_tree_base_iterator._M_node + 0x14;
          _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
          ::lower_bound((_Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                         *)&__i_3,(int *)_Var23._M_node);
          local_48c._M_node =
               *(_Rb_tree_node_base **)((int)_Var9.super__Rb_tree_base_iterator._M_node + 0x14);
          if ((__i_3.super__Rb_tree_base_iterator._M_node ==
               (_Rb_tree_base_iterator)local_48c._M_node) ||
             (state_index < *(int *)((int)__i_3.super__Rb_tree_base_iterator._M_node + 0x10))) {
            local_46c = __i_3.super__Rb_tree_base_iterator._M_node;
            local_54c.second.
            super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
            super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
            _M_end_of_storage = (AArgScript *)0x0;
            local_54c.second.
            super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
            super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
            _M_finish = (AArgScript *)0x0;
            local_54c.first = state_index;
            local_54c.second.
            super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
            super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
            _M_start = (AArgScript *)0x0;
            local_54c.second.
            super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
            super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
            _M_start = __simple_alloc<quest::AArgScript,std::__default_alloc_template<true,_0>_>::
                       allocate(0);
            local_54c.second.
            super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
            super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
            _M_finish = local_54c.second.
                        super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                        super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>
                        ._M_start;
            local_54c.second.
            super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
            super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
            _M_end_of_storage =
                 local_54c.second.
                 super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                 super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                 _M_start;
            local_54c.second.
            super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
            super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
            _M_finish = __uninitialized_copy_aux<__gnu_cxx::__normal_iterator<const_quest::AArgScript*,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,_quest::AArgScript*>
                                  ((__normal_iterator<const_quest::AArgScript*,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                                    )0x0,(__normal_iterator<const_quest::AArgScript*,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                                          )0x0,
                                   local_54c.second.
                                   super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>
                                   .
                                   super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>
                                   ._M_start,(__false_type)0x0);
            __i_1.super__Rb_tree_base_iterator._M_node =
                 (_Rb_tree_base_iterator)(_Rb_tree_base_iterator)local_46c._M_node;
            _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
            ::insert_unique(&local_47c,
                            (_Rb_tree_iterator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>&,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>*>
                             )_Var23._M_node,
                            (pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                             *)&__i_1);
            __i_3.super__Rb_tree_base_iterator._M_node =
                 (_Rb_tree_base_iterator)
                 (_Rb_tree_base_iterator)
                 local_47c.
                 super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                 .
                 super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                 ._M_header;
            pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>::
            ~pair(&local_54c,(int)_Var23._M_node);
          }
          iVar17 = lVar16 * 0x24 + *(int *)((int)__i_3.super__Rb_tree_base_iterator._M_node + 0x14);
          local_664._M_c = iVar13;
          local_654._M_c = -1;
          local_664._M_sbuf = pbVar7;
          local_654._M_sbuf = pbVar2;
LAB_081af690:
                    /* Unresolved local var: int __eof@[???]
                       Unresolved local var: bool __thiseof@[DW_OP_reg3(EBX)]
                       Unresolved local var: bool __beof@[DW_OP_reg0(EAX)] */
          iVar13 = istreambuf_iterator<char,std::char_traits<char>_>::_M_get(&local_654);
          iVar18 = istreambuf_iterator<char,std::char_traits<char>_>::_M_get(&local_664);
          bVar5 = false;
          if (iVar13 == -1) {
            if (iVar18 == -1) {
LAB_081af813:
              bVar5 = true;
            }
          }
          else if (iVar18 != -1) goto LAB_081af813;
          if (!bVar5) {
            iVar13 = istreambuf_iterator<char,std::char_traits<char>_>::_M_get(&local_654);
            local_655 = (char)iVar13;
            _Var24._M_current = *(char **)(iVar17 + 8);
            if (_Var24._M_current == *(char **)(iVar17 + 0xc)) {
              vector<char,std::allocator<char>_>::_M_insert_aux
                        ((vector<char,std::allocator<char>_> *)(iVar17 + 4),_Var24,&local_655);
            }
            else {
              iVar13 = 0;
              if (_Var24._M_current != (char *)0x0) {
                *_Var24._M_current = local_655;
                iVar13 = *(int *)(iVar17 + 8);
              }
              *(int *)(iVar17 + 8) = iVar13 + 1;
            }
                    /* Unresolved local var: int __eof@[???] */
            if ((local_654._M_sbuf == (basic_streambuf<char,std::char_traits<char>_> *)0x0) ||
               (iVar13 = std::streambuf::sbumpc(), iVar13 != -1)) {
              local_654._M_c = -1;
            }
            else {
              local_654._M_sbuf = (basic_streambuf<char,std::char_traits<char>_> *)0x0;
            }
            goto LAB_081af690;
          }
        }
        else {
          iVar13 = std::string::compare((char *)&type_name);
          if (iVar13 == 0) {
            LOCK();
            DAT_083223c8 = DAT_083223c8 + 1;
            UNLOCK();
            s_1._M_dataplus._M_p = (_Alloc_hider)&DAT_083223cc;
            cVar12 = std::ios::widen((char)&inf + (char)*(undefined4 *)(_inf + -0xc));
            std::getline<char,std::char_traits<char>,std::allocator<char>>
                      ((istream *)&inf,(string *)&s_1,cVar12);
            _Var23._M_node = (_Rb_tree_node_base *)(this->m_mapOwnArgQuest + event_index);
            local_668 = pcVar15;
            _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
            ::lower_bound((_Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                           *)&__i_2,(uint *)_Var23._M_node);
            local_40c._M_node =
                 &(this->m_mapOwnArgQuest[event_index]._M_t.
                   super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                   .
                   super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,true>
                  ._M_header)->super__Rb_tree_node_base;
            if ((__i_2.super__Rb_tree_base_iterator._M_node ==
                 (_Rb_tree_base_iterator)local_40c._M_node) ||
               (local_668 < *(char **)((int)__i_2.super__Rb_tree_base_iterator._M_node + 0x10))) {
              local_3ec = __i_2.super__Rb_tree_base_iterator._M_node;
              local_42c.
              super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
              .
              super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
              ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                            )(_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                              *)0x0;
                    /* Unresolved local var:
                       _Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                       * __ret@[DW_OP_reg0(EAX)] */
              local_42c.
              super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
              .
              super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
              ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                            )std::__default_alloc_template<true,0>::allocate(0x20);
              (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                 *)local_42c.
                   super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                   .
                   super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                   ._M_header)->super__Rb_tree_node_base)._M_parent = (_Rb_tree_node_base *)0x0;
              (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                 *)local_42c.
                   super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                   .
                   super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                   ._M_header)->super__Rb_tree_node_base)._M_color = _M_red;
              local_42c._M_node_count = 0;
              (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                 *)local_42c.
                   super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                   .
                   super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                   ._M_header)->super__Rb_tree_node_base)._M_left =
                   (_Rb_tree_node_base *)
                   local_42c.
                   super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                   .
                   super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                   ._M_header;
              (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                 *)local_42c.
                   super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                   .
                   super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                   ._M_header)->super__Rb_tree_node_base)._M_right =
                   (_Rb_tree_node_base *)
                   local_42c.
                   super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                   .
                   super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                   ._M_header;
              local_56c = local_668;
              _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
              ::_Rb_tree(&local_568,&local_42c);
              __i.super__Rb_tree_base_iterator._M_node =
                   (_Rb_tree_base_iterator)(_Rb_tree_base_iterator)local_3ec._M_node;
              _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
              ::insert_unique(&local_3fc,
                              (_Rb_tree_iterator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>&,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>*>
                               )_Var23._M_node,
                              (pair<const_unsigned_int,std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>
                               *)&__i);
              __i_2.super__Rb_tree_base_iterator._M_node =
                   (_Rb_tree_base_iterator)
                   (_Rb_tree_base_iterator)
                   local_3fc.
                   super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                   .
                   super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,true>
                   ._M_header;
              _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
              ::~_Rb_tree(&local_568,(int)_Var23._M_node);
              _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
              ::clear(&local_42c);
              std::__default_alloc_template<true,0>::deallocate
                        ((void *)local_42c.
                                 super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                                 .
                                 super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                                 ._M_header,0x20);
            }
            _Var9.super__Rb_tree_base_iterator._M_node = __i_2.super__Rb_tree_base_iterator._M_node;
            _Var23 = __i_2.super__Rb_tree_base_iterator._M_node + 0x14;
            _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
            ::lower_bound((_Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                           *)&__i_3,(int *)_Var23._M_node);
            local_48c._M_node =
                 *(_Rb_tree_node_base **)((int)_Var9.super__Rb_tree_base_iterator._M_node + 0x14);
            if ((__i_3.super__Rb_tree_base_iterator._M_node ==
                 (_Rb_tree_base_iterator)local_48c._M_node) ||
               (state_index < *(int *)((int)__i_3.super__Rb_tree_base_iterator._M_node + 0x10))) {
              local_46c = __i_3.super__Rb_tree_base_iterator._M_node;
              local_57c.second.
              super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
              super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
              _M_end_of_storage = (AArgScript *)0x0;
              local_57c.first = state_index;
              local_57c.second.
              super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
              super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
              _M_start = (AArgScript *)0x0;
              local_57c.second.
              super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
              super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
              _M_finish = (AArgScript *)0x0;
              local_57c.second.
              super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
              super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
              _M_start = __simple_alloc<quest::AArgScript,std::__default_alloc_template<true,_0>_>::
                         allocate(0);
              local_57c.second.
              super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
              super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
              _M_finish = local_57c.second.
                          super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                          super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>
                          ._M_start;
              local_57c.second.
              super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
              super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
              _M_end_of_storage =
                   local_57c.second.
                   super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                   super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>
                   ._M_start;
              local_57c.second.
              super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
              super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
              _M_finish = __uninitialized_copy_aux<__gnu_cxx::__normal_iterator<const_quest::AArgScript*,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,_quest::AArgScript*>
                                    ((__normal_iterator<const_quest::AArgScript*,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                                      )0x0,(__normal_iterator<const_quest::AArgScript*,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                                            )0x0,
                                     local_57c.second.
                                     super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>
                                     .
                                     super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>
                                     ._M_start,(__false_type)0x0);
              __i_1.super__Rb_tree_base_iterator._M_node =
                   (_Rb_tree_base_iterator)(_Rb_tree_base_iterator)local_46c._M_node;
              _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
              ::insert_unique(&local_47c,
                              (_Rb_tree_iterator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>&,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>*>
                               )_Var23._M_node,
                              (pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                               *)&__i_1);
              __i_3.super__Rb_tree_base_iterator._M_node =
                   (_Rb_tree_base_iterator)
                   (_Rb_tree_base_iterator)
                   local_47c.
                   super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                   .
                   super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                   ._M_header;
              pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>::
              ~pair(&local_57c,(int)_Var23._M_node);
            }
            piVar22 = (int *)(lVar16 * 0x24 +
                             *(int *)((int)__i_3.super__Rb_tree_base_iterator._M_node + 0x14));
            std::string::_M_mutate((uint)piVar22,0,*(uint *)(*piVar22 + -0xc));
            if (-1 < *(int *)((int)s_1._M_dataplus._M_p + -4)) {
              std::string::_M_leak_hard();
            }
            it._M_current = (char *)s_1._M_dataplus._M_p;
            while( true ) {
              if (-1 < *(int *)((int)s_1._M_dataplus._M_p + -4)) {
                std::string::_M_leak_hard();
              }
              if (it._M_current ==
                  (char *)((int)s_1._M_dataplus._M_p + *(int *)((int)s_1._M_dataplus._M_p + -0xc)))
              break;
              _Var23._M_node = (_Rb_tree_node_base *)(this->m_mapOwnArgQuest + event_index);
              local_66c = pcVar15;
              _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
              ::lower_bound((_Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                             *)&__i_2,(uint *)_Var23._M_node);
              local_40c._M_node =
                   &(this->m_mapOwnArgQuest[event_index]._M_t.
                     super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,true>
                    ._M_header)->super__Rb_tree_node_base;
              if ((__i_2.super__Rb_tree_base_iterator._M_node ==
                   (_Rb_tree_base_iterator)local_40c._M_node) ||
                 (local_66c < *(char **)((int)__i_2.super__Rb_tree_base_iterator._M_node + 0x10))) {
                local_3ec = __i_2.super__Rb_tree_base_iterator._M_node;
                local_42c.
                super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                .
                super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                              )(_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                                *)0x0;
                    /* Unresolved local var:
                       _Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                       * __ret@[DW_OP_reg0(EAX)] */
                local_42c.
                super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                .
                super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                              )std::__default_alloc_template<true,0>::allocate(0x20);
                (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                   *)local_42c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header)->super__Rb_tree_node_base)._M_parent = (_Rb_tree_node_base *)0x0;
                (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                   *)local_42c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header)->super__Rb_tree_node_base)._M_color = _M_red;
                local_42c._M_node_count = 0;
                (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                   *)local_42c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header)->super__Rb_tree_node_base)._M_left =
                     (_Rb_tree_node_base *)
                     local_42c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header;
                (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                   *)local_42c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header)->super__Rb_tree_node_base)._M_right =
                     (_Rb_tree_node_base *)
                     local_42c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header;
                local_58c = local_66c;
                _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                ::_Rb_tree(&local_588,&local_42c);
                __i.super__Rb_tree_base_iterator._M_node =
                     (_Rb_tree_base_iterator)(_Rb_tree_base_iterator)local_3ec._M_node;
                _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                ::insert_unique(&local_3fc,
                                (_Rb_tree_iterator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>&,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>*>
                                 )_Var23._M_node,
                                (pair<const_unsigned_int,std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>
                                 *)&__i);
                __i_2.super__Rb_tree_base_iterator._M_node =
                     (_Rb_tree_base_iterator)
                     (_Rb_tree_base_iterator)
                     local_3fc.
                     super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,true>
                     ._M_header;
                _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                ::~_Rb_tree(&local_588,(int)_Var23._M_node);
                _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                ::clear(&local_42c);
                std::__default_alloc_template<true,0>::deallocate
                          ((void *)local_42c.
                                   super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                                   .
                                   super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                                   ._M_header,0x20);
              }
              _Var9.super__Rb_tree_base_iterator._M_node =
                   __i_2.super__Rb_tree_base_iterator._M_node;
              _Var23 = __i_2.super__Rb_tree_base_iterator._M_node + 0x14;
              _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
              ::lower_bound((_Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                             *)&__i_3,(int *)_Var23._M_node);
              local_48c._M_node =
                   *(_Rb_tree_node_base **)((int)_Var9.super__Rb_tree_base_iterator._M_node + 0x14);
              if ((__i_3.super__Rb_tree_base_iterator._M_node ==
                   (_Rb_tree_base_iterator)local_48c._M_node) ||
                 (state_index < *(int *)((int)__i_3.super__Rb_tree_base_iterator._M_node + 0x10))) {
                local_46c = __i_3.super__Rb_tree_base_iterator._M_node;
                local_59c.first = state_index;
                local_59c.second.
                super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                _M_start = (AArgScript *)0x0;
                local_59c.second.
                super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                _M_finish = (AArgScript *)0x0;
                local_59c.second.
                super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                _M_end_of_storage = (AArgScript *)0x0;
                local_59c.second.
                super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                _M_start = __simple_alloc<quest::AArgScript,std::__default_alloc_template<true,_0>_>
                           ::allocate(0);
                local_59c.second.
                super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                _M_finish = local_59c.second.
                            super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>
                            .
                            super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>
                            ._M_start;
                local_59c.second.
                super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                _M_end_of_storage =
                     local_59c.second.
                     super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                     super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>
                     ._M_start;
                local_59c.second.
                super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                _M_finish = __uninitialized_copy_aux<__gnu_cxx::__normal_iterator<const_quest::AArgScript*,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,_quest::AArgScript*>
                                      ((__normal_iterator<const_quest::AArgScript*,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                                        )0x0,(__normal_iterator<const_quest::AArgScript*,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                                              )0x0,
                                       local_59c.second.
                                       super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>
                                       .
                                       super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>
                                       ._M_start,(__false_type)0x0);
                __i_1.super__Rb_tree_base_iterator._M_node =
                     (_Rb_tree_base_iterator)(_Rb_tree_base_iterator)local_46c._M_node;
                _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                ::insert_unique(&local_47c,
                                (_Rb_tree_iterator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>&,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>*>
                                 )_Var23._M_node,
                                (pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                                 *)&__i_1);
                __i_3.super__Rb_tree_base_iterator._M_node =
                     (_Rb_tree_base_iterator)
                     (_Rb_tree_base_iterator)
                     local_47c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header;
                pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                ::~pair(&local_59c,(int)_Var23._M_node);
              }
              std::string::append(lVar16 * 0x24 +
                                  *(int *)((int)__i_3.super__Rb_tree_base_iterator._M_node + 0x14),
                                  '\x01');
              it._M_current = it._M_current + 1;
            }
                    /* Unresolved local var: _Atomic_word __result@[DW_OP_reg0(EAX)] */
            LOCK();
            piVar22 = (int *)((int)s_1._M_dataplus._M_p + -4);
            iVar13 = *piVar22;
            *piVar22 = *piVar22 + -1;
            UNLOCK();
            if (iVar13 < 1) {
              std::string::_Rep::_M_destroy((allocator *)((int)s_1._M_dataplus._M_p + -0xc));
            }
          }
          else {
            iVar17 = std::string::compare((char *)&type_name);
            iVar13 = ie._M_c;
            pbVar7 = ie._M_sbuf;
            if (iVar17 == 0) {
              _Var23._M_node = (_Rb_tree_node_base *)(this->m_mapOwnArgQuest + event_index);
              local_670 = pcVar15;
              _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
              ::lower_bound((_Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                             *)&__i_2,(uint *)_Var23._M_node);
              local_40c._M_node =
                   &(this->m_mapOwnArgQuest[event_index]._M_t.
                     super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,true>
                    ._M_header)->super__Rb_tree_node_base;
              if ((__i_2.super__Rb_tree_base_iterator._M_node ==
                   (_Rb_tree_base_iterator)local_40c._M_node) ||
                 (local_670 < *(char **)((int)__i_2.super__Rb_tree_base_iterator._M_node + 0x10))) {
                local_3ec = __i_2.super__Rb_tree_base_iterator._M_node;
                local_42c.
                super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                .
                super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                              )(_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                                *)0x0;
                    /* Unresolved local var:
                       _Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                       * __ret@[DW_OP_reg0(EAX)] */
                local_42c.
                super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                .
                super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                              )std::__default_alloc_template<true,0>::allocate(0x20);
                (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                   *)local_42c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header)->super__Rb_tree_node_base)._M_parent = (_Rb_tree_node_base *)0x0;
                (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                   *)local_42c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header)->super__Rb_tree_node_base)._M_color = _M_red;
                local_42c._M_node_count = 0;
                (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                   *)local_42c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header)->super__Rb_tree_node_base)._M_left =
                     (_Rb_tree_node_base *)
                     local_42c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header;
                (((_Rb_tree_node<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>
                   *)local_42c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header)->super__Rb_tree_node_base)._M_right =
                     (_Rb_tree_node_base *)
                     local_42c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header;
                local_5ac = local_670;
                _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                ::_Rb_tree(&local_5a8,&local_42c);
                __i.super__Rb_tree_base_iterator._M_node =
                     (_Rb_tree_base_iterator)(_Rb_tree_base_iterator)local_3ec._M_node;
                _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                ::insert_unique(&local_3fc,
                                (_Rb_tree_iterator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>&,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>*>
                                 )_Var23._M_node,
                                (pair<const_unsigned_int,std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>
                                 *)&__i);
                __i_2.super__Rb_tree_base_iterator._M_node =
                     (_Rb_tree_base_iterator)
                     (_Rb_tree_base_iterator)
                     local_3fc.
                     super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,true>
                     ._M_header;
                _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                ::~_Rb_tree(&local_5a8,(int)_Var23._M_node);
                _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                ::clear(&local_42c);
                std::__default_alloc_template<true,0>::deallocate
                          ((void *)local_42c.
                                   super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                                   .
                                   super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                                   ._M_header,0x20);
              }
              _Var9.super__Rb_tree_base_iterator._M_node =
                   __i_2.super__Rb_tree_base_iterator._M_node;
              _Var23 = __i_2.super__Rb_tree_base_iterator._M_node + 0x14;
              _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
              ::lower_bound((_Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                             *)&__i_3,(int *)_Var23._M_node);
              local_48c._M_node =
                   *(_Rb_tree_node_base **)((int)_Var9.super__Rb_tree_base_iterator._M_node + 0x14);
              if ((__i_3.super__Rb_tree_base_iterator._M_node ==
                   (_Rb_tree_base_iterator)local_48c._M_node) ||
                 (state_index < *(int *)((int)__i_3.super__Rb_tree_base_iterator._M_node + 0x10))) {
                local_46c = __i_3.super__Rb_tree_base_iterator._M_node;
                local_5bc.second.
                super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                _M_end_of_storage = (AArgScript *)0x0;
                local_5bc.second.
                super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                _M_finish = (AArgScript *)0x0;
                local_5bc.first = state_index;
                local_5bc.second.
                super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                _M_start = (AArgScript *)0x0;
                local_5bc.second.
                super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                _M_start = __simple_alloc<quest::AArgScript,std::__default_alloc_template<true,_0>_>
                           ::allocate(0);
                local_5bc.second.
                super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                _M_finish = local_5bc.second.
                            super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>
                            .
                            super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>
                            ._M_start;
                local_5bc.second.
                super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                _M_end_of_storage =
                     local_5bc.second.
                     super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                     super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>
                     ._M_start;
                local_5bc.second.
                super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>.
                super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>.
                _M_finish = __uninitialized_copy_aux<__gnu_cxx::__normal_iterator<const_quest::AArgScript*,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,_quest::AArgScript*>
                                      ((__normal_iterator<const_quest::AArgScript*,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                                        )0x0,(__normal_iterator<const_quest::AArgScript*,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                                              )0x0,
                                       local_5bc.second.
                                       super__Vector_base<quest::AArgScript,std::allocator<quest::AArgScript>_>
                                       .
                                       super__Vector_alloc_base<quest::AArgScript,std::allocator<quest::AArgScript>,true>
                                       ._M_start,(__false_type)0x0);
                __i_1.super__Rb_tree_base_iterator._M_node =
                     (_Rb_tree_base_iterator)(_Rb_tree_base_iterator)local_46c._M_node;
                _Rb_tree<int,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::_Select1st<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,std::less<int>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                ::insert_unique(&local_47c,
                                (_Rb_tree_iterator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>&,std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>*>
                                 )_Var23._M_node,
                                (pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                                 *)&__i_1);
                __i_3.super__Rb_tree_base_iterator._M_node =
                     (_Rb_tree_base_iterator)
                     (_Rb_tree_base_iterator)
                     local_47c.
                     super__Rb_tree_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>
                     .
                     super__Rb_tree_alloc_base<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>,std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>,true>
                     ._M_header;
                pair<const_int,std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>
                ::~pair(&local_5bc,(int)_Var23._M_node);
              }
              iVar17 = lVar16 * 0x24 +
                       *(int *)((int)__i_3.super__Rb_tree_base_iterator._M_node + 0x14);
              local_68c._M_c = iVar13;
              local_67c._M_c = -1;
              local_68c._M_sbuf = pbVar7;
              local_67c._M_sbuf = pbVar2;
LAB_081b03ac:
                    /* Unresolved local var: int __eof@[???]
                       Unresolved local var: bool __thiseof@[DW_OP_reg3(EBX)]
                       Unresolved local var: bool __beof@[DW_OP_reg0(EAX)] */
              iVar13 = istreambuf_iterator<char,std::char_traits<char>_>::_M_get(&local_67c);
              iVar18 = istreambuf_iterator<char,std::char_traits<char>_>::_M_get(&local_68c);
              bVar5 = false;
              if (iVar13 == -1) {
                if (iVar18 == -1) {
LAB_081b0ab3:
                  bVar5 = true;
                }
              }
              else if (iVar18 != -1) goto LAB_081b0ab3;
              if (!bVar5) {
                iVar13 = istreambuf_iterator<char,std::char_traits<char>_>::_M_get(&local_67c);
                local_67d = (char)iVar13;
                _Var24._M_current = *(char **)(iVar17 + 0x14);
                if (_Var24._M_current == *(char **)(iVar17 + 0x18)) {
                  vector<char,std::allocator<char>_>::_M_insert_aux
                            ((vector<char,std::allocator<char>_> *)(iVar17 + 0x10),_Var24,&local_67d
                            );
                }
                else {
                  iVar13 = 0;
                  if (_Var24._M_current != (char *)0x0) {
                    *_Var24._M_current = local_67d;
                    iVar13 = *(int *)(iVar17 + 0x14);
                  }
                  *(int *)(iVar17 + 0x14) = iVar13 + 1;
                }
                    /* Unresolved local var: int __eof@[???] */
                if ((local_67c._M_sbuf == (basic_streambuf<char,std::char_traits<char>_> *)0x0) ||
                   (iVar13 = std::streambuf::sbumpc(), iVar13 != -1)) {
                  local_67c._M_c = -1;
                }
                else {
                  local_67c._M_sbuf = (basic_streambuf<char,std::char_traits<char>_> *)0x0;
                }
                goto LAB_081b03ac;
              }
              _Var23._M_node = (_Rb_tree_node_base *)(this->m_mapOwnArgQuest + event_index);
              _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
              ::lower_bound((_Rb_tree<unsigned_int,std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::_Select1st<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>_>_>
                             *)&__i_2,(uint *)_Var23._M_node);
              local_40c._M_node =
                   &(this->m_mapOwnArgQuest[event_index]._M_t.
                     super__Rb_tree_base<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>_>_>_>_>,std::allocator<std::pair<const_unsigned_int,_std::map<int,_std::vector<quest::AArgScript,_std::allocator<quest::AArgScript>_>,_std::less<int>,_std::allocator<std::pair<const_int,_std::vector<quest::AArgScript,_std::