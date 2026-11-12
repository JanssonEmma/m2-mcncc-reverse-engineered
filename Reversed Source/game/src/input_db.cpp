#include "input.h"
#include "char_manager.h"
#include "packet.h"
#include "char.h"
#include "db.h"
#include "log.h"
#include "locale.hpp"
#include "login_sim.h"
#include <arpa/inet.h>
#include <algorithm>
#include "building.h"
#include "banword.h"
#include "block_country.h"
#include "constants.h"
#include "gm.h"
#include "refine.h"
#include "priv_manager.h"
#include "questmanager.h"
#include "questpc.h"
#include "event.h"
#include "shop_manager.h"
#include "skill.h"
#include "utils.h"
#include "config.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "map_location.h"
#include "start_position.h"
#include "marriage.h"
#include "wedding.h"
#include "p2p.h"
#include "party.h"
#include "dev_log.h"
#include "item_manager.h"
#include "locale_service.h"
#include "mob_manager.h"
#include "motion.h"
#include "sectree_manager.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/main.h"
#include "../../libthecore/include/signal.h"
#include "../../libthecore/include/utils.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <memory>

bool PCBangPlayTimeWriter_RequestUpdateIPList(const char* who, bool force);
bool castle_boot();
extern LPFDWATCH main_fdw;
void SendNotice(const char* text);
bool IsPCBangIP(const char* ip);

CInputDB::~CInputDB() = default;

BYTE CInputDB::GetType()
{
    return 5;
}

bool CInputDB::Process(DESC* desc, const void* original, int bytes, int& consumed)
{
    constexpr int headerSize = sizeof(TPacketDBHeader);
    const unsigned char* frame = static_cast<const unsigned char*>(original);
    BYTE lastHeader = 0;
    int lastPacketSize = 0;
    m_iBufferLeft = bytes;

    while (m_iBufferLeft >= headerSize) {
        TPacketDBHeader packet;
        std::memcpy(&packet, frame, sizeof(packet));
        const BYTE header = packet.bHeader;
        m_dwHandle = packet.dwHandle;
        if (packet.dwSize > INT_MAX) {
            _sys_err("Process", 0x9f8, "Invalid DB packet size %u", static_cast<unsigned>(packet.dwSize));
            return true;
        }
        const int size = static_cast<int>(packet.dwSize);
        sys_log(1, "DBCLIENT: header %d handle %d size %d bytes %d",
                header, static_cast<int>(m_dwHandle), size, bytes);
        if (size > m_iBufferLeft - headerSize)
            return true;

        m_payloadSize = size;
        if (Analyze(desc, header, reinterpret_cast<const char*>(frame + headerSize)) < 0) {
            _sys_err("Process", 0x9f8,
                "in InputDB: UNKNOWN HEADER: %d, LAST HEADER: %d(%d), REMAIN BYTES: %d, DESC: %d",
                header, lastHeader, lastPacketSize, m_iBufferLeft, desc->GetSocket());
            printdata(static_cast<const unsigned char*>(original), bytes);
        }
        lastPacketSize = headerSize + size;
        frame += lastPacketSize;
        m_iBufferLeft -= lastPacketSize;
        consumed += lastPacketSize;
        lastHeader = header;
    }
    return true;
}

void CInputDB::FinishBoot(DWORD maxItemId)
{
    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    if (!items.SetMaxItemID(maxItemId)) {
        _sys_err("Boot", 0x3e0, "not enough item id contact your administrator!");
        thecore_shutdown();
        return;
    }

    typedef bool (ITEM_MANAGER::*DropLoader)(const char*);
    const struct {
        const char* filename;
        const char* label;
        int errorLine;
        DropLoader load;
    } drops[] = {
        { "common_drop_item.txt", "CommonDropItem", 0x400, &ITEM_MANAGER::ReadCommonDropItemFile },
        { "etc_drop_item.txt", "ETCDropItem", 0x408, &ITEM_MANAGER::ReadEtcDropItemFile },
        { "drop_item_group.txt", "DropItemGroup", 0x410, &ITEM_MANAGER::ReadDropItemGroup },
        { "special_item_group.txt", "SpecialItemGroup", 0x418, &ITEM_MANAGER::ReadSpecialDropItemFile },
        { "mob_drop_item.txt", "MOBDropItemFile", 0x420, &ITEM_MANAGER::ReadMonsterDropItemGroup },
    };
    for (const auto& drop : drops) {
        char path[256];
        ::snprintf(path, sizeof(path) - 1, "%s/%s", LocaleService_GetBasePath().c_str(), drop.filename);
        sys_log(0, "LoadLocaleFile: %s: %s", drop.label, path);
        if (!(items.*drop.load)(path)) {
            _sys_err("Boot", drop.errorLine, "cannot load %s: %s", drop.label, path);
            thecore_shutdown();
            return;
        }
    }

    const std::string& mapPath = LocaleService_GetMapPath();
    char index[256];
    ::snprintf(index, sizeof(index) - 1, "%s/index", mapPath.c_str());
    sys_log(0, "LoadLocaleFile: MapIndex: %s", index);
    if (!SECTREE_MANAGER::instance().Build(index, mapPath.c_str())) {
        _sys_err("Boot", 0x428, "cannot load MapIndex: %s", index);
        thecore_shutdown();
        return;
    }

    building::CManager::instance().FinalizeBoot();
    CMotionManager::instance().Build();
    signal_timer_enable(30);
    if (test_server)
        CMobManager::instance().DumpRegenCount("mob_count");
    PCBangPlayTimeWriter_RequestUpdateIPList("__boot__", true);
    castle_boot();
    db_clientdesc->DBPacket(127, 0, 0, 0);
    dev_log("input_db.cpp", 0x445, "Boot", 0x800, const_cast<char*>("<sent HEADER_GD_BLOCK_COUNTRY_IP>"));
}

namespace {
class BootReader {
public:
    BootReader(const char* data, std::size_t size) : current(data), remaining(size) {}

    template<class T>
    bool Read(T& value)
    {
        const char* data = Take(sizeof(T));
        if (!data)
            return false;
        std::memcpy(&value, data, sizeof(T));
        return true;
    }

    const char* Take(std::size_t size)
    {
        if (size > remaining)
            return 0;
        const char* result = current;
        current += size;
        remaining -= size;
        return result;
    }

    template<class T>
    bool Table(T*& rows, std::uint16_t& count, const char* name, int line)
    {
        std::uint16_t width;
        if (Read(width) && Read(count) && width == sizeof(T)) {
            const char* bytes = Take(static_cast<std::size_t>(width) * count);
            if (bytes) {
                rows = reinterpret_cast<T*>(const_cast<char*>(bytes));
                return true;
            }
        }
        _sys_err("Boot", line, "%s table size error", name);
        return false;
    }

    bool ReadEnd(std::uint16_t& marker)
    {
        if (remaining < sizeof(marker))
            return false;
        Take(remaining - sizeof(marker));
        return Read(marker);
    }

private:
    const char* current;
    std::size_t remaining;
};

bool ReadItemAttributes(BootReader& packet, bool rare)
{
    TItemAttrTable* rows;
    std::uint16_t count;
    if (!packet.Table(rows, count, rare ? "item rare" : "item attr", rare ? 0x31d : 0x2fd))
        return false;

    sys_log(0, rare ? "BOOT: ITEM_RARE: %d" : "BOOT: ITEM_ATTR: %d", count);
    TItemAttrMap& attributes = rare ? g_map_itemRare : g_map_itemAttr;
    for (unsigned i = 0; i < count; ++i) {
        const TItemAttrTable& row = rows[i];
        const DWORD index = row.dwApplyIndex;
        if (index < 82) {
            attributes[index] = row;
            sys_log(0, rare ? "ITEM_RARE[%d]: %s %lu" : "ITEM_ATTR[%d]: %s %lu",
                    static_cast<int>(index), row.szApply, static_cast<unsigned long>(row.dwProb));
        }
    }
    return true;
}
}

#include "guild.h"
#include "guild_manager.h"

namespace {
#pragma pack(push, 1)
struct MarriageDbPair { std::uint32_t first, second; };
struct MarriageDbAdd {
    MarriageDbPair couple;
    std::int32_t marriedAt;
    char firstName[25], secondName[25];
};
struct MarriageDbUpdate {
    MarriageDbPair couple;
    std::int32_t lovePoints;
    BYTE married;
};
struct MarriageDbWeddingReady {
    MarriageDbPair couple;
    std::uint32_t map;
};
#pragma pack(pop)
static_assert(sizeof(MarriageDbPair) == 8, "marriage pair wire size");
static_assert(sizeof(MarriageDbAdd) == 62, "marriage add wire size");
static_assert(sizeof(MarriageDbUpdate) == 13, "marriage update wire size");
static_assert(sizeof(MarriageDbWeddingReady) == 12, "wedding ready wire size");
}

