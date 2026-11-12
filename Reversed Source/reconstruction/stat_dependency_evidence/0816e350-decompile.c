
void dev_log(char *file,int line,char *function,int level,char *fmt,...)

{
  int __fd;
  int iVar1;
  int iVar2;
  char *pcVar3;
  bool bVar4;
  timeval tv;
  tm ltm;
  char acStack_5e [2];
  char strtime [64];
  
                    /* Unresolved local var: int fd@[DW_OP_reg7(EDI)]
                       Unresolved local var: char * strlevel@[DW_OP_reg2(EDX)]
                       Unresolved local var: int year@[???]
                       Unresolved local var: int mon@[DW_OP_reg2(EDX)]
                       Unresolved local var: int day@[DW_OP_reg0(EAX)]
                       Unresolved local var: int hour@[DW_OP_reg0(EAX)]
                       Unresolved local var: int min@[DW_OP_reg0(EAX)]
                       Unresolved local var: int sec@[DW_OP_reg0(EAX)]
                       Unresolved local var: int usec@[DW_OP_reg0(EAX)]
                       Unresolved local var: int nlen@[DW_OP_reg3(EBX)]
                       Unresolved local var: va_list args@[???] */
  if (test_server == 0) {
    return;
  }
  if ((s_log_mask & level) == 0) {
    return;
  }
  __fd = open("DEV_LOG.log",0x209,0x1b6);
  if (__fd < 0) {
    return;
  }
  gettimeofday((timeval *)&tv,(__timezone_ptr_t)0x0);
  localtime_r((time_t *)&tv,(tm *)&ltm);
  iVar1 = sprintf(strtime,"%02d%02d %02d:%02d.%02d.%06d",ltm.tm_mon + 1,ltm.tm_mday,ltm.tm_hour,
                  ltm.tm_min,ltm.tm_sec,tv.tv_usec);
  acStack_5e[iVar1] = '\0';
  pcVar3 = "LIB2";
  if (level != 0x200) {
    if (level < 0x201) {
      pcVar3 = "INFO";
      if (level == 0x10) goto LAB_0816e468;
      if (level < 0x11) {
        pcVar3 = "ERR";
        if (level == 4) goto LAB_0816e468;
        if (level < 5) {
          pcVar3 = "WARN";
          bVar4 = level == 2;
        }
        else {
          pcVar3 = "CRIT";
          bVar4 = level == 8;
        }
      }
      else {
        pcVar3 = "MAX";
        if (level == 0x40) goto LAB_0816e468;
        if (level < 0x41) {
          pcVar3 = "MIN";
          bVar4 = level == 0x20;
        }
        else {
          pcVar3 = "LIB0";
          if (level == 0x80) goto LAB_0816e468;
          pcVar3 = "LIB1";
          bVar4 = level == 0x100;
        }
      }
    }
    else {
      pcVar3 = "DEB3";
      if (level == 0x4000) goto LAB_0816e468;
      if (level < 0x4001) {
        pcVar3 = "DEB0";
        if (level == 0x800) goto LAB_0816e468;
        if (level < 0x801) {
          pcVar3 = "LIB3";
          bVar4 = level == 0x400;
        }
        else {
          pcVar3 = "DEB1";
          if (level == 0x1000) goto LAB_0816e468;
          pcVar3 = "DEB2";
          bVar4 = level == 0x2000;
        }
      }
      else {
        pcVar3 = "USR1";
        if (level == 0x10000) goto LAB_0816e468;
        if (level < 0x10001) {
          pcVar3 = "USR0";
          bVar4 = level == 0x8000;
        }
        else {
          pcVar3 = "USR2";
          if (level == 0x20000) goto LAB_0816e468;
          pcVar3 = "USR3";
          bVar4 = level == 0x40000;
        }
      }
    }
    if (!bVar4) {
      pcVar3 = "UNKNOWN";
    }
  }
LAB_0816e468:
  iVar1 = sprintf(dev_log::buf,"%s %-4s (%-15s,%4d,%-24s) ",strtime,pcVar3,file,line,function,
                  tv.tv_usec);
  iVar2 = vsprintf(dev_log::buf + iVar1,fmt,&stack0x00000018);
  iVar1 = iVar1 + iVar2;
  dev_log::buf[iVar1] = '\n';
  dev_log::buf[iVar1 + 1] = '\0';
  write(__fd,dev_log::buf,iVar1 + 1);
  close(__fd);
  return;
}

