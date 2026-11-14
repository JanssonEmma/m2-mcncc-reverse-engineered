#include "building.h"
#include "buffer_manager.h"

#include "char.h"
#include "char_manager.h"
#include "config.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "guild.h"
#include "guild_manager.h"
#include "item_manager.h"
#include "questmanager.h"
#include "questpc.h"
#include "sectree.h"
#include "sectree_manager.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/memcpy.h"

#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <memory>
#include <utility>

namespace building {
namespace {
std::int32_t AddBuildingCoordinate(long origin, long offset)
{
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(origin) + static_cast<std::uint32_t>(offset));
}

std::int32_t SubtractBuildingCoordinate(long origin, long offset)
{
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(origin) - static_cast<std::uint32_t>(offset));
}

bool IsInsideLand(const TLand& land, std::int32_t x, std::int32_t y)
{
    const auto minX = land.x;
    if (x < minX)
        return false;
    const auto minY = land.y;
    return y >= minY && x <= AddBuildingCoordinate(minX, land.width) &&
           y <= AddBuildingCoordinate(minY, land.height);
}

std::int32_t TruncateBuildingCoordinate(long double coordinate)
{
    if (!(coordinate > -2147483649.0L && coordinate < 2147483648.0L)) {
        std::feraiseexcept(FE_INVALID);
        return INT32_MIN;
    }
    return static_cast<std::int32_t>(coordinate);
}

#pragma pack(push, 1)
struct ObjectInsertPacket {
    BYTE header;
    std::uint32_t vid;
    float angle;
    std::int32_t x, y, z;
    BYTE type;
    WORD race;
    BYTE movingSpeed, attackSpeed, state;
    std::uint32_t affect[2];
};

struct ObjectRemovePacket {
    BYTE header;
    std::uint32_t vid;
};
struct LandListHeader {
    BYTE header;
    std::uint16_t size;
};

struct LandListElement {
    std::uint32_t id;
    std::int32_t x, y, width, height;
    std::uint32_t guildId;
};
#pragma pack(pop)

static_assert(sizeof(LandListHeader) == 3, "Original land list header");
static_assert(sizeof(LandListElement) == 24, "Original land list element");

static_assert(sizeof(ObjectInsertPacket) == 35, "Original building insertion packet");
static_assert(sizeof(ObjectRemovePacket) == 5, "Original building removal packet");

LandListElement MakeLandListElement(const TLand& data)
{
    return {
        static_cast<std::uint32_t>(data.dwID), static_cast<std::int32_t>(data.x),
        static_cast<std::int32_t>(data.y), static_cast<std::int32_t>(data.width),
        static_cast<std::int32_t>(data.height), static_cast<std::uint32_t>(data.dwGuildID),
    };
}
}

CObject::CObject(TObject* data, TObjectProto* proto) : m_pProto(proto)
{
    Initialize(2);
    thecore_memcpy(&m_data, data, sizeof(m_data));
    m_chNPC = 0;
}

CObject::~CObject() { Destroy(); }

void CObject::EncodeInsertPacket(LPENTITY entity)
{
    LPDESC desc = entity->GetDesc();
    if (!desc)
        return;

    sys_log(0, "ObjectInsertPacket vid %lu vnum %lu rot %f %f %f",
            static_cast<unsigned long>(m_dwVID), static_cast<unsigned long>(m_data.dwVnum), m_data.xRot, m_data.yRot, m_data.zRot);
    ObjectInsertPacket packet = {};
    packet.header = 1;
    packet.vid = m_dwVID;
    packet.angle = m_data.zRot;
    packet.x = GetX();
    packet.y = GetY();
    packet.z = GetZ();
    packet.type = 5;
    packet.race = m_data.dwVnum;
    const auto encodeRotation = [](float rotation) -> std::uint32_t {
        constexpr float limit = 0x1p63f;
        if (!(rotation >= -limit && rotation < limit))
            return 0;
        return static_cast<std::uint32_t>(static_cast<std::int64_t>(rotation));
    };
    packet.affect[0] = encodeRotation(m_data.xRot);
    packet.affect[1] = encodeRotation(m_data.yRot);
    desc->Packet(&packet, sizeof(packet));
}

void CObject::EncodeRemovePacket(LPENTITY entity)
{
    if (LPDESC desc = entity->GetDesc()) {
        sys_log(0, "ObjectRemovePacket vid %lu", static_cast<unsigned long>(m_dwVID));
        const ObjectRemovePacket packet = {2, static_cast<std::uint32_t>(m_dwVID)};
        desc->Packet(&packet, sizeof(packet));
    }
}

void CObject::Save() {}

void CObject::SetVID(DWORD vid)
{
    m_dwVID = static_cast<std::uint32_t>(vid);
}

