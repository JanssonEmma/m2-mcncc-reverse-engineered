#include "../../common/length.h"
#include "../../common/tables.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"
#include "config.h"
#include "constants.h"
#include "packet.h"
#include <cstdint>
#include "desc_client.h"
#include "desc_manager.h"
#include "locale.hpp"
#include "char.h"
#include "priv_manager.h"
#include "arena.h"
#include "building.h"
#include "dungeon.h"
#include "log.h"
#include "marriage.h"
#include "messenger_manager.h"
#include "OXEvent.h"
#include "party.h"
#include "pvp.h"
#include "sectree_manager.h"
#include "utils.h"
#include "war_map.h"
#include "wedding.h"
#include "locale_service.h"
#include "input.h"
#include "block_country.h"
#include "buffer_manager.h"
#include "guild_manager.h"
#include "dev_log.h"
#include <cstring>

static_assert(sizeof(TPlayerLoadPacket) == 9, "player load request");
static_assert(sizeof(TPlayerDeletePacket) == 44, "player delete request");
static_assert(sizeof(TEmpireSelectPacket) == 5, "empire selection request");
static_assert(sizeof(TPacketGDChangeName) == 29, "rename request");

// Original login privilege notices.
static void _send_bonus_info(CHARACTER* character)
{
    const char* messages[] = {
        "\276\306\300\314\305\333\040\265\345\267\323\267\374\040\040\045\144\045\045\040\303\337\260\241\040\300\314\272\245\306\256\040\301\337\300\324\264\317\264\331\056",
        "\260\361\265\345\040\265\345\267\323\267\374\040\045\144\045\045\040\303\337\260\241\040\300\314\272\245\306\256\040\301\337\300\324\264\317\264\331\056",
        "\264\353\271\332\260\361\265\345\040\265\345\267\323\267\374\040\045\144\045\045\040\303\337\260\241\040\300\314\272\245\306\256\040\301\337\300\324\264\317\264\331\056",
        "\260\346\307\350\304\241\040\045\144\045\045\040\303\337\260\241\040\310\271\265\346\040\300\314\272\245\306\256\040\301\337\300\324\264\317\264\331\056"
    };
    int bonuses[4];
    for (BYTE type = 1; type <= 4; ++type)
        bonuses[type - 1] = CPrivManager::instance().GetPriv(character, type);
    for (unsigned index = 0; index < 4; ++index) {
        if (bonuses[index])
            character->ChatPacket(2, locale_find(messages[index], g_iUseLocale), bonuses[index]);
    }
}

void CInputLogin::CharacterSelect(DESC* desc, const char* data)
{
    const auto& account = desc->GetAccountTable();
    const BYTE slot = static_cast<BYTE>(data[1]);
    sys_log(0, "player_select: login: %s index: %d", account.login, slot);
    if (!account.id) {
        sys_err("no account table");
        return;
    }
    if (slot >= 4) {
        sys_err("index overflow %d, login: %s", slot, account.login);
        return;
    }
    const auto& player = account.players[slot];
    if (player.bChangeName) {
        sys_err("name must be changed idx %d, login %s, name %s", slot, account.login, player.szName);
        return;
    }

    const TPlayerLoadPacket packet = {
        static_cast<std::uint32_t>(account.id), static_cast<std::uint32_t>(player.dwID), slot
    };
    db_clientdesc->DBPacket(0x03, desc->GetHandle(), &packet, sizeof(packet));
}

void CInputLogin::CharacterDelete(DESC* desc, const char* data)
{
    const auto& account = desc->GetAccountTable();
    if (!account.id) {
        sys_err("PlayerDelete: no login data");
        return;
    }
    const BYTE slot = static_cast<BYTE>(data[1]);
    TPlayerDeletePacket packet = {};
    std::strncpy(packet.private_code, data + 2, sizeof(packet.private_code) - 1);
    sys_log(0, "PlayerDelete: login: %s index: %d , social_id %s", account.login, slot, packet.private_code);
    if (slot >= 4) {
        sys_err("PlayerDelete: index overflow %d, login: %s", slot, account.login);
        return;
    }
    if (!account.players[slot].dwID) {
        sys_err("PlayerDelete: Wrong Social ID index %d, login: %s", slot, account.login);
        const BYTE failure = 0x0b;
        desc->Packet(&failure, sizeof(failure));
        return;
    }

    trim_and_lower(packet.login, account.login, 30);
    packet.player_id = static_cast<std::uint32_t>(account.players[slot].dwID);
    packet.account_index = slot;
    db_clientdesc->DBPacket(0x06, desc->GetHandle(), &packet, sizeof(packet));
}