int CInputDB::Analyze(DESC*, BYTE header, const char* data)
{
    sys_log(0, "CInpufDB::Analyze[%d] ", static_cast<unsigned>(header));
    if ((header >= 0x3b && header <= 0x3f) || header == 0x41) {
        constexpr std::size_t recordSizes[] = {
            sizeof(TPacketPartyCreate), sizeof(TPacketPartyDelete), sizeof(TPacketPartyAdd),
            sizeof(TPacketPartyRemove), sizeof(TPacketPartyStateChange), 0, sizeof(TPacketPartySetMemberLevel)
        };
        const auto recordSize = recordSizes[header - 0x3b];
        if (m_payloadSize < recordSize)
            return -1;

        std::uint32_t leaderPid;
        std::memcpy(&leaderPid, data, sizeof(leaderPid));
        std::uint32_t memberPid = 0;
        if (recordSize >= 8)
            std::memcpy(&memberPid, data + 4, sizeof(memberPid));

        switch (header) {
        case 0x3b:
            PartyCreate(leaderPid);
            break;
        case 0x3c:
            PartyDelete(leaderPid);
            break;
        case 0x3d:
            PartyAdd(leaderPid, memberPid, static_cast<BYTE>(data[8]));
            break;
        case 0x3e:
            PartyRemove(memberPid);
            break;
        case 0x3f:
            PartyStateChange(leaderPid, memberPid, static_cast<BYTE>(data[8]), data[9] != 0);
            break;
        case 0x41:
            PartySetMemberLevel(leaderPid, memberPid, static_cast<BYTE>(data[8]));
            break;
        }
        return 0;
    }
    if (header == 0x5e)
        return GuildChangeGrade(data) ? 0 : -1;
    if (header == 0x6a)
        return GuildWarReserveAdd(data) ? 0 : -1;
    if (header == 0x6b)
        return GuildWarReserveDelete(data) ? 0 : -1;
    if (header == 0x6c)
        return GuildWarBet(data) ? 0 : -1;
    if (header == 0x5c) {
        if (m_payloadSize < sizeof(TPacketDGGuildMember))
            return -1;
        GuildAddMember(data);
        return 0;
    }
    if (header == 0x5d || header == 0x60) {
        if (m_payloadSize < sizeof(TPacketGuild))
            return -1;
        if (header == 0x5d)
            GuildRemoveMember(data);
        else
            GuildDisband(data);
        return 0;
    }
    if (header == 0x5f) {
        if (m_payloadSize < sizeof(TPacketGuildChangeMemberData))
            return -1;
        GuildChangeMemberData(data);
        return 0;
    }
    if (header == 0x61)
        return GuildWar(data) ? 0 : -1;
    if (header == 0x62)
        return GuildWarScore(data) ? 0 : -1;
    if (header == 0x01)
        return Notice(data) ? 0 : -1;
    if (header == 0x1e) {
        if (m_payloadSize < sizeof(TAccountTable))
            return -1;
        LoginSuccess(m_dwHandle, data);
        return 0;
    }
    if (header == 0x1f || header == 0x21) {
        DESC* desc = DESC_MANAGER::instance().FindByHandle(m_dwHandle);
        LoginFailure(desc, header == 0x1f ? "NOID" : "WRONGPWD");
        return 0;
    }
    if (header == 0x22)
        return LoginAlready(DESC_MANAGER::instance().FindByHandle(m_dwHandle), data) ? 0 : -1;
    if (header == 0x31) {
        if (!m_payloadSize)
            return -1;
        EmpireSelect(DESC_MANAGER::instance().FindByHandle(m_dwHandle), data);
        return 0;
    }
    if (header == 0x78)
        return ReloadProto(data) ? 0 : -1;

    if (header == 0x79) {
        if (m_payloadSize < sizeof(TPacketDGChangeName))
            return -1;
        ChangeName(DESC_MANAGER::instance().FindByHandle(m_dwHandle), data);
        return 0;
    }
    if (header == 0x7a)
        return AuthLogin(DESC_MANAGER::instance().FindByHandle(m_dwHandle), data) ? 0 : -1;
    if (header == 0x23) {
        if (m_payloadSize < sizeof(TPlayerTable))
            return -1;
        PlayerLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), data);
        return 0;
    }
    if (header == 0x24)
        return 0;

    switch (header) {
        case 0x25:
            if (m_payloadSize < sizeof(TPacketDGCreateSuccess))
                return -1;
            PlayerCreateSuccess(DESC_MANAGER::instance().FindByHandle(m_dwHandle), data);
            return 0;
        case 0x26:
        case 0x27:
            PlayerCreateFailure(DESC_MANAGER::instance().FindByHandle(m_dwHandle), header == 0x26 ? 1 : 0);
            return 0;
        case 0x28:
            if (!m_payloadSize || static_cast<BYTE>(data[0]) >= 4)
                return -1;
            PlayerDeleteSuccess(DESC_MANAGER::instance().FindByHandle(m_dwHandle), data);
            return 0;
        case 0x29:
            PlayerDeleteFail(DESC_MANAGER::instance().FindByHandle(m_dwHandle));
            return 0;
    }

    if (header == 0x7e)
        return MoneyLog(data) ? 0 : -1;

    if (header == 0x81)
        return BillingExpire(data) ? 0 : -1;

    switch (header) {
        case 0x96: return MarriageAdd(data) ? 0 : -1;
        case 0x97: return MarriageUpdate(data) ? 0 : -1;
        case 0x9a: return WeddingReady(data) ? 0 : -1;
        case 0x98: return MarriageRemove(data) ? 0 : -1;
        case 0x99: return WeddingRequest(data) ? 0 : -1;
        case 0x9b: return WeddingStart(data) ? 0 : -1;
        case 0x9c: return WeddingEnd(data) ? 0 : -1;
        case 0x9d: return MyshopPricelistRes(data) ? 0 : -1;
        case 0x7c: return ChangeEmpirePriv(data) ? 0 : -1;
        case 0x7d: return ChangeGuildPriv(data) ? 0 : -1;
        case 0x7f: return ChangeCharacterPriv(data) ? 0 : -1;
        case 0x8c: return CreateObject(data) ? 0 : -1;
        case 0x8d: return DeleteObject(data) ? 0 : -1;
        case 0x8e: return UpdateLand(data) ? 0 : -1;
    }

    if (header == 0x9e)
        return ReloadAdmin(data) ? 0 : -1;

    if (header == 0x80)
        return BillingRepair(data) ? 0 : -1;
    if (header == 0x82)
        return BillingLogin(data) ? 0 : -1;

    if (header == 0x83)
        return VCard(data) ? 0 : -1;

    if (header == 0x84)
        return BillingCheck(data) ? 0 : -1;

    switch (header) {
        case 0x39:
            GuildSkillRecharge();
            return 0;
        case 0x3a: return GuildExpUpdate(data) ? 0 : -1;
        case 0x38: return GuildSkillUpdate(data) ? 0 : -1;
        case 0x64: return GuildLoad(data) ? 0 : -1;
        case 0x65: return GuildLadder(data) ? 0 : -1;
        case 0x66: return GuildSkillUsableChange(data) ? 0 : -1;
        case 0x2d:
            return SafeboxLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), data) ? 0 : -1;
        case 0x33:
            return MallLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), data) ? 0 : -1;
        case 0x2e:
            return SafeboxChangeSize(DESC_MANAGER::instance().FindByHandle(m_dwHandle), data) ? 0 : -1;
        case 0x2f:
            SafeboxWrongPassword(DESC_MANAGER::instance().FindByHandle(m_dwHandle));
            return 0;
        case 0x30:
            return SafeboxChangePasswordAnswer(DESC_MANAGER::instance().FindByHandle(m_dwHandle), data) ? 0 : -1;
    }

    if (header == 0x2a)
        return ItemLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), data) ? 0 : -1;

    if (header == 0x2c)
        return QuestLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), data) ? 0 : -1;

    if (header == 0xab)
        return AddBlockCountryIp(data) ? 0 : -1;
    if (header == 0xac)
        return BlockException(data) ? 0 : -1;

    if (header == 0x32)
        return AffectLoad(DESC_MANAGER::instance().FindByHandle(m_dwHandle), data) ? 0 : -1;

    if (header == 0x2b) {
        Boot(data);
        return 0;
    }
    if (header == 0x67) {
        TPacketDGGuildMoneyChange packet;
        if (m_payloadSize < sizeof(packet))
            return -1;
        std::memcpy(&packet, data, sizeof(packet));
        GuildMoneyChange(packet.dwGuild, packet.lTotalGold);
        return 0;
    }
    if (header == 0x68) {
        TPacketDGGuildMoneyWithdraw packet;
        if (m_payloadSize < sizeof(packet))
            return -1;
        std::memcpy(&packet, data, sizeof(packet));
        GuildWithdrawMoney(packet.dwGuild, packet.lChangeGold);
        return 0;
    }
    if (header == 0x69)
        return SetEventFlag(data) ? 0 : -1;
    if (header == 0xfe)
        return MapLocations(data) ? 0 : -1;
    if (header == 0xff)
        return P2P(data) ? 0 : -1;
    if (header == 0x5a)
        return Time(data) ? 0 : -1;
    return -1;
}

bool CInputDB::LoginAlready(DESC* desc, const char* data)
{
    if (!desc)
        return true;
    constexpr std::size_t loginWidth = 31;
    if (!std::memchr(data, 0, std::min(loginWidth, m_payloadSize)))
        return false;
    if (DESC* existing = DESC_MANAGER::instance().FindByLoginName(data)) {
        existing->DisconnectOfSameLogin();
    } else {
        char packet[26] = {};
        packet[0] = 7;
        std::strncpy(packet + 1, data, sizeof(packet) - 1);
        P2P_MANAGER::instance().Send(packet, sizeof(packet));
    }
    LoginFailure(desc, "ALREADY");
    return true;
}

bool CInputDB::AuthLogin(DESC* desc, const char* data)
{
    if (!m_payloadSize)
        return false;
    if (desc) {
        const BYTE result = static_cast<BYTE>(data[0]);
        BYTE packet[6] = { 0x96, 0, 0, 0, 0, result };
        const std::uint32_t key = result ? desc->GetLoginKey() : 0;
        std::memcpy(packet + 1, &key, sizeof(key));
        desc->Packet(packet, sizeof(packet));
        sys_log(0, "AuthLogin result %u key %lu", static_cast<unsigned>(result), desc->GetLoginKey());
    }
    return true;
}

bool CInputDB::SetEventFlag(const char* data)
{
    constexpr std::size_t nameWidth = 33;
    std::int32_t value;
    if (m_payloadSize < nameWidth + sizeof(value))
        return false;
    const char* end = static_cast<const char*>(std::memchr(data, 0, nameWidth));
    if (!end)
        return false;
    std::memcpy(&value, data + nameWidth, sizeof(value));
    quest::CQuestManager::instance().SetEventFlag(std::string(data, end), value);
    return true;
}

bool CInputDB::MapLocations(const char* data)
{
    constexpr std::size_t recordSize = 50;
    if (!m_payloadSize)
        return false;
    const unsigned count = static_cast<BYTE>(data[0]);
    if (m_payloadSize < 1 + count * recordSize)
        return false;
    sys_log(0, "InputDB::MapLocations %d", count);
    const char* record = data + 1;
    for (unsigned i = 0; i < count; ++i, record += recordSize) {
        const char* host = record + 32;
        if (!std::memchr(host, 0, 16))
            return false;
    }
    record = data + 1;
    for (unsigned i = 0; i < count; ++i, record += recordSize) {
        std::uint16_t port;
        std::memcpy(&port, record + 48, sizeof(port));
        for (unsigned j = 0; j < 32 && record[j]; ++j)
            CMapLocation::instance().Insert(static_cast<BYTE>(record[j]), record + 32, port);
    }
    return true;
}

