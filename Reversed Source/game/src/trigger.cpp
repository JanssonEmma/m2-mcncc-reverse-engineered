#include "char.h"
#include "shop_manager.h"
#include "config.h"
#include "sectree.h"
#include "utils.h"
#include "../../libthecore/include/log.h"
#include <cstdint>
#include <cstdlib>

int OnClickShop(CHARACTER* character, CHARACTER* causer);

typedef struct STriggerFunction {
    int (*func)(CHARACTER*, CHARACTER*);
} TTriggerFunction;

TTriggerFunction OnClickTriggers[3] = {
    {0},
    {OnClickShop},
    {0},
};

// Original 0813ce60.
void CHARACTER::AssignTriggers(const TMobTable* table) {
    if (table->bOnClickType >= 3) {
        _sys_err("AssignTriggers", 31, "%s has invalid OnClick value %d",
                 GetName(), static_cast<int>(table->bOnClickType));
        std::abort();
    }
    m_triggerOnClick.bType = table->bOnClickType;
    m_triggerOnClick.pFunc = OnClickTriggers[table->bOnClickType].func;
}

int OnIdleDefault(CHARACTER* character, CHARACTER*)
{
    character->OnIdle();
    return passes_per_sec;
}

class FuncFindMobVictim
{
public:
    FuncFindMobVictim(LPCHARACTER mob, int maxDistance)
        : mob(mob), minDistance(0x7fffffff), maxDistance(maxDistance),
          x(static_cast<std::uint32_t>(mob->GetX())), y(static_cast<std::uint32_t>(mob->GetY())),
          victim(nullptr), building(nullptr)
    {
    }

    bool operator()(LPENTITY entity)
    {
        if (!entity->IsType(0))
            return false;
        LPCHARACTER target = static_cast<LPCHARACTER>(entity);
        const BYTE type = target->GetCharType();
        if (type == CHAR_TYPE_BUILDING &&
            (target->IsAffectFlag(9) || target->IsAffectFlag(10) || target->IsAffectFlag(11)))
            building = target;

        if (type != CHAR_TYPE_PC &&
            !(type == CHAR_TYPE_MONSTER && mob->IsAttackMob() && !mob->IsAggressive()))
            return false;
        if (target->IsDead() || target->IsAffectFlag(19) || target->IsAffectFlag(2) ||
            target->IsAffectFlag(28))
            return false;
        if (target->IsAffectFlag(21) && !mob->IsImmune(0x20) && mob->GetLevel() <= target->GetLevel())
            return false;
        const BYTE empire = target->GetEmpire();
        if ((empire == 1 && mob->IsNoAttackShinsu()) ||
            (empire == 2 && mob->IsNoAttackChunjo()) ||
            (empire == 3 && mob->IsNoAttackJinno()))
            return false;

        const auto dx = static_cast<std::int32_t>(x - static_cast<std::uint32_t>(target->GetX()));
        const auto dy = static_cast<std::int32_t>(y - static_cast<std::uint32_t>(target->GetY()));
        const int distance = DISTANCE_APPROX(dx, dy);
        if (distance < minDistance && distance <= maxDistance) {
            victim = target;
            minDistance = distance;
        }
        return true;
    }

    LPCHARACTER GetVictim()
    {
        if (building)
        {
            const auto doubleHP = static_cast<std::int32_t>(static_cast<std::uint32_t>(mob->GetHP()) * 2u);
            if (doubleHP > mob->GetMaxHP())
                return building;
        }
        return victim ? victim : building;
    }

private:
    LPCHARACTER mob;
    int minDistance;
    int maxDistance;
    std::uint32_t x;
    std::uint32_t y;
    LPCHARACTER victim;
    LPCHARACTER building;
};

LPCHARACTER FindVictim(LPCHARACTER mob, int maxDistance)
{
    if (!mob->GetSectree())
        return nullptr;
    FuncFindMobVictim finder(mob, maxDistance);
    mob->GetSectree()->ForEachAround(finder);
    return finder.GetVictim();
}

// Original 0813cee0.
int OnClickShop(CHARACTER* character, CHARACTER* causer) {
    CShopManager::instance().StartShopping(causer, character, 0);
    return 1;
}