void CInputLogin::Empire(DESC* desc, const char* data)
{
    const auto& account = desc->GetAccountTable();
    if (account.bEmpire) {
        for (const auto& player : account.players) {
            if (player.dwID) {
                sys_err("EmpireSelectFailed %u", static_cast<unsigned>(player.dwID));
                return;
            }
        }
    }

    const TEmpireSelectPacket packet = {
        static_cast<std::uint32_t>(account.id), static_cast<BYTE>(data[1])
    };
    sys_log(0, "EmpireSelect");
    db_clientdesc->DBPacket(0x0f, desc->GetHandle(), &packet, sizeof(packet));
}

void CInputLogin::ChangeName(DESC* desc, const char* data)
{
    const auto& account = desc->GetAccountTable();
    if (!account.id) {
        sys_err("no account table");
        return;
    }
    const BYTE slot = static_cast<BYTE>(data[1]);
    if (slot >= 4 || !account.players[slot].bChangeName)
        return;

    TPacketGDChangeName packet = {};
    std::strncpy(packet.name, data + 2, sizeof(packet.name) - 1);
    if (!check_name(packet.name)) {
        const BYTE failure[] = {0x09, 0};
        desc->Packet(failure, sizeof(failure));
        return;
    }
    packet.pid = static_cast<std::uint32_t>(account.players[slot].dwID);
    db_clientdesc->DBPacket(0x2c, desc->GetHandle(), &packet, sizeof(packet));
}

void CInputLogin::Login(DESC* desc, const char* data)
{
    struct LoginRequest {
        char login[31];
        char password[17];
    };
    static_assert(sizeof(LoginRequest) == 48, "password login request");

    LoginRequest request = {};
    char login[31] = {};
    std::memcpy(login, data + 1, sizeof(login) - 1);
    trim_and_lower(request.login, login, 30);
    sys_log(0, "InputLogin::Login : %s", request.login);

    const char* status = nullptr;
    if (g_iUseLocale && !test_server)
        status = "VERSION";
    else if (g_bNoMoreClient)
        status = "SHUTDOWN";
    else if (g_iUserLimit) {
        int total, localCount;
        int* empireCounts;
        DESC_MANAGER::instance().GetUserCount(total, &empireCounts, localCount);
        if (static_cast<unsigned>(g_iUserLimit) <= static_cast<unsigned>(total))
            status = "FULL";
    }
    if (status) {
        struct LoginFailure {
            BYTE header;
            char status[9];
        } packet = {0x07, {}};
        std::strncpy(packet.status, status, sizeof(packet.status) - 1);
        desc->Packet(&packet, sizeof(packet));
        return;
    }

    std::strncpy(request.password, data + 32, sizeof(request.password) - 1);
    db_clientdesc->DBPacket(0x01, desc->GetHandle(), &request, sizeof(request));
}