bool CInputDB::P2P(const char* data)
{
    if (m_payloadSize < 19 || !std::memchr(data, 0, 16))
        return false;
    std::uint16_t port;
    std::memcpy(&port, data + 16, sizeof(port));
    sys_log(0, "InputDB:P2P %s:%d", data, port);
    P2P_MANAGER& peers = P2P_MANAGER::instance();
    if (!peers.IsExist(data, port)) {
        CLIENT_DESC* peer = DESC_MANAGER::instance().CreateConnectionDesc(main_fdw, data, port, 9, false);
        peers.RegisterConnector(peer);
        peer->SetP2P(data, port, static_cast<BYTE>(data[18]));
    }
    return true;
}

bool CInputDB::GuildChangeGrade(const char* data)
{
    if (m_payloadSize < 8)
        return false;
    std::uint32_t guildId;
    std::memcpy(&guildId, data, sizeof(guildId));
    if (CGuild* guild = CGuildManager::instance().TouchGuild(guildId))
        guild->P2PChangeGrade(static_cast<BYTE>(data[4]));
    return true;
}

bool CInputDB::GuildWar(const char* data)
{
    TPacketGuildWar packet;
    if (m_payloadSize < sizeof(packet))
        return false;
    std::memcpy(&packet, data, sizeof(packet));
    const BYTE type = packet.bType;
    const BYTE state = packet.bWar;
    const DWORD first = packet.dwGuildFrom;
    const DWORD second = packet.dwGuildTo;
    sys_log(0, "InputDB::GuildWar %lu %lu state %d", static_cast<DWORD>(first),
        static_cast<DWORD>(second), static_cast<int>(state));
    CGuildManager& manager = CGuildManager::instance();
    switch (state) {
        case 1:
        case 3: manager.DeclareWar(first, second, type); break;
        case 2: manager.RefuseWar(first, second); break;
        case 4: manager.WaitStartWar(first, second); break;
        case 5: manager.CancelWar(first, second); break;
        case 6: manager.StartWar(first, second); break;
        case 7: manager.EndWar(first, second); break;
        case 8: manager.WarOver(first, second, type != 0); break;
        case 9: manager.ReserveWar(first, second, type); break;
        default: _sys_err("GuildWar", 0x5bb, "Unknown guild war state"); break;
    }
    return true;
}

bool CInputDB::GuildWarScore(const char* data)
{
    TPacketGuildWarScore packet;
    if (m_payloadSize < sizeof(packet))
        return false;
    std::memcpy(&packet, data, sizeof(packet));
    CGuildManager::instance().TouchGuild(packet.dwGuildGainPoint)->SetWarScoreAgainstTo(packet.dwGuildOpponent, packet.lScore);
    return true;
}

bool CInputDB::GuildWarReserveAdd(const char* data)
{
    TGuildWarReserve packet;
    if (m_payloadSize < sizeof(packet))
        return false;
    std::memcpy(&packet, data, sizeof(packet));
    CGuildManager::instance().ReserveWarAdd(&packet);
    return true;
}

bool CInputDB::GuildWarReserveDelete(const char* data)
{
    std::uint32_t id;
    if (m_payloadSize < sizeof(id))
        return false;
    std::memcpy(&id, data, sizeof(id));
    CGuildManager::instance().ReserveWarDelete(id);
    return true;
}

bool CInputDB::GuildWarBet(const char* data)
{
    TPacketGDGuildWarBet packet;
    if (m_payloadSize < sizeof(packet))
        return false;
    std::memcpy(&packet, data, sizeof(packet));
    if (!std::memchr(packet.szLogin, '\0', sizeof(packet.szLogin)))
        return false;
    CGuildManager::instance().ReserveWarBet(&packet);
    return true;
}

void CInputDB::GuildMoneyChange(DWORD guildId, int gold)
{
    if (CGuild* guild = CGuildManager::instance().TouchGuild(guildId))
        guild->RecvMoneyChange(gold);
}

void CInputDB::GuildWithdrawMoney(DWORD guildId, int gold)
{
    if (CGuild* guild = CGuildManager::instance().TouchGuild(guildId))
        guild->RecvWithdrawMoneyGive(gold);
}

void CInputDB::PartyCreate(DWORD leader)
{
    CPartyManager::instance().P2PCreateParty(leader);
}

void CInputDB::PartyDelete(DWORD leader)
{
    CPartyManager::instance().P2PDeleteParty(leader);
}

void CInputDB::PartyAdd(DWORD leader, DWORD member, BYTE role)
{
    CPartyManager::instance().P2PJoinParty(leader, member, role);
}

void CInputDB::PartyRemove(DWORD member)
{
    CPartyManager::instance().P2PQuitParty(member);
}

void CInputDB::PartyStateChange(DWORD leader, DWORD member, BYTE role, bool apply)
{
    if (CParty* party = CPartyManager::instance().P2PCreateParty(leader))
        party->SetRole(member, role, apply);
}

void CInputDB::PartySetMemberLevel(DWORD leader, DWORD member, BYTE level)
{
    if (CParty* party = CPartyManager::instance().P2PCreateParty(leader))
        party->P2PSetMemberLevel(member, level);
}

void CInputDB::GuildSkillRecharge()
{
    CGuildManager::instance().SkillRecharge();
}

bool CInputDB::GuildSkillUpdate(const char* data)
{
    if (m_payloadSize < sizeof(TPacketGuildSkillUpdate))
        return false;
    TPacketGuildSkillUpdate packet;
    std::memcpy(&packet, data, sizeof(packet));
    if (CGuild* guild = CGuildManager::instance().TouchGuild(packet.guild_id)) {
        guild->UpdateSkill(packet.skill_point, packet.skill_levels);
        guild->GuildPointChange(7, packet.amount, packet.save != 0);
    }
    return true;
}

bool CInputDB::GuildExpUpdate(const char* data)
{
    if (m_payloadSize < sizeof(TPacketGuildExpUpdate))
        return false;
    TPacketGuildExpUpdate packet;
    std::memcpy(&packet, data, sizeof(packet));
    sys_log(1, "GuildExpUpdate %d", packet.amount);
    if (CGuild* guild = CGuildManager::instance().TouchGuild(packet.guild_id))
        guild->GuildPointChange(3, packet.amount, false);
    return true;
}

bool CInputDB::GuildLoad(const char* data)
{
    if (m_payloadSize < 4)
        return false;
    std::uint32_t id;
    std::memcpy(&id, data, sizeof(id));
    CGuildManager::instance().LoadGuild(id);
    return true;
}

bool CInputDB::GuildLadder(const char* data)
{
    struct GuildLadderPacket {
        std::uint32_t id;
        std::int32_t point, wins, draws, losses;
    } packet;
    static_assert(sizeof(packet) == 20, "guild ladder packet size");
    if (m_payloadSize < sizeof(packet))
        return false;
    std::memcpy(&packet, data, sizeof(packet));
    sys_log(0, "Recv GuildLadder %lu %d / w %d d %d l %d",
            static_cast<unsigned long>(packet.id), packet.point, packet.wins, packet.draws, packet.losses);
    if (CGuild* guild = CGuildManager::instance().TouchGuild(packet.id)) {
        guild->SetLadderPoint(packet.point);
        guild->SetWarRecords(packet.wins, packet.draws, packet.losses);
    }
    return true;
}

bool CInputDB::GuildSkillUsableChange(const char* data)
{
    if (m_payloadSize < sizeof(TPacketGuildSkillUsableChange))
        return false;
    TPacketGuildSkillUsableChange packet;
    std::memcpy(&packet, data, sizeof(packet));
    if (CGuild* guild = CGuildManager::instance().TouchGuild(packet.dwGuild))
        guild->SkillUsableChange(packet.dwSkillVnum, packet.bUsable != 0);
    return true;
}

bool CInputDB::MarriageAdd(const char* data)
{
    if (m_payloadSize < sizeof(MarriageDbAdd))
        return false;
    MarriageDbAdd packet;
    std::memcpy(&packet, data, sizeof(packet));
    packet.firstName[24] = '\0';
    packet.secondName[24] = '\0';
    const DWORD first = packet.couple.first;
    const DWORD second = packet.couple.second;
    sys_log(0, "MarriageAdd %lu %lu %lu %s %s", first, second,
            static_cast<unsigned long>(static_cast<std::uint32_t>(packet.marriedAt)), packet.firstName, packet.secondName);
    marriage::CManager::instance().Add(first, second, packet.marriedAt,
                                       packet.firstName, packet.secondName);
    return true;
}

bool CInputDB::MarriageUpdate(const char* data)
{
    if (m_payloadSize < sizeof(MarriageDbUpdate))
        return false;
    MarriageDbUpdate packet;
    std::memcpy(&packet, data, sizeof(packet));
    const DWORD first = packet.couple.first;
    const DWORD second = packet.couple.second;
    sys_log(0, "MarriageUpdate %lu %lu %ld %d", first, second,
            static_cast<long>(packet.lovePoints), packet.married);
    marriage::CManager::instance().Update(first, second, packet.lovePoints, packet.married);
    return true;
}

bool CInputDB::WeddingReady(const char* data)
{
    if (m_payloadSize < sizeof(MarriageDbWeddingReady))
        return false;
    MarriageDbWeddingReady packet;
    std::memcpy(&packet, data, sizeof(packet));
    const DWORD first = packet.couple.first;
    const DWORD second = packet.couple.second;
    sys_log(0, "WeddingReady %lu %lu %lu", first, second, static_cast<unsigned long>(packet.map));
    marriage::CManager::instance().WeddingReady(first, second, packet.map);
    return true;
}

bool CInputDB::CreateObject(const char* data)
{
    building::TObject object;
    if (m_payloadSize < sizeof(object))
        return false;
    std::memcpy(&object, data, sizeof(object));
    building::CManager::instance().LoadObject(&object, false);
    return true;
}

bool CInputDB::DeleteObject(const char* data)
{
    std::uint32_t id;
    if (m_payloadSize < sizeof(id))
        return false;
    std::memcpy(&id, data, sizeof(id));
    building::CManager::instance().DeleteObject(id);
    return true;
}

