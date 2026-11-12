
/* DWARF original prototype: void Query(LogManager * this, char * c_pszFormat, ...) */

void __thiscall LogManager::Query(LogManager *this,char *c_pszFormat,...)

{
  char szQuery [4096];
  
                    /* Unresolved local var: va_list args@[???] */
  vsnprintf(szQuery,0x1000,c_pszFormat,&stack0x0000000c);
  CAsyncSQL::AsyncQuery(&this->m_sql,szQuery);
  return;
}