bool CObject::Show(long map, long x, long y)
{
    map = static_cast<std::int32_t>(map);
    x = static_cast<std::int32_t>(x);
    y = static_cast<std::int32_t>(y);
    SECTREE_MANAGER& sectors = SECTREE_MANAGER::instance();
    LPSECTREE tree = sectors.Get(map, x, y);
    if (!tree) {
        _sys_err("Show", 0x99, "cannot find sectree by %dx%d mapindex %d",
                 static_cast<int>(x), static_cast<int>(y), static_cast<int>(map));
        return false;
    }

    if (LPSECTREE current = GetSectree()) {
        current->RemoveEntity(this);
        ViewCleanup();
    }
    m_data.x = x;
    m_data.y = y;
    m_data.lMapIndex = map;
    Save();
    SetXYZ(x, y, 0);
    SetMapIndex(map);
    tree->InsertEntity(this);
    UpdateSectree();
    auto& regionManager = SECTREE_MANAGER::instance();
    const auto rotation = TruncateBuildingCoordinate(m_data.zRot);
    const auto endY = AddBuildingCoordinate(y, m_pProto->lRegion[3]);
    const auto endX = AddBuildingCoordinate(x, m_pProto->lRegion[2]);
    const auto startY = AddBuildingCoordinate(y, m_pProto->lRegion[1]);
    const auto startX = AddBuildingCoordinate(x, m_pProto->lRegion[0]);
    regionManager.ForAttrRegion(map, startX, startY, endX, endY, rotation, ATTR_OBJECT, ATTR_REGION_MODE_SET);
    return true;
}

void CObject::RegenNPC()
{
    if (!m_pProto || !m_pProto->dwNPCVnum || !m_pkLand)
        return;

    CGuild* guild = CGuildManager::instance().FindGuild(m_pkLand->GetOwner());
    if (!guild)
        return;

    const float radians = static_cast<float>(
        static_cast<long double>(m_data.zRot) * 2 * 3.1419520378112793f / 360.0f);
    const float npcX = static_cast<float>(m_pProto->lNPCX);
    const float npcY = static_cast<float>(m_pProto->lNPCY);
    const float cosX = std::cos(radians) * npcX;
    const float sinForX = std::sin(radians);
    const auto offsetX = TruncateBuildingCoordinate(cosX + static_cast<long double>(sinForX) * npcY);
    const float cosY = std::cos(radians) * npcY;
    const float sinX = std::sin(radians) * npcX;
    const auto offsetY = TruncateBuildingCoordinate(static_cast<long double>(cosY) - sinX);
    const auto z = GetZ();
    const auto y = AddBuildingCoordinate(GetY(), offsetY);
    const auto x = AddBuildingCoordinate(GetX(), offsetX);
    auto& characters = CHARACTER_MANAGER::instance();
    const auto mapIndex = GetMapIndex();
    const auto rotation = TruncateBuildingCoordinate(m_data.zRot);
    m_chNPC = characters.SpawnMob(m_pProto->dwNPCVnum, mapIndex, x, y, z, false, rotation, true);
    if (!m_chNPC) {
        _sys_err("RegenNPC", 0x11e, "Cannot create guild npc");
        return;
    }

    m_chNPC->SetGuild(guild);
    if (m_pProto->dwVnum - 14061 < 3) {
        const auto masterPid = guild->GetMasterPID();
        if (quest::PC* pc = quest::CQuestManager::instance().GetPC(masterPid)) {
            const auto guildLevel = guild->GetLevel();
            pc->SetFlag("alter_of_power.build_level", guildLevel, false);
        }
    }
}

void CObject::ApplySpecialEffect()
{
    if (!m_pProto || m_pProto->dwVnum - 14061 >= 3)
        return;

    const DWORD guildId = m_pkLand ? m_pkLand->GetOwner() : 0;
    if (CGuild* guild = CGuildManager::instance().FindGuild(guildId)) {
        const auto vnum = m_pProto->dwVnum;
        if (vnum - 14061u < 3u)
            guild->SetMemberCountBonus(static_cast<int>(vnum - 14060u) * 6);
    }
}

CLand::CLand(TLand* data)
{
    thecore_memcpy(&m_data, data, sizeof(m_data));
}

CLand::~CLand()
{
    Destroy();
}

void CLand::Destroy()
{
    auto it = m_map_pkObject.begin();
    while (it != m_map_pkObject.end()) {
        CObject* object = (it++)->second;
        CManager::instance().UnregisterObject(object);
    }
    for (const auto& entry : m_map_pkObject)
        delete entry.second;
    m_map_pkObject.clear();
    m_map_pkObjectByVID.clear();
}

CObject* CLand::FindObject(DWORD id)
{
    const auto found = m_map_pkObject.find(static_cast<std::uint32_t>(id));
    return found == m_map_pkObject.end() ? nullptr : found->second;
}

CObject* CLand::FindObjectByVnum(DWORD vnum)
{
    const auto objectVnum = static_cast<std::uint32_t>(vnum);
    for (const auto& entry : m_map_pkObject)
    {
        if (entry.second->GetVnum() == objectVnum)
            return entry.second;
    }
    return nullptr;
}

