
/* DWARF original prototype: void BroadcastTargetPacket(CHARACTER * this) */

void __thiscall CHARACTER::BroadcastTargetPacket(CHARACTER *this)

{
  CHARACTER *this_00;
  DESC *this_01;
  _Rb_tree_iterator<CHARACTER*,CHARACTER*_const&,CHARACTER*_const*> _Var1;
  int iVar2;
  char *pcVar3;
  _Rb_tree_base_iterator _Var4;
  _Rb_tree_iterator<CHARACTER*,CHARACTER*_const&,CHARACTER*_const*> it;
  TPacketGCTarget p;
  
  if ((this->m_set_pkChrTargetedBy)._M_t._M_node_count != 0) {
    p.header = '?';
    p.dwVID = (this->m_vid).m_id;
    if ((this->super_CEntity).m_lpDesc == (LPDESC)0x0) {
      iVar2 = MINMAX(0,((this->m_points).hp * 100) / (this->m_pointsInstant).iMaxHP,100);
      p.bHPPercent = (BYTE)iVar2;
    }
    else {
      p.bHPPercent = '\0';
    }
    _Var4._M_node =
         &((this->m_set_pkChrTargetedBy)._M_t.
           super__Rb_tree_base<CHARACTER*,std::allocator<CHARACTER*>_>.
           super__Rb_tree_alloc_base<CHARACTER*,std::allocator<CHARACTER*>,true>._M_header)->
          super__Rb_tree_node_base;
    it.super__Rb_tree_base_iterator._M_node =
         (_Rb_tree_base_iterator)((_Rb_tree_node_base *)&(_Var4._M_node)->_M_color)->_M_left;
    while (_Var1.super__Rb_tree_base_iterator._M_node = it.super__Rb_tree_base_iterator._M_node,
          it.super__Rb_tree_base_iterator._M_node != (_Rb_tree_base_iterator)_Var4._M_node) {
                    /* Unresolved local var: LPCHARACTER pkChr@[DW_OP_reg3(EBX)]
                       Unresolved local var:
                       _Rb_tree_iterator<CHARACTER*,CHARACTER*_const&,CHARACTER*_const*> __tmp@[???]
                        */
      _Rb_tree_base_iterator::_M_increment(&it.super__Rb_tree_base_iterator);
      this_00 = *(CHARACTER **)((int)_Var1.super__Rb_tree_base_iterator._M_node + 0x10);
      this_01 = (this_00->super_CEntity).m_lpDesc;
      if (this_01 == (DESC *)0x0) {
        pcVar3 = GetName(this_00);
        _sys_err("BroadcastTargetPacket",0x12c1,"%s %p does not have desc",pcVar3,this_00);
                    /* WARNING: Subroutine does not return */
        abort();
      }
      DESC::Packet(this_01,&p,6);
      _Var4._M_node =
           &((this->m_set_pkChrTargetedBy)._M_t.
             super__Rb_tree_base<CHARACTER*,std::allocator<CHARACTER*>_>.
             super__Rb_tree_alloc_base<CHARACTER*,std::allocator<CHARACTER*>,true>._M_header)->
            super__Rb_tree_node_base;
    }
  }
  return;
}

