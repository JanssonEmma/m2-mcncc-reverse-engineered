
/* DWARF original prototype:
   _Rb_tree_iterator<std::pair<const_long_unsigned_int,_SGuildMember>,std::pair<const_long_unsigned_int,_SGuildMember>&,std::pair<const_long_unsigned_int,_SGuildMember>*>
   find(_Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_SGuildMember>,std::_Select1st<std::pair<const_long_unsigned_int,_SGuildMember>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>_>
   * this, DWORD * __k) */

_Rb_tree_iterator<std::pair<const_long_unsigned_int,_SGuildMember>,std::pair<const_long_unsigned_int,_SGuildMember>&,std::pair<const_long_unsigned_int,_SGuildMember>*>
 __thiscall
_Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_SGuildMember>,std::_Select1st<std::pair<const_long_unsigned_int,_SGuildMember>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>_>
::find(_Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_SGuildMember>,std::_Select1st<std::pair<const_long_unsigned_int,_SGuildMember>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>_>
       *this,DWORD *__k)

{
  _Rb_tree_node<std::pair<const_long_unsigned_int,_SGuildMember>_> *p_Var1;
  _Rb_tree_node<std::pair<const_long_unsigned_int,_SGuildMember>_> *p_Var2;
  _Rb_tree_node<std::pair<const_long_unsigned_int,_SGuildMember>_> *p_Var3;
  _Rb_tree_node<std::pair<const_long_unsigned_int,_SGuildMember>_> *p_Var4;
  uint *in_stack_0000000c;
  _Rb_tree_iterator<std::pair<const_long_unsigned_int,_SGuildMember>,std::pair<const_long_unsigned_int,_SGuildMember>&,std::pair<const_long_unsigned_int,_SGuildMember>*>
  __j;
  
                    /* Unresolved local var:
                       _Rb_tree_node<std::pair<const_long_unsigned_int,_SGuildMember>_> *
                       __y@[DW_OP_reg2(EDX)]
                       Unresolved local var:
                       _Rb_tree_node<std::pair<const_long_unsigned_int,_SGuildMember>_> *
                       __x@[DW_OP_reg0(EAX)] */
  p_Var1 = (_Rb_tree_node<std::pair<const_long_unsigned_int,_SGuildMember>_> *)*__k;
  p_Var2 = (_Rb_tree_node<std::pair<const_long_unsigned_int,_SGuildMember>_> *)
           (p_Var1->super__Rb_tree_node_base)._M_parent;
  p_Var4 = p_Var1;
  if (p_Var2 != (_Rb_tree_node<std::pair<const_long_unsigned_int,_SGuildMember>_> *)0x0) {
    do {
      if ((p_Var2->_M_value_field).first < *in_stack_0000000c) {
        p_Var3 = (_Rb_tree_node<std::pair<const_long_unsigned_int,_SGuildMember>_> *)
                 (p_Var2->super__Rb_tree_node_base)._M_right;
      }
      else {
        p_Var3 = (_Rb_tree_node<std::pair<const_long_unsigned_int,_SGuildMember>_> *)
                 (p_Var2->super__Rb_tree_node_base)._M_left;
        p_Var4 = p_Var2;
      }
      p_Var2 = p_Var3;
    } while (p_Var3 != (_Rb_tree_node<std::pair<const_long_unsigned_int,_SGuildMember>_> *)0x0);
  }
  if ((p_Var4 == p_Var1) || (*in_stack_0000000c < (p_Var4->_M_value_field).first)) {
    (this->
    super__Rb_tree_base<std::pair<const_long_unsigned_int,_SGuildMember>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>_>
    ).
    super__Rb_tree_alloc_base<std::pair<const_long_unsigned_int,_SGuildMember>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>,true>
    ._M_header = p_Var1;
  }
  else {
    (this->
    super__Rb_tree_base<std::pair<const_long_unsigned_int,_SGuildMember>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>_>
    ).
    super__Rb_tree_alloc_base<std::pair<const_long_unsigned_int,_SGuildMember>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>,true>
    ._M_header = p_Var4;
  }
  return (_Rb_tree_iterator<std::pair<const_long_unsigned_int,_SGuildMember>,std::pair<const_long_unsigned_int,_SGuildMember>&,std::pair<const_long_unsigned_int,_SGuildMember>*>
          )(_Rb_tree_base_iterator)this;
}

