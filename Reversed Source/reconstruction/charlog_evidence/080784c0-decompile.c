
/* DWARF original prototype: ulong EscapeString(LogManager * this, void * to, void * from, ulong
   length) */

ulong __thiscall LogManager::EscapeString(LogManager *this,void *to,void *from,ulong length)

{
  ulong uVar1;
  
  CAsyncSQL::GetSQLHandle(&this->m_sql);
  uVar1 = mysql_real_escape_string();
  return uVar1;
}

