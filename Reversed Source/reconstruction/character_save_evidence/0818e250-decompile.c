
/* DWARF original prototype: void DelayedSave(CHARACTER_MANAGER * this, LPCHARACTER ch) */

void __thiscall CHARACTER_MANAGER::DelayedSave(CHARACTER_MANAGER *this,LPCHARACTER ch)

{
  pair<std::_Rb_tree_iterator<CHARACTER*,_CHARACTER*&,_CHARACTER**>,bool> __p;
  
  _Rb_tree<CHARACTER*,CHARACTER*,std::_Identity<CHARACTER*>,std::less<CHARACTER*>,std::allocator<CHARACTER*>_>
  ::insert_unique(&(this->m_set_pkChrForDelayedSave)._M_t,&ch);
  return;
}

