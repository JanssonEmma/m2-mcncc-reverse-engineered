
/* DWARF original prototype: void CharLog(LogManager * this, DWORD dwPID, DWORD x, DWORD y, DWORD
   dw, char * c_pszText, char * c_pszHint) */

void __thiscall
LogManager::CharLog(LogManager *this,DWORD dwPID,DWORD x,DWORD y,DWORD dw,char *c_pszText,
                   char *c_pszHint)

{
  size_t length;
  char *pcVar1;
  
  length = strlen(c_pszHint);
  EscapeString(this,__escape_hint,c_pszHint,length);
  pcVar1 = get_table_postfix();
  sys_log(0,
          "INSERT DELAYED INTO log%s (type, time, who, x, y, what, how, hint) VALUES(\'CHARACTER\', NOW(), %lu, %lu, %lu, %lu, \'%s\', \'%s\')"
          ,pcVar1,dwPID,x,y,dw,c_pszText,c_pszHint);
  pcVar1 = get_table_postfix();
  Query(this,
        "INSERT DELAYED INTO log%s (type, time, who, x, y, what, how, hint) VALUES(\'CHARACTER\', NOW(), %lu, %lu, %lu, %lu, \'%s\', \'%s\')"
        ,pcVar1,dwPID,x,y,dw,c_pszText,__escape_hint);
  return;
}

