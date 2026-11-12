
/* DWARF original prototype: LPSECTREE Find(SECTREE_MAP * this, DWORD dwPackage) */

LPSECTREE __thiscall SECTREE_MAP::Find(SECTREE_MAP *this,DWORD dwPackage)

{
  LPSECTREE pSVar1;
  _Rb_tree_iterator<std::pair<const_long_unsigned_int,_SECTREE*>,std::pair<const_long_unsigned_int,_SECTREE*>&,std::pair<const_long_unsigned_int,_SECTREE*>*>
  it;
  
  _Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_SECTREE*>,std::_Select1st<std::pair<const_long_unsigned_int,_SECTREE*>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SECTREE*>_>_>
  ::find((_Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_SECTREE*>,std::_Select1st<std::pair<const_long_unsigned_int,_SECTREE*>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SECTREE*>_>_>
          *)&it,(DWORD *)&this->
                          super_map<long_unsigned_int,SECTREE*,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SECTREE*>_>_>
        );
  pSVar1 = (LPSECTREE)0x0;
  if (it.super__Rb_tree_base_iterator._M_node !=
      (_Rb_tree_base_iterator)
      &((this->
        super_map<long_unsigned_int,SECTREE*,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SECTREE*>_>_>
        )._M_t.
        super__Rb_tree_base<std::pair<const_long_unsigned_int,_SECTREE*>,std::allocator<std::pair<const_long_unsigned_int,_SECTREE*>_>_>
        .
        super__Rb_tree_alloc_base<std::pair<const_long_unsigned_int,_SECTREE*>,std::allocator<std::pair<const_long_unsigned_int,_SECTREE*>_>,true>
       ._M_header)->super__Rb_tree_node_base) {
    pSVar1 = *(LPSECTREE *)((int)it.super__Rb_tree_base_iterator._M_node + 0x14);
  }
  return pSVar1;
}

