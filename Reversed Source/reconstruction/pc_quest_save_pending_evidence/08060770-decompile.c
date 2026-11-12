
/* DWARF original prototype: void Save(PC * this) */

void __thiscall PC::Save(PC *this)

{
  int *piVar1;
  _Rb_tree_node<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>
  *p_Var2;
  undefined4 uVar3;
  _Rb_tree_iterator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>&,std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>*>
  _Var4;
  size_t sVar5;
  int iVar6;
  uint uVar7;
  TQuestTable *pTVar8;
  char *pcVar9;
  int local_fc;
  long lValue;
  int i;
  string stState;
  string stName;
  _Rb_tree_iterator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>&,std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>*>
  it;
  TQuestTable local_9c;
  
  uVar7 = (this->m_FlagSaveMap)._M_t._M_node_count;
  if (uVar7 != 0) {
    if ((char)quest::PC::Save()::s_table == '\0') {
      quest::PC::Save()::s_table._0_1_ = '\x01';
      Save::s_table.super__Vector_base<SQuestTable,std::allocator<SQuestTable>_>.
      super__Vector_alloc_base<SQuestTable,std::allocator<SQuestTable>,true>._M_finish =
           (TQuestTable *)0x0;
      Save::s_table.super__Vector_base<SQuestTable,std::allocator<SQuestTable>_>.
      super__Vector_alloc_base<SQuestTable,std::allocator<SQuestTable>,true>._M_start =
           (TQuestTable *)0x0;
      Save::s_table.super__Vector_base<SQuestTable,std::allocator<SQuestTable>_>.
      super__Vector_alloc_base<SQuestTable,std::allocator<SQuestTable>,true>._M_end_of_storage =
           (TQuestTable *)0x0;
      atexit(__tcf_0);
      uVar7 = (this->m_FlagSaveMap)._M_t._M_node_count;
    }
    pTVar8 = &local_9c;
    for (iVar6 = 0x1a; iVar6 != 0; iVar6 = iVar6 + -1) {
      pTVar8->dwPID = 0;
      pTVar8 = (TQuestTable *)pTVar8->szName;
    }
    *(undefined2 *)&pTVar8->dwPID = 0;
    iVar6 = (int)Save::s_table.super__Vector_base<SQuestTable,std::allocator<SQuestTable>_>.
                 super__Vector_alloc_base<SQuestTable,std::allocator<SQuestTable>,true>._M_finish -
            (int)Save::s_table.super__Vector_base<SQuestTable,std::allocator<SQuestTable>_>.
                 super__Vector_alloc_base<SQuestTable,std::allocator<SQuestTable>,true>._M_start >>
            1;
    if (uVar7 < (uint)(iVar6 * -0x73ecade3)) {
      vector<SQuestTable,std::allocator<SQuestTable>_>::erase
                (&Save::s_table,
                 Save::s_table.super__Vector_base<SQuestTable,std::allocator<SQuestTable>_>.
                 super__Vector_alloc_base<SQuestTable,std::allocator<SQuestTable>,true>._M_start +
                 uVar7,(__normal_iterator<SQuestTable*,std::vector<SQuestTable,_std::allocator<SQuestTable>_>_>
                        )Save::s_table.super__Vector_base<SQuestTable,std::allocator<SQuestTable>_>.
                         super__Vector_alloc_base<SQuestTable,std::allocator<SQuestTable>,true>.
                         _M_finish);
    }
    else {
      vector<SQuestTable,std::allocator<SQuestTable>_>::_M_fill_insert
                (&Save::s_table,
                 (__normal_iterator<SQuestTable*,std::vector<SQuestTable,_std::allocator<SQuestTable>_>_>
                  )Save::s_table.super__Vector_base<SQuestTable,std::allocator<SQuestTable>_>.
                   super__Vector_alloc_base<SQuestTable,std::allocator<SQuestTable>,true>._M_finish,
                 uVar7 + iVar6 * 0x73ecade3,&local_9c);
    }
    i = 0;
    p_Var2 = (this->m_FlagSaveMap)._M_t.
             super__Rb_tree_base<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::allocator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>_>
             .
             super__Rb_tree_alloc_base<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::allocator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>,true>
             ._M_header;
    it.super__Rb_tree_base_iterator._M_node =
         (_Rb_tree_base_iterator)(p_Var2->super__Rb_tree_node_base)._M_left;
    if (it.super__Rb_tree_base_iterator._M_node != (_Rb_tree_base_iterator)p_Var2) {
      local_fc = -0x6a;
      do {
        _Var4.super__Rb_tree_base_iterator._M_node = it.super__Rb_tree_base_iterator._M_node;
                    /* Unresolved local var: string * stComp@[???]
                       Unresolved local var: int iPos@[DW_OP_reg3(EBX)]
                       Unresolved local var: TQuestTable * r@[???] */
        pcVar9 = (char *)((int)it.super__Rb_tree_base_iterator._M_node + 0x10);
        uVar3 = *(undefined4 *)((int)it.super__Rb_tree_base_iterator._M_node + 0x14);
        _Rb_tree_base_iterator::_M_increment(&it.super__Rb_tree_base_iterator);
        sVar5 = strlen(".");
        iVar6 = std::string::find(pcVar9,0x8200398,0);
        if (iVar6 < 0) {
          _sys_err("Save",0x219,"quest::PC::Save : cannot find . in FlagMap",sVar5);
        }
        else {
          LOCK();
          DAT_083223c8 = DAT_083223c8 + 1;
          UNLOCK();
          stName._M_dataplus._M_p = (_Alloc_hider)&DAT_083223cc;
          std::string::assign((string *)&stName,(uint)pcVar9,0);
          LOCK();
          DAT_083223c8 = DAT_083223c8 + 1;
          UNLOCK();
          stState._M_dataplus._M_p = (_Alloc_hider)&DAT_083223cc;
          std::string::assign((string *)&stState,(uint)pcVar9,iVar6 + 1);
          if (*(int *)((int)stName._M_dataplus._M_p + -0xc) == 0) {
LAB_080608f8:
                    /* Unresolved local var: size_t __n@[???] */
            iVar6 = *(int *)((int)_Var4.super__Rb_tree_base_iterator._M_node + 0x10);
            *(undefined1 *)(iVar6 + *(int *)(iVar6 + -0xc)) = std::string::_Rep::_S_terminal;
            _sys_err("Save",0x225,"quest::PC::Save : invalid quest data: %s",
                     *(undefined4 *)((int)_Var4.super__Rb_tree_base_iterator._M_node + 0x10));
          }
          else {
            if (*(int *)((int)stState._M_dataplus._M_p + -0xc) == 0) goto LAB_080608f8;
                    /* Unresolved local var: size_t __n@[???] */
            *(undefined1 *)
             (*(int *)((int)stState._M_dataplus._M_p + -0xc) + (int)stState._M_dataplus._M_p) =
                 std::string::_Rep::_S_terminal;
                    /* Unresolved local var: size_t __n@[???] */
            *(undefined1 *)
             ((int)stName._M_dataplus._M_p + *(int *)((int)stName._M_dataplus._M_p + -0xc)) =
                 std::string::_Rep::_S_terminal;
            sys_log(1,"QUEST Save Flag %s, %s %ld (%d)",stName._M_dataplus._M_p,
                    stState._M_dataplus._M_p,uVar3,i);
            uVar7 = *(uint *)((int)stName._M_dataplus._M_p + -0xc);
            if (uVar7 < 0x20) {
              if (0x3f < *(uint *)((int)stState._M_dataplus._M_p + -0xc)) {
                pcVar9 = "quest::PC::Save : quest state overflow";
                iVar6 = 0x233;
                goto LAB_08060af7;
              }
              i = i + 1;
              pcVar9 = Save::s_table.super__Vector_base<SQuestTable,std::allocator<SQuestTable>_>.
                       super__Vector_alloc_base<SQuestTable,std::allocator<SQuestTable>,true>.
                       _M_start[1].szName + local_fc + -4;
              *(DWORD *)pcVar9 = this->m_dwID;
                    /* Unresolved local var: size_t __n@[???] */
              *(undefined1 *)(uVar7 + (int)stName._M_dataplus._M_p) = std::string::_Rep::_S_terminal
              ;
              strcpy(pcVar9 + 4,(char *)stName._M_dataplus._M_p);
                    /* Unresolved local var: size_t __n@[???] */
              *(undefined1 *)
               ((int)stState._M_dataplus._M_p + *(int *)((int)stState._M_dataplus._M_p + -0xc)) =
                   std::string::_Rep::_S_terminal;
              strcpy(pcVar9 + 0x25,(char *)stState._M_dataplus._M_p);
              *(undefined4 *)(pcVar9 + 0x66) = uVar3;
              local_fc = local_fc + 0x6a;
            }
            else {
              pcVar9 = "quest::PC::Save : quest name overflow";
              iVar6 = 0x22d;
LAB_08060af7:
              _sys_err("Save",iVar6,pcVar9);
            }
          }
          LOCK();
          piVar1 = (int *)((int)stState._M_dataplus._M_p + -4);
          iVar6 = *piVar1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (iVar6 < 1) {
            std::string::_Rep::_M_destroy((allocator *)((int)stState._M_dataplus._M_p + -0xc));
          }
          LOCK();
          piVar1 = (int *)((int)stName._M_dataplus._M_p + -4);
          iVar6 = *piVar1;
          *piVar1 = *piVar1 + -1;
          UNLOCK();
          if (iVar6 < 1) {
            std::string::_Rep::_M_destroy((allocator *)((int)stName._M_dataplus._M_p + -0xc));
          }
        }
      } while (it.super__Rb_tree_base_iterator._M_node !=
               (_Rb_tree_base_iterator)
               &((this->m_FlagSaveMap)._M_t.
                 super__Rb_tree_base<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::allocator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>_>
                 .
                 super__Rb_tree_alloc_base<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::allocator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>,true>
                ._M_header)->super__Rb_tree_node_base);
      if (0 < i) {
        sys_log(1,"QuestPC::Save %d",i);
        CLIENT_DESC::DBPacketHeader(db_clientdesc,'\v',0,i * 0x6a);
        CLIENT_DESC::Packet(db_clientdesc,
                            Save::s_table.
                            super__Vector_base<SQuestTable,std::allocator<SQuestTable>_>.
                            super__Vector_alloc_base<SQuestTable,std::allocator<SQuestTable>,true>.
                            _M_start,i * 0x6a);
      }
    }
    if ((this->m_FlagSaveMap)._M_t._M_node_count != 0) {
      _Rb_tree<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::_Select1st<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>,std::less<std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>_>,std::allocator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>_>
      ::_M_erase(&(this->m_FlagSaveMap)._M_t,
                 (_Rb_tree_node<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>
                  *)(((this->m_FlagSaveMap)._M_t.
                      super__Rb_tree_base<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::allocator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>_>
                      .
                      super__Rb_tree_alloc_base<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::allocator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>,true>
                     ._M_header)->super__Rb_tree_node_base)._M_parent);
      p_Var2 = (this->m_FlagSaveMap)._M_t.
               super__Rb_tree_base<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::allocator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::allocator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>,true>
               ._M_header;
      (p_Var2->super__Rb_tree_node_base)._M_left = &p_Var2->super__Rb_tree_node_base;
      (((this->m_FlagSaveMap)._M_t.
        super__Rb_tree_base<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::allocator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>_>
        .
        super__Rb_tree_alloc_base<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::allocator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>,true>
       ._M_header)->super__Rb_tree_node_base)._M_parent = (_Rb_tree_node_base *)0x0;
      p_Var2 = (this->m_FlagSaveMap)._M_t.
               super__Rb_tree_base<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::allocator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>,std::allocator<std::pair<const_std::basic_string<char,_std::char_traits<char>,_std::allocator<char>_>,_int>_>,true>
               ._M_header;
      (p_Var2->super__Rb_tree_node_base)._M_right = &p_Var2->super__Rb_tree_node_base;
      (this->m_FlagSaveMap)._M_t._M_node_count = 0;
    }
  }
  return;
}

