
/* DWARF original prototype:
   _Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
   lower_bound(_Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
   * this, int * __k) */

_Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
__thiscall
_Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
::lower_bound(_Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
              *this,int *__k)

{
  _Rb_tree_node<std::pair<const_int,_int>_> *p_Var1;
  _Rb_tree_node<std::pair<const_int,_int>_> *p_Var2;
  _Rb_tree_node<std::pair<const_int,_int>_> *p_Var3;
  int *in_stack_0000000c;
  
                    /* Unresolved local var: _Rb_tree_node<std::pair<const_int,_int>_> *
                       __y@[DW_OP_reg1(ECX)]
                       Unresolved local var: _Rb_tree_node<std::pair<const_int,_int>_> *
                       __x@[DW_OP_reg2(EDX)] */
  p_Var1 = (_Rb_tree_node<std::pair<const_int,_int>_> *)*__k;
  p_Var2 = (_Rb_tree_node<std::pair<const_int,_int>_> *)(p_Var1->super__Rb_tree_node_base)._M_parent
  ;
  if (p_Var2 != (_Rb_tree_node<std::pair<const_int,_int>_> *)0x0) {
    do {
      if ((p_Var2->_M_value_field).first < *in_stack_0000000c) {
        p_Var3 = (_Rb_tree_node<std::pair<const_int,_int>_> *)
                 (p_Var2->super__Rb_tree_node_base)._M_right;
      }
      else {
        p_Var3 = (_Rb_tree_node<std::pair<const_int,_int>_> *)
                 (p_Var2->super__Rb_tree_node_base)._M_left;
        p_Var1 = p_Var2;
      }
      p_Var2 = p_Var3;
    } while (p_Var3 != (_Rb_tree_node<std::pair<const_int,_int>_> *)0x0);
  }
  (this->super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>)
  .
  super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
  ._M_header = p_Var1;
  return (_Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
          )(_Rb_tree_base_iterator)this;
}

