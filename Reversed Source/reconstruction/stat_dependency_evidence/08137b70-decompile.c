
/* DWARF original prototype:
   _Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
   insert_unique(_Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
   * this,
   _Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
   __position, pair<const_int,int> * __v) */

_Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
__thiscall
_Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
::insert_unique(_Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
                *this,_Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
                      __position,pair<const_int,int> *__v)

{
  _Rb_tree_base_iterator __y_;
  _Rb_tree_base_iterator _Var1;
  pair<const_int,int> *in_stack_00000010;
  _Rb_tree_base_iterator __v_00;
  _Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
  __before;
  _Rb_tree_node<std::pair<const_int,_int>_> *local_2c;
  
  __y_._M_node = (_Rb_tree_node_base *)__v->first;
  if (__y_._M_node ==
      (*(_Rb_tree_node_base **)__position.super__Rb_tree_base_iterator._M_node)->_M_left) {
    if ((*(int *)((int)__position.super__Rb_tree_base_iterator._M_node + 4) == 0) ||
       (__v_00._M_node = __y_._M_node, (int)__y_._M_node[1]._M_color <= in_stack_00000010->first))
    goto LAB_08137c10;
  }
  else {
    if (__y_._M_node == *(_Rb_tree_node_base **)__position.super__Rb_tree_base_iterator._M_node) {
      _Var1._M_node = (__y_._M_node)->_M_right;
      if (in_stack_00000010->first <= (int)(__y_._M_node)->_M_right[1]._M_color) goto LAB_08137c10;
    }
    else {
      __before.super__Rb_tree_base_iterator._M_node =
           (_Rb_tree_base_iterator)(_Rb_tree_base_iterator)__y_._M_node;
      _Rb_tree_base_iterator::_M_decrement(&__before.super__Rb_tree_base_iterator);
      if ((in_stack_00000010->first <=
           *(int *)((int)__before.super__Rb_tree_base_iterator._M_node + 0x10)) ||
         (__y_._M_node = (_Rb_tree_node_base *)__v->first,
         (int)__y_._M_node[1]._M_color <= in_stack_00000010->first)) {
LAB_08137c10:
        insert_unique((_Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
                       *)__position.super__Rb_tree_base_iterator._M_node,in_stack_00000010);
        (this->
        super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>).
        super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
        ._M_header = local_2c;
        return (_Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
                )(_Rb_tree_base_iterator)this;
      }
      _Var1 = __before.super__Rb_tree_base_iterator._M_node;
      __v_00._M_node = __y_._M_node;
      if (*(int *)((int)__before.super__Rb_tree_base_iterator._M_node + 0xc) != 0)
      goto LAB_08137bde;
    }
    __y_._M_node = (_Rb_tree_node_base *)0x0;
    __v_00._M_node = _Var1._M_node;
  }
LAB_08137bde:
  _M_insert(this,(_Rb_tree_node_base *)__position.super__Rb_tree_base_iterator._M_node,__y_._M_node,
            (pair<const_int,int> *)__v_00._M_node);
  return (_Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
          )(_Rb_tree_base_iterator)this;
}

