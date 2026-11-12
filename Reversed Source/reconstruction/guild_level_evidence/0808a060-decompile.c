
/* DWARF original prototype: void SGuildMember(SGuildMember * this, DWORD pid, BYTE grade, BYTE
   is_general, BYTE job, BYTE level, DWORD offer_exp, char * name) */

void __thiscall
SGuildMember::SGuildMember
          (SGuildMember *this,DWORD pid,BYTE grade,BYTE is_general,BYTE job,BYTE level,
          DWORD offer_exp,char *name)

{
  allocator local_1c [24];
  
  this->pid = pid;
  this->grade = grade;
  this->is_general = is_general;
  this->job = job;
  this->level = level;
  this->offer_exp = offer_exp;
  std::string::string((string *)&this->name,name,local_1c);
  return;
}

