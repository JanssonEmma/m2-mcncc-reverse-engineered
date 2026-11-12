
/* DWARF original prototype: TGuildMember * GetMember(CGuild * this, DWORD pid) */

TGuildMember * __thiscall CGuild::GetMember(CGuild *this,DWORD pid)

{
  _Rb_tree_iterator<std::pair<const_long_unsigned_int,_SGuildMember>,std::pair<const_long_unsigned_int,_SGuildMember>&,std::pair<const_long_unsigned_int,_SGuildMember>*>
  it;
  
  _Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_SGuildMember>,std::_Select1st<std::pair<const_long_unsigned_int,_SGuildMember>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>_>
  ::find((_Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_SGuildMember>,std::_Select1st<std::pair<const_long_unsigned_int,_SGuildMember>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>_>
          *)&it,(DWORD *)&this->m_member);
  return (TGuildMember *)
         ((int)it.super__Rb_tree_base_iterator._M_node + 0x14U &
         (it.super__Rb_tree_base_iterator._M_node ==
         (_Rb_tree_base_iterator)
         &((this->m_member)._M_t.
           super__Rb_tree_base<std::pair<const_long_unsigned_int,_SGuildMember>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>_>
           .
           super__Rb_tree_alloc_base<std::pair<const_long_unsigned_int,_SGuildMember>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>,true>
          ._M_header)->super__Rb_tree_node_base) - 1);
}

