
/* DWARF original prototype: void _M_increment(_Rb_tree_base_iterator * this) */

void __thiscall _Rb_tree_base_iterator::_M_increment(_Rb_tree_base_iterator *this)

{
  _Rb_tree_node_base *p_Var1;
  _Rb_tree_node_base *p_Var2;
  
  p_Var2 = this->_M_node;
  p_Var1 = p_Var2->_M_right;
  if (p_Var1 == (_Rb_tree_node_base *)0x0) {
                    /* Unresolved local var: _Rb_tree_node_base * __y@[DW_OP_reg0(EAX)] */
    p_Var1 = p_Var2->_M_parent;
    if (p_Var2 == p_Var1->_M_right) {
      do {
        this->_M_node = p_Var1;
        p_Var1 = p_Var1->_M_parent;
        p_Var2 = this->_M_node;
      } while (p_Var2 == p_Var1->_M_right);
    }
    if (p_Var2->_M_right != p_Var1) {
      this->_M_node = p_Var1;
    }
  }
  else {
    this->_M_node = p_Var1;
    for (p_Var2 = p_Var1->_M_left; p_Var2 != (_Rb_tree_node_base *)0x0; p_Var2 = p_Var2->_M_left) {
      p_Var2 = this->_M_node->_M_left;
      this->_M_node = p_Var2;
    }
  }
  return;
}

