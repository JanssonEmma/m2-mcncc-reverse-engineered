
/* DWARF original prototype: CMob * Get(CMobManager * this, DWORD dwVnum) */

CMob * __thiscall CMobManager::Get(CMobManager *this,DWORD dwVnum)

{
  CMob *pCVar1;
  _Rb_tree_iterator<std::pair<const_long_unsigned_int,_CMob*>,std::pair<const_long_unsigned_int,_CMob*>&,std::pair<const_long_unsigned_int,_CMob*>*>
  it;
  
  _Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_CMob*>,std::_Select1st<std::pair<const_long_unsigned_int,_CMob*>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_CMob*>_>_>
  ::find((_Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_CMob*>,std::_Select1st<std::pair<const_long_unsigned_int,_CMob*>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_CMob*>_>_>
          *)&it,(DWORD *)&this->m_map_pkMobByVnum);
  pCVar1 = (CMob *)0x0;
  if (it.super__Rb_tree_base_iterator._M_node !=
      (_Rb_tree_base_iterator)
      &((this->m_map_pkMobByVnum)._M_t.
        super__Rb_tree_base<std::pair<const_long_unsigned_int,_CMob*>,std::allocator<std::pair<const_long_unsigned_int,_CMob*>_>_>
        .
        super__Rb_tree_alloc_base<std::pair<const_long_unsigned_int,_CMob*>,std::allocator<std::pair<const_long_unsigned_int,_CMob*>_>,true>
       ._M_header)->super__Rb_tree_node_base) {
    pCVar1 = *(CMob **)((int)it.super__Rb_tree_base_iterator._M_node + 0x14);
  }
  return pCVar1;
}