bool CInputDB::UpdateLand(const char* data)
{
    building::TLand land;
    if (m_payloadSize < sizeof(land))
        return false;
    std::memcpy(&land, data, sizeof(land));
    building::CManager::instance().UpdateLand(&land);
    return true;
}

bool CInputDB::MarriageRemove(const char* data)
{
    if (m_payloadSize < sizeof(MarriageDbPair))
        return false;
    MarriageDbPair packet;
    std::memcpy(&packet, data, sizeof(packet));
    const DWORD first = packet.first;
    const DWORD second = packet.second;
    sys_log(0, "MarriageRemove %lu %lu", first, second);
    marriage::CManager::instance().Remove(first, second);
    return true;
}

bool CInputDB::WeddingRequest(const char* data)
{
    if (m_payloadSize < sizeof(MarriageDbPair))
        return false;
    MarriageDbPair packet;
    std::memcpy(&packet, data, sizeof(packet));
    const DWORD first = packet.first;
    const DWORD second = packet.second;
    marriage::WeddingManager::instance().Request(first, second);
    return true;
}

bool CInputDB::WeddingStart(const char* data)
{
    if (m_payloadSize < sizeof(MarriageDbPair))
        return false;
    MarriageDbPair packet;
    std::memcpy(&packet, data, sizeof(packet));
    const DWORD first = packet.first;
    const DWORD second = packet.second;
    sys_log(0, "WeddingStart %lu %lu", first, second);
    marriage::CManager::instance().WeddingStart(first, second);
    return true;
}

bool CInputDB::WeddingEnd(const char* data)
{
    if (m_payloadSize < sizeof(MarriageDbPair))
        return false;
    MarriageDbPair packet;
    std::memcpy(&packet, data, sizeof(packet));
    const DWORD first = packet.first;
    const DWORD second = packet.second;
    sys_log(0, "WeddingEnd %lu %lu", first, second);
    marriage::CManager::instance().WeddingEnd(first, second);
    return true;
}

bool CInputDB::ChangeEmpirePriv(const char* data)
{
    if (m_payloadSize < 11)
        return false;
    std::int32_t value, endTime;
    std::memcpy(&value, data + 1, sizeof(value));
    std::memcpy(&endTime, data + 7, sizeof(endTime));
    CPrivManager::instance().GiveEmpirePriv(static_cast<BYTE>(data[5]), static_cast<BYTE>(data[0]),
                                            value, static_cast<BYTE>(data[6]), endTime);
    return true;
}

bool CInputDB::ChangeGuildPriv(const char* data)
{
    if (m_payloadSize < 14)
        return false;
    std::int32_t value, endTime;
    std::uint32_t guild;
    std::memcpy(&value, data + 1, sizeof(value));
    std::memcpy(&guild, data + 5, sizeof(guild));
    std::memcpy(&endTime, data + 10, sizeof(endTime));
    CPrivManager::instance().GiveGuildPriv(guild, static_cast<BYTE>(data[0]), value,
                                           static_cast<BYTE>(data[9]), endTime);
    return true;
}

bool CInputDB::ChangeCharacterPriv(const char* data)
{
    if (m_payloadSize < 10)
        return false;
    std::int32_t value;
    std::uint32_t pid;
    std::memcpy(&value, data + 1, sizeof(value));
    std::memcpy(&pid, data + 5, sizeof(pid));
    CPrivManager::instance().GiveCharacterPriv(pid, static_cast<BYTE>(data[0]), value,
                                               static_cast<BYTE>(data[9]));
    return true;
}

bool CInputDB::MoneyLog(const char* data)
{
    if (m_payloadSize < 9)
        return false;
    const BYTE type = static_cast<BYTE>(data[0]);
    if (type != 4 && !g_bAuthServer) {
        std::uint32_t vnum;
        std::int32_t amount;
        std::memcpy(&vnum, data + 1, sizeof(vnum));
        std::memcpy(&amount, data + 5, sizeof(amount));
        LogManager::instance().MoneyLog(type, vnum, amount);
    }
    return true;
}

bool CInputDB::Notice(const char* data)
{
    char notice[257] = {};
    std::memcpy(notice, data, std::min(m_payloadSize, sizeof(notice) - 1));
    sys_log(0, "InputDB:: Notice: %s", notice);
    SendNotice(notice);
    return true;
}

bool CInputDB::BillingExpire(const char* data)
{
    constexpr std::size_t loginWidth = 32;
    std::uint32_t seconds;
    if (m_payloadSize < loginWidth + sizeof(seconds) || !std::memchr(data, 0, loginWidth))
        return false;
    std::memcpy(&seconds, data + loginWidth, sizeof(seconds));
    if (DESC* desc = DESC_MANAGER::instance().FindByLoginName(data)) {
        if (seconds <= 60) {
            sys_log(0, "BILLING_EXPIRE: %s %lu", data, static_cast<unsigned long>(seconds));
            desc->DelayedDisconnect(std::max<std::uint32_t>(5, seconds));
        } else if (seconds - static_cast<std::uint32_t>(desc->GetBillingExpireSecond()) > 60) {
            desc->SetBillingExpireSecond(seconds);
            if (CHARACTER* character = desc->GetCharacter()) {
                character->ChatPacket(1, LC_TEXT("\260\341\300\347\261\342\260\243\300\314 %d\272\320 \310\304 \270\270\267\341 \265\313\264\317\264\331."),
                                      static_cast<int>(seconds / 60));
            }
        }
    }
    return true;
}

bool CInputDB::BillingCheck(const char* data)
{
    std::uint32_t count;
    if (m_payloadSize < sizeof(count))
        return false;
    std::memcpy(&count, data, sizeof(count));
    if (count > (m_payloadSize - sizeof(count)) / sizeof(std::uint32_t))
        return false;
    const char* entry = data + sizeof(count);
    for (std::uint32_t i = 0; i < count; ++i, entry += sizeof(std::uint32_t)) {
        std::uint32_t key;
        std::memcpy(&key, entry, sizeof(key));
        sys_log(0, "BILLING: NOT_LOGIN %lu", static_cast<unsigned long>(key));
        DBManager::instance().SetBilling(key, false, true);
    }
    return true;
}

bool CInputDB::MyshopPricelistRes(const char* data)
{
    constexpr std::size_t headerSize = 5;
    constexpr std::size_t entrySize = 8;
    if (m_payloadSize < headerSize)
        return false;
    const BYTE count = static_cast<BYTE>(data[4]);
    if (count > SHOP_PRICELIST_MAX_NUM ||
        m_payloadSize < headerSize + static_cast<std::size_t>(count) * entrySize)
        return false;
    if (DESC* desc = DESC_MANAGER::instance().FindByHandle(m_dwHandle)) {
        if (CHARACTER* character = desc->GetCharacter()) {
            sys_log(0, "RecvMyshopPricelistRes name[%s]", character->GetName());
            character->UseSilkBotaryReal(data + headerSize, count);
        }
    }
    return true;
}

bool CInputDB::AddBlockCountryIp(const char* data)
{
    TPacketBlockCountryIp packet;
    if (m_payloadSize < sizeof(packet))
        return false;
    std::memcpy(&packet, data, sizeof(packet));
    add_blocked_country_ip(&packet);
    return true;
}

bool CInputDB::BlockException(const char* data)
{
    TPacketBlockException packet;
    if (m_payloadSize < sizeof(packet))
        return false;
    std::memcpy(&packet, data, sizeof(packet));
    packet.login[sizeof(packet.login) - 1] = '\0';
    block_exception(&packet);
    return true;
}

bool CInputDB::Time(const char* data)
{
    std::int32_t serverTime;
    if (m_payloadSize < sizeof(serverTime))
        return false;
    std::memcpy(&serverTime, data, sizeof(serverTime));
    set_global_time(serverTime);
    return true;
}