CObject* CLand::FindObjectByVID(DWORD vid)
{
    const auto found = m_map_pkObjectByVID.find(static_cast<std::uint32_t>(vid));
    return found == m_map_pkObjectByVID.end() ? nullptr : found->second;
}

void CLand::InsertObject(CObject* object)
{
    m_map_pkObject.insert(std::make_pair(object->GetID(), object));
    m_map_pkObjectByVID.insert(std::make_pair(object->GetVID(), object));
    object->SetLand(this);
}

CManager::CManager() {}
CManager::~CManager()
{
    Destroy();
}

void CManager::Destroy()
{
    auto it = m_map_pkLand.begin();
    while (it != m_map_pkLand.end()) {
        CLand* land = (it++)->second;
        delete land;
    }
    m_map_pkLand.clear();
}

void CManager::GracefulShutdown()
{
}

CLand* CManager::FindLand(DWORD id)
{
    const auto found = m_map_pkLand.find(static_cast<std::uint32_t>(id));
    return found == m_map_pkLand.end() ? 0 : found->second;
}

TObjectProto* CManager::GetObjectProto(DWORD vnum)
{
    const auto found = m_map_pkObjectProto.find(static_cast<std::uint32_t>(vnum));
    return found == m_map_pkObjectProto.end() ? 0 : found->second;
}

bool CManager::LoadLand(TLand* table)
{
    if (!map_allow_find(table->lMapIndex))
        return false;

    std::unique_ptr<CLand> land(new CLand(table));
    if (m_map_pkLand.insert(std::make_pair(land->GetID(), land.get())).second)
        land.release();
    const auto height = static_cast<unsigned long>(static_cast<std::uint32_t>(table->height));
    const auto width = static_cast<unsigned long>(static_cast<std::uint32_t>(table->width));
    const auto y = static_cast<long>(table->y);
    const auto x = static_cast<long>(table->x);
    const auto map = static_cast<long>(table->lMapIndex);
    const auto id = static_cast<unsigned long>(table->dwID);
    sys_log(0, "LAND: %lu map %ld %ldx%ld w %lu h %lu", id, map, x, y, width, height);
    return true;
}

bool CManager::LoadObjectProto(const TObjectProto* table, int size)
{
    m_vec_kObjectProto.resize(size);
    if (size > 0)
        thecore_memcpy(m_vec_kObjectProto.data(), table, sizeof(TObjectProto) * size);

    for (TObjectProto& proto : m_vec_kObjectProto) {
        const auto npc = static_cast<unsigned long>(proto.dwNPCVnum);
        const auto life = static_cast<long>(proto.lLife);
        const auto upgradeTime = static_cast<unsigned long>(proto.dwUpgradeLimitTime);
        const auto upgradeVnum = static_cast<unsigned long>(proto.dwUpgradeVnum);
        const auto price = static_cast<unsigned long>(proto.dwPrice);
        const auto objectVnum = static_cast<unsigned long>(proto.dwVnum);
        sys_log(0, "ObjectProto %lu price %lu upgrade %lu upg_limit %lu life %ld NPC %lu",
                objectVnum, price, upgradeVnum, upgradeTime, life, npc);
        for (const TObjectMaterial& material : proto.kMaterials) {
            const auto materialVnum = material.dwItemVnum;
            if (!materialVnum)
                break;
            if (!ITEM_MANAGER::instance().GetTable(materialVnum)) {
                _sys_err("LoadObjectProto", 0x265, "          mat: ERROR!! no item by vnum %lu", static_cast<unsigned long>(material.dwItemVnum));
                return false;
            }
            const auto count = static_cast<unsigned long>(material.dwCount);
            const auto vnum = static_cast<unsigned long>(material.dwItemVnum);
            sys_log(0, "          mat: %lu %lu", vnum, count);
        }
        m_map_pkObjectProto.insert(std::make_pair(proto.dwVnum, &proto));
    }
    return true;
}

bool CManager::LoadObject(TObject* table, bool boot)
{
    CLand* land = FindLand(table->dwLandID);
    if (!land) {
        sys_log(0, "Cannot find land by id %lu", static_cast<unsigned long>(table->dwLandID));
        return false;
    }

    TObjectProto* proto = GetObjectProto(table->dwVnum);
    if (!proto) {
        const auto id = static_cast<unsigned long>(table->dwID);
        const auto vnum = static_cast<unsigned long>(table->dwVnum);
        _sys_err("LoadObject", 0x2ff, "Cannot find object %lu in prototype (id %lu)", vnum, id);
        return false;
    }

    const auto logY = static_cast<long>(table->y);
    const auto logX = static_cast<long>(table->x);
    const auto logMap = static_cast<long>(table->lMapIndex);
    const auto logVnum = static_cast<unsigned long>(table->dwVnum);
    const auto logId = static_cast<unsigned long>(table->dwID);
    sys_log(0, "OBJ: id %lu vnum %lu map %ld pos %ldx%ld", logId, logVnum, logMap, logX, logY);
    CObject* object = new CObject(table, proto);
    const DWORD vid = CHARACTER_MANAGER::instance().AllocVID();
    object->SetVID(vid);
    m_map_pkObjByVID.insert(std::make_pair(vid, object));
    m_map_pkObjByID.insert(std::make_pair(table->dwID, object));
    land->InsertObject(object);

    if (boot) {
        object->SetMapIndex(table->lMapIndex);
        object->SetXYZ(table->x, table->y, 0);
    } else {
        const auto y = table->y;
        const auto x = table->x;
        const auto map = table->lMapIndex;
        object->Show(map, x, y);
        if (proto->dwNPCVnum)
            object->RegenNPC();
        object->ApplySpecialEffect();
    }
    return true;
}

