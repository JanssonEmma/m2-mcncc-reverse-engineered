
/* DWARF original prototype: void PacketView(CEntity * this, void * data, int bytes, LPENTITY
   except) */

void __thiscall CEntity::PacketView(CEntity *this,void *data,int bytes,LPENTITY except)

{
  FuncPacketView __f;
  _Rb_tree_node_base *local_5c [4];
  _Rb_tree_node<std::pair<CEntity*_const,_int>_> *local_4c [4];
  FuncPacketView local_3c;
  FuncPacketView f;
  
  if (this->m_pSectree != (SECTREE *)0x0) {
    f.super_FuncPacketAround.m_data = data;
    f.super_FuncPacketAround.m_bytes = bytes;
    f.super_FuncPacketAround.m_except = except;
    if (this->m_bIsObserver == false) {
      local_4c[0] = (this->m_map_view)._M_t.
                    super__Rb_tree_base<std::pair<CEntity*_const,_int>,std::allocator<std::pair<CEntity*_const,_int>_>_>
                    .
                    super__Rb_tree_alloc_base<std::pair<CEntity*_const,_int>,std::allocator<std::pair<CEntity*_const,_int>_>,true>
                    ._M_header;
      local_5c[0] = (local_4c[0]->super__Rb_tree_node_base)._M_left;
      __f.super_FuncPacketAround.m_bytes = bytes;
      __f.super_FuncPacketAround.m_data = data;
      __f.super_FuncPacketAround.m_except = except;
      for_each<std::_Rb_tree_iterator<std::pair<CEntity*_const,_int>,_std::pair<CEntity*_const,_int>&,_std::pair<CEntity*_const,_int>*>,_FuncPacketView>
                (&local_3c,
                 (_Rb_tree_iterator<std::pair<CEntity*_const,_int>,std::pair<CEntity*_const,_int>&,std::pair<CEntity*_const,_int>*>
                  )local_5c,
                 (_Rb_tree_iterator<std::pair<CEntity*_const,_int>,std::pair<CEntity*_const,_int>&,std::pair<CEntity*_const,_int>*>
                  )local_4c,__f);
    }
    if ((this != f.super_FuncPacketAround.m_except) && (this->m_lpDesc != (DESC *)0x0)) {
      DESC::Packet(this->m_lpDesc,f.super_FuncPacketAround.m_data,f.super_FuncPacketAround.m_bytes);
    }
  }
  return;
}

