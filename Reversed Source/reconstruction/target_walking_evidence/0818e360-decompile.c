
/* DWARF original prototype: void PacketMonsterLog(CHARACTER_MANAGER * this, LPCHARACTER ch, void *
   buf, int size) */

void __thiscall
CHARACTER_MANAGER::PacketMonsterLog(CHARACTER_MANAGER *this,LPCHARACTER ch,void *buf,int size)

{
  int iVar1;
  int iVar2;
  _Rb_tree_base_iterator _Var3;
  _Rb_tree_iterator<CHARACTER*,CHARACTER*_const&,CHARACTER*_const*> it;
  
  _Var3._M_node =
       &((this->m_set_pkChrMonsterLog)._M_t.
         super__Rb_tree_base<CHARACTER*,std::allocator<CHARACTER*>_>.
         super__Rb_tree_alloc_base<CHARACTER*,std::allocator<CHARACTER*>,true>._M_header)->
        super__Rb_tree_node_base;
  it.super__Rb_tree_base_iterator._M_node =
       (_Rb_tree_base_iterator)((_Rb_tree_node_base *)&(_Var3._M_node)->_M_color)->_M_left;
  do {
    if (it.super__Rb_tree_base_iterator._M_node == (_Rb_tree_base_iterator)_Var3._M_node) {
      return;
    }
                    /* Unresolved local var: LPCHARACTER c@[DW_OP_reg3(EBX)]
                       Unresolved local var: LPDESC d@[???] */
    iVar1 = *(int *)((int)it.super__Rb_tree_base_iterator._M_node + 0x10);
    if (ch == (LPCHARACTER)0x0) {
LAB_0818e3c1:
      if (*(DESC **)(iVar1 + 0x18) != (DESC *)0x0) {
        DESC::Packet(*(DESC **)(iVar1 + 0x18),buf,size);
      }
    }
    else {
      iVar2 = DISTANCE_APPROX(*(int *)(iVar1 + 0x24) - (ch->super_CEntity).m_pos.x,
                              *(int *)(iVar1 + 0x28) - (ch->super_CEntity).m_pos.y);
      if (iVar2 < 0x1771) goto LAB_0818e3c1;
    }
    _Rb_tree_base_iterator::_M_increment(&it.super__Rb_tree_base_iterator);
    _Var3._M_node =
         &((this->m_set_pkChrMonsterLog)._M_t.
           super__Rb_tree_base<CHARACTER*,std::allocator<CHARACTER*>_>.
           super__Rb_tree_alloc_base<CHARACTER*,std::allocator<CHARACTER*>,true>._M_header)->
          super__Rb_tree_node_base;
  } while( true );
}