void CObject::Destroy()
{
    if (const auto* proto = m_pProto) {
        const auto y = GetY();
        const auto maxY = AddBuildingCoordinate(y, proto->lRegion[3]);
        const auto endOffsetX = proto->lRegion[2];
        const auto x = GetX();
        const auto maxX = AddBuildingCoordinate(x, endOffsetX);
        const auto minY = AddBuildingCoordinate(y, proto->lRegion[1]);
        const auto minX = AddBuildingCoordinate(x, proto->lRegion[0]);
        auto& sectors = SECTREE_MANAGER::instance();
        const auto mapIndex = GetMapIndex();
        const auto rotation = TruncateBuildingCoordinate(m_data.zRot);
        sectors.ForAttrRegion(mapIndex, minX, minY, maxX, maxY,
            rotation, ATTR_OBJECT, ATTR_REGION_MODE_REMOVE);
    }
    CEntity::Destroy();
    if (LPSECTREE sector = GetSectree())
        sector->RemoveEntity(this);
    if (CHARACTER* npc = m_chNPC)
        CHARACTER_MANAGER::instance().DestroyCharacter(npc);
    RemoveSpecialEffect();
}

void CObject::RemoveSpecialEffect()
{
    if (!m_pProto || m_pProto->dwVnum - 14061 >= 3)
        return;

    const DWORD guildId = m_pkLand ? m_pkLand->GetOwner() : 0;
    if (CGuild* guild = CGuildManager::instance().FindGuild(guildId))
        guild->SetMemberCountBonus(0);
}

const TLand& CLand::GetData()
{
    return m_data;
}

void CLand::PutData(const TLand* data)
{
    thecore_memcpy(&m_data, data, sizeof(m_data));
    if (!m_data.dwGuildID)
        return;

    const TMapRegion* region = SECTREE_MANAGER::instance().GetMapRegion(m_data.lMapIndex);
    if (!region)
        return;

    CHARACTER_MANAGER& manager = CHARACTER_MANAGER::instance();
    CHARACTER_SET* landMasters = manager.GetCharacterSetByRaceNum(20040);
    for (auto it = landMasters->begin(); it != landMasters->end();) {
        CHARACTER* character = *it++;
        if (character->GetMapIndex() != m_data.lMapIndex)
            continue;

        const auto x = SubtractBuildingCoordinate(character->GetX(), region->sx);
        const auto y = SubtractBuildingCoordinate(character->GetY(), region->sy);
        const auto minX = m_data.x;
        if (x > AddBuildingCoordinate(minX, m_data.width) || x < minX)
            continue;
        const auto minY = m_data.y;
        if (y <= AddBuildingCoordinate(minY, m_data.height) && y >= minY)
            manager.DestroyCharacter(character);
    }
}

void CLand::DeleteObject(DWORD id)
{
    const auto objectId = static_cast<std::uint32_t>(id);
    CObject* object = FindObject(objectId);
    if (!object)
        return;

    sys_log(0, "Land::DeleteObject %lu", static_cast<unsigned long>(objectId));
    CManager::instance().UnregisterObject(object);
    m_map_pkObject.erase(objectId);
    m_map_pkObjectByVID.erase(object->GetVID());
    delete object;
}

CObject* CManager::FindObjectByVID(DWORD vid)
{
    const auto object = m_map_pkObjByVID.find(static_cast<std::uint32_t>(vid));
    return object == m_map_pkObjByVID.end() ? nullptr : object->second;
}

void CManager::UnregisterObject(CObject* object)
{
    m_map_pkObjByID.erase(object->GetID());
    m_map_pkObjByVID.erase(object->GetVID());
}

void CManager::DeleteObject(DWORD id)
{
    const auto objectId = static_cast<std::uint32_t>(id);
    sys_log(0, "OBJ_DEL: %lu", static_cast<unsigned long>(objectId));
    const auto found = m_map_pkObjByID.find(objectId);
    if (found != m_map_pkObjByID.end())
        found->second->GetLand()->DeleteObject(objectId);
}

