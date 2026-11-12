
/* DWARF original prototype: void ApplyPoint(CHARACTER * this, BYTE bApplyType, long lVal) */

void __thiscall CHARACTER::ApplyPoint(CHARACTER *this,BYTE bApplyType,long lVal)

{
  int iVar1;
  byte bVar2;
  char *pcVar3;
  uint uVar4;
  uint uVar5;
  BYTE type;
  map<int,int,std::less<int>,std::allocator<std::pair<const_int,_int>_>_> *local_c4;
  int iChange;
  BYTE bSkillVnum;
  uint local_a0;
  uint local_9c;
  undefined4 local_98;
  uint local_90;
  uint local_8c;
  undefined4 local_88;
  uint local_80;
  uint local_7c;
  undefined4 local_78;
  uint local_70;
  _Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
  __i_1;
  _Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
  local_4c;
  _Rb_tree_base_iterator local_3c;
  _Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
  __i;
  
  uVar4 = (uint)bApplyType;
  switch(uVar4) {
  case 0:
    break;
  case 1:
  case 2:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
  case 0x10:
  case 0x11:
  case 0x12:
  case 0x13:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
  case 0x20:
  case 0x21:
  case 0x22:
  case 0x23:
  case 0x24:
  case 0x25:
  case 0x26:
  case 0x27:
  case 0x28:
  case 0x29:
  case 0x2a:
  case 0x2b:
  case 0x2c:
  case 0x2d:
  case 0x2e:
  case 0x2f:
  case 0x30:
  case 0x31:
  case 0x32:
  case 0x34:
  case 0x35:
  case 0x36:
  case 0x37:
  case 0x38:
  case 0x39:
  case 0x3a:
  case 0x3b:
  case 0x3c:
  case 0x3d:
  case 0x3e:
  case 0x3f:
  case 0x40:
  case 0x41:
  case 0x42:
  case 0x43:
  case 0x44:
  case 0x45:
  case 0x46:
  case 0x47:
  case 0x48:
  case 0x49:
  case 0x4a:
  case 0x4b:
  case 0x4c:
  case 0x4e:
  case 0x4f:
  case 0x50:
  case 0x51:
    type = (&aApplyInfo)[uVar4];
    goto LAB_0812f88d;
  case 3:
    PointChange(this,'\r',lVal,false,false);
    bVar2 = GetJob(this);
    PointChange(this,'\x06',lVal * *(int *)(JobInitialPoints + (uint)bVar2 * 0x40 + 0x18),false,
                false);
    bVar2 = GetJob(this);
    iVar1 = *(int *)(JobInitialPoints + (uint)bVar2 * 0x40 + 0x34);
    type = '\n';
    goto LAB_0812f889;
  case 4:
    PointChange(this,'\x0f',lVal,false,false);
    bVar2 = GetJob(this);
    type = '\b';
    iVar1 = *(int *)(JobInitialPoints + (uint)bVar2 * 0x40 + 0x1c);
LAB_0812f889:
    lVal = lVal * iVar1;
LAB_0812f88d:
    PointChange(this,type,lVal,false,false);
    break;
  case 0x33:
    if (this->m_pSkillLevels != (TPlayerSkill *)0x0) {
                    /* Unresolved local var: int iAdd@[DW_OP_reg3(EBX)] */
      uVar4 = lVal & 0x7fffff;
      bVar2 = (byte)((uint)lVal >> 0x18);
      uVar5 = (uint)bVar2;
      sys_log(1,"APPLY_SKILL skill %d add? %d change %d",uVar5,(uint)((lVal & 0x800000U) != 0),uVar4
             );
      if ((lVal & 0x800000U) == 0) {
        local_80 = uVar5;
        _Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
        ::lower_bound((_Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
                       *)&__i_1,(int *)&this->m_SkillDamageBonus);
        if ((__i_1.super__Rb_tree_base_iterator._M_node ==
             (_Rb_tree_base_iterator)
             &((this->m_SkillDamageBonus)._M_t.
               super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
              ._M_header)->super__Rb_tree_node_base) ||
           ((int)local_80 < *(int *)((int)__i_1.super__Rb_tree_base_iterator._M_node + 0x10))) {
          local_3c = __i_1.super__Rb_tree_base_iterator._M_node;
          __i = __i_1;
          local_8c = local_80;
          local_88 = 0;
          _Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
          ::insert_unique(&local_4c,
                          (_Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
                           )&this->m_SkillDamageBonus,(pair<const_int,int> *)&__i);
          __i_1.super__Rb_tree_base_iterator._M_node =
               (_Rb_tree_base_iterator)
               (_Rb_tree_base_iterator)
               local_4c.
               super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
               ._M_header;
        }
        *(int *)((int)__i_1.super__Rb_tree_base_iterator._M_node + 0x14) =
             *(int *)((int)__i_1.super__Rb_tree_base_iterator._M_node + 0x14) - uVar4;
      }
      else {
        local_70 = uVar5;
        _Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
        ::lower_bound((_Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
                       *)&__i,(int *)&this->m_SkillDamageBonus);
        local_3c._M_node =
             &((this->m_SkillDamageBonus)._M_t.
               super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
              ._M_header)->super__Rb_tree_node_base;
        if ((__i.super__Rb_tree_base_iterator._M_node == (_Rb_tree_base_iterator)local_3c._M_node)
           || ((int)local_70 < *(int *)((int)__i.super__Rb_tree_base_iterator._M_node + 0x10))) {
          __i_1 = __i;
          local_78 = 0;
          local_7c = local_70;
          _Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
          ::insert_unique(&local_4c,
                          (_Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
                           )&this->m_SkillDamageBonus,(pair<const_int,int> *)&__i_1);
          __i.super__Rb_tree_base_iterator._M_node =
               (_Rb_tree_base_iterator)
               (_Rb_tree_base_iterator)
               local_4c.
               super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>
               .
               super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
               ._M_header;
        }
        *(int *)((int)__i.super__Rb_tree_base_iterator._M_node + 0x14) =
             *(int *)((int)__i.super__Rb_tree_base_iterator._M_node + 0x14) + uVar4;
      }
      local_c4 = &this->m_SkillDamageBonus;
      local_90 = (uint)bVar2;
      _Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
      ::lower_bound((_Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
                     *)&__i_1,(int *)local_c4);
      if ((__i_1.super__Rb_tree_base_iterator._M_node ==
           (_Rb_tree_base_iterator)
           &((this->m_SkillDamageBonus)._M_t.
             super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>
             .
             super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
            ._M_header)->super__Rb_tree_node_base) ||
         ((int)local_90 < *(int *)((int)__i_1.super__Rb_tree_base_iterator._M_node + 0x10))) {
        local_3c = __i_1.super__Rb_tree_base_iterator._M_node;
        local_9c = local_90;
        local_98 = 0;
        __i = __i_1;
        _Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
        ::insert_unique(&local_4c,
                        (_Rb_tree_iterator<std::pair<const_int,_int>,std::pair<const_int,_int>&,std::pair<const_int,_int>*>
                         )local_c4,(pair<const_int,int> *)&__i);
        __i_1.super__Rb_tree_base_iterator._M_node =
             (_Rb_tree_base_iterator)
             (_Rb_tree_base_iterator)
             local_4c.
             super__Rb_tree_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>_>
             .
             super__Rb_tree_alloc_base<std::pair<const_int,_int>,std::allocator<std::pair<const_int,_int>_>,true>
             ._M_header;
      }
      if (*(int *)((int)__i_1.super__Rb_tree_base_iterator._M_node + 0x14) == 0) {
        local_a0 = (uint)bVar2;
        _Rb_tree<int,std::pair<const_int,_int>,std::_Select1st<std::pair<const_int,_int>_>,std::less<int>,std::allocator<std::pair<const_int,_int>_>_>
        ::erase(&local_c4->_M_t,(int *)&local_a0);
      }
    }
    break;
  default:
    pcVar3 = GetName(this);
    _sys_err("ApplyPoint",0xe2c,"Unknown apply type %d name %s",uVar4,pcVar3);
  }
  return;
}

