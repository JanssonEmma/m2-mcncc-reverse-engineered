
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void __LocaleService_Init_CIBN(void)

{
  int *piVar1;
  int iVar2;
  pair<std::_Rb_tree_iterator<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>&,_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>*>,bool>
  __p;
  allocator local_2c [16];
  int local_1c [5];
  
  check_name = check_name_gb2312;
  is_twobyte = is_twobyte_gb2312;
  exp_table = exp_table_gb2312;
  aiPercentByDeltaLev = aiPercentByDeltaLev_gb2312;
  aiPercentByDeltaLevForBoss = aiPercentByDeltaLev_gb2312;
  aiChainLightningCountBySkillLevel = aiChainLightningCountBySkillLevel_gb2312;
  g_iUseLocale = 1;
  PK_PROTECT_LEVEL = 10;
  strlen("gb2312");
  std::string::assign((char *)&g_stLocale,0x81f7e32);
  strlen("locale/cibn");
  std::string::assign((char *)&g_stServiceBasePath,0x81f7e39);
  strlen("locale/cibn/map");
  std::string::assign((char *)&g_stServiceMapPath,0x81f7e45);
  strlen("locale/cibn/gb2312_string.txt");
  std::string::assign((char *)&g_stLocaleFilename,0x81f7e55);
  strlen("locale/cibn/quest");
  std::string::assign((char *)&g_stQuestDir,0x81f7e73);
  if (DAT_08323218 != 0) {
    _Rb_tree<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,std::_Identity<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>,std::less<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>,std::allocator<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>_>
    ::_M_erase((_Rb_tree<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,std::_Identity<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>,std::less<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>,std::allocator<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>_>
                *)&g_setQuestObjectDir,
               *(_Rb_tree_node<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>
                 **)(g_setQuestObjectDir + 4));
    *(int *)(g_setQuestObjectDir + 8) = g_setQuestObjectDir;
    *(undefined4 *)(g_setQuestObjectDir + 4) = 0;
    *(int *)(g_setQuestObjectDir + 0xc) = g_setQuestObjectDir;
    DAT_08323218 = 0;
  }
  std::string::string((string *)local_1c,"locale/cibn/quest/object",local_2c);
  _Rb_tree<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,std::_Identity<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>,std::less<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>,std::allocator<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>_>
  ::insert_unique((_Rb_tree<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,std::_Identity<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>,std::less<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>,std::allocator<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>_>
                   *)&g_setQuestObjectDir,(string *)local_1c);
  LOCK();
  piVar1 = (int *)(local_1c[0] + -4);
  iVar2 = *piVar1;
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (iVar2 < 1) {
    std::string::_Rep::_M_destroy((allocator *)(local_1c[0] + -0xc));
  }
  return;
}