void CManager::UpdateLand(TLand* table)
{
    CLand* land = FindLand(table->dwID);
    if (!land) {
        _sys_err("UpdateLand", 0x290, "cannot find land by id %lu", static_cast<unsigned long>(table->dwID));
        return;
    }

    land->PutData(table);
    const auto& clients = DESC_MANAGER::instance().GetClientSet();
    auto client = clients.begin();
    const LandListHeader header = {0x82, sizeof(LandListHeader) + sizeof(LandListElement)};
    const LandListElement element = MakeLandListElement(*table);
    sys_log(0, "BUILDING: UpdateLand %lu pos %dx%d guild %lu", static_cast<unsigned long>(table->dwID),
            static_cast<int>(element.x), static_cast<int>(element.y), static_cast<unsigned long>(table->dwGuildID));
    while (client != clients.end()) {
        DESC* desc = *client++;
        CHARACTER* character = desc->GetCharacter();
        if (character && static_cast<std::int32_t>(character->GetMapIndex()) == table->lMapIndex) {
            desc->BufferedPacket(&header, sizeof(header));
            desc->Packet(&element, sizeof(element));
        }
    }
}

CLand* CManager::FindLand(long map, long x, long y)
{
    const long mapIndex = static_cast<std::int32_t>(map);
    sys_log(0, "BUILDING: FindLand %ld %ld %ld", mapIndex,
            static_cast<long>(static_cast<std::int32_t>(x)),
            static_cast<long>(static_cast<std::int32_t>(y)));
    const TMapRegion* region = SECTREE_MANAGER::instance().GetMapRegion(mapIndex);
    if (!region)
        return 0;

    const auto relativeX = SubtractBuildingCoordinate(x, region->sx);
    const auto relativeY = SubtractBuildingCoordinate(y, region->sy);

    auto it = m_map_pkLand.begin();
    while (it != m_map_pkLand.end()) {
        CLand* land = (it++)->second;
        const TLand& data = land->GetData();
        if (data.lMapIndex == mapIndex && IsInsideLand(data, relativeX, relativeY))
            return land;
    }
    return 0;
}

void CManager::SendLandList(LPDESC desc, long map)
{
    const auto mapIndex = static_cast<std::int32_t>(map);
    TEMP_BUFFER buffer;
    WORD count = 0;
    auto it = m_map_pkLand.begin();
    while (it != m_map_pkLand.end()) {
        CLand* land = (it++)->second;
        const TLand& data = land->GetData();
        if (data.lMapIndex != mapIndex)
            continue;

        const LandListElement element = MakeLandListElement(data);
        buffer.write(&element, sizeof(element));
        ++count;
    }

    const int loggedSize = buffer.size();
    sys_log(0, "SendLandList map %ld count %u elem_size: %d", static_cast<long>(mapIndex), static_cast<unsigned>(count), loggedSize);
    if (count) {
        const LandListHeader header = {0x82, static_cast<std::uint16_t>(sizeof(LandListHeader) + buffer.size())};
        desc->BufferedPacket(&header, sizeof(header));
        const int payloadSize = buffer.size();
        desc->Packet(buffer.read_peek(), payloadSize);
    }
}

void CLand::RequestUpdate(DWORD guild)
{
    const std::uint32_t packet[] = {static_cast<std::uint32_t>(m_data.dwID), static_cast<std::uint32_t>(guild)};
    db_clientdesc->DBPacket(0x3e, 0, packet, sizeof(packet));
    sys_log(0, "RequestUpdate id %lu guild %lu", static_cast<unsigned long>(packet[0]), static_cast<unsigned long>(packet[1]));
}

void CLand::ClearLand()
{
    for (const auto& object : m_map_pkObject)
        RequestDeleteObject(object.second->GetID());
    SetOwner(0);

    const TLand& data = GetData();
    const TMapRegion* region = SECTREE_MANAGER::instance().GetMapRegion(data.lMapIndex);
    if (region) {
        auto& characters = CHARACTER_MANAGER::instance();
        const auto y = AddBuildingCoordinate(AddBuildingCoordinate(region->sy, data.y), data.height / 2);
        const auto x = AddBuildingCoordinate(AddBuildingCoordinate(region->sx, data.x), data.width / 2);
        characters.SpawnMob(20040, data.lMapIndex, x, y, 0, false, -1, true);
    }
}

void CManager::ClearLandByGuildID(DWORD guild)
{
    if (CLand* land = FindLandByGuild(guild))
    {
        land->ClearLand();
        sys_log(0, "LAND_CLEAR: request Land Clear. LandID: %d", static_cast<int>(land->GetID()));
    }
    else
        sys_log(0, "LAND_CLEAR: there is no GUILD id like %d", static_cast<int>(guild));
}

void CLand::SetOwner(DWORD guild)
{
    const auto owner = static_cast<std::uint32_t>(guild);
    if (m_data.dwGuildID != owner) {
        m_data.dwGuildID = owner;
        RequestUpdate(owner);
    }
}

CLand* CManager::FindLandByGuild(DWORD guild)
{
    const auto guildId = static_cast<std::uint32_t>(guild);
    auto it = m_map_pkLand.begin();
    while (it != m_map_pkLand.end()) {
        CLand* land = (it++)->second;
        if (land->GetData().dwGuildID == guildId)
            return land;
    }
    return 0;
}