// 080f3390
void CInputLogin::Entergame(DESC* desc, const char*)
{
    CHARACTER* character = desc->GetCharacter();
    if (!character) {
        desc->SetPhase(0);
        return;
    }

    auto& sectors = SECTREE_MANAGER::instance();
    PIXEL_POSITION position = character->GetXYZ();
    const long map = character->GetMapIndex();
    if (!sectors.GetMovablePosition(map, position.x, position.y, position)) {
        PIXEL_POSITION recall = position;
        sectors.GetRecallPositionByEmpire(map, character->GetEmpire(), recall);
        sys_err("!GetMovablePosition (name %s %ldx%ld map %ld changed to %ldx%ld)",
                character->GetName(), position.x, position.y, map, recall.x, recall.y);
        position = recall;
    }

    auto& guilds = CGuildManager::instance();
    guilds.SendGuildNames(character);
    guilds.LoginMember(character);
    character->Show(map, position.x, position.y, position.z, false);
    sectors.SendNPCPosition(character);
    character->ReviveInvisible(5);
    desc->SetPhase(5);

    sys_log(0, "ENTERGAME: %s %ldx%ldx%ld %s map_index %ld", character->GetName(),
            character->GetX(), character->GetY(), character->GetZ(), desc->GetHostName(), map);
    if (character->GetHorseLevel())
        character->EnterHorse();
    character->ResetPlayTime(0);
    character->StartSaveEvent();
    character->StartRecoveryEvent();
    character->StartCheckSpeedHackEvent();

    CPVPManager::instance().Connect(character);
    CPVPManager::instance().SendList(desc);
    MessengerManager::instance().Login(character->GetName());
    CPartyManager::instance().SetParty(character);
    guilds.SendGuildWar(character);
    building::CManager::instance().SendLandList(desc, map);
    marriage::CManager::instance().Login(character);

    BYTE timePacket[5] = {0x6a};
    const auto time = static_cast<std::uint32_t>(get_global_time());
    std::memcpy(timePacket + 1, &time, sizeof(time));
    desc->Packet(timePacket, sizeof(timePacket));
    const BYTE channelPacket[] = {0x79, g_bChannel};
    desc->Packet(channelPacket, sizeof(channelPacket));

    character->SendGreetMessage();
    _send_bonus_info(character);
    const bool germany = LC_IsGermany();
    if (germany && test_server)
        sys_log(0, "GERMANY : EnterGame : %s ", character->GetName());
    for (BYTE type = 0; type < 10; ++type) {
        const int remaining = character->GetPremiumRemainSeconds(type);
        if (remaining <= 0)
            continue;
        character->AddAffect(500 + type, 0, 0, 0, remaining, 0, true, false);
        sys_log(0, "%sPREMIUM: %s type %d %dmin", germany ? "GERMANY : " : "",
                character->GetName(), type, remaining);
    }

    if (g_bCheckClientVersion && g_stClientVersion != desc->GetClientVersion()) {
        character->ChatPacket(2, locale_find("\305\254\266\363\300\314\276\360\306\256\040\271\366\300\374\300\314\040\306\262\267\301\040\267\316\261\327\276\306\277\364\040\265\313\264\317\264\331\056\040\301\244\273\363\300\373\300\270\267\316\040\306\320\304\241\040\310\304\040\301\242\274\323\307\317\274\274\277\344\056", g_iUseLocale));
        desc->DelayedDisconnect(10);
        LogManager::instance().HackLog("VERSION_CONFLICT", desc->GetAccountTable().login,
                                       character->GetName(), desc->GetHostName());
    }

    auto& warMaps = CWarMapManager::instance();
    auto& weddingMaps = marriage::WeddingManager::instance();
    auto& arenas = CArenaManager::instance();
    bool returnToStart = false;
    if (map >= 10000) {
        if (warMaps.IsWarMap(map))
            character->SetWarMap(warMaps.Find(map));
        else if (weddingMaps.IsWeddingMap(map))
            character->SetWeddingMap(weddingMaps.Find(map));
        else
            character->SetDungeon(CDungeonManager::instance().Find(map));
    } else if (arenas.IsArenaMap(map)) {
        const int member = arenas.IsMember(map, character->GetPlayerID());
        if (member == 2) {
            character->SetObserverMode(true);
            character->SetArenaObserverMode(true);
            if (arenas.RegisterObserverPtr(character, map, character->GetX() / 100, character->GetY() / 100))
                sys_log(0, "ARENA : Observer add failed");
        } else if (member == 1) {
            const BYTE duelPacket[] = {0x28, 3, 0};
            desc->Packet(duelPacket, sizeof(duelPacket));
        } else if (member == 0) {
            returnToStart = character->GetGMLevel() == 0;
        }

        if ((member == 1 || member == 2) && character->IsHorseRiding()) {
            character->StopRiding();
            character->HorseSummon(false, false);
        }
        if (member == 1) {
            if (CParty* party = character->GetParty()) {
                if (party->GetMemberCount() == 2)
                    CPartyManager::instance().DeleteParty(party);
                else
                    party->Quit(character->GetPlayerID());
            }
        }
    } else if (map == 113) {
        if (!COXEventManager::instance().Enter(character))
            returnToStart = character->GetGMLevel() == 0;
    } else if (warMaps.IsWarMap(map) || weddingMaps.IsWeddingMap(map)) {
        returnToStart = !test_server;
    }

    if (returnToStart) {
        const BYTE empire = character->GetEmpire();
        const long* start = g_start_position[empire < 4 ? empire : 0];
        character->WarpSet(start[0], start[1], 0);
    }

    if (g_TeenDesc) {
        BYTE teenPacket[7] = {0x10};
        const auto account = static_cast<std::uint32_t>(desc->GetAccountTable().id);
        std::memcpy(teenPacket + 3, &account, sizeof(account));
        g_TeenDesc->Packet(teenPacket, sizeof(teenPacket));
        sys_log(0, "TEEN_SEND : (%u, %s)", account, character->GetName());
    }
}

