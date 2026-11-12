
void sys_log(int lv,char *format,...)

{
  tm *__tp;
  char *__s;
  size_t sVar1;
  time_t ct;
  
                    /* Unresolved local var: va_list args@[???] */
  if ((lv <= log_level) && (log_file_sys != (LPLOGFILE)0x0)) {
                    /* Unresolved local var: char * time_s@[DW_OP_reg3(EBX)] */
    ct = time((time_t *)0x0);
    __tp = localtime(&ct);
    __s = asctime(__tp);
    fprintf((FILE *)log_file_sys->fp,sys_log_header_string);
    sVar1 = strlen(__s);
    __s[sVar1 - 1] = '\0';
    fprintf((FILE *)log_file_sys->fp,"%-15.15s :: ",__s + 4);
    vfprintf((FILE *)log_file_sys->fp,format,&stack0x0000000c);
    fputc(10,(FILE *)log_file_sys->fp);
    fflush((FILE *)log_file_sys->fp);
    if (0 < log_level) {
      fprintf(__stdoutp,sys_log_header_string);
      vfprintf(__stdoutp,format,&stack0x0000000c);
      fputc(10,__stdoutp);
      fflush(__stdoutp);
    }
  }
  return;
}

