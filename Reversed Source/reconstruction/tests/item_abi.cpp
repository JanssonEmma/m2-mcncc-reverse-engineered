#include <cstddef>
#include <map>
#define private public
#include "../../game/src/item.h"
#undef private
#include "../../game/src/packet.h"
#define SIZE(T,N) typedef char size_##T[(sizeof(T)==N)?1:-1]
#define OFFSET(T,M,N) typedef char off_##T##_##M[(offsetof(T,M)==N)?1:-1]
SIZE(TPlayerItemAttribute,3);
OFFSET(TPlayerItemAttribute,bType,0);
OFFSET(TPlayerItemAttribute,sValue,1);
SIZE(SItemLimit,5);
OFFSET(SItemLimit,bType,0);
OFFSET(SItemLimit,lValue,1);
SIZE(SItemApply,5);
OFFSET(SItemApply,bType,0);
OFFSET(SItemApply,lValue,1);
SIZE(SItemTable,154);
OFFSET(SItemTable,dwVnum,0);
OFFSET(SItemTable,szName,4);
OFFSET(SItemTable,szLocaleName,29);
OFFSET(SItemTable,bType,54);
OFFSET(SItemTable,bSubType,55);
OFFSET(SItemTable,bWeight,56);
OFFSET(SItemTable,bSize,57);
OFFSET(SItemTable,dwAntiFlags,58);
OFFSET(SItemTable,dwFlags,62);
OFFSET(SItemTable,dwWearFlags,66);
OFFSET(SItemTable,dwImmuneFlag,70);
OFFSET(SItemTable,dwGold,74);
OFFSET(SItemTable,dwShopBuyPrice,78);
OFFSET(SItemTable,aLimits,82);
OFFSET(SItemTable,aApplies,92);
OFFSET(SItemTable,alValues,107);
OFFSET(SItemTable,alSockets,131);
OFFSET(SItemTable,dwRefinedVnum,143);
OFFSET(SItemTable,wRefineSet,147);
OFFSET(SItemTable,bAlterToMagicItemPct,149);
OFFSET(SItemTable,bSpecular,150);
OFFSET(SItemTable,bGainSocketPct,151);
OFFSET(SItemTable,sAddonType,152);
SIZE(packet_item_ground_add,21);
OFFSET(packet_item_ground_add,bHeader,0);
OFFSET(packet_item_ground_add,x,1);
OFFSET(packet_item_ground_add,y,5);
OFFSET(packet_item_ground_add,z,9);
OFFSET(packet_item_ground_add,dwVID,13);
OFFSET(packet_item_ground_add,dwVnum,17);
SIZE(packet_item_ground_del,5);
OFFSET(packet_item_ground_del,bHeader,0);
OFFSET(packet_item_ground_del,dwVID,1);
SIZE(SPlayerItem,52);
OFFSET(SPlayerItem,id,0);
OFFSET(SPlayerItem,window,4);
OFFSET(SPlayerItem,pos,5);
OFFSET(SPlayerItem,count,7);
OFFSET(SPlayerItem,vnum,11);
OFFSET(SPlayerItem,alSockets,15);
OFFSET(SPlayerItem,aAttr,27);
OFFSET(SPlayerItem,owner,48);
SIZE(packet_item_update,36);
OFFSET(packet_item_update,header,0);
OFFSET(packet_item_update,pos,1);
OFFSET(packet_item_update,count,2);
OFFSET(packet_item_update,alSockets,3);
OFFSET(packet_item_update,aAttr,15);
SIZE(packet_item_set,40);
OFFSET(packet_item_set,header,0);
OFFSET(packet_item_set,pos,1);
OFFSET(packet_item_set,vnum,2);
OFFSET(packet_item_set,count,6);
OFFSET(packet_item_set,alSockets,7);
OFFSET(packet_item_set,aAttr,19);
SIZE(packet_item_set2,44);
OFFSET(packet_item_set2,header,0);
OFFSET(packet_item_set2,pos,1);
OFFSET(packet_item_set2,vnum,2);
OFFSET(packet_item_set2,count,6);
OFFSET(packet_item_set2,flags,7);
OFFSET(packet_item_set2,alSockets,11);
OFFSET(packet_item_set2,aAttr,23);
SIZE(packet_item_ownership,30);
OFFSET(packet_item_ownership,bHeader,0);
OFFSET(packet_item_ownership,dwVID,1);
OFFSET(packet_item_ownership,szName,5);

#include "../../game/src/constants.h"
SIZE(aiAccessorySocketAddPct,12);
SIZE(aiAccessorySocketEffectivePct,16);
SIZE(aiAccessorySocketDegradeTime,16);
SIZE(aiAccessorySocketPutPct,16);
int (CItem::*accessory_grade_getter)()=&CItem::GetAccessorySocketGrade;
int (CItem::*accessory_max_getter)()=&CItem::GetAccessorySocketMaxGrade;
int (CItem::*accessory_time_getter)()=&CItem::GetAccessorySocketDownGradeTime;
void (CItem::*accessory_grade_setter)(int)=&CItem::SetAccessorySocketGrade;
void (CItem::*accessory_time_setter)(DWORD)=&CItem::SetAccessorySocketDownGradeTime;