// 080f2890. The original dispatcher does not call this stub.
void CInputLogin::MatrixAnswer(DESC*, const char*)
{
}

int CInputLogin::Analyze(DESC* desc, BYTE header, const char* data)
{
    switch (header) {
        case 0x01: Login(desc, data); break;
        case 0x04: CharacterCreate(desc, data); break;
        case 0x05: CharacterDelete(desc, data); break;
        case 0x06: CharacterSelect(desc, data); break;
        case 0x07:
        case 0x69: break;
        case 0x0a: Entergame(desc, data); break;
        case 0x5a: Empire(desc, data); break;
        case 0x6a: ChangeName(desc, data); break;
        case 0x6d: LoginByKey(desc, data); break;
        case 0x65: GuildMarkCRCList(desc, data); break;
        case 0x66: return GuildMarkUpload(desc, data, m_iBufferLeft);
        case 0x67: CRCReport(desc->GetCharacter(), data); break;
        case 0x68: GuildMarkIDXList(desc, data); break;
        case 0x70: return GuildSymbolUpload(desc, data, m_iBufferLeft);
        case 0x71: GuildSymbolCRC(desc, data); break;
        case 0xf1: Version(desc->GetCharacter(), data); break;
        case 0xfc: Handshake(desc, data); break;
        case 0xfd:
            if (!LC_IsGermany() && !LC_IsEurope())
                Version(desc->GetCharacter(), data);
            break;
        case 0xfe: Pong(desc); break;
        default:
            sys_err("login phase does not handle this packet! header %d", header);
            break;
    }
    return 0;
}

namespace {
std::uint32_t ReadMarkWord(const char* data)
{
    std::uint32_t value;
    std::memcpy(&value, data, sizeof(value));
    return value;
}
}

int CInputLogin::GuildMarkUpload(DESC*, const char* data, int bytes)
{
    constexpr int markBytes = 0x300;
    if (bytes < 5 + markBytes)
        return -1;

    const unsigned long guild = ReadMarkWord(data + 1);
    sys_log(0, "MARK_SERVER: GuildMarkUpload guild_id(%lu) buffer_left(%d) mark_size(%d)",
            guild, bytes, markBytes);
    Guild::IMarkManager* manager = CGuildManager::instance().GetMarkManager();
    if (!manager) {
        _sys_err("GuildMarkUpload", 0x364, "MARK_SERVER: GuildMarkUpload guild_id(%lu) - MARK_SERVER_IS_NULL", guild);
        return -1;
    }
    const unsigned long markID = manager->GetMarkID(guild);
    sys_log(0, "MARK_SERVER: GuildMarkUpload guild_id(%lu) - mark_id(%lu)", guild, markID);
    if (!markID) {
        _sys_err("GuildMarkUpload", 0x36e, "MARK_SERVER: GuildMarkUpload guild_id(%lu) - MARK_SERVER_IS_FULL", guild);
        return -1;
    }

    Guild::SMark mark;
    static_assert(sizeof(mark) == markBytes, "old guild mark size");
    std::memcpy(&mark, data + 5, markBytes);
    manager->UploadMark(markID, mark);
    manager->SaveMarkData("guild_mark.tga");
    return 0;
}

