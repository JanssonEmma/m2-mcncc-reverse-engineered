
/* DWARF original prototype: void LevelChange(CGuild * this, DWORD pid, BYTE level) */

void __thiscall CGuild::LevelChange(CGuild *this,DWORD pid,BYTE level)

{
  DESC *this_00;
  _Rb_tree_base_iterator _Var1;
  _Rb_tree_iterator<CHARACTER*,CHARACTER*_const&,CHARACTER*_const*> it;
  TPacketGCGuild pack;
  TPacketGuildChangeMemberData gd_guild;
  _Rb_tree_iterator<std::pair<const_long_unsigned_int,_SGuildMember>,std::pair<const_long_unsigned_int,_SGuildMember>&,std::pair<const_long_unsigned_int,_SGuildMember>*>
  cit;
  
  _Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_SGuildMember>,std::_Select1st<std::pair<const_long_unsigned_int,_SGuildMember>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>_>
  ::find((_Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_SGuildMember>,std::_Select1st<std::pair<const_long_unsigned_int,_SGuildMember>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>_>
          *)&cit,(DWORD *)&this->m_member);
  if (cit.super__Rb_tree_base_iterator._M_node !=
      (_Rb_tree_base_iterator)
      &((this->m_member)._M_t.
        super__Rb_tree_base<std::pair<const_long_unsigned_int,_SGuildMember>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>_>
        .
        super__Rb_tree_alloc_base<std::pair<const_long_unsigned_int,_SGuildMember>,std::allocator<std::pair<const_long_unsigned_int,_SGuildMember>_>,true>
       ._M_header)->super__Rb_tree_node_base) {
    *(BYTE *)((int)cit.super__Rb_tree_base_iterator._M_node + 0x1b) = level;
    gd_guild.guild_id = (this->m_data).guild_id;
    gd_guild.pid = pid;
    gd_guild.offer = *(DWORD *)((int)cit.super__Rb_tree_base_iterator._M_node + 0x1c);
    gd_guild.grade = *(BYTE *)((int)cit.super__Rb_tree_base_iterator._M_node + 0x18);
    gd_guild.level = level;
    CLIENT_DESC::DBPacket(db_clientdesc,'\x18',0,&gd_guild,0xe);
    pack = (TPacketGCGuild)((uint)pack & 0xffffff00 | 0x4b);
    *(undefined1 *)((int)cit.super__Rb_tree_base_iterator._M_node + 0x20) = 0;
    _Var1._M_node =
         &((this->m_memberOnline)._M_t.super__Rb_tree_base<CHARACTER*,std::allocator<CHARACTER*>_>.
           super__Rb_tree_alloc_base<CHARACTER*,std::allocator<CHARACTER*>,true>._M_header)->
          super__Rb_tree_node_base;
    it.super__Rb_tree_base_iterator._M_node =
         (_Rb_tree_base_iterator)((_Rb_tree_node_base *)&(_Var1._M_node)->_M_color)->_M_left;
    while (it.super__Rb_tree_base_iterator._M_node != (_Rb_tree_base_iterator)_Var1._M_node) {
                    /* Unresolved local var: LPDESC d@[???] */
      this_00 = *(DESC **)(*(int *)((int)it.super__Rb_tree_base_iterator._M_node + 0x10) + 0x18);
      if (this_00 != (DESC *)0x0) {
        pack.size = 0x11;
        pack.subheader = '\x02';
        DESC::BufferedPacket(this_00,&pack,4);
        DESC::Packet(this_00,(void *)((int)cit.super__Rb_tree_base_iterator._M_node + 0x14),0xd);
      }
      _Rb_tree_base_iterator::_M_increment(&it.super__Rb_tree_base_iterator);
      _Var1._M_node =
           &((this->m_memberOnline)._M_t.super__Rb_tree_base<CHARACTER*,std::allocator<CHARACTER*>_>
             .super__Rb_tree_alloc_base<CHARACTER*,std::allocator<CHARACTER*>,true>._M_header)->
            super__Rb_tree_node_base;
    }
  }
  return;
}

