
/* DWARF original name: for_each<std::_Rb_tree_iterator<std::pair<CEntity* const, int>,
   std::pair<CEntity* const, int>&, std::pair<CEntity* const, int>*>, FuncPacketView> */

FuncPacketView *
for_each<std::_Rb_tree_iterator<std::pair<CEntity*_const,_int>,_std::pair<CEntity*_const,_int>&,_std::pair<CEntity*_const,_int>*>,_FuncPacketView>
          (FuncPacketView *__return_storage_ptr__,
          _Rb_tree_iterator<std::pair<CEntity*_const,_int>,std::pair<CEntity*_const,_int>&,std::pair<CEntity*_const,_int>*>
          __first,_Rb_tree_iterator<std::pair<CEntity*_const,_int>,std::pair<CEntity*_const,_int>&,std::pair<CEntity*_const,_int>*>
                  __last,FuncPacketView __f)

{
  DESC *this;
  int iVar1;
  
  iVar1 = *(int *)__first.super__Rb_tree_base_iterator._M_node;
  if (iVar1 != *(int *)__last.super__Rb_tree_base_iterator._M_node) {
    do {
      if ((*(LPENTITY *)(iVar1 + 0x10) != __f.super_FuncPacketAround.m_except) &&
         (this = (*(LPENTITY *)(iVar1 + 0x10))->m_lpDesc, this != (DESC *)0x0)) {
        DESC::Packet(this,__f.super_FuncPacketAround.m_data,__f.super_FuncPacketAround.m_bytes);
      }
      _Rb_tree_base_iterator::_M_increment
                ((_Rb_tree_base_iterator *)__first.super__Rb_tree_base_iterator._M_node);
      iVar1 = *(int *)__first.super__Rb_tree_base_iterator._M_node;
    } while (iVar1 != *(int *)__last.super__Rb_tree_base_iterator._M_node);
  }
  (__return_storage_ptr__->super_FuncPacketAround).m_data = __f.super_FuncPacketAround.m_data;
  (__return_storage_ptr__->super_FuncPacketAround).m_bytes = __f.super_FuncPacketAround.m_bytes;
  (__return_storage_ptr__->super_FuncPacketAround).m_except = __f.super_FuncPacketAround.m_except;
  return __return_storage_ptr__;
}

