
void _sys_err(char *func,int line,char *format,...)

{
  tm *__tp;
  char *__s;
  size_t sVar1;
  int iVar2;
  time_t ct;
  char buf [1026];
  
                    /* Unresolved local var: va_list args@[???]
                       Unresolved local var: char * time_s@[DW_OP_reg3(EBX)]
                       Unresolved local var: int len@[DW_OP_reg2(EDX)] */
  ct = time((time_t *)0x0);
  __tp = localtime(&ct);
  __s = asctime(__tp);
  if (log_file_err != (LPLOGFILE)0x0) {
    sVar1 = strlen(__s);
    __s[sVar1 - 1] = '\0';
    iVar2 = snprintf(buf,0x400,"SYSERR: %-15.15s :: %s: ",__s + 4,func);
    buf[0x401] = '\0';
    if (iVar2 < 0x400) {
      vsnprintf(buf + iVar2,0x400 - iVar2,format,&stack0x00000010);
    }
    strcat(buf,"\n");
    fputs(buf,(FILE *)log_file_err->fp);
    fflush((FILE *)log_file_err->fp);
    fputs(buf,(FILE *)log_file_sys->fp);
    fflush((FILE *)log_file_sys->fp);
  }
  return;
}

