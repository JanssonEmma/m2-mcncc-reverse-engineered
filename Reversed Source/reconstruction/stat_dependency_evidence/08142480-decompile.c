
/* DWARF original prototype: void RequestSetMemberLevel(CParty * this, DWORD pid, BYTE level) */

void __thiscall CParty::RequestSetMemberLevel(CParty *this,DWORD pid,BYTE level)

{
  TPacketPartySetMemberLevel p;
  
  p.dwLeaderPID = GetLeaderPID(this);
  p.dwPID = pid;
  p.bLevel = level;
  CLIENT_DESC::DBPacket(db_clientdesc,'8',0,&p,9);
  return;
}