void CInputDB::Boot(const char* data)
{
    signal_timer_disable();
    BootReader packet(data, m_payloadSize);
    std::uint32_t declaredSize;
    BYTE version;
    if (!packet.Read(declaredSize) || !packet.Read(version)) {
        _sys_err("Boot", 0x26f, "truncated boot header");
        thecore_shutdown();
        return;
    }
    sys_log(0, "BOOT: PACKET: %d", static_cast<int>(declaredSize));
    sys_log(0, "BOOT: VERSION: %d", version);
    if (declaredSize != m_payloadSize) {
        _sys_err("Boot", 0x26f, "boot size mismatch declared %lu received %lu",
                 static_cast<unsigned long>(declaredSize), static_cast<unsigned long>(m_payloadSize));
        thecore_shutdown();
        return;
    }
    if (version != 5) {
        _sys_err("Boot", 0x26f, "boot version error");
        thecore_shutdown();
        return;
    }

    sys_log(0, "sizeof(TMobTable) = %d", static_cast<int>(sizeof(TMobTable)));
    sys_log(0, "sizeof(TItemTable) = %d", static_cast<int>(sizeof(TItemTable)));
    sys_log(0, "sizeof(TShopTable) = %d", static_cast<int>(sizeof(TShopTable)));
    sys_log(0, "sizeof(TSkillTable) = %d", static_cast<int>(sizeof(TSkillTable)));
    sys_log(0, "sizeof(TRefineTable) = %d", static_cast<int>(sizeof(TRefineTable)));
    sys_log(0, "sizeof(TItemAttrTable) = %d", static_cast<int>(sizeof(TItemAttrTable)));
    sys_log(0, "sizeof(TItemRareTable) = %d", static_cast<int>(sizeof(TItemAttrTable)));
    sys_log(0, "sizeof(TBanwordTable) = %d", static_cast<int>(sizeof(TBanwordTable)));
    sys_log(0, "sizeof(TLand) = %d", static_cast<int>(sizeof(building::TLand)));
    sys_log(0, "sizeof(TObjectProto) = %d", static_cast<int>(sizeof(building::TObjectProto)));
    sys_log(0, "sizeof(TObject) = %d", static_cast<int>(sizeof(building::TObject)));
    sys_log(0, "sizeof(TAdminManager) = %d", static_cast<int>(sizeof(tAdminInfo)));

    std::uint16_t count;
    TMobTable* mobs;
    TItemTable* items;
    TShopTable* shops;
    TSkillTable* skills;
    TRefineTable* recipes;
    TBanwordTable* words;
    building::TLand* lands;
    building::TObjectProto* prototypes;
    building::TObject* objects;

    const bool tablesLoaded = [&]() {
        if (!packet.Table(mobs, count, "mob", 0x28a))
            return false;
        sys_log(0, "BOOT: MOB: %d", count);
        if (count)
            CMobManager::instance().Initialize(mobs, count);
        if (!packet.Table(items, count, "item", 0x2a0))
            return false;
        sys_log(0, "BOOT: ITEM: %d", count);
        if (count)
            ITEM_MANAGER::instance().Initialize(items, count);
        if (!packet.Table(shops, count, "shop", 0x2b7))
            return false;
        sys_log(0, "BOOT: SHOP: %d", count);
        if (count)
            CShopManager::instance().Initialize(shops, count);
        if (!packet.Table(skills, count, "skill", 0x2ce))
            return false;
        sys_log(0, "BOOT: SKILL: %d", count);
        if (count && !CSkillManager::instance().Initialize(skills, count)) {
            _sys_err("Boot", 0x2dc, "cannot initialize skill table");
            return false;
        }
        if (!packet.Table(recipes, count, "refine", 0x2e8))
            return false;
        sys_log(0, "BOOT: REFINE: %d", count);
        if (count)
            CRefineManager::instance().Initialize(recipes, count);
        if (!ReadItemAttributes(packet, false) || !ReadItemAttributes(packet, true))
            return false;
        if (!packet.Table(words, count, "ban word", 0x33e))
            return false;
        CBanwordManager::instance().Initialize(words, count);

        building::CManager& buildings = building::CManager::instance();
        if (!packet.Table(lands, count, "land", 0x353))
            return false;
        for (unsigned i = 0; i < count; ++i)
            buildings.LoadLand(&lands[i]);
        if (!packet.Table(prototypes, count, "object proto", 0x368))
            return false;
        buildings.LoadObjectProto(prototypes, count);
        if (!packet.Table(objects, count, "object", 0x379))
            return false;
        for (unsigned i = 0; i < count; ++i)
            buildings.LoadObject(&objects[i], true);
        return true;
    }();
    if (!tablesLoaded) {
        thecore_shutdown();
        return;
    }

    std::int32_t serverTime;
    std::uint32_t maxItemId;
    std::uint16_t hostWidth;
    if (!packet.Read(serverTime) || !packet.Read(maxItemId) || !packet.Read(hostWidth) || !packet.Read(count)) {
        _sys_err("Boot", 0x3d6, "truncated boot metadata");
        thecore_shutdown();
        return;
    }
    set_global_time(serverTime);
    sys_log(0, "GM Value Count %d %d", count, hostWidth);
    for (unsigned i = 0; i < count; ++i) {
        const char* host = packet.Take(hostWidth);
        if (!host || !std::memchr(host, '\0', hostWidth)) {
            _sys_err("Boot", 0x3d6, "invalid GM host record");
            thecore_shutdown();
            return;
        }
        gm_new_host_inert(host);
        sys_log(0, "GM HOST : IP[%s] ", host);
    }

    tAdminInfo* admins;
    if (!packet.Table(admins, count, "admin", 0x3d6)) {
        thecore_shutdown();
        return;
    }
    for (unsigned i = 0; i < count; ++i)
        gm_new_insert(admins[i]);

    // Ignore the optional legacy trailer and validate the last word of the frame.
    std::uint16_t marker = 0;
    if (!packet.ReadEnd(marker) || marker != 0xffff) {
        _sys_err("Boot", 0x3d6, "boot packet end check error [%x]!=0xffff", marker);
        thecore_shutdown();
        return;
    }
    sys_log(0, "boot packet end check ok [%x]==0xffff", marker);
    FinishBoot(maxItemId);
}

// 080e7000
bool GetServerLocation(TAccountTable& account, BYTE)
{
    bool hasCharacter = false;
    for (TSimplePlayer& player : account.players) {
        if (!player.dwID)
            continue;
        hasCharacter = true;
        long index = 0;
        long address = player.lAddr;
        WORD port = player.wPort;
        CMapLocation& locations = CMapLocation::instance();
        if (!locations.Get(player.x, player.y, index, address, port)) {
            _sys_err("GetServerLocation", 0x48, "location error name %s mapindex %d %d x %d empire %d",
                     player.szName, static_cast<int>(index), static_cast<int>(player.x),
                     static_cast<int>(player.y), account.bEmpire);
            const unsigned empire = account.bEmpire < 4 ? account.bEmpire : 0;
            player.x = g_start_position[empire][0];
            player.y = g_start_position[empire][1];
            index = 0;
            if (!locations.Get(player.x, player.y, index, address, port)) {
                _sys_err("GetServerLocation", 0x55, "cannot find server for mapindex %d %d x %d (name %s)",
                         static_cast<int>(index), static_cast<int>(player.x), static_cast<int>(player.y), player.szName);
                continue;
            }
        }
        player.lAddr = address;
        player.wPort = port;
        in_addr ip;
        ip.s_addr = address;
        sys_log(0, "success to %s:%d", inet_ntoa(ip), port);
    }
    return hasCharacter;
}

void CInputDB::LoginSuccess(DWORD handle, const char* data)
{
    TAccountTable account;
    std::memcpy(&account, data, sizeof(account));
    account.login[sizeof(account.login) - 1] = 0;
    account.status[sizeof(account.status) - 1] = 0;
    sys_log(0, "LoginSuccess");

    const auto simulator = g_sim.find(handle);
    if (simulator != g_sim.end()) {
        sys_log(0, "CInputDB::LoginSuccess - already exist sim [%s]", account.login);
        db_clientdesc->DBPacket(3, 0, &simulator->second->load, sizeof(simulator->second->load));
        return;
    }

    DESC* desc = DESC_MANAGER::instance().FindByHandle(handle);
    const bool ok = std::strcmp(account.status, "OK") == 0;
    if (!desc || !ok) {
        if (desc)
            sys_log(0, "CInputDB::LoginSuccess - status[%s] is not OK [%s]", account.status, account.login);
        else
            sys_log(0, "CInputDB::LoginSuccess - cannot find handle [%s]", account.login);
        TLogoutPacket logout = {};
        std::strncpy(logout.login, account.login, sizeof(logout.login) - 1);
        db_clientdesc->DBPacket(2, handle, &logout, sizeof(logout));
        if (desc)
            LoginFailure(desc, account.status);
        return;
    }

    sys_log(0, "login(%s).empire(%d)", account.login, account.bEmpire);
    for (TSimplePlayer& player : account.players) {
        player.szName[sizeof(player.szName) - 1] = 0;
        sys_log(0, "\tplayer(%s).job(%d)", player.szName, player.byJob);
    }
    const bool hasCharacter = GetServerLocation(account, account.bEmpire);
    desc->BindAccountTable(&account);
    const BYTE empire = hasCharacter ? desc->GetEmpire()
        : number_ex(1, 3, const_cast<char*>("input_db.cpp"), 0xa3);
    const BYTE packet[] = { 0x5a, empire };
    desc->Packet(packet, sizeof(packet));
    desc->SetPhase(3);
    desc->SendLoginSuccessPacket();
    sys_log(0, "InputDB::login_success: %s", account.login);
}

void CInputDB::PlayerLoad(DESC* desc, const char* data)
{
    static_assert(sizeof(TPlayerTable) == 0x71a, "2009 DB player load record");
    if (!desc)
        return;

    TPlayerTable player;
    std::memcpy(&player, data, sizeof(player));
    player.name[sizeof(player.name) - 1] = 0;
    const unsigned empire = desc->GetEmpire() < 4 ? desc->GetEmpire() : 0;
    static const long startMap[] = {0, 1, 21, 41};
    SECTREE_MANAGER& maps = SECTREE_MANAGER::instance();
    if (!player.lMapIndex) {
        player.lMapIndex = maps.GetMapIndex(player.x, player.y);
        if (!player.lMapIndex)
            player.lMapIndex = startMap[empire];
    }

    long map;
    PIXEL_POSITION position;
    if (!maps.GetValidLocation(player.lMapIndex, player.x, player.y, map, position)) {
        _sys_err("PlayerLoad", 0x1f6, "InputDB::PlayerLoad : cannot find valid location %d x %d (name: %s)",
                 static_cast<int>(player.x), static_cast<int>(player.y), player.name);
        desc->SetPhase(0);
        return;
    }
    player.lMapIndex = map;
    player.x = position.x;
    player.y = position.y;

    CHARACTER* current = desc->GetCharacter();
    if (current || desc->IsPhase(5)) {
        _sys_err("PlayerLoad", 0x201, "login state already has main state (character %s %p)",
                 current ? current->GetName() : player.name, static_cast<void*>(current));
        return;
    }

    CHARACTER_MANAGER& characters = CHARACTER_MANAGER::instance();
    if (characters.FindPC(player.name)) {
        _sys_err("PlayerLoad", 0x207, "InputDB: PlayerLoad : %s already exist in game", player.name);
        return;
    }

    CHARACTER* character = characters.CreateCharacter(player.name, player.id);
    character->BindDesc(desc);
    character->SetPlayerProto(&player);
    character->SetEmpire(desc->GetEmpire());
    desc->BindCharacter(character);

    TPacketGGLogin login = {};
    login.bHeader = 1;
    std::strncpy(login.szName, character->GetName(), sizeof(login.szName) - 1);
    login.dwPID = character->GetPlayerID();
    login.bEmpire = character->GetEmpire();
    login.bMapIndex = maps.GetMapIndex(character->GetX(), character->GetY());
    login.bChannel = g_bChannel;
    P2P_MANAGER::instance().Send(&login, sizeof(login));

    char details[51];
    ::snprintf(details, sizeof(details), "%s %ld %d %ld %d", inet_ntoa(desc->GetAddr().sin_addr),
               static_cast<long>(character->GetGold()), g_bChannel, character->GetMapIndex(), character->GetAlignment());
    LogManager& log = LogManager::instance();
    log.CharLog(character->GetPlayerID(), character->GetX(), character->GetY(), 0, "LOGIN", details);
    if (LC_IsYMIR() || LC_IsKorea() || LC_IsBrazil()) {
        log.LoginLog(true, desc->GetAccountTable().id, character->GetPlayerID(), character->GetLevel(),
                     character->GetJob(), character->GetRealPoint(31));
        if (LC_IsYMIR() || LC_IsKorea())
            character->m_isinPCBang = IsPCBangIP(desc->GetHostName());
    }

    desc->SetPhase(4);
    character->MainCharacterPacket();
    if (!map_allow_find(map > 9999 ? map / 10000 : map)) {
        _sys_err("PlayerLoad", 0x23a, "InputDB::PlayerLoad : entering %d map is not allowed here (name: %s, empire %u)",
                 static_cast<int>(map), player.name, static_cast<unsigned>(desc->GetEmpire()));
        character->SetWarpLocation(startMap[empire], g_start_position[empire][0] / 100, g_start_position[empire][1] / 100);
        desc->SetPhase(0);
        return;
    }
    quest::CQuestManager::instance().BroadcastEventFlagOnLogin(character);
    for (BYTE i = 0; i < 36; ++i)
        character->SetQuickslot(i, player.quickslot[i]);
    character->PointsPacket();
    character->SkillLevelPacket();
    sys_log(0, "InputDB: player_load %s %dx%dx%d LEVEL %d MOV_SPEED %d JOB %d ATG %d DFG %d GMLv %d",
            player.name, static_cast<int>(character->GetX()), static_cast<int>(character->GetY()),
            static_cast<int>(character->GetZ()), character->GetLevel(), static_cast<int>(character->GetPoint(19)),
            character->GetJob(), static_cast<int>(character->GetPoint(18)), static_cast<int>(character->GetPoint(16)),
            character->GetGMLevel());
    character->QuerySafeboxSize();
}

