
/* DWARF original prototype: void Packet(DESC * this, void * c_pvData, int iSize) */

void __thiscall DESC::Packet(DESC *this,void *c_pvData,int iSize)

{
  byte bVar1;
  char *pcVar2;
  LPBUFFER pbVar3;
  TrafficProfiler *this_00;
  pair<long_unsigned_int,long_unsigned_int>_conflict *ppVar4;
  time_t tVar5;
  int iVar6;
  DWORD *dest;
  DWORD DVar7;
  DWORD *pdwWritePoint;
  TPacketGGRelay p;
  
  if (iSize < 1) {
    __assert("Packet","desc.cpp",0x155);
  }
  if (this->m_iPhase == 0) {
    return;
  }
  pcVar2 = (this->m_stRelayName)._M_dataplus._M_p;
  if (*(int *)(pcVar2 + -0xc) == 0) {
    if (this->m_lpBufferedOutputBuffer != (LPBUFFER)0x0) {
      buffer_write(this->m_lpBufferedOutputBuffer,c_pvData,iSize);
      c_pvData = buffer_read_peek(this->m_lpBufferedOutputBuffer);
      iSize = buffer_size(this->m_lpBufferedOutputBuffer);
    }
    if (g_bTrafficProfileOn != '\0') {
      if (singleton<TrafficProfiler>::ms_singleton == (TrafficProfiler *)0x0) {
        __assert("instance","../../common/singleton.h",0x1a);
      }
      this_00 = singleton<TrafficProfiler>::ms_singleton;
                    /* Unresolved local var: pair<long_unsigned_int,long_unsigned_int>.conflict *
                       rTrafficInfo@[???] */
      bVar1 = *(byte *)c_pvData;
      ppVar4 = singleton<TrafficProfiler>::ms_singleton->m_aTrafficVec[1].
               super__Vector_base<std::pair<long_unsigned_int,_long_unsigned_int>,std::allocator<std::pair<long_unsigned_int,_long_unsigned_int>_>_>
               .
               super__Vector_alloc_base<std::pair<long_unsigned_int,_long_unsigned_int>,std::allocator<std::pair<long_unsigned_int,_long_unsigned_int>_>,true>
               ._M_start;
      singleton<TrafficProfiler>::ms_singleton->m_dwTotalTraffic =
           singleton<TrafficProfiler>::ms_singleton->m_dwTotalTraffic + iSize;
      ppVar4 = ppVar4 + bVar1;
      DVar7 = this_00->m_dwTotalPacket;
      if (ppVar4->second == 0) {
        DVar7 = DVar7 + 1;
      }
      this_00->m_dwTotalPacket = DVar7;
      ppVar4->first = ppVar4->first + iSize;
      ppVar4->second = ppVar4->second + 1;
      tVar5 = time((time_t *)0x0);
      if (this_00->m_dwFlushCycle <= (uint)(tVar5 - this_00->m_tmProfileStartTime)) {
        TrafficProfiler::Flush(this_00);
      }
    }
    if (this->m_bEncrypted == false) {
      pbVar3 = this->m_lpOutputBuffer;
      if (pbVar3 == (LPBUFFER)0x0) {
        __assert("__packet_encode","protocol.h",0x2f);
      }
      if (c_pvData == (void *)0x0) {
        __assert("__packet_encode","protocol.h",0x30);
      }
      iVar6 = buffer_has_space(pbVar3);
      if (iVar6 < iSize) {
LAB_08139dd4:
        this->m_iPhase = 0;
      }
      else {
        buffer_write(pbVar3,c_pvData,iSize);
      }
    }
    else {
      iVar6 = buffer_has_space(this->m_lpOutputBuffer);
      if (iVar6 < (int)(iSize + 8U)) {
        sys_log(0,"buffer mem_size  = %d",this->m_lpOutputBuffer->mem_size);
        sys_log(0,"buffer write_pos = %d",this->m_lpOutputBuffer->write_point_pos);
        sys_log(0,"iSize = %d",iSize);
        goto LAB_08139dd4;
      }
      dest = buffer_write_peek(this->m_lpOutputBuffer);
      pbVar3 = this->m_lpOutputBuffer;
      if (pbVar3 == (LPBUFFER)0x0) {
        __assert("__packet_encode","protocol.h",0x2f);
      }
      if (c_pvData == (void *)0x0) {
        __assert("__packet_encode","protocol.h",0x30);
      }
      iVar6 = buffer_has_space(pbVar3);
      if (iSize <= iVar6) {
        buffer_write(pbVar3,c_pvData,iSize);
                    /* Unresolved local var: int iSize2@[DW_OP_reg0(EAX)] */
        iVar6 = TEA_Encrypt(dest,dest,this->m_adwEncryptionKey,iSize);
        if (iSize < iVar6) {
          buffer_write_proceed(this->m_lpOutputBuffer,iVar6 - iSize);
        }
      }
    }
    if (this->m_lpBufferedOutputBuffer != (LPBUFFER)0x0) {
      buffer_delete(this->m_lpBufferedOutputBuffer);
      this->m_lpBufferedOutputBuffer = (LPBUFFER)0x0;
    }
LAB_08139de2:
    fdwatch_add_fd(this->m_lpFdw,this->m_sock,this,2,1);
  }
  else {
    p.bHeader = '\x03';
                    /* Unresolved local var: size_t __n@[???] */
    pcVar2[*(int *)(pcVar2 + -0xc)] = std::string::_Rep::_S_terminal;
    strncpy(p.szName,(this->m_stRelayName)._M_dataplus._M_p,0x18);
    p.szName[0x18] = '\0';
    pbVar3 = this->m_lpOutputBuffer;
    p.lSize = iSize;
    if (pbVar3 == (LPBUFFER)0x0) {
      __assert("__packet_encode","protocol.h",0x2f);
    }
    iVar6 = buffer_has_space(pbVar3);
    if (0x1d < iVar6) {
      buffer_write(pbVar3,&p,0x1e);
      strlen("");
      std::string::assign((char *)&this->m_stRelayName,0x81f5c80);
      pbVar3 = this->m_lpOutputBuffer;
      if (pbVar3 == (LPBUFFER)0x0) {
        __assert("__packet_encode","protocol.h",0x2f);
      }
      if (c_pvData == (void *)0x0) {
        __assert("__packet_encode","protocol.h",0x30);
      }
      iVar6 = buffer_has_space(pbVar3);
      if (iSize <= iVar6) {
        buffer_write(pbVar3,c_pvData,iSize);
        goto LAB_08139de2;
      }
    }
    this->m_iPhase = 0;
  }
  return;
}

