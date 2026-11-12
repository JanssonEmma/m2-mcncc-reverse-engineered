#include "char.h"
#include "item.h"
#include "dev_log.h"
#include "../../libthecore/include/log.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <strings.h>

namespace {
const char* const raceNames[] = {
    "MAIN_RACE_WARRIOR_M", "MAIN_RACE_ASSASSIN_W", "MAIN_RACE_SURA_M", "MAIN_RACE_SHAMAN_W",
    "MAIN_RACE_WARRIOR_W", "MAIN_RACE_ASSASSIN_M", "MAIN_RACE_SURA_W", "MAIN_RACE_SHAMAN_M",
    "MAIN_RACE_MAX_NUM",
};
const char* const weaponNames[] = {
    "WEAPON_SWORD", "WEAPON_DAGGER", "WEAPON_BOW", "WEAPON_TWO_HANDED", "WEAPON_BELL",
    "WEAPON_FAN", "WEAPON_ARROW", "WEAPON_MOUNT_SPEAR", "WEAPON_NUM_TYPES",
};
const char* const raceDirectories[] = {
    "data/pc/warrior", "data/pc/assassin", "data/pc/sura", "data/pc/shaman",
    "data/pc2/warrior", "data/pc2/assassin", "data/pc2/sura", "data/pc2/shaman",
};
const char* const weaponDirectories[] = {
    "onehand_sword", "dualhand_sword", "bow", "twohand_sword", "bell", "fan",
};

}

const char* FN_race_name(int race)
{
    return race >= 0 && race <= 8 ? raceNames[race] : "UNKNOWN";
}

const char* FN_weapon_type(int weapon)
{
    return weapon >= 0 && weapon <= 8 ? weaponNames[weapon] : "UNKNOWN";
}

const char* FN_race_string(int race)
{
    return race >= 0 && race < 8 ? raceNames[race] + sizeof("MAIN_RACE_") - 1 : "UNKNOWN_RACE";
}

const char* FN_weapon_string(int weapon)
{
    if (weapon == 7)
        return weaponNames[weapon];
    return weapon >= 0 && weapon < 7 ? weaponNames[weapon] + sizeof("WEAPON_") - 1 : "UNKNOWN";
}

DWORD FN_attack_speed_from_file(const char* path, int weapon)
{
    const DWORD fallback = weapon == 2 ? 1500 : 1000;
    std::FILE* file = std::fopen(path, "r");
    if (!file)
        return fallback;

    char line[1024];
    const char delimiters[] = " \t\r\n";
    std::uint32_t result = 1000;
    while (std::fgets(line, sizeof(line), file)) {
        const char* key = std::strtok(line, delimiters);
        const char* value = std::strtok(nullptr, delimiters);
        if (!key || !value || strcasecmp(key, "DirectInputTime") != 0)
            continue;

        const double milliseconds = std::strtof(value, nullptr) * 1000.0;
        result = milliseconds >= -9223372036854775808.0 && milliseconds < 9223372036854775808.0
            ? static_cast<std::uint32_t>(static_cast<std::int64_t>(milliseconds)) : 0;
        break;
    }
    std::fclose(file);
    return result ? result : fallback;
}

DWORD FN_attack_speed(const char* directory, int weapon)
{
    DWORD speed = 1500;
    if (weapon >= 6)
        return speed;
    for (int combo = 1; combo <= 4; ++combo) {
        char path[256];
        if (weapon == 2)
            std::snprintf(path, sizeof(path), "%s/bow/attack.msa", directory);
        else
            std::snprintf(path, sizeof(path), "%s/%s/combo_%02d.msa",
                          directory, weaponDirectories[weapon], combo);
        const auto speedFromFile = static_cast<std::int32_t>(FN_attack_speed_from_file(path, weapon));
        speed = static_cast<std::uint32_t>(std::min(static_cast<std::int32_t>(speed), speedFromFile));
    }
    return speed;
}


class ANI
{
public:
    ANI();

    bool load();

    bool load_one(int race, const char* directory);

    void print_attack_speed();

    DWORD attack_speed(int race, int weapon);

private:
    std::uint32_t m_speed[8][8];
};

// 080a8230
ANI::ANI()
{
    for (auto& race : m_speed) {
        for (std::uint32_t& speed : race)
            speed = 1000;
        race[2] = 1500;
    }
}

// 080a82b0
bool ANI::load()
{
    for (int race = 0; race < 8; ++race) {
        const char* directory = raceDirectories[race];
        if (!load_one(race, directory)) {
            _sys_err("load", 0xe6, "<FAIL> ANI directory = %s", directory);
            return false;
        }
    }
    return true;
}

// 080a8360
bool ANI::load_one(int race, const char* directory)
{
    if (!directory || !*directory)
        return false;
    for (int weapon = 0; weapon < 8; ++weapon) {
        m_speed[race][weapon] = FN_attack_speed(directory, weapon);
        dev_log("ani.cpp", 0xf7, "load_one", 0x800,
                const_cast<char*>("(race,weapon) = (%s,%s), speed = %u"),
                FN_race_name(race), FN_weapon_type(weapon),
                static_cast<unsigned>(m_speed[race][weapon]));
    }
    return true;
}

// 080a8500
void ANI::print_attack_speed()
{
    for (int race = 0; race < 8; ++race) {
        for (int weapon = 0; weapon < 8; ++weapon)
            std::printf("[%s][%s] = %lu\n", FN_race_string(race), FN_weapon_string(weapon),
                        static_cast<unsigned long>(attack_speed(race, weapon)));
        std::putchar('\n');
    }
}

// 080a8410
DWORD ANI::attack_speed(int race, int weapon)
{
    return race >= 0 && race < 8 && weapon >= 0 && weapon < 8 ? m_speed[race][weapon] : 1000;
}

static ANI animations;

void ani_init()
{
    animations.load();
}

DWORD ani_attack_speed(CHARACTER* character)
{
    if (!character)
        return 1000;
    LPITEM weapon = character->GetWear(4);
    if (!weapon || weapon->GetType() != 1)
        return 1000;

    const WORD race = character->GetRaceNum();
    const BYTE subtype = weapon->GetSubType();
    dev_log("ani.cpp", 0x16b, "ani_attack_speed", 0x800,
            const_cast<char*>("%s : (race,weapon) = (%s,%s) POINT_ATT_SPEED = %d"),
            character->GetName(), FN_race_name(race), FN_weapon_type(subtype),
            static_cast<int>(character->GetPoint(POINT_ATT_SPEED)));
    return animations.attack_speed(race, subtype == 3 ? 0 : subtype);
}

void ani_print_attack_speed()
{
    animations.print_attack_speed();
}
