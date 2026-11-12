#include <map>
#include <cstddef>
#define private public
#define protected public
#include "../../game/src/entity.h"
#include "../../game/src/horse_rider.h"
#undef private
#undef protected
#define ABI(name, expression) typedef char name[(expression) ? 1 : -1]
ABI(position_size, sizeof(PIXEL_POSITION)==12);
ABI(position_y, offsetof(PIXEL_POSITION,y)==4);
ABI(position_z, offsetof(PIXEL_POSITION,z)==8);
ABI(horse_info_size, sizeof(THorseInfo)==10);
ABI(horse_drop_time, offsetof(THorseInfo,dwHorseHealthDropTime)==6);
ABI(horse_rider_size, sizeof(CHorseRider)==24);
ABI(horse_data_offset, offsetof(CHorseRider,m_Horse)==4);
ABI(horse_regen_offset, offsetof(CHorseRider,m_eventStaminaRegen)==16);
ABI(horse_consume_offset, offsetof(CHorseRider,m_eventStaminaConsume)==20);
ABI(entity_observer_offset, offsetof(CEntity,m_bIsObserver)==4);
ABI(entity_change_offset, offsetof(CEntity,m_bObserverModeChange)==5);
ABI(entity_map_offset, offsetof(CEntity,m_map_view)==8);
// Modern libstdc++ embeds its tree sentinel, unlike GCC3.3's allocated sentinel.
// Following offsets are source-relative, not falsely asserted target-equal.
ABI(entity_index_follows_map, offsetof(CEntity,m_lMapIndex)==8+sizeof(CEntity::ENTITY_MAP));

ABI(horse_stat_size,sizeof(THorseStat)==48);
ABI(horse_stat_health,offsetof(THorseStat,iMaxHealth)==8);
ABI(horse_stat_stamina,offsetof(THorseStat,iMaxStamina)==12);
ABI(horse_stat_armor,offsetof(THorseStat,iArmor)==44);
