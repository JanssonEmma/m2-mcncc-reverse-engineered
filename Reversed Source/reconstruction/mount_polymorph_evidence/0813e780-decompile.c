
/* DWARF original prototype: void RefreshAffect(CHARACTER * this) */

void __thiscall CHARACTER::RefreshAffect(CHARACTER *this)

{
  DWORD DVar1;
  DWORD DVar2;
  _List_node<CAffect*> *p_Var3;
  bool bVar4;
  long lVar5;
  long lVar6;
  long lVar7;
  _List_node<CAffect*> *p_Var8;
  TAffectFlag afOld;
  _List_iterator<CAffect*,CAffect*&,CAffect**> __tmp;
  _List_iterator<CAffect*,CAffect*&,CAffect**> it;
  
                    /* Unresolved local var: long lMovSpd@[DW_OP_reg6(ESI)]
                       Unresolved local var: long lAttSpd@[DW_OP_reg7(EDI)] */
  DVar1 = (this->m_afAffectFlag).bits[1];
  p_Var8 = (_List_node<CAffect*> *)
           (((this->m_list_pkAffect).super__List_base<CAffect*,std::allocator<CAffect*>_>.
             super__List_alloc_base<CAffect*,std::allocator<CAffect*>,true>._M_node)->
           super__List_node_base)._M_next;
  DVar2 = (this->m_afAffectFlag).bits[0];
  lVar5 = GetPoint(this,'\x13');
  lVar6 = GetPoint(this,'\x11');
  if (p_Var8 != (this->m_list_pkAffect).super__List_base<CAffect*,std::allocator<CAffect*>_>.
                super__List_alloc_base<CAffect*,std::allocator<CAffect*>,true>._M_node) {
    do {
                    /* Unresolved local var: CAffect * pkAff@[DW_OP_reg0(EAX)] */
      p_Var3 = (_List_node<CAffect*> *)(p_Var8->super__List_node_base)._M_next;
      ComputeAffect(this,p_Var8->_M_data,true);
      p_Var8 = p_Var3;
    } while (p_Var3 != (this->m_list_pkAffect).super__List_base<CAffect*,std::allocator<CAffect*>_>.
                       super__List_alloc_base<CAffect*,std::allocator<CAffect*>,true>._M_node);
  }
  bVar4 = false;
  if ((DVar2 == (this->m_afAffectFlag).bits[0]) && (DVar1 == (this->m_afAffectFlag).bits[1])) {
    bVar4 = true;
  }
  if (((!bVar4) || (lVar7 = GetPoint(this,'\x13'), lVar5 != lVar7)) ||
     (lVar5 = GetPoint(this,'\x11'), lVar6 != lVar5)) {
    UpdatePacket(this);
  }
  return;
}

