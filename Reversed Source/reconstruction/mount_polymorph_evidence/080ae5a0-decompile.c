
/* DWARF original prototype: bool DeclareWar(CGuild * this, DWORD dwOppGID, BYTE type, BYTE state)
    */

bool __thiscall CGuild::DeclareWar(CGuild *this,DWORD dwOppGID,BYTE type,BYTE state)

{
  bool bVar1;
  BYTE state_local;
  BYTE type_local;
  pair<const_long_unsigned_int,SGuildWar> local_8c [2];
  TGuildWar gw;
  _Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_SGuildWar>,std::_Select1st<std::pair<const_long_unsigned_int,_SGuildWar>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SGuildWar>_>_>
  local_2c [2];
  
  _Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_SGuildWar>,std::_Select1st<std::pair<const_long_unsigned_int,_SGuildWar>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SGuildWar>_>_>
  ::find(local_2c,(DWORD *)&this->m_EnemyGuild);
  bVar1 = local_2c[0].
          super__Rb_tree_base<std::pair<const_long_unsigned_int,_SGuildWar>,std::allocator<std::pair<const_long_unsigned_int,_SGuildWar>_>_>
          .
          super__Rb_tree_alloc_base<std::pair<const_long_unsigned_int,_SGuildWar>,std::allocator<std::pair<const_long_unsigned_int,_SGuildWar>_>,true>
          ._M_header ==
          (_Rb_tree_alloc_base<std::pair<const_long_unsigned_int,_SGuildWar>,std::allocator<std::pair<const_long_unsigned_int,_SGuildWar>_>,true>
           )(this->m_EnemyGuild)._M_t.
            super__Rb_tree_base<std::pair<const_long_unsigned_int,_SGuildWar>,std::allocator<std::pair<const_long_unsigned_int,_SGuildWar>_>_>
            .
            super__Rb_tree_alloc_base<std::pair<const_long_unsigned_int,_SGuildWar>,std::allocator<std::pair<const_long_unsigned_int,_SGuildWar>_>,true>
            ._M_header;
  if (bVar1) {
    gw.type = type;
    local_8c[0].second.state = (DWORD)state;
    local_8c[0].first = dwOppGID;
    local_8c[0].second.type = type;
    local_8c[0].second._13_3_ = gw._13_3_;
    local_8c[0].second.war_start_time = 0;
    gw.war_start_time = 0;
    gw.score = 0;
    gw.map_index = 0;
    local_8c[0].second.score = 0;
    local_8c[0].second.map_index = 0;
    gw.state = local_8c[0].second.state;
    _Rb_tree<long_unsigned_int,std::pair<const_long_unsigned_int,_SGuildWar>,std::_Select1st<std::pair<const_long_unsigned_int,_SGuildWar>_>,std::less<long_unsigned_int>,std::allocator<std::pair<const_long_unsigned_int,_SGuildWar>_>_>
    ::insert_unique(&(this->m_EnemyGuild)._M_t,local_8c);
    GuildWarPacket(this,dwOppGID,type,state);
  }
  return bVar1;
}

