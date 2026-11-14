#include "item_manager.h"
#include "item_manager_private_types.h"
#include <algorithm>
#include <memory>
#include <utility>
#include "text_file_loader.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {
std::uint32_t ScaleDropProbability(float probability)
{
    const double scaled = static_cast<double>(probability) * 10000.0;
    constexpr double limit = 9223372036854775808.0;
    // The x87 integer-indefinite result has a zero low dword.
    if (!(scaled >= -limit && scaled < limit))
        return 0;
    return static_cast<std::uint32_t>(static_cast<std::int64_t>(scaled));
}
}

bool ITEM_MANAGER::ReadEtcDropItemFile(const char* filename)
{
    FILE* file = std::fopen(filename, "r");
    if (!file) {
        _sys_err("ReadEtcDropItemFile", 0x639, "Cannot open %s", filename);
        return false;
    }
    char line[512];
    while (std::fgets(line, sizeof(line), file)) {
        if (!line[0] || line[0] == '\n')
            continue;
        char name[257];
        std::strncpy(name, line, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        char* separator = std::strrchr(name, '\t');
        if (!separator)
            continue;
        *separator = '\0';
        const float probability = std::atof(separator + 1);
        if (!name[0] || probability == 0.0f)
            continue;

        DWORD vnum;
        if (!ITEM_MANAGER::instance().GetVnumByOriginalName(name, vnum)) {
            _sys_err("ReadEtcDropItemFile", 0x65c, "No such an item (name: %s)", name);
            std::fclose(file);
            return false;
        }
        m_map_dwEtcItemDropProb[vnum] = ScaleDropProbability(probability);
        sys_log(0, "ETC_DROP_ITEM: %s prob %f", name, static_cast<double>(probability));
    }
    std::fclose(file);
    return true;
}

std::vector<CItemDropInfo> g_vec_pkCommonDropItem[6];

bool ITEM_MANAGER::ReadCommonDropItemFile(const char* filename)
{
    FILE* file = std::fopen(filename, "r");
    if (!file) {
        _sys_err("ReadCommonDropItemFile", 0x564, "Cannot open %s", filename);
        return false;
    }

    char line[1024];
    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    while (std::fgets(line, sizeof(line), file)) {
        if (!line[0] || line[0] == '\n')
            continue;

        char* field = line;
        for (int rank = 0; rank < 4; ++rank) {
            int levelStart = 0;
            int levelEnd = 0;
            float probability = 0;
            char itemName[sizeof(TItemTable::szName)] = {};

            for (int column = 0; column < 6; ++column) {
                char* separator = std::strchr(field, '\t');
                if (!separator)
                    break;
                *separator = '\0';
                switch (column) {
                    case 1: levelStart = std::atoi(field); break;
                    case 2: levelEnd = std::atoi(field); break;
                    case 3: probability = std::atof(field); break;
                    case 4: std::strncpy(itemName, field, sizeof(itemName) - 1); break;
                    case 5: static_cast<void>(std::atoi(field)); break;
                    // Column 0 is consumed without conversion.
                }
                field = separator + 1;
            }

            const int percent = static_cast<std::int32_t>(ScaleDropProbability(probability));
            DWORD vnum;
            if (!items.GetVnumByOriginalName(itemName, vnum)) {
                vnum = static_cast<std::uint32_t>(std::atoi(itemName));
                if (!items.GetTable(vnum)) {
                    _sys_err("ReadCommonDropItemFile", 0x59e, "No such an item (name: %s)", itemName);
                    std::fclose(file);
                    return false;
                }
            }
            if (levelStart)
                g_vec_pkCommonDropItem[rank].push_back(CItemDropInfo(levelStart, levelEnd, percent, vnum));
        }
    }
    std::fclose(file);

    for (int rank = 0; rank < 6; ++rank) {
        std::vector<CItemDropInfo>& drops = g_vec_pkCommonDropItem[rank];
        std::sort(drops.begin(), drops.end());
        sys_log(1, "CommonItemDrop rank %d", rank);
        for (const CItemDropInfo& drop : drops) {
            sys_log(1, "CommonItemDrop %d %d %d %lu",
                    drop.m_iLevelStart, drop.m_iLevelEnd, drop.m_iPercent, drop.m_dwVnum);
        }
    }
    return true;
}

bool ITEM_MANAGER::ReadDropItemGroup(const char* filename)
{
    CTextFileLoader loader;
    if (!loader.Load(filename))
        return false;

    for (DWORD node = 0; node < loader.GetChildNodeCount(); ++node) {
        loader.SetChildNode(node);
        std::string name;
        loader.GetCurrentNodeName(&name);

        int groupVnum, mobVnum;
        if (!loader.GetTokenInteger("vnum", &groupVnum)) {
            _sys_err("ReadDropItemGroup", 0x796, "ReadDropItemGroup : Syntax error %s : no vnum, node %s",
                     filename, name.c_str());
            loader.SetParentNode();
            return false;
        }
        if (!loader.GetTokenInteger("mob", &mobVnum)) {
            _sys_err("ReadDropItemGroup", 0x79d, "ReadDropItemGroup : Syntax error %s : no mob vnum, node %s",
                     filename, name.c_str());
            loader.SetParentNode();
            return false;
        }
        sys_log(0, "DROP_ITEM_GROUP %s %d", name.c_str(), mobVnum);

        std::map<DWORD, CDropItemGroup*>::iterator existing = m_map_pkDropItemGroup.find(static_cast<std::uint32_t>(mobVnum));
        std::unique_ptr<CDropItemGroup> newGroup;
        if (existing == m_map_pkDropItemGroup.end())
            newGroup.reset(new CDropItemGroup(static_cast<std::uint32_t>(groupVnum), static_cast<std::uint32_t>(mobVnum), name));
        CDropItemGroup* group = newGroup ? newGroup.get() : existing->second;

        for (int row = 1; row < 256; ++row) {
            char key[4];
            std::snprintf(key, sizeof(key), "%d", row);
            TTokenVector* tokens;
            if (!loader.GetTokenVector(key, &tokens))
                break;
            if (tokens->size() < 2) {
                _sys_err("ReadDropItemGroup", 0x7c3, "Incomplete drop row %d : node %s", row, name.c_str());
                return false;
            }

            const char* itemName = (*tokens)[0].c_str();
            DWORD vnum;
            if (!GetVnumByOriginalName(itemName, vnum)) {
                vnum = static_cast<std::uint32_t>(std::atoi(itemName));
                if (!ITEM_MANAGER::instance().GetTable(vnum)) {
                    _sys_err("ReadDropItemGroup", 0x7c3, "ReadDropItemGroup : there is no item %s : node %s",
                             itemName, name.c_str());
                    return false;
                }
            }
            const float percent = std::atof((*tokens)[1].c_str());
            const DWORD probability = ScaleDropProbability(percent);
            const int count = tokens->size() > 2 ? std::atoi((*tokens)[2].c_str()) : 1;
            if (count < 1) {
                _sys_err("ReadDropItemGroup", 0x7d2, "ReadDropItemGroup : there is no count for item %s : node %s",
                         itemName, name.c_str());
                return false;
            }
            sys_log(0, "        %s %d %d", itemName, static_cast<int>(probability), count);
            group->AddItem(vnum, probability, count);
        }

        if (newGroup) {
            m_map_pkDropItemGroup.insert(std::make_pair(static_cast<std::uint32_t>(mobVnum), newGroup.get()));
            newGroup.release();
        }
        loader.SetParentNode();
    }
    return true;
}

bool ITEM_MANAGER::ReadMonsterDropItemGroup(const char* filename)
{
    CTextFileLoader loader;
    if (!loader.Load(filename))
        return false;

    for (DWORD node = 0; node < loader.GetChildNodeCount(); ++node) {
        loader.SetChildNode(node);
        std::string name, type;
        loader.GetCurrentNodeName(&name);
        int mob, killDrop = 1, levelLimit = 0;

        if (!loader.GetTokenString("type", &type)) {
            _sys_err("ReadMonsterDropItemGroup", 0x680,
                     "ReadMonsterDropItemGroup : Syntax error %s : no type (kill|drop), node %s", filename, name.c_str());
            return false;
        }
        if (!loader.GetTokenInteger("mob", &mob)) {
            _sys_err("ReadMonsterDropItemGroup", 0x687,
                     "ReadMonsterDropItemGroup : Syntax error %s : no mob vnum, node %s", filename, name.c_str());
            return false;
        }
        if (type == "kill" && !loader.GetTokenInteger("kill_drop", &killDrop)) {
            _sys_err("ReadMonsterDropItemGroup", 0x691,
                     "ReadMonsterDropItemGroup : Syntax error %s : no kill drop count, node %s", filename, name.c_str());
            return false;
        }
        if (type == "limit" && !loader.GetTokenInteger("level_limit", &levelLimit)) {
            _sys_err("ReadMonsterDropItemGroup", 0x69e,
                     "ReadmonsterDropItemGroup : Syntax error %s : no level_limit, node %s", filename, name.c_str());
            return false;
        }
        sys_log(0, "MOB_ITEM_GROUP %s [%s] %d %d", name.c_str(), type.c_str(), mob, killDrop);
        if (!killDrop) {
            loader.SetParentNode();
            continue;
        }

        std::unique_ptr<CMobItemGroup> kill;
        std::unique_ptr<CDropItemGroup> drop;
        std::unique_ptr<CLevelItemGroup> limit;
        std::unique_ptr<CBuyerThiefGlovesItemGroup> glove;
        const DWORD key = static_cast<std::uint32_t>(mob);
        if (type == "kill")
            kill.reset(new CMobItemGroup(key, killDrop, name));
        else if (type == "drop")
            drop.reset(new CDropItemGroup(0, key, name));
        else if (type == "limit")
            limit.reset(new CLevelItemGroup(levelLimit));
        else if (type == "thiefgloves")
            glove.reset(new CBuyerThiefGlovesItemGroup(0, key, name));
        else {
            _sys_err("ReadMonsterDropItemGroup", 0x776,
                     "ReadMonsterDropItemGroup : Syntax error %s : invalid type %s (kill|drop), node %s",
                     filename, type.c_str(), name.c_str());
            return false;
        }

        for (int row = 1; row < 256; ++row) {
            char key[4];
            std::snprintf(key, sizeof(key), "%d", row);
            TTokenVector* tokens;
            if (!loader.GetTokenVector(key, &tokens))
                break;
            if (tokens->size() < (kill ? 4u : 3u))
                return false;

            const char* item = (*tokens)[0].c_str();
            DWORD vnum;
            if (!GetVnumByOriginalName(item, vnum)) {
                vnum = static_cast<std::uint32_t>(std::atoi(item));
                if (!ITEM_MANAGER::instance().GetTable(vnum)) {
                    if (kill)
                        _sys_err("ReadMonsterDropItemGroup", 0x6c3,
                                 "ReadMonsterDropItemGroup : there is no item %s : node %s : vnum %d",
                                 item, name.c_str(), static_cast<int>(vnum));
                    else if (!limit)
                        _sys_err("ReadMonsterDropItemGroup", drop ? 0x6f8 : 0x759,
                                 "ReadDropItemGroup : there is no item %s : node %s", item, name.c_str());
                    return false;
                }
            }

            const int count = std::atoi((*tokens)[1].c_str());
            if (count < 1) {
                if (kill)
                    _sys_err("ReadMonsterDropItemGroup", 0x6cb,
                             "ReadMonsterDropItemGroup : there is no count for item %s : node %s : vnum %d, count %d",
                             item, name.c_str(), static_cast<int>(vnum), count);
                else if (!limit)
                    _sys_err("ReadMonsterDropItemGroup", drop ? 0x700 : 0x761,
                             "ReadMonsterDropItemGroup : there is no count for item %s : node %s", item, name.c_str());
                return false;
            }

            if (kill) {
                const int weight = std::atoi((*tokens)[2].c_str());
                if (!weight) {
                    _sys_err("ReadMonsterDropItemGroup", 0x6d4,
                             "ReadMonsterDropItemGroup : there is no drop percent for item %s : node %s : vnum %d, count %d, pct %d",
                             item, name.c_str(), static_cast<int>(vnum), count, weight);
                    return false;
                }
                const int rare = MINMAX(0, std::atoi((*tokens)[3].c_str()), 100);
                sys_log(0, "        %s count %d rare %d", item, count, rare);
                kill->AddItem(vnum, count, weight, rare);
            } else {
                const float percent = std::atof((*tokens)[2].c_str());
                const DWORD probability = ScaleDropProbability(percent);
                if (!limit)
                    sys_log(0, "        name %s pct %d count %d", item, static_cast<int>(probability), count);
                if (drop)
                    drop->AddItem(vnum, probability, count);
                else if (limit)
                    limit->AddItem(vnum, probability, count);
                else
                    glove->AddItem(vnum, probability, count);
            }
        }

        if (kill && m_map_pkMobItemGroup.insert(std::make_pair(key, kill.get())).second)
            kill.release();
        if (drop && m_map_pkDropItemGroup.insert(std::make_pair(key, drop.get())).second)
            drop.release();
        if (limit && m_map_pkLevelItemGroup.insert(std::make_pair(key, limit.get())).second)
            limit.release();
        if (glove && m_map_pkGloveItemGroup.insert(std::make_pair(key, glove.get())).second)
            glove.release();
        loader.SetParentNode();
    }
    return true;
}

bool ITEM_MANAGER::ReadSpecialDropItemFile(const char* filename)
{
    CTextFileLoader loader;
    if (!loader.Load(filename))
        return false;

    for (DWORD node = 0; node < loader.GetChildNodeCount(); ++node) {
        loader.SetChildNode(node);
        std::string groupName;
        loader.GetCurrentNodeName(&groupName);
        int groupVnum;
        if (!loader.GetTokenInteger("vnum", &groupVnum)) {
            _sys_err("ReadSpecialDropItemFile", 0x5d3,
                     "ReadSpecialDropItemFile : Syntax error %s : no vnum, node %s",
                     filename, groupName.c_str());
            loader.SetParentNode();
            return false;
        }
        sys_log(0, "DROP_ITEM_GROUP %s %d", groupName.c_str(), groupVnum);
        std::unique_ptr<CSpecialItemGroup> group(new CSpecialItemGroup(static_cast<std::uint32_t>(groupVnum)));

        for (int row = 1; row < 256; ++row) {
            char key[4];
            std::snprintf(key, sizeof(key), "%d", row);
            TTokenVector* tokens;
            if (!loader.GetTokenVector(key, &tokens))
                break;
            if (tokens->size() < 3)
                return false;

            const std::string& name = (*tokens)[0];
            DWORD vnum;
            if (!GetVnumByOriginalName(name.c_str(), vnum)) {
                if (name == "\260\346\307\350\304\241")
                    vnum = CSpecialItemGroup::EXP;
                else if (name == "mob")
                    vnum = CSpecialItemGroup::MOB;
                else if (name == "slow")
                    vnum = CSpecialItemGroup::SLOW;
                else if (name == "drain_hp")
                    vnum = CSpecialItemGroup::DRAIN_HP;
                else if (name == "poison")
                    vnum = CSpecialItemGroup::POISON;
                else if (name == "group")
                    vnum = CSpecialItemGroup::MOB_GROUP;
                else {
                    vnum = static_cast<std::uint32_t>(std::atoi(name.c_str()));
                    if (!ITEM_MANAGER::instance().GetTable(vnum)) {
                        _sys_err("ReadSpecialDropItemFile", 0x60b,
                                 "ReadSpecialDropItemFile : there is no item %s : node %s",
                                 name.c_str(), groupName.c_str());
                        return false;
                    }
                }
            }
            const int count = std::atoi((*tokens)[1].c_str());
            const int weight = std::atoi((*tokens)[2].c_str());
            const int rare = tokens->size() > 3 ? std::atoi((*tokens)[3].c_str()) : 0;
            sys_log(0, "        name %s count %d prob %d rare %d", name.c_str(), count, weight, rare);
            group->AddItem(vnum, count, weight, rare);
            if (groupVnum < 30000)
                m_ItemToSpecialGroup[vnum] = groupVnum;
        }
        loader.SetParentNode();
        if (m_map_pkSpecialItemGroup.insert(std::make_pair(static_cast<std::uint32_t>(groupVnum), group.get())).second)
            group.release();
    }
    return true;
}
