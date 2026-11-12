#include "../../game/src/char.h"
static void (CHARACTER::*sync)(BYTE,BYTE,BYTE)=&CHARACTER::SyncQuickslot;
static void (CHARACTER::*chain)(LPITEM,BYTE,BYTE)=&CHARACTER::ChainQuickslotItem;
static LPITEM (CHARACTER::*find)(DWORD)=&CHARACTER::FindSpecifyItem;
typedef char quickslot_is_two[sizeof(TQuickslot)==2?1:-1];

void original_immune_signature() { void (CHARACTER::*setter)(DWORD)=&CHARACTER::SetImmuneFlag;(void)setter; }
