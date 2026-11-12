
/* DWARF original prototype: void ComputeAffect(CHARACTER * this, CAffect * pkAff, bool bAdd) */

void __thiscall CHARACTER::ComputeAffect(CHARACTER *this,CAffect *pkAff,bool bAdd)

{
  DWORD *pDVar1;
  DWORD DVar2;
  int amount;
  byte bVar3;
  bool bAdd_local;
  
  if ((((!bAdd) || (pkAff->dwType < 0x97)) || (0xa2 < pkAff->dwType)) ||
     ((this->m_pGuild != (CGuild *)0x0 &&
      (DVar2 = CGuild::UnderAnyWar(this->m_pGuild,'\x03'), DVar2 != 0)))) {
    DVar2 = pkAff->dwFlag;
    if (DVar2 != 0) {
      if (bAdd) {
        pDVar1 = (this->m_afAffectFlag).bits + ((int)(DVar2 - 1) >> 5);
        *pDVar1 = *pDVar1 | 1 << ((byte)(DVar2 - 1) & 0x1f);
      }
      else {
        bVar3 = (byte)(DVar2 - 1) & 0x1f;
        pDVar1 = (this->m_afAffectFlag).bits + ((int)(DVar2 - 1) >> 5);
        *pDVar1 = *pDVar1 & (-2 << bVar3 | 0xfffffffeU >> 0x20 - bVar3);
      }
    }
    if (bAdd) {
      amount = pkAff->lApplyValue;
    }
    else {
      amount = -pkAff->lApplyValue;
    }
    PointChange(this,pkAff->bApplyOn,amount,false,false);
    if (pkAff->dwType == 0x4e) {
      if (!bAdd) {
        StopMuyeongEvent(this);
        return;
      }
      StartMuyeongEvent(this);
      return;
    }
  }
  return;
}

