#include <map>
#include <set>
#include <vector>
#define protected public
#include "../../game/src/item_manager.h"
#undef protected
typedef char item_lookup_dword4[sizeof(DWORD)==4?1:-1];
typedef char item_lookup_table154[sizeof(TItemTable)==154?1:-1];
void item_lookup_signatures(){
 TItemTable* (ITEM_MANAGER::*table)(DWORD)=&ITEM_MANAGER::GetTable;
 int (ITEM_MANAGER::*number)(DWORD)=&ITEM_MANAGER::RealNumber;
 (void)table;(void)number;
}
