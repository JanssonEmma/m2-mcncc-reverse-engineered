
/* DWARF original prototype: SECTREE_MAP * GetMap(SECTREE_MANAGER * this, long lMapIndex) */

SECTREE_MAP * __thiscall SECTREE_MANAGER::GetMap(SECTREE_MANAGER *this,long lMapIndex)

{
  SECTREE_MAP *pSVar1;
  _Rb_tree_iterator<std::pair<const_long_unsigned_int,_SECTREE_MAP*>,std::pair<const_long_unsigned_int,_SECTREE_MAP*>&,std::pair<const_long_unsigned_int,_SECTREE_MAP*>*>
  it;
  
  _Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_SECTREE_MAP*>,std::_Select1st<std::pair<const_long_unsigned_int,_SECTREE_MAP*>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SECTREE_MAP*>_>_>
  ::find((_Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_SECTREE_MAP*>,std::_Select1st<std::pair<const_long_unsigned_int,_SECTREE_MAP*>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SECTREE_MAP*>_>_>
          *)&it,(DWORD *)&this->m_map_pkSectree);
  pSVar1 = (SECTREE_MAP *)0x0;
  if (it.super__Rb_tree_base_iterator._M_node !=
      (_Rb_tree_base_iterator)
      &((this->m_map_pkSectree)._M_t.
        super__Rb_tree_base<std::pair<const_long_unsigned_int,_SECTREE_MAP*>,std::allocator<std::pair<const_long_unsigned_int,_SECTREE_MAP*>_>_>
        .
        super__Rb_tree_alloc_base<std::pair<const_long_unsigned_int,_SECTREE_MAP*>,std::allocator<std::pair<const_long_unsigned_int,_SECTREE_MAP*>_>,true>
       ._M_header)->super__Rb_tree_node_base) {
    pSVar1 = *(SECTREE_MAP **)((int)it.super__Rb_tree_base_iterator._M_node + 0x14);
  }
  return pSVar1;
}