void CInputDB::PlayerCreateSuccess(DESC* desc, const char* data)
{
    if (!desc)
        return;

    TPacketDGCreateSuccess reply;
    std::memcpy(&reply, data, sizeof(reply));
    const BYTE slot = reply.bAccountCharacterIndex;
    if (slot >= 4) {
        const BYTE failure = 9;
        desc->Packet(&failure, sizeof(failure));
        return;
    }

    TSimplePlayer& player = reply.player;
    long mapIndex = 0;
    long address = player.lAddr;
    WORD port = player.wPort;
    if (CMapLocation::instance().Get(player.x, player.y, mapIndex, address, port)) {
        player.lAddr = address;
        player.wPort = port;
    } else {
        _sys_err("PlayerCreateSuccess", 0xdb,
                 "InputDB::PlayerCreateSuccess: cannot find server for mapindex %d %d x %d (name %.25s)",
                 static_cast<int>(mapIndex), static_cast<int>(player.x), static_cast<int>(player.y), player.szName);
    }

    desc->GetAccountTable().players[slot] = player;
    const TPacketGCPlayerCreateSuccess packet = {8, slot, player};
    desc->Packet(&packet, sizeof(packet));

    if (china_event_server && player.byJob < 4) {
        static const struct StarterItem {
            DWORD vnum;
            WORD position;
        } starterItems[4][9] = {
            {{11243, 2}, {12223, 3}, {15103, 4}, {93, 1}, {16143, 8}, {17103, 9}, {3083, 0}, {13193, 11}, {14103, 12}},
            {{11443, 0}, {12363, 3}, {15103, 4}, {1053, 2}, {2083, 1}, {16083, 7}, {17083, 8}, {13193, 9}, {14103, 10}},
            {{11643, 0}, {12503, 2}, {15103, 3}, {93, 1}, {16123, 4}, {17143, 7}, {13193, 8}, {14103, 9}, {0, 0}},
            {{11843, 0}, {12643, 1}, {15103, 2}, {7083, 3}, {5053, 4}, {16123, 6}, {17143, 7}, {13193, 8}, {14103, 9}},
        };
        TPlayerItem item = {};
        item.window = 1;
        item.count = 1;
        item.owner = player.dwID;
        for (const StarterItem& entry : starterItems[player.byJob]) {
            if (!entry.vnum)
                continue;
            item.id = ITEM_MANAGER::instance().GetNewID();
            item.pos = entry.position;
            item.vnum = entry.vnum;
            db_clientdesc->DBPacketHeader(0x1e, 0, sizeof(item));
            db_clientdesc->Packet(&item, sizeof(item));
        }
    }
    sys_log(0, "InputDB::PlayerCreateSuccess: %.25s", player.szName);
}

void CInputDB::PlayerCreateFailure(DESC* desc, BYTE type)
{
    if (!desc)
        return;

    const TPacketGCCreateFailure packet = {9, type};
    desc->Packet(&packet, sizeof(packet));
}

void CInputDB::PlayerDeleteSuccess(DESC* desc, const char* data)
{
    if (!desc)
        return;

    const BYTE slot = static_cast<BYTE>(data[0]);
    const BYTE header = 10;
    desc->BufferedPacket(&header, sizeof(header));
    desc->Packet(&slot, sizeof(slot));
    desc->GetAccountTable().players[slot].dwID = 0;
}

void CInputDB::PlayerDeleteFail(DESC* desc)
{
    if (!desc)
        return;

    const BYTE failure = 11;
    desc->Packet(&failure, sizeof(failure));
}

bool CInputDB::SafeboxLoad(DESC* desc, const char* data)
{
    return LoadStorage(desc, data, false);
}

bool CInputDB::MallLoad(DESC* desc, const char* data)
{
    return LoadStorage(desc, data, true);
}

bool CInputDB::SafeboxChangeSize(DESC* desc, const char* data)
{
    if (!m_payloadSize)
        return false;
    CHARACTER* character = desc ? desc->GetCharacter() : nullptr;
    if (character)
        character->ChangeSafeboxSize(static_cast<BYTE>(data[0]));
    return true;
}

void CInputDB::SafeboxWrongPassword(DESC* desc)
{
    CHARACTER* character = desc ? desc->GetCharacter() : nullptr;
    if (character) {
        const BYTE packet = 0x57;
        desc->Packet(&packet, sizeof(packet));
        character->m_bOpeningSafebox = false;
    }
}

bool CInputDB::SafeboxChangePasswordAnswer(DESC* desc, const char* data)
{
    if (!m_payloadSize)
        return false;
    CHARACTER* character = desc ? desc->GetCharacter() : nullptr;
    if (character) {
        const char* message = data[0]
            ? LC_TEXT("<\303\242\260\355> \303\242\260\355 \272\361\271\320\271\370\310\243\260\241 \272\257\260\346\265\307\276\372\275\300\264\317\264\331.")
            : LC_TEXT("<\303\242\260\355> \261\342\301\270 \272\361\271\320\271\370\310\243\260\241 \306\262\267\310\275\300\264\317\264\331.");
        character->ChatPacket(1, "%s", message);
    }
    return true;
}

bool CInputDB::LoadStorage(DESC* desc, const char* data, bool mall)
{
    if (m_payloadSize < sizeof(TSafeboxTable))
        return false;

    TSafeboxTable table;
    std::memcpy(&table, data, sizeof(table));
    const auto count = table.wItemCount;
    if (count > (m_payloadSize - sizeof(table)) / sizeof(TPlayerItem))
        return false;

    if (!desc)
        return true;
    const DWORD account = desc->GetAccountTable().id;
    if (account != table.dwID) {
        if (mall)
            _sys_err("MallLoad", 0x517, "safebox has different id %lu != %lu", account, static_cast<unsigned long>(table.dwID));
        else
            _sys_err("SafeboxLoad", 0x4bb, "SafeboxLoad: safebox has different id %lu != %lu", account, static_cast<unsigned long>(table.dwID));
        return true;
    }

    CHARACTER* character = desc->GetCharacter();
    if (!character)
        return true;
    if (!mall && (character->GetShopOwner() || character->GetExchange() ||
                  character->GetMyShop() || character->IsCubeOpen())) {
        character->ChatPacket(1, "%s", LC_TEXT("\264\331\270\245\260\305\267\241\303\242\300\314 \277\255\270\260\273\363\305\302\277\241\274\255\264\302 \303\242\260\355\270\246 \277\255\274\366\260\241 \276\370\275\300\264\317\264\331."));
        character->m_bOpeningSafebox = false;
        return true;
    }

    std::vector<TPlayerItem> items(count);
    if (count)
        std::memcpy(items.data(), data + sizeof(table), count * sizeof(TPlayerItem));
    if (mall) {
        character->LoadMall(count, items.data());
    } else {
        const int size = character->GetPremiumRemainSeconds(2) > 0 ||
                         character->IsEquipUniqueGroup(0x2725) ? 27 : 9;
        character->LoadSafebox(size, table.dwGold, count, items.data());
    }
    return true;
}

void CInputDB::EmpireSelect(DESC* desc, const char* data)
{
    sys_log(0, "EmpireSelect %p", static_cast<void*>(desc));
    if (!desc)
        return;

    TAccountTable& account = desc->GetAccountTable();
    account.bEmpire = static_cast<BYTE>(data[0]);
    const BYTE packet[2] = {0x5a, account.bEmpire};
    desc->Packet(packet, sizeof(packet));

    const unsigned empire = account.bEmpire < 4 ? account.bEmpire : 0;
    for (TSimplePlayer& player : account.players) {
        if (player.dwID) {
            player.x = g_start_position[empire][0];
            player.y = g_start_position[empire][1];
        }
    }
    GetServerLocation(account, account.bEmpire);
    desc->SendLoginSuccessPacket();
}

void CInputDB::ChangeName(DESC* desc, const char* data)
{
    if (!desc || !desc->GetAccountTable().id)
        return;

    TPacketDGChangeName reply;
    std::memcpy(&reply, data, sizeof(reply));
    for (TSimplePlayer& player : desc->GetAccountTable().players) {
        if (player.dwID != reply.pid)
            continue;

        std::strncpy(player.szName, reply.name, sizeof(player.szName) - 1);
        player.szName[sizeof(player.szName) - 1] = '\0';
        player.bChangeName = 0;

        TPacketGCChangeName packet = {0x6b, reply.pid, {}};
        std::strncpy(packet.name, reply.name, sizeof(packet.name) - 1);
        desc->Packet(&packet, sizeof(packet));
        return;
    }
}

