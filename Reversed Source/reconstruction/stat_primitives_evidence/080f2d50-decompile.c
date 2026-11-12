
bool RaceToJob(uint race,uint *ret_job)

{
  bool bVar1;
  
  bVar1 = false;
  *ret_job = 0;
  switch(race) {
  case 0:
  case 4:
    *ret_job = 0;
    break;
  case 1:
  case 5:
    *ret_job = 1;
    break;
  case 2:
  case 6:
    *ret_job = 2;
    break;
  case 3:
  case 7:
    *ret_job = 3;
    break;
  default:
    goto switchD_080f2d66_default;
  }
  bVar1 = true;
switchD_080f2d66_default:
  return bVar1;
}

