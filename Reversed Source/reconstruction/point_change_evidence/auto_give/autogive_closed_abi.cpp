#include <map>
#include <vector>
#include <queue>
#include <string>
#include "../../game/src/db.h"
#include "../../game/src/char.h"
#include <cstddef>
#define GATE(n,x) typedef char n[(x)?1:-1]
GATE(money_size,sizeof(TPacketMoneyLog)==9);
GATE(money_vnum,offsetof(TPacketMoneyLog,vnum)==1);
GATE(money_gold,offsetof(TPacketMoneyLog,gold)==5);
GATE(use_size,sizeof(TUseTime)==56);
GATE(use_time,offsetof(TUseTime,dwUseSec)==36);
GATE(use_ip,offsetof(TUseTime,szIP)==40);
static void (DBManager::*money)(BYTE,DWORD,int)=&DBManager::SendMoneyLog;
static bool (CHARACTER::*setslot)(BYTE,TQuickslot&)=&CHARACTER::SetQuickslot;
static bool (CHARACTER::*getslot)(BYTE,TQuickslot**)=&CHARACTER::GetQuickslot;
static bool (CHARACTER::*delslot)(BYTE)=&CHARACTER::DelQuickslot;
static bool (CHARACTER::*grid)(BYTE,BYTE,int)=&CHARACTER::IsEmptyItemGrid;
static int (CHARACTER::*empty)(BYTE)=&CHARACTER::GetEmptyInventory;

#include "../../game/src/packet.h"
GATE(quick_add_size,sizeof(packet_quickslot_add)==4);
GATE(quick_add_pos,offsetof(packet_quickslot_add,pos)==1);
GATE(quick_add_slot,offsetof(packet_quickslot_add,slot)==2);
GATE(quick_del_size,sizeof(packet_quickslot_del)==2);
GATE(quick_del_pos,offsetof(packet_quickslot_del,pos)==1);
GATE(use_login,offsetof(TUseTime,dwLoginKey)==0);
GATE(use_name,offsetof(TUseTime,szLogin)==4);
GATE(use_bill,offsetof(TUseTime,bBillType)==35);