EVENTFUNC(quest_login_event)
{
    const auto pid = *static_cast<const std::uint32_t*>(event->info);
    CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(pid);
    DESC* desc = character ? character->GetDesc() : nullptr;
    if (!desc || desc->IsPhase(0))
        return 0;

    // The handshake, login, selection, loading and dead phases retry.
    if (desc->IsPhase(1) || desc->IsPhase(2) || desc->IsPhase(3)
        || desc->IsPhase(4) || desc->IsPhase(6))
        return passes_per_sec;

    if (desc->IsPhase(5)) {
        sys_log(0, "QUEST_LOAD: Login pc %d by event", static_cast<int>(pid));
        quest::CQuestManager::instance().Login(pid);
    } else {
        _sys_err("quest_login_event", 0x46a, "input_db.cpp:quest_login_event INVALID PHASE pid %d",
                 static_cast<int>(pid));
    }
    return 0;
}

bool CInputDB::ItemLoad(DESC* desc, const char* data)
{
    static_assert(sizeof(TPlayerItem) == 52, "2009 DB player item record");
    CHARACTER* character = desc ? desc->GetCharacter() : nullptr;
    if (!character || character->m_bItemLoaded)
        return true;

    std::uint32_t count;
    if (m_payloadSize < sizeof(count))
        return false;
    std::memcpy(&count, data, sizeof(count));
    if (count > (m_payloadSize - sizeof(count)) / sizeof(TPlayerItem))
        return false;
    data += sizeof(count);

    sys_log(0, "ITEM_LOAD: COUNT %s %lu", character->GetName(), static_cast<unsigned long>(count));
    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    std::vector<CItem*> displaced;
    for (std::uint32_t index = 0; index < count; ++index, data += sizeof(TPlayerItem)) {
        TPlayerItem row;
        std::memcpy(&row, data, sizeof(row));
        const bool inventory = row.window == 1;
        const bool wear = row.window == 2;
        if (!inventory && !wear)
            continue;
        CItem* item = items.CreateItem(row.vnum, row.count, row.id, false, -1);
        if (!item) {
            _sys_err("ItemLoad", 0x625, "cannot create item by vnum %lu (name %s id %lu)",
                     static_cast<unsigned long>(row.vnum), character->GetName(), static_cast<unsigned long>(row.id));
            continue;
        }

        sys_log(0, "ITEM_LOAD: %s %s:%lu %d", character->GetName(), item->GetName(), item->GetID(),
                static_cast<int>(item->GetCount()));
        item->SetSkipSave(true);
        long sockets[] = {row.alSockets[0], row.alSockets[1], row.alSockets[2]};
        item->SetSockets(sockets);
        item->SetAttributes(row.aAttr);

        const bool occupied = inventory
            ? row.pos >= 90 || !character->IsEmptyItemGrid(static_cast<BYTE>(row.pos), item->GetSize(), -1)
            : row.pos >= 19 || character->GetWear(static_cast<BYTE>(row.pos)) != nullptr;
        if (occupied) {
            sys_log(0, "ITEM_RESTORE: %s %s", character->GetName(), item->GetName());
            displaced.push_back(item);
        } else if (wear && !item->EquipTo(character, static_cast<BYTE>(row.pos))) {
            _sys_err("ItemLoad", 0x640, "cannot equip %s to %s", item->GetName(), character->GetName());
            items.DestroyItem(item);
            continue;
        } else if (inventory && !item->AddToCharacter(character, static_cast<BYTE>(row.pos))) {
            displaced.push_back(item);
        }
        item->SetSkipSave(false);
    }

    for (CItem* item : displaced) {
        const int slot = character->GetEmptyInventory(item->GetSize());
        if (slot < 0 || !item->AddToCharacter(character, static_cast<BYTE>(slot))) {
            _sys_err("ItemLoad", 0x654, "cannot give %s to %s", item->GetName(), character->GetName());
            items.DestroyItem(item);
        }
    }

    character->CheckMaximumPoints();
    character->PointsPacket();
    character->m_bItemLoaded = true;
    return true;
}

bool CInputDB::QuestLoad(DESC* desc, const char* data)
{
    std::uint32_t count;
    if (m_payloadSize < sizeof(count))
        return false;
    std::memcpy(&count, data, sizeof(count));
    if (count > (m_payloadSize - sizeof(count)) / sizeof(TQuestTable))
        return false;

    CHARACTER* character = desc ? desc->GetCharacter() : nullptr;
    if (!character)
        return true;

    const auto pid = character->GetPlayerID();
    static std::vector<TQuestTable> rows;
    rows.resize(count);
    if (count) {
        std::memcpy(rows.data(), data + sizeof(count), count * sizeof(TQuestTable));
        if (pid != rows.front().dwPID) {
            _sys_err("QuestLoad", 0x482, "PID differs %lu %lu",
                     static_cast<unsigned long>(pid), static_cast<unsigned long>(rows.front().dwPID));
            return true;
        }
    }

    sys_log(0, "QUEST_LOAD: count %d", static_cast<int>(count));
    quest::CQuestManager& manager = quest::CQuestManager::instance();
    quest::PC* pc = manager.GetPCForce(pid);
    if (!pc) {
        _sys_err("QuestLoad", 0x48d, "null quest::PC with id %lu", static_cast<unsigned long>(pid));
        return true;
    }
    if (pc->IsLoaded())
        return true;

    for (const TQuestTable& row : rows) {
        std::string flag(row.szName, std::find(row.szName, row.szName + sizeof(row.szName), '\0'));
        flag += '.';
        flag.append(row.szState, std::find(row.szState, row.szState + sizeof(row.szState), '\0'));
        sys_log(0, "            %s %d", flag.c_str(), row.lValue);
        pc->SetFlag(flag, row.lValue, false);
    }
    pc->SetLoaded();
    pc->Build();

    if (desc->IsPhase(5)) {
        sys_log(0, "QUEST_LOAD: Login pc %d", static_cast<int>(pid));
        manager.Login(pid);
    } else {
        void* eventData;
        const DWORD info = __AllocEventInfo(sizeof(std::uint32_t), &eventData);
        *static_cast<std::uint32_t*>(eventData) = pid;
        event_create_ex(quest_login_event, info, passes_per_sec, const_cast<char*>("input_db.cpp"), 0x4ae);
    }
    return true;
}

// 080eaa30
void CInputDB::GuildAddMember(const char* data)
{
    TPacketDGGuildMember packet;
    std::memcpy(&packet, data, sizeof(packet));
    packet.szName[sizeof(packet.szName) - 1] = '\0';
    if (CGuild* guild = CGuildManager::instance().TouchGuild(packet.dwGuild))
        guild->AddMember(&packet);
}

// 080eaab0
void CInputDB::GuildRemoveMember(const char* data)
{
    TPacketGuild packet;
    std::memcpy(&packet, data, sizeof(packet));
    if (CGuild* guild = CGuildManager::instance().TouchGuild(packet.dwGuild))
        guild->RemoveMember(packet.dwInfo);
}

// 080eac50
void CInputDB::GuildDisband(const char* data)
{
    std::uint32_t guildId;
    std::memcpy(&guildId, data, sizeof(guildId));
    CGuildManager::instance().DisbandGuild(guildId);
}

// 080eabb0
void CInputDB::GuildChangeMemberData(const char* data)
{
    sys_log(0, "Recv GuildChangeMemberData");
    TPacketGuildChangeMemberData packet;
    std::memcpy(&packet, data, sizeof(packet));
    if (CGuild* guild = CGuildManager::instance().TouchGuild(packet.guild_id))
        guild->ChangeMemberData(packet.pid, packet.offer, packet.level, packet.grade);
}

// 080eb250
bool CInputDB::AffectLoad(DESC* desc, const char* data)
{
    static_assert(sizeof(TPacketAffectElement) == 21, "2009 DB affect record");
    std::uint32_t fields[2];
    if (m_payloadSize < sizeof(fields))
        return false;
    std::memcpy(fields, data, sizeof(fields));
    const std::uint32_t count = fields[1];
    if (count > (m_payloadSize - sizeof(fields)) / sizeof(TPacketAffectElement))
        return false;

    CHARACTER* character = desc ? desc->GetCharacter() : nullptr;
    if (!character || character->GetPlayerID() != fields[0])
        return true;

    std::vector<TPacketAffectElement> elements(count);
    if (count)
        std::memcpy(elements.data(), data + sizeof(fields), count * sizeof(TPacketAffectElement));
    character->LoadAffect(count, elements.data());
    return true;
}

// 080ed020
bool CInputDB::ReloadAdmin(const char* data)
{
    BootReader packet(data, m_payloadSize);
    std::uint16_t hostWidth;
    std::uint16_t hostCount;
    if (!packet.Read(hostWidth) || !packet.Read(hostCount) || hostWidth != 16)
        return false;
    const char* hosts = packet.Take(static_cast<std::size_t>(hostWidth) * hostCount);
    if (!hosts)
        return false;
    for (unsigned i = 0; i < hostCount; ++i) {
        if (!std::memchr(hosts + i * hostWidth, '\0', hostWidth))
            return false;
    }

    std::uint16_t adminWidth;
    std::uint16_t adminCount;
    if (!packet.Read(adminWidth) || !packet.Read(adminCount) || adminWidth != sizeof(tAdminInfo))
        return false;
    const char* entries = packet.Take(static_cast<std::size_t>(adminWidth) * adminCount);
    if (!entries)
        return false;

    std::vector<tAdminInfo> admins(adminCount);
    if (adminCount)
        std::memcpy(admins.data(), entries, adminWidth * static_cast<std::size_t>(adminCount));
    for (const auto& admin : admins) {
        if (!std::memchr(admin.m_szAccount, '\0', sizeof(admin.m_szAccount)) ||
            !std::memchr(admin.m_szName, '\0', sizeof(admin.m_szName)) ||
            !std::memchr(admin.m_szContactIP, '\0', sizeof(admin.m_szContactIP)) ||
            !std::memchr(admin.m_szServerIP, '\0', sizeof(admin.m_szServerIP)))
            return false;
    }

    gm_new_clear();
    for (unsigned i = 0; i < hostCount; ++i)
        gm_new_host_inert(hosts + i * hostWidth);
    for (const auto& admin : admins)
        gm_new_insert(admin);

    // Refresh cached levels for removed admins as well as entries in the new list.
    CHARACTER_MANAGER::instance().for_each_pc([](CHARACTER* character) {
        if (character->GetDesc())
            character->SetGMLevel();
    });
    return true;
}

