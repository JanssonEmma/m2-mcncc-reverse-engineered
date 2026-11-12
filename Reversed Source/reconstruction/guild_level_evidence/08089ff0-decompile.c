
/* DWARF original prototype: void SGuildMember(SGuildMember * this, LPCHARACTER ch, BYTE grade,
   DWORD offer_exp) */

void __thiscall
SGuildMember::SGuildMember(SGuildMember *this,LPCHARACTER ch,BYTE grade,DWORD offer_exp)

{
  BYTE BVar1;
  char *pcVar2;
  allocator local_2c [28];
  
  this->pid = ch->m_dwPlayerID;
  this->is_general = '\0';
  this->grade = grade;
  BVar1 = CHARACTER::GetJob(ch);
  this->job = BVar1;
  this->level = (ch->m_points).level;
  this->offer_exp = offer_exp;
  pcVar2 = CHARACTER::GetName(ch);
  std::string::string((string *)&this->name,pcVar2,local_2c);
  return;
}