CObject* CLand::FindObjectByNPC(CHARACTER* npc)
{
    if (npc) {
        for (const auto& entry : m_map_pkObject) {
            if (entry.second->GetNPC() == npc)
                return entry.second;
        }
    }
    return 0;
}

CObject* CLand::FindObjectByGroup(DWORD group)
{
    const auto groupId = static_cast<std::uint32_t>(group);
    for (const auto& entry : m_map_pkObject)
    {
        CObject* object = entry.second;
        if (object->GetGroup() == groupId)
            return object;
    }
    return nullptr;
}

void CLand::RequestDeleteObjectByVID(DWORD vid)
{
    const auto objectVid = static_cast<std::uint32_t>(vid);
    CObject* object = FindObjectByVID(objectVid);
    if (!object)
    {
        _sys_err("RequestDeleteObjectByVID", 0x21f, "no object by vid %lu", static_cast<unsigned long>(objectVid));
        return;
    }
    const std::uint32_t id = object->GetID();
    db_clientdesc->DBPacket(0x3d, 0, &id, sizeof(id));
    sys_log(0, "RequestDeleteObject vid %lu id %lu", static_cast<unsigned long>(objectVid), static_cast<DWORD>(id));
}

void CManager::ClearLand(DWORD id)
{
    CLand* land = FindLand(id);
    if (land)
    {
        land->ClearLand();
        sys_log(0, "LAND_CLEAR: request Land Clear. LandID: %d", static_cast<int>(land->GetID()));
    }
    else
        sys_log(0, "LAND_CLEAR: there is no LAND id like %d", static_cast<int>(id));
}

bool CLand::RequestCreateWall(long map, float rotation)
{
    const long x = AddBuildingCoordinate(m_data.x, m_data.width / 2);
    const long y = AddBuildingCoordinate(m_data.y, m_data.height / 2);
    constexpr long offset = 1362;
    if (rotation == 0 || rotation == 180)
    {
        const long delta = rotation == 0 ? offset : -offset;
        const auto gate = AddBuildingCoordinate(y, delta);
        RequestCreateObject(14201, map, x, gate, x, gate, rotation == 0 ? 0.0f : 180.0f, false);
        RequestCreateObject(14202, map, x, SubtractBuildingCoordinate(y, delta), x, gate, 0, false);
        RequestCreateObject(14203, map, SubtractBuildingCoordinate(x, 1000), y, x, gate, 0, false);
        RequestCreateObject(14204, map, AddBuildingCoordinate(x, 1000), y, x, gate, 0, false);
    }
    else if (rotation == 90 || rotation == 270)
    {
        const long delta = rotation == 90 ? offset : -offset;
        const auto gate = AddBuildingCoordinate(x, delta);
        RequestCreateObject(14201, map, gate, y, gate, y, 90, false);
        RequestCreateObject(14202, map, SubtractBuildingCoordinate(x, delta), y, gate, y, 90, false);
        RequestCreateObject(14203, map, x, SubtractBuildingCoordinate(y, 1000), gate, y, 90, false);
        RequestCreateObject(14204, map, x, AddBuildingCoordinate(y, 1000), gate, y, 90, false);
    }
    if (test_server)
    {
        const long bottomLeftY = AddBuildingCoordinate(m_data.y, 50);
        const long bottomLeftX = AddBuildingCoordinate(m_data.x, 50);
        RequestCreateObject(14200, map, bottomLeftX, bottomLeftY, 0, 0, 0, false);

        const long bottomRightY = AddBuildingCoordinate(m_data.y, 50);
        const long bottomRightX = AddBuildingCoordinate(m_data.x, AddBuildingCoordinate(m_data.width, -50));
        RequestCreateObject(14200, map, bottomRightX, bottomRightY, 0, 0, 90, false);

        const long topRightY = AddBuildingCoordinate(m_data.y, AddBuildingCoordinate(m_data.height, -50));
        const long topRightX = AddBuildingCoordinate(m_data.x, AddBuildingCoordinate(m_data.width, -50));
        RequestCreateObject(14200, map, topRightX, topRightY, 0, 0, 180, false);

        const long topLeftY = AddBuildingCoordinate(m_data.y, AddBuildingCoordinate(m_data.height, -50));
        const long topLeftX = AddBuildingCoordinate(m_data.x, 50);
        RequestCreateObject(14200, map, topLeftX, topLeftY, 0, 0, 270, false);
    }
    return true;
}

void CLand::RequestDeleteWall()
{
    for (auto it = m_map_pkObject.begin(); it != m_map_pkObject.end(); ++it)
    {
        CObject* object = it->second;
        const DWORD id = object->GetID();
        const DWORD vnum = object->m_pProto ? object->m_pProto->dwVnum : 0;
        if (vnum >= 14201 && vnum <= 14204)
            RequestDeleteObject(id);
        if (test_server && vnum == 14200)
            RequestDeleteObject(id);
    }
}