void CInputLogin::GuildMarkIDXList(DESC* desc, const char* data)
{
    sys_log(0, "MARK_SERVER: GuildMarkIDXList");
    Guild::IMarkManager* manager = CGuildManager::instance().GetMarkManager();
    if (!manager)
        return;

    const BYTE header = 0x66;
    const auto* indices = manager->GetCompressedMarkIndices();
    const std::uint32_t size = !manager->IsMarkIndicesCRC(ReadMarkWord(data + 1)) && indices
        ? indices->size : 0;
    TEMP_BUFFER buffer;
    buffer.write(&header, sizeof(header));
    buffer.write(&size, sizeof(size));
    if (size)
        buffer.write(indices->data, size);
    desc->LargePacket(buffer.read_peek(), buffer.size());
    if (size)
        sys_log(0, "MARK_SERVER: GuildMarkIDXList - DIFF_CRC packet(%d/%d)", static_cast<int>(size), buffer.size());
    else
        sys_log(0, "MARK_SERVER: GuildMarkIDXList - SAME_CRC packet(%d)", buffer.size());
}

void CInputLogin::GuildMarkCRCList(DESC* desc, const char* data)
{
    sys_log(0, "MARK_SERVER: GuildMarkCRCList");
    Guild::IMarkManager* manager = CGuildManager::instance().GetMarkManager();
    if (!manager)
        return;

    Guild::IMarkManager::SBlockCRCTable crc;
    static_assert(sizeof(crc) == 0x100, "old guild CRC list size");
    std::memcpy(&crc, data + 1, sizeof(crc));
    Guild::IMarkManager::SDiffBlockIDTable differences;
    manager->GetDiffBlockIDTable(crc, &differences);
    sys_log(0, "MARK_SERVER: diff_count(%d)", static_cast<int>(differences.m_dwIDNum));

    const BYTE header = 0x65;
    const BYTE count = static_cast<BYTE>(differences.m_dwIDNum);
    TEMP_BUFFER buffer;
    buffer.write(&header, sizeof(header));
    buffer.write(&count, sizeof(count));
    for (unsigned long index = 0; index < differences.m_dwIDNum; ++index) {
        const unsigned long id = differences.m_aBlockID[index];
        const Guild::SCompressedBlock* block = manager->GetCompressedBlockPtr(id);
        if (!block || !block->m_dwCompressedSize)
            continue;
        const BYTE wireID = static_cast<BYTE>(id);
        const std::uint32_t size = block->m_dwCompressedSize;
        sys_log(0, "MARK_SERVER id %d size %d", static_cast<int>(id), static_cast<int>(size));
        buffer.write(&wireID, sizeof(wireID));
        buffer.write(&size, sizeof(size));
        buffer.write(block->m_abBuf, size);
    }
    sys_log(0, "MARK_SERVER: Send Size (%d )", buffer.size());
    desc->LargePacket(buffer.read_peek(), buffer.size());
}

int CInputLogin::GuildSymbolUpload(DESC* desc, const char* data, int bytes)
{
    constexpr int headerSize = 7;
    if (bytes < headerSize)
        return -1;
    sys_log(0, "GuildSymbolUpload iBufferLeft %d", bytes);
    std::uint16_t packetSize;
    std::memcpy(&packetSize, data + 1, sizeof(packetSize));
    if (packetSize < headerSize || bytes < packetSize)
        return -1;

    const unsigned long guild = ReadMarkWord(data + 3);
    if (!test_server && !building::CManager::instance().FindLandByGuild(guild)) {
        desc->SetPhase(0);
        return 0;
    }
    Guild::IMarkManager* manager = CGuildManager::instance().GetMarkManager();
    if (!manager)
        return -1;

    const int payloadSize = packetSize - headerSize;
    const auto* symbol = reinterpret_cast<const unsigned char*>(data + headerSize);
    if (payloadSize >= 4) {
        sys_log(0, "GuildSymbolUpload Do Upload %02X%02X%02X%02X %d",
                static_cast<unsigned>(symbol[0]), static_cast<unsigned>(symbol[1]),
                static_cast<unsigned>(symbol[2]), static_cast<unsigned>(symbol[3]), headerSize);
    }
    manager->UploadSymbol(guild, payloadSize, symbol);
    manager->SaveSymbol("guild_symbol.tga");
    return payloadSize;
}

void CInputLogin::GuildSymbolCRC(DESC* desc, const char* data)
{
    const unsigned long guild = ReadMarkWord(data + 1);
    const unsigned long crc = ReadMarkWord(data + 5);
    const unsigned long size = ReadMarkWord(data + 9);
    sys_log(0, "GuildSymbolCRC %lu %lu %lu", guild, crc, size);
    if (Guild::IMarkManager* manager = CGuildManager::instance().GetMarkManager())
        manager->SendGuildSymbol(desc, guild, crc, size);
}