// 080ebd80
bool CInputDB::BillingRepair(const char* data)
{
    constexpr std::size_t repairSize = 51;
    constexpr std::size_t loginWidth = 31;
    constexpr std::size_t hostWidth = 16;
    BootReader packet(data, m_payloadSize);
    std::uint32_t count;
    if (!packet.Read(count) || count > (m_payloadSize - sizeof(count)) / repairSize)
        return false;

    for (std::uint32_t i = 0; i < count; ++i) {
        const char* entry = packet.Take(repairSize);
        const char* login = entry + sizeof(std::uint32_t);
        const char* host = login + loginWidth;
        sys_log(0, "BILLING: REPAIR %.*s host %.*s",
                static_cast<int>(loginWidth), login, static_cast<int>(hostWidth), host);
    }
    return true;
}

// 080ebf90
bool CInputDB::BillingLogin(const char* data)
{
    constexpr std::size_t loginSize = 5;
    BootReader packet(data, m_payloadSize);
    std::uint32_t count;
    if (!packet.Read(count) || count > (m_payloadSize - sizeof(count)) / loginSize)
        return false;

    DBManager& billing = DBManager::instance();
    for (std::uint32_t i = 0; i < count; ++i) {
        std::uint32_t key;
        packet.Read(key);
        const bool enabled = *packet.Take(1) != '\0';
        billing.SetBilling(key, enabled, false);
    }
    return true;
}

// 080ec120
bool CInputDB::VCard(const char* data)
{
    static_assert(sizeof(TPacketGDVCard) == 116, "2009 vcard wire record");
    TPacketGDVCard card;
    if (m_payloadSize < sizeof(card))
        return false;
    std::memcpy(&card, data, sizeof(card));
    if (!std::memchr(card.szSellCharacter, '\0', sizeof(card.szSellCharacter)) ||
        !std::memchr(card.szSellAccount, '\0', sizeof(card.szSellAccount)) ||
        !std::memchr(card.szBuyCharacter, '\0', sizeof(card.szBuyCharacter)) ||
        !std::memchr(card.szBuyAccount, '\0', sizeof(card.szBuyAccount)))
        return false;

    const unsigned long id = card.dwID;
    sys_log(0, "VCARD: %lu %s %s %s %s", id, card.szSellCharacter, card.szSellAccount,
            card.szBuyCharacter, card.szBuyAccount);
    DBManager& db = DBManager::instance();
    std::unique_ptr<SQLMsg> lookup(db.DirectQuery(
        "SELECT sell_account, buy_account, time FROM vcard WHERE id=%lu", id));
    SQLResult* result = lookup && !lookup->uiSQLErrno ? lookup->Get() : nullptr;
    MYSQL_ROW row = result && result->pSQLResult && result->iNumRows == 1 ?
                    mysql_fetch_row(result->pSQLResult) : nullptr;
    if (!row) {
        sys_log(0, "VCARD_FAIL: no data");
        return true;
    }
    if (!row[0] || std::strcmp(row[0], card.szSellAccount)) {
        sys_log(0, "VCARD_FAIL: sell account differ %s", row[0] ? row[0] : "");
        return true;
    }
    if (!row[1] || *row[1]) {
        sys_log(0, "VCARD_FAIL: buy account already exist");
        return true;
    }

    char* end = nullptr;
    errno = 0;
    const long seconds = row[2] ? std::strtol(row[2], &end, 10) : -1;
    if (!end || end == row[2] || *end || errno == ERANGE || seconds < 0 || seconds > INT_MAX) {
        sys_log(0, "VCARD_FAIL: time null");
        return true;
    }

    char sellCharacter[2 * sizeof(card.szSellCharacter)]{};
    char sellAccount[2 * sizeof(card.szSellAccount)]{};
    char buyCharacter[2 * sizeof(card.szBuyCharacter)]{};
    char buyAccount[2 * sizeof(card.szBuyAccount)]{};
    if (db.EscapeString(sellCharacter, card.szSellCharacter, std::strlen(card.szSellCharacter)) >= sizeof(sellCharacter) ||
        db.EscapeString(sellAccount, card.szSellAccount, std::strlen(card.szSellAccount)) >= sizeof(sellAccount) ||
        db.EscapeString(buyCharacter, card.szBuyCharacter, std::strlen(card.szBuyCharacter)) >= sizeof(buyCharacter) ||
        db.EscapeString(buyAccount, card.szBuyAccount, std::strlen(card.szBuyAccount)) >= sizeof(buyAccount))
        return false;

    std::unique_ptr<SQLMsg> balance(db.DirectQuery(
        "SELECT LimitTime FROM GameTime WHERE UserID='%s'", buyAccount));
    result = balance && !balance->uiSQLErrno ? balance->Get() : nullptr;
    row = result && result->pSQLResult && result->iNumRows == 1 ?
          mysql_fetch_row(result->pSQLResult) : nullptr;
    end = nullptr;
    errno = 0;
    const long long oldLimit = row && row[0] ? std::strtoll(row[0], &end, 10) : 0;
    if (!end || end == row[0] || *end || errno == ERANGE || !seconds || oldLimit > LLONG_MAX - seconds) {
        sys_log(0, "VCARD_FAIL: cannot modify GameTime table");
        return true;
    }

    // Redeem the card and credit the account in one statement. The old balance
    // and unredeemed card predicates protect against another writer.
    const long long newLimit = oldLimit + seconds;
    std::unique_ptr<SQLMsg> redemption(db.DirectQuery(
        "UPDATE vcard,GameTime SET vcard.sell_pid='%s', vcard.buy_pid='%s', vcard.buy_account='%s', "
        "vcard.sell_time=NOW(), vcard.new_time=%lld, GameTime.LimitTime=%lld "
        "WHERE vcard.id=%lu AND vcard.sell_account='%s' AND vcard.buy_account='' AND vcard.time=%ld "
        "AND GameTime.UserID='%s' AND GameTime.LimitTime=%lld",
        sellCharacter, buyCharacter, buyAccount, newLimit, newLimit, id, sellAccount, seconds, buyAccount, oldLimit));
    result = redemption && !redemption->uiSQLErrno ? redemption->Get() : nullptr;
    if (!result || result->iAffectedRows < 1)
        sys_log(0, "VCARD_FAIL: cannot modify vcard table");
    else
        sys_log(0, "VCARD_SUCCESS: %s %s", card.szBuyAccount, card.szBuyCharacter);
    return true;
}

namespace {
template<class T>
bool ReadReloadTable(BootReader& packet, std::vector<T>& rows)
{
    std::uint16_t count;
    if (!packet.Read(count))
        return false;

    const std::size_t size = static_cast<std::size_t>(count) * sizeof(T);
    const char* data = packet.Take(size);
    if (!data)
        return false;
    rows.resize(count);
    if (size)
        std::memcpy(rows.data(), data, size);
    return true;
}

template<std::size_t Size>
bool HasProtoTerminator(const char (&text)[Size])
{
    return std::memchr(text, '\0', Size) != nullptr;
}

bool IsValidReloadSkill(const TSkillTable& skill)
{
    return HasProtoTerminator(skill.szName) &&
           HasProtoTerminator(skill.szPointOn) &&
           HasProtoTerminator(skill.szPointOn2) &&
           HasProtoTerminator(skill.szPointOn3) &&
           HasProtoTerminator(skill.szPointPoly) &&
           HasProtoTerminator(skill.szPointPoly2) &&
           HasProtoTerminator(skill.szPointPoly3) &&
           HasProtoTerminator(skill.szDurationPoly) &&
           HasProtoTerminator(skill.szDurationPoly2) &&
           HasProtoTerminator(skill.szDurationPoly3) &&
           HasProtoTerminator(skill.szSPCostPoly) &&
           HasProtoTerminator(skill.szGrandMasterAddSPCostPoly) &&
           HasProtoTerminator(skill.szDurationSPCostPoly) &&
           HasProtoTerminator(skill.szCooldownPoly) &&
           HasProtoTerminator(skill.szMasterBonusPoly) &&
           HasProtoTerminator(skill.szSplashAroundDamageAdjustPoly);
}
}

// 080eb550
bool CInputDB::ReloadProto(const char* data)
{
    BootReader packet(data, m_payloadSize);
    std::vector<TSkillTable> skills;
    std::vector<TBanwordTable> banwords;
    std::vector<TItemTable> items;
    std::vector<TMobTable> mobs;
    if (!ReadReloadTable(packet, skills) || !ReadReloadTable(packet, banwords) ||
        !ReadReloadTable(packet, items) || !ReadReloadTable(packet, mobs))
        return false;

    for (const auto& skill : skills) {
        if (!IsValidReloadSkill(skill))
            return false;
    }
    for (const auto& banword : banwords) {
        if (!banword.szWord[0] || !HasProtoTerminator(banword.szWord))
            return false;
    }
    for (const auto& item : items) {
        if (!HasProtoTerminator(item.szName) || !HasProtoTerminator(item.szLocaleName))
            return false;
    }
    for (const auto& mob : mobs) {
        if (!HasProtoTerminator(mob.szName) || !HasProtoTerminator(mob.szLocaleName) ||
            !HasProtoTerminator(mob.szFolder))
            return false;
    }

    if (!skills.empty())
        CSkillManager::instance().Initialize(skills.data(), skills.size());
    CBanwordManager::instance().Initialize(banwords.data(), banwords.size());

    sys_log(0, "RELOAD: ITEM: %d", static_cast<int>(items.size()));
    if (!items.empty())
        ITEM_MANAGER::instance().Initialize(items.data(), items.size());

    sys_log(0, "RELOAD: MOB: %d", static_cast<int>(mobs.size()));
    if (!mobs.empty())
        CMobManager::instance().Initialize(mobs.data(), mobs.size());

    CMotionManager::instance().Build();
    CHARACTER_MANAGER::instance().for_each_pc([](CHARACTER* character) {
        character->ComputePoints();
    });
    return true;
}