void CLand::DrawWall(DWORD vnum, long map, long& x, long& y, char length, float rotation)
{
    const int angle = (TruncateBuildingCoordinate(rotation) % 360 / 90) * 90;
    long dx = 0;
    long dy = 0;
    switch (angle)
    {
        case 0: dx = -500; break;
        case 90: dy = 500; break;
        case 180: dx = 500; break;
        case 270: dy = -500; break;
    }
    const int blockCount = static_cast<std::int8_t>(length);
    if (blockCount <= 0)
        return;
    const float wallAngle = static_cast<float>(angle);
    for (int block = 0; block < blockCount; ++block)
    {
        const long wallY = y;
        const long wallX = x;
        RequestCreateObject(vnum, map, wallX, wallY, 0, 0, wallAngle, false);
        x = AddBuildingCoordinate(x, dx);
        y = AddBuildingCoordinate(y, dy);
    }
}

bool CLand::RequestCreateWallBlocks(DWORD vnum, long map, char wallSize,
                                     bool east, bool west, bool south, bool north)
{
    const auto baseVnum = static_cast<std::uint32_t>(vnum);
    const int size = static_cast<std::int8_t>(wallSize);
    const int offset = size * 500 + 1300;
    long x = AddBuildingCoordinate(m_data.x, m_data.width / 2 - offset);
    long y = AddBuildingCoordinate(m_data.y, m_data.height / 2 + offset);
    const bool doors[] = {west, north, east, south};
    const int blocks = std::clamp(size, 0, 3);

    for (int side = 0; side < 4; ++side)
    {
        const float rotation = static_cast<float>(270 - side * 90);
        const int direction = side == 0 || side == 3 ? -1 : 1;
        long& axis = side % 2 ? x : y;
        RequestCreateObject(baseVnum - 4u, map, x, y, 0, 0, rotation, false);
        axis = AddBuildingCoordinate(axis, direction * 700);
        if (doors[side])
        {
            DrawWall(baseVnum - 3u, map, x, y, static_cast<char>(blocks), rotation);
            axis = AddBuildingCoordinate(axis, direction * 700);
            RequestCreateObject(baseVnum - 1u, map, x, y, 0, 0, rotation, false);
            axis = AddBuildingCoordinate(axis, direction * 1300);
            DrawWall(baseVnum - 3u, map, x, y, static_cast<char>(blocks), rotation);
        }
        else
            DrawWall(baseVnum - 3u, map, x, y, static_cast<char>(blocks * 2 + 4), rotation);
        axis = AddBuildingCoordinate(axis, direction * 100);
    }
    return true;
}

void CLand::RequestDeleteWallBlocks(DWORD vnum)
{
    const auto baseVnum = static_cast<std::uint32_t>(vnum);
    for (auto it = m_map_pkObject.begin(); it != m_map_pkObject.end(); ++it)
    {
        CObject* object = it->second;
        const DWORD objectVnum = object->m_pProto ? object->m_pProto->dwVnum : 0;
        if (objectVnum == baseVnum - 4u || objectVnum == baseVnum - 3u || objectVnum == baseVnum - 1u)
            RequestDeleteObject(object->GetID());
    }
}

void CLand::RequestDeleteObject(DWORD id)
{
    const auto packet = static_cast<std::uint32_t>(id);
    if (!FindObject(packet)) {
        _sys_err("RequestDeleteObject", 0x211, "no object by id %lu", static_cast<unsigned long>(packet));
        return;
    }
    db_clientdesc->DBPacket(0x3d, 0, &packet, sizeof(packet));
    sys_log(0, "RequestDeleteObject id %lu", static_cast<unsigned long>(packet));
}