CInputLogin::~CInputLogin() {}
BYTE CInputLogin::GetType() { return 2; }

// Insert in game/src/input_login.cpp; 080f2d50, old input_login.cpp DWARF.
// Requires common/length.h EJobs. ret_job is a required nonnull output pointer.
bool RaceToJob(unsigned race, unsigned* ret_job) {
    *ret_job=0;
    switch(race) {
    case 0: case 4: *ret_job=JOB_WARRIOR; break;
    case 1: case 5: *ret_job=JOB_ASSASSIN; break;
    case 2: case 6: *ret_job=JOB_SURA; break;
    case 3: case 7: *ret_job=JOB_SHAMAN; break;
    default: return false;
    }
    return true;
}

// 080f2b00. Character creation in this build uses NewPlayerTable2.
bool NewPlayerTable(TPlayerTable* table, const char* name, BYTE job, BYTE shape,
                    BYTE empire, BYTE, BYTE, BYTE, BYTE)
{
    if (job >= 4 || empire < 1 || empire > 3)
        return false;

    *table = {};
    std::strncpy(table->name, name, sizeof(table->name) - 1);
    table->part_base = shape;
    table->level = 1;
    table->job = job;

    const TJobInitialPoints& points = JobInitialPoints[job];
    table->st = points.st;
    table->ht = points.ht;
    table->dx = points.dx;
    table->iq = points.iq;
    table->hp = points.max_hp + points.ht * points.hp_per_ht;
    table->sp = points.max_sp + points.iq * points.sp_per_iq;
    table->stamina = points.max_stamina;

    char source[] = "input_login.cpp";
    table->x = g_create_position[empire][0] + number_ex(-300, 300, source, 0x122);
    table->y = g_create_position[empire][1] + number_ex(-300, 300, source, 0x123);
    if (china_event_server) {
        table->level = 35;
        for (int level = 1; level < 35; ++level) {
            table->sRandomHP += number_ex(points.hp_per_lv_begin, points.hp_per_lv_end, source, 0x130);
            table->sRandomSP += number_ex(points.sp_per_lv_begin, points.sp_per_lv_end, source, 0x131);
            table->stat_point += 3;
        }
        table->gold = 1000000;
        table->hp += table->sRandomHP;
        table->sp += table->sRandomSP;
    }
    return true;
}

bool NewPlayerTable2(TPlayerTable* table, const char* name, BYTE race, BYTE shape, BYTE empire)
{
    if (race >= 8) {
        _sys_err("NewPlayerTable2", 0x177, "NewPlayerTable2.OUT_OF_RACE_RANGE(%d >= max(%d))\n",
                 static_cast<int>(race), 8);
        return false;
    }
    unsigned job;
    if (!RaceToJob(race, &job)) {
        _sys_err("NewPlayerTable2", 0x17f, "NewPlayerTable2.RACE_TO_JOB_ERROR(%d)\n",
                 static_cast<int>(race));
        return false;
    }
    if (empire < 1 || empire > 3)
        return false;

    sys_log(0, "NewPlayerTable2(name=%s, race=%d, job=%d)", name, static_cast<int>(race),
            static_cast<int>(job));
    *table = {};
    std::strncpy(table->name, name, sizeof(table->name) - 1);
    table->level = 1;
    table->job = race;
    table->part_base = shape;

    const TJobInitialPoints& points = JobInitialPoints[job];
    table->st = points.st;
    table->ht = points.ht;
    table->dx = points.dx;
    table->iq = points.iq;
    table->hp = points.max_hp + points.ht * points.hp_per_ht;
    table->sp = points.max_sp + points.iq * points.sp_per_iq;
    table->stamina = points.max_stamina;

    table->x = g_create_position[empire][0] + number_ex(-300, 300, const_cast<char*>("input_login.cpp"), 0x197);
    table->y = g_create_position[empire][1] + number_ex(-300, 300, const_cast<char*>("input_login.cpp"), 0x198);
    return true;
}

