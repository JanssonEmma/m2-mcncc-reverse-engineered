#include "../../game/src/item.h"
#include "../../game/src/constants.h"
#include <cstddef>
typedef char weighted_record_size[sizeof(TItemAttrTable)==69?1:-1];
typedef char weighted_index_offset[offsetof(TItemAttrTable,dwApplyIndex)==33?1:-1];
typedef char weighted_prob_offset[offsetof(TItemAttrTable,dwProb)==37?1:-1];
typedef char weighted_values_offset[offsetof(TItemAttrTable,lValues)==41?1:-1];
typedef char weighted_set_offset[offsetof(TItemAttrTable,bMaxLevelBySet)==61?1:-1];
typedef char weighted_high_size[sizeof(aiItemMagicAttributePercentHigh)==20?1:-1];
typedef char weighted_low_size[sizeof(aiItemMagicAttributePercentLow)==20?1:-1];
void CheckWeightedAPIs() {
    void (CItem::*add)(BYTE,BYTE)=&CItem::AddAttr;
    void (CItem::*level)(BYTE)=&CItem::PutAttributeWithLevel;
    void (CItem::*table)(int*)=&CItem::PutAttribute;
    void (CItem::*magic)()=&CItem::AlterToMagicItem;
    std::map<DWORD,TItemAttrTable>* normal=&g_map_itemAttr;
    std::map<DWORD,TItemAttrTable>* rare=&g_map_itemRare;
    (void)add;(void)level;(void)table;(void)magic;(void)normal;(void)rare;
}