bool CLand::RequestCreateObject(DWORD vnum, long map, long x, long y,
                                float xRot, float yRot, float zRot, bool checkAnother)
{
    vnum = static_cast<std::uint32_t>(vnum);
    map = static_cast<std::int32_t>(map);
    x = static_cast<std::int32_t>(x);
    y = static_cast<std::int32_t>(y);
    SECTREE_MANAGER& sectors = SECTREE_MANAGER::instance();
    const TObjectProto* proto = CManager::instance().GetObjectProto(vnum);
    if (!proto) {
        _sys_err("RequestCreateObject", 0x1d1, "Invalid Object vnum %lu", vnum);
        return false;
    }

    const TMapRegion* region = sectors.GetMapRegion(map);
    if (!region)
        return false;

    sys_log(0, "RequestCreateObject(vnum=%lu, map=%ld, pos=(%ld,%ld), rot=(%.1f,%.1f,%.1f) region(%d,%d ~ %d,%d)",
            vnum, map, x, y, xRot, yRot, zRot, region->sx, region->sy, region->ex, region->ey);
    const auto worldX = AddBuildingCoordinate(region->sx, x);
    const auto worldY = AddBuildingCoordinate(region->sy, y);
    const auto landX = AddBuildingCoordinate(region->sx, m_data.x);
    const auto landY = AddBuildingCoordinate(region->sy, m_data.y);
    const auto landEndX = AddBuildingCoordinate(landX, m_data.width);
    const auto landEndY = AddBuildingCoordinate(landY, m_data.height);
    const auto startX = AddBuildingCoordinate(worldX, proto->lRegion[0]);
    const auto startY = AddBuildingCoordinate(worldY, proto->lRegion[1]);
    const auto endX = AddBuildingCoordinate(worldX, proto->lRegion[2]);
    const auto endY = AddBuildingCoordinate(worldY, proto->lRegion[3]);
    const float radians = static_cast<float>(static_cast<long double>(zRot) * 2 * 3.141592025756836f / 360.0f);
    const float cosAngle = std::cos(radians);
    const float sinAngle = std::sin(radians);
    const auto rotate = [](float first, float second) {
        return static_cast<long double>(first) + second;
    };
    const float minOffsetX = rotate(cosAngle * proto->lRegion[0], sinAngle * proto->lRegion[1]);
    const float minOffsetY = rotate(-sinAngle * proto->lRegion[0], cosAngle * proto->lRegion[1]);
    const long double maxOffsetX = rotate(cosAngle * proto->lRegion[2], sinAngle * proto->lRegion[3]);
    const long double maxOffsetY = rotate(-sinAngle * proto->lRegion[2], cosAngle * proto->lRegion[3]);
    const long double baseX = static_cast<float>(worldX);
    const long double baseY = static_cast<float>(worldY);

    const auto minX = TruncateBuildingCoordinate(baseX + minOffsetX);
    const auto minY = TruncateBuildingCoordinate(baseY + minOffsetY);
    const auto maxX = TruncateBuildingCoordinate(baseX + maxOffsetX);
    const auto maxY = TruncateBuildingCoordinate(baseY + maxOffsetY);
    if (minX < landX || maxX > landEndX || minY < landY || maxY > landEndY) {
        _sys_err("RequestCreateObject", 0x1f2,
                 "invalid position: object is outside of land region\nLAND: %d %d ~ %d %d\nOBJ: %d %d ~ %d %d",
                 static_cast<int>(landX), static_cast<int>(landY), static_cast<int>(landEndX),
                 static_cast<int>(landEndY), static_cast<int>(startX), static_cast<int>(startY),
                 static_cast<int>(endX), static_cast<int>(endY));
        return false;
    }

    if (checkAnother && sectors.ForAttrRegion(map, startX, startY, endX, endY,
                                              TruncateBuildingCoordinate(zRot), ATTR_OBJECT, ATTR_REGION_MODE_CHECK)) {
        _sys_err("RequestCreateObject", 0x1f9, "another object already exist");
        return false;
    }

    const TPacketGDCreateObject packet = {
        static_cast<std::uint32_t>(vnum), static_cast<std::uint32_t>(m_data.dwID),
        static_cast<std::int32_t>(map), static_cast<std::int32_t>(worldX), static_cast<std::int32_t>(worldY),
        xRot, yRot, zRot,
    };
    db_clientdesc->DBPacket(0x3c, 0, &packet, sizeof(packet));
    return true;
}

void CObject::Reconstruct(DWORD vnum)
{
    const TMapRegion* region = SECTREE_MANAGER::instance().GetMapRegion(m_data.lMapIndex);
    if (region) {
        CLand* land = m_pkLand;
        land->RequestDeleteObject(m_data.dwID);
        const float zRotation = m_data.zRot;
        const float yRotation = m_data.yRot;
        const float xRotation = m_data.xRot;
        const auto y = SubtractBuildingCoordinate(m_data.y, region->sy);
        const auto x = SubtractBuildingCoordinate(m_data.x, region->sx);
        land->RequestCreateObject(vnum, m_data.lMapIndex, x, y, xRotation, yRotation, zRotation, false);
    }
}

void CManager::FinalizeBoot()
{
    for (auto it = m_map_pkObjByID.begin(); it != m_map_pkObjByID.end();) {
        CObject* object = (it++)->second;
        const auto x = object->GetX();
        const auto map = object->GetMapIndex();
        const auto y = object->GetY();
        object->Show(map, x, y);
        object->RegenNPC();
        object->ApplySpecialEffect();
    }

    sys_log(0, "FinalizeBoot");
    for (auto it = m_map_pkLand.begin(); it != m_map_pkLand.end();) {
        const TLand& land = (it++)->second->GetData();
        sys_log(0, "LandMaster map_index=%d pos=(%d, %d)",
                static_cast<int>(land.lMapIndex), static_cast<int>(land.x), static_cast<int>(land.y));
        if (land.dwGuildID || !map_allow_find(land.lMapIndex))
            continue;
        const TMapRegion* region = SECTREE_MANAGER::instance().GetMapRegion(land.lMapIndex);
        if (region) {
            auto& characters = CHARACTER_MANAGER::instance();
            const auto y = AddBuildingCoordinate(AddBuildingCoordinate(region->sy, land.y), land.height / 2);
            const auto x = AddBuildingCoordinate(AddBuildingCoordinate(region->sx, land.x), land.width / 2);
            characters.SpawnMob(20040, land.lMapIndex, x, y, 0, false, -1, true);
        }
    }
}
}
