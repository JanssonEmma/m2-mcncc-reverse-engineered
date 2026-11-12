
/* WARNING: Type propagation algorithm not settling */
/* DWARF original prototype: DWORD UnderAnyWar(CGuild * this, BYTE bType) */

DWORD __thiscall CGuild::UnderAnyWar(CGuild *this,BYTE bType)

{
  _Rb_tree_node<std::pair<const_long_unsigned_int,_SGuildWar>_> *p_Var1;
  _Rb_tree_iterator<std::pair<const_long_unsigned_int,_SGuildWar>,std::pair<const_long_unsigned_int,_SGuildWar>&,std::pair<const_long_unsigned_int,_SGuildWar>*>
  it;
  
  p_Var1 = (this->m_EnemyGuild)._M_t.
           super__Rb_tree_base<std::pair<const_long_unsigned_int,_SGuildWar>,std::allocator<std::pair<const_long_unsigned_int,_SGuildWar>_>_>
           .
           super__Rb_tree_alloc_base<std::pair<const_long_unsigned_int,_SGuildWar>,std::allocator<std::pair<const_long_unsigned_int,_SGuildWar>_>,true>
           ._M_header;
  it.super__Rb_tree_base_iterator._M_node =
       (_Rb_tree_base_iterator)(p_Var1->super__Rb_tree_node_base)._M_left;
  if (it.super__Rb_tree_base_iterator._M_node != (_Rb_tree_base_iterator)p_Var1) {
    do {
      if (((2 < bType) || (*(BYTE *)((int)it.super__Rb_tree_base_iterator._M_node + 0x20) == bType))
         && (*(int *)((int)it.super__Rb_tree_base_iterator._M_node + 0x1c) == 6)) {
        return *(DWORD *)((int)it.super__Rb_tree_base_iterator._M_node + 0x10);
      }
      _Rb_tree_base_iterator::_M_increment(&it.super__Rb_tree_base_iterator);
    } while (it.super__Rb_tree_base_iterator._M_node !=
             (_Rb_tree_base_iterator)
             &((this->m_EnemyGuild)._M_t.
               super__Rb_tree_base<std::pair<const_long_unsigned_int,_SGuildWar>,std::allocator<std::pair<const_long_unsigned_int,_SGuildWar>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_long_unsigned_int,_SGuildWar>,std::allocator<std::pair<const_long_unsigned_int,_SGuildWar>_>,true>
              ._M_header)->super__Rb_tree_node_base);
  }
  return 0;
}

