
/* DWARF original prototype: size_t
   erase(_Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
   * this, int * __x) */

size_t __thiscall
_Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
::erase(_Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
        *this,int *__x)

{
  size_t sVar1;
  bool bVar2;
  _Rb_tree_base_iterator local_6c;
  _Rb_tree_base_iterator local_5c;
  _Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
  local_4c;
  _Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
  local_3c;
  pair<std::_Rb_tree_iterator<std::pair<const_int,_int>,_std::pair<const_int,_int>&,_std::pair<const_int,_int>*>,std::_Rb_tree_iterator<std::pair<const_int,_int>,_std::pair<const_int,_int>&,_std::pair<const_int,_int>*>_>
  __p;
  
                    /* Unresolved local var: size_t __n@[???] */
                    /* Unresolved local var: ptrdiff_t __n@[DW_OP_reg6(ESI)] */
  sVar1 = 0;
  lower_bound(&local_3c,(int *)this);
  upper_bound(&local_4c,(int *)this);
  local_5c._M_node =
       (_Rb_tree_node_base *)
       local_3c.
       super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>;
  local_6c._M_node =
       (_Rb_tree_node_base *)
       local_4c.
       super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>;
  __p.first.super__Rb_tree_base_iterator._M_node =
       (_Rb_tree_base_iterator)
       (_Rb_tree_base_iterator)
       local_3c.
       super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>.
       super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
       ._M_header;
  bVar2 = local_3c.
          super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>
          .
          super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
          ._M_header !=
          local_4c.
          super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>
          .
          super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
          ._M_header;
  __p.second.super__Rb_tree_base_iterator._M_node =
       (_Rb_tree_base_iterator)
       (_Rb_tree_base_iterator)
       local_4c.
       super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>.
       super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
       ._M_header;
  local_4c.
  super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>.
  super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
  ._M_header = local_3c.
               super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
               ._M_header;
  local_3c.
  super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>.
  super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
  ._M_header = (_Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
                )(_Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
                  )local_6c._M_node;
  if (bVar2) {
    do {
      sVar1 = sVar1 + 1;
      _Rb_tree_base_iterator::_M_increment(&local_5c);
    } while (local_5c._M_node != local_6c._M_node);
  }
  local_6c = __p.second.super__Rb_tree_base_iterator._M_node;
  local_5c = __p.first.super__Rb_tree_base_iterator._M_node;
  erase(this,(_Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
              )&local_5c,
        (_Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
         )&local_6c);
  return sVar1;
}

