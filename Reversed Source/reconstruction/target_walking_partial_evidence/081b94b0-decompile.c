
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

DWORD get_dword_time(void)

{
  timeval tv;
  
  gettimeofday((timeval *)&tv,(__timezone_ptr_t)0x0);
  return tv.tv_usec / 1000 + -0x43d4db10 + tv.tv_sec * 1000;
}

