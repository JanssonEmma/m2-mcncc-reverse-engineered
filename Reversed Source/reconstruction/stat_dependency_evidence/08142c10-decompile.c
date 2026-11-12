
/* DWARF original prototype: void SendPartyInfoOneToAll(CParty * this, LPCHARACTER ch) */

void __thiscall CParty::SendPartyInfoOneToAll(CParty *this,LPCHARACTER ch)

{
  _Rb_tree_node<std::pair<const_long_unsigned_int,_CParty::SMember>_> *p_Var1;
  CHARACTER *this_00;
  DWORD DVar2;
  DWORD DVar3;
  char *pcVar4;
  char *pcVar5;
  TPacketGCPartyUpdate p;
  _Rb_tree_iterator<std::pair<const_long_unsigned_int,_CParty::SMember>,std::pair<const_long_unsigned_int,_CParty::SMember>&,std::pair<const_long_unsigned_int,_CParty::SMember>*>
  it;
  
  if ((ch->super_CEntity).m_lpDesc != (LPDESC)0x0) {
    CHARACTER::BuildUpdatePartyPacket(ch,&p);
    p_Var1 = (this->m_memberMap)._M_t.
             super__Rb_tree_base<std::pair<const_long_unsigned_int,_CParty::SMember>,std::allocator<std::pair<const_long_unsigned_int,_CParty::SMember>_>_>
             .
             super__Rb_tree_alloc_base<std::pair<const_long_unsigned_int,_CParty::SMember>,std::allocator<std::pair<const_long_unsigned_int,_CParty::SMember>_>,true>
             ._M_header;
    it.super__Rb_tree_base_iterator._M_node =
         (_Rb_tree_base_iterator)(p_Var1->super__Rb_tree_node_base)._M_left;
    if (it.super__Rb_tree_base_iterator._M_node != (_Rb_tree_base_iterator)p_Var1) {
      do {
        this_00 = *(CHARACTER **)((int)it.super__Rb_tree_base_iterator._M_node + 0x14);
        if ((this_00 != (CHARACTER *)0x0) && ((this_00->super_CEntity).m_lpDesc != (LPDESC)0x0)) {
          DVar2 = (this_00->m_vid).m_id;
          pcVar4 = CHARACTER::GetName(this_00);
          DVar3 = (ch->m_vid).m_id;
          pcVar5 = CHARACTER::GetName(ch);
          sys_log(2,"PARTY send info %s[%d] to %s[%d]",pcVar5,DVar3,pcVar4,DVar2);
          DESC::Packet(*(DESC **)(*(int *)((int)it.super__Rb_tree_base_iterator._M_node + 0x14) +
                                 0x18),&p,0x15);
        }
        _Rb_tree_base_iterator::_M_increment(&it.super__Rb_tree_base_iterator);
      } while (it.super__Rb_tree_base_iterator._M_node !=
               (_Rb_tree_base_iterator)
               &((this->m_memberMap)._M_t.
                 super__Rb_tree_base<std::pair<const_long_unsigned_int,_CParty::SMember>,std::allocator<std::pair<const_long_unsigned_int,_CParty::SMember>_>_>
                 .
                 super__Rb_tree_alloc_base<std::pair<const_long_unsigned_int,_CParty::SMember>,std::allocator<std::pair<const_long_unsigned_int,_CParty::SMember>_>,true>
                ._M_header)->super__Rb_tree_node_base);
    }
  }
  return;
}

