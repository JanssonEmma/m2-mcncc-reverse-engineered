
/* DWARF original prototype: char * GetName(CHARACTER * this) */

char * __thiscall CHARACTER::GetName(CHARACTER *this)

{
  int iVar1;
  char *pcVar2;
  
  pcVar2 = (this->m_stName)._M_dataplus._M_p;
  iVar1 = *(int *)(pcVar2 + -0xc);
  if (iVar1 == 0) {
    pcVar2 = "";
    if (this->m_pkMobData != (CMob *)0x0) {
      pcVar2 = (this->m_pkMobData->m_table).szLocaleName;
    }
  }
  else {
                    /* Unresolved local var: size_t __n@[???] */
    pcVar2[iVar1] = std::string::_Rep::_S_terminal;
    pcVar2 = (this->m_stName)._M_dataplus._M_p;
  }
  return pcVar2;
}

