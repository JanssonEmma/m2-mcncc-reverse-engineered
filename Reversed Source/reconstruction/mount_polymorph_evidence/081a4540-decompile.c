
/* DWARF original prototype: void Unmount(CQuestManager * this, uint pc) */

void __thiscall CQuestManager::Unmount(CQuestManager *this,uint pc)

{
  NPC *this_00;
  PC *pc_00;
  map<unsigned_int,quest::NPC,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_quest::NPC>_>_>
  *__position;
  undefined1 local_44c [512];
  undefined4 local_24c;
  NPC local_248;
  _Rb_tree_base_iterator local_5c;
  _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_quest::NPC>,std::_Select1st<std::pair<const_unsigned_int,_quest::NPC>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_quest::NPC>_>_>
  local_4c;
  _Rb_tree_node<std::pair<const_unsigned_int,_quest::NPC>_> *local_3c;
  _Rb_tree_iterator<std::pair<const_unsigned_int,_quest::NPC>,std::pair<const_unsigned_int,_quest::NPC>&,std::pair<const_unsigned_int,_quest::NPC>*>
  __i;
  
                    /* Unresolved local var: PC * pPC@[DW_OP_reg7(EDI)] */
  pc_00 = GetPC(this,pc);
  if (pc_00 == (PC *)0x0) {
    _sys_err("Unmount",0x308,"QUEST no such pc id : %d",pc);
  }
  else if (pc_00->m_bLoaded != false) {
    __position = &this->m_mapNPC;
    _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_quest::NPC>,std::_Select1st<std::pair<const_unsigned_int,_quest::NPC>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_quest::NPC>_>_>
    ::lower_bound((_Rb_tree<unsigned_int,std::pair<const_unsigned_int,_quest::NPC>,std::_Select1st<std::pair<const_unsigned_int,_quest::NPC>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_quest::NPC>_>_>
                   *)&__i,(uint *)__position);
    local_3c = (this->m_mapNPC)._M_t.
               super__Rb_tree_base<std::pair<const_unsigned_int,_quest::NPC>,std::allocator<std::pair<const_unsigned_int,_quest::NPC>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_quest::NPC>,std::allocator<std::pair<const_unsigned_int,_quest::NPC>_>,true>
               ._M_header;
    if ((__i.super__Rb_tree_base_iterator._M_node == (_Rb_tree_base_iterator)local_3c) ||
       (*(int *)((int)__i.super__Rb_tree_base_iterator._M_node + 0x10) != 0)) {
      local_5c = __i.super__Rb_tree_base_iterator._M_node;
      this_00 = (NPC *)(local_44c + 0x10);
      NPC::NPC(this_00);
      local_24c = 0;
      NPC::NPC(&local_248,this_00);
      local_44c._0_4_ = local_5c._M_node;
      _Rb_tree<unsigned_int,std::pair<const_unsigned_int,_quest::NPC>,std::_Select1st<std::pair<const_unsigned_int,_quest::NPC>_>,std::less<unsigned_int>,std::allocator<std::pair<const_unsigned_int,_quest::NPC>_>_>
      ::insert_unique(&local_4c,
                      (_Rb_tree_iterator<std::pair<const_unsigned_int,_quest::NPC>,std::pair<const_unsigned_int,_quest::NPC>&,std::pair<const_unsigned_int,_quest::NPC>*>
                       )__position,(pair<const_unsigned_int,quest::NPC> *)local_44c);
      __i.super__Rb_tree_base_iterator._M_node =
           (_Rb_tree_base_iterator)
           (_Rb_tree_base_iterator)
           local_4c.
           super__Rb_tree_base<std::pair<const_unsigned_int,_quest::NPC>,std::allocator<std::pair<const_unsigned_int,_quest::NPC>_>_>
           .
           super__Rb_tree_alloc_base<std::pair<const_unsigned_int,_quest::NPC>,std::allocator<std::pair<const_unsigned_int,_quest::NPC>_>,true>
           ._M_header;
      NPC::~NPC(&local_248,(int)__position);
      NPC::~NPC(this_00,(int)__position);
    }
    NPC::OnUnmount((NPC *)((int)__i.super__Rb_tree_base_iterator._M_node + 0x14),pc_00);
  }
  return;
}