void CInputLogin::CharacterCreate(DESC* desc, const char* data)
{
    static_assert(sizeof(TPlayerTable) == 0x71a, "old DB player table");
    static_assert(sizeof(TPlayerCreatePacket) == 0x74f, "old DB character creation request");

    const BYTE slot = static_cast<BYTE>(data[1]);
    char name[25] = {};
    std::memcpy(name, data + 2, sizeof(name) - 1);
    std::uint16_t job;
    std::memcpy(&job, data + 0x1b, sizeof(job));
    const BYTE shape = static_cast<BYTE>(data[0x1d]);
    sys_log(0, "PlayerCreate: name %s pos %d job %d shape %d", name, slot, job, shape);

    if (check_name(name) && shape < 2) {
        TPlayerCreatePacket packet = {};
        if (NewPlayerTable2(&packet.player_table, name, static_cast<BYTE>(job), shape, desc->GetEmpire())) {
            const TAccountTable& account = desc->GetAccountTable();
            // Leave a zero byte before the destination for an empty login.
            char normalized[32] = {};
            trim_and_lower(normalized + 1, account.login, 30);
            std::memcpy(packet.login, normalized + 1, sizeof(packet.login));
            std::strncpy(packet.passwd, account.passwd, sizeof(packet.passwd) - 1);
            packet.account_id = account.id;
            packet.account_index = slot;
            sys_log(0, "PlayerCreate: name %s account_id %d, TPlayerCreatePacketSize( %d ) ,Packet->Gold %d",
                    name, slot, static_cast<int>(sizeof(packet)), static_cast<int>(packet.player_table.gold));
            db_clientdesc->DBPacket(0x05, desc->GetHandle(), &packet, sizeof(packet));
            return;
        }
        _sys_err("CharacterCreate", 0x1bf, "player_prototype error: job %d face %d ", job, shape);
    }

    TPacketGCLoginFailure failure = {};
    failure.header = 0x09;
    desc->Packet(&failure, sizeof(failure));
}

void CInputLogin::LoginByKey(DESC* desc, const char* data)
{
    static_assert(sizeof(TPacketGDLoginByKey) == 67, "DB login-by-key request");
    char loginField[LOGIN_MAX_LEN + 1];
    std::memcpy(loginField, data + 1, LOGIN_MAX_LEN);
    loginField[LOGIN_MAX_LEN] = '\0';

    // trim_and_lower examines the preceding byte for an empty login.
    char normalized[LOGIN_MAX_LEN + 2] = {};
    char* login = normalized + 1;
    trim_and_lower(login, loginField, LOGIN_MAX_LEN);
    const char* host = desc->GetHostName();

    dev_log("input_login.cpp", 0x84, "LoginByKey", L_DEB0,
            const_cast<char*>(" check_blocked_country_start"));
    if (!is_block_exception(login) && is_blocked_country_ip(host)) {
        sys_log(0, "BLOCK_COUNTRY_IP (%s)", host);
        desc->SetPhase(0);
        return;
    }
    dev_log("input_login.cpp", 0x8c, "LoginByKey", L_DEB0,
            const_cast<char*>(" check_blocked_country_end"));

    if (g_bNoMoreClient) {
        LoginFailure(desc, "SHUTDOWN");
        return;
    }
    if (g_iUserLimit) {
        int total;
        int* empireUserCount;
        int localCount;
        DESC_MANAGER::instance().GetUserCount(total, &empireUserCount, localCount);
        if (static_cast<unsigned>(g_iUserLimit) <= static_cast<unsigned>(total)) {
            LoginFailure(desc, "FULL");
            return;
        }
    }

    TPacketGDLoginByKey packet = {};
    std::memcpy(&packet.dwLoginKey, data + 0x20, sizeof(packet.dwLoginKey));
    std::memcpy(packet.adwClientKey, data + 0x24, sizeof(packet.adwClientKey));
    DWORD clientKey[4];
    for (int i = 0; i < 4; ++i)
        clientKey[i] = packet.adwClientKey[i];
    sys_log(0, "LOGIN_BY_KEY: %s key %lu", login, static_cast<unsigned long>(packet.dwLoginKey));
    desc->SetLoginKey(packet.dwLoginKey);
    desc->SetSecurityKey(clientKey);

    std::strncpy(packet.szLogin, login, sizeof(packet.szLogin) - 1);
    std::strncpy(packet.szIP, host, sizeof(packet.szIP) - 1);
    db_clientdesc->DBPacket(0x65, desc->GetHandle(), &packet, sizeof(packet));
}
