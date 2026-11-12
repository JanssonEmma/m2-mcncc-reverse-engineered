#include "../../game/src/blend_item.cpp"
#include <cstddef>
typedef char blend_info_size[sizeof(BLEND_ITEM_INFO)==48?1:-1];
typedef char blend_values_offset[offsetof(BLEND_ITEM_INFO,apply_value)==8?1:-1];
typedef char blend_durations_offset[offsetof(BLEND_ITEM_INFO,apply_duration)==28?1:-1];
typedef char apply_name_size[sizeof(TValueName)==8?1:-1];
typedef char apply_value_offset[offsetof(TValueName,lValue)==4?1:-1];
typedef char apply_table_size[sizeof(c_aApplyTypeNames)==616?1:-1];
void CheckBlendAPI() {
    bool (*init)()=Blend_Item_init;
    bool (*load)(char*)=Blend_Item_load;
    bool (*set)(CItem*)=Blend_Item_set_value;
    bool (*find)(DWORD)=Blend_Item_find;
    long (*lookup)(char*)=FN_get_apply_type;
    int (*index)()=FN_random_index;
    (void)init;(void)load;(void)set;(void)find;(void)lookup;(void)index;
}
