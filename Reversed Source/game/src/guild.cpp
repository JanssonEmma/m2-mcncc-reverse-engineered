// Complete bounded old guild member/level methods; original guild.cpp placement.
// Other CGuild lifecycle/DB/war behavior remains pending; no substitutes are added.
#include "guild_manager.h"
#include "constants.h"
#include "questmanager.h"
#include "war_map.h"
#include "locale.hpp"
#include "locale_service.h"
#include "skill.h"
#include "log.h"
#include <cstdio>
#include "db.h"
#include <cstdlib>
#include "guild.h"
#include "char.h"
#include "char_manager.h"
#include <memory>
#include "packet.h"
#include "desc.h"
#include "desc_client.h"
#include "buffer_manager.h"
#include <cstring>
#include <cstdint>
#include "p2p.h"
#include "../../common/length.h"
#include "config.h"
#include "utils.h"
#include "building.h"
#include <algorithm>
#include "../../libthecore/include/log.h"

//08089f80/08089ff0. _dummy is intentionally NOT initialized by either old ctor.
SGuildMember::SGuildMember(CHARACTER* ch,BYTE grade,DWORD offer_exp)
    : pid(ch->GetPlayerID()),grade(grade),is_general(0),job(ch->GetJob()),
      level(static_cast<BYTE>(ch->GetLevel())),offer_exp(offer_exp),name(ch->GetName()) {}
//0808a060/0808a0b0. Original name parameter is char*, not const char*.
SGuildMember::SGuildMember(DWORD pid,BYTE grade,BYTE is_general,BYTE job,
        BYTE level,DWORD offer_exp,char* name)
    : pid(pid),grade(grade),is_general(is_general),job(job),level(level),
      offer_exp(offer_exp),name(name) {}

//0808cf20. Lookup does not insert; return the actual mapped object's address.
TGuildMember* CGuild::GetMember(DWORD pid) {
    std::map<DWORD,TGuildMember>::iterator it=m_member.find(pid);
    return it==m_member.end()?0:&it->second;
}
//0808f5d0. Every existing member call persists/broadcasts, even unchanged levels.
void CGuild::LevelChange(DWORD pid,BYTE level) {
    std::map<DWORD,TGuildMember>::iterator member=m_member.find(pid);
    if(member==m_member.end()) return;
    member->second.level=level;
    TPacketGuildChangeMemberData db;
    db.guild_id=GetID();db.pid=pid;db.offer=member->second.offer_exp;
    db.grade=member->second.grade;db.level=level;
    // Original HEADER_GD_GUILD_CHANGE_MEMBER_DATA24, handle0, complete14-byte record.
    db_clientdesc->DBPacket(24,0,&db,sizeof(db));
    SendMemberData(member->second);
}


#include "guild.h"
// 0808f0e0
int CGuild::GetSkillLevel(DWORD vnum)
{
    const std::uint32_t index = static_cast<std::uint32_t>(vnum) - 151u;
    return index < 12 ? m_data.abySkill[index] : 0;
}

int CGuild::GetTotalLevel()
{
    std::uint32_t total = 0;
    for (const auto& entry : m_member)
        total += entry.second.level;
    return static_cast<std::int32_t>(total);
}

void CGuild::AdvanceLevel(int level)
{
    if (m_data.level != level)
        m_data.level = static_cast<BYTE>(std::min(20, level));
}

int CGuild::GetMaxMemberCount()
{
    if (static_cast<unsigned int>(m_iMemberCountBonus) > 18)
        m_iMemberCountBonus = 0;
    return 30 + 2 * m_data.level + m_iMemberCountBonus;
}

bool CGuild::HasLand()
{
    const DWORD guildId = GetID();
    return building::CManager::instance().FindLandByGuild(guildId) != nullptr;
}

void CGuild::SendAllGradePacket(CHARACTER* ch)
{
    DESC* desc = ch->GetDesc();
    if (!desc)
        return;

    static_assert(sizeof(TGuildGrade) == 10, "Guild grade packet layout");
    const BYTE count = sizeof(m_data.grade_array) / sizeof(m_data.grade_array[0]);
    const TPacketGCGuild packet = {0x4b, 0xaa, 0x03};
    TEMP_BUFFER buffer;
    buffer.write(&packet, sizeof(packet));
    buffer.write(&count, sizeof(count));
    for (BYTE grade = 1; grade <= count; ++grade)
    {
        buffer.write(&grade, sizeof(grade));
        buffer.write(&m_data.grade_array[grade - 1], sizeof(TGuildGrade));
    }
    const int size = buffer.size();
    const void* data = buffer.read_peek();
    desc->Packet(data, size);
}

void CGuild::SendGuildInfoPacket(CHARACTER* ch)
{
    DESC* desc = ch->GetDesc();
    if (!desc)
        return;

#pragma pack(push, 1)
    struct GuildInfo
    {
        WORD memberCount;
        WORD maxMemberCount;
        std::uint32_t guildId;
        std::uint32_t masterPid;
        std::uint32_t exp;
        BYTE level;
        char name[GUILD_NAME_MAX_LEN + 1];
        std::uint32_t gold;
        BYTE hasLand;
    };
#pragma pack(pop)
    static_assert(sizeof(GuildInfo) == 35, "Guild info packet layout");
    const TPacketGCGuild packet = {0x4b, 0x27, 0x08};
    GuildInfo info = {};
    info.memberCount = static_cast<WORD>(m_member.size());
    info.maxMemberCount = static_cast<WORD>(GetMaxMemberCount());
    info.guildId = m_data.guild_id;
    info.masterPid = m_data.master_pid;
    info.exp = m_data.exp;
    info.level = m_data.level;
    std::strncpy(info.name, m_data.name, GUILD_NAME_MAX_LEN);
    info.gold = m_data.gold;
    info.hasLand = HasLand();
    desc->BufferedPacket(&packet, sizeof(packet));
    desc->Packet(&info, sizeof(info));
}

namespace {
void WriteGuildMember(TEMP_BUFFER& buffer, TGuildMember& member)
{
    member._dummy = 1;
    const std::uint32_t pid = member.pid;
    const std::uint32_t offer = member.offer_exp;
    BYTE record[13] = {0, 0, 0, 0, member.grade, member.is_general, member.job, member.level};
    std::memcpy(record, &pid, sizeof(pid));
    std::memcpy(record + 8, &offer, sizeof(offer));
    record[12] = member._dummy;
    buffer.write(record, sizeof(record));

    const char padding[CHARACTER_NAME_MAX_LEN + 1] = {};
    const std::size_t nameSize = std::min(member.name.size(), sizeof(padding));
    if (nameSize)
        buffer.write(member.name.data(), nameSize);
    if (nameSize < sizeof(padding))
        buffer.write(padding, sizeof(padding) - nameSize);
}
}

void CGuild::SendListPacket(CHARACTER* ch)
{
    DESC* desc = ch->GetDesc();
    if (!desc)
        return;

    const TPacketGCGuild packet = {0x4b, static_cast<WORD>(sizeof(TPacketGCGuild) +
        38 * m_member.size()), 0x02};
    TEMP_BUFFER buffer;
    buffer.write(&packet, sizeof(packet));
    for (std::map<DWORD, TGuildMember>::iterator it = m_member.begin(); it != m_member.end(); ++it)
    {
        TGuildMember& member = it->second;
        WriteGuildMember(buffer, member);
        if (test_server) {
            const int job = member.job;
            sys_log(0, "name %s job %d  ", member.name.c_str(), job);
        }
    }
    const int size = buffer.size();
    const void* data = buffer.read_peek();
    desc->Packet(data, size);
    for (std::set<CHARACTER*>::const_iterator it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
        SendLoginPacket(ch, *it);
    for (std::set<DWORD>::const_iterator it = m_memberP2POnline.begin(); it != m_memberP2POnline.end(); ++it)
        SendLoginPacket(ch, *it);
}

void CGuild::SendSkillInfoPacket(CHARACTER* ch) const
{
    DESC* desc = ch->GetDesc();
    if (!desc)
        return;

    const TPacketGCGuild packet = {0x4b, 0x16, 0x0c};
    desc->BufferedPacket(&packet, sizeof(packet));
    desc->BufferedPacket(&m_data.skill_point, sizeof(m_data.skill_point));
    desc->BufferedPacket(m_data.abySkill, sizeof(m_data.abySkill));
    const WORD power = static_cast<WORD>(m_data.power);
    desc->BufferedPacket(&power, sizeof(power));
    const WORD maxPower = static_cast<WORD>(m_data.max_power);
    desc->Packet(&maxPower, sizeof(maxPower));
}

CGuild::CGuild(TGuildCreateParameter& parameter)
{
    Initialize();
    std::strncpy(m_data.name, parameter.name, sizeof(m_data.name) - 1);
    m_data.master_pid = parameter.master->GetPlayerID();
    for (unsigned i = 0; i < 15; ++i) {
        TGuildGrade& grade = m_data.grade_array[i];
        const char* title = locale_find(i ? "\261\346\265\345\277\370" : "\261\346\265\345\300\345", g_iUseLocale);
        std::strncpy(grade.grade_name, title, sizeof(grade.grade_name) - 1);
        grade.auth_flag = i ? 0 : 0x0f;
    }

    DBManager& db = DBManager::instance();
    const char* postfix = get_table_postfix();
    char escapedName[2 * sizeof(m_data.name) + 1]{};
    db.EscapeString(escapedName, m_data.name, std::strlen(m_data.name));
    std::unique_ptr<SQLMsg> message(db.DirectQuery(
        "INSERT INTO guild%s(name, master, sp, level, exp, skill_point, skill) "
        "VALUES('%s', %lu, 1000, 1, 0, 0, '\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0')",
        postfix, escapedName, static_cast<unsigned long>(m_data.master_pid)));
    SQLResult* result = message ? message->Get() : nullptr;
    if (!result || !result->iInsertID)
        return;
    m_data.guild_id = result->iInsertID;
    for (unsigned i = 0; i < 15; ++i) {
        const TGuildGrade& grade = m_data.grade_array[i];
        char escapedGrade[2 * sizeof(grade.grade_name) + 1]{};
        db.EscapeString(escapedGrade, grade.grade_name, std::strlen(grade.grade_name));
        db.Query("INSERT INTO guild_grade%s VALUES(%lu, %u, '%s', %u)",
                 postfix, static_cast<unsigned long>(m_data.guild_id), i + 1,
                 escapedGrade, static_cast<unsigned>(grade.auth_flag));
    }

    ComputeGuildPoints();
    m_data.power = m_data.max_power;
    const std::uint32_t id = m_data.guild_id;
    if (db_clientdesc) {
        db_clientdesc->DBPacket(28, 0, &id, sizeof(id));
    }
    SendDBSkillUpdate(0);
    CHARACTER_MANAGER::instance().for_each_pc([this, id](CHARACTER* player) {
        if (DESC* desc = player->GetDesc()) {
            TPacketGCGuild header{};
            header.header = 0x4b;
            header.size = 20;
            header.subheader = 0x10;
            desc->BufferedPacket(&header, sizeof(header));
            desc->BufferedPacket(&id, sizeof(id));
            desc->Packet(m_data.name, GUILD_NAME_MAX_LEN);
        }
    });
    RequestAddMember(parameter.master, 1);
}

void CGuild::RequestAddMember(CHARACTER* character, int grade)
{
    if (character->GetGuild())
        return;
    const std::uint32_t pid = character->GetPlayerID();
    if (m_member.find(pid) != m_member.end()) {
        _sys_err("RequestAddMember", 0x8d, "Already a member in guild %s[%d]",
                 character->GetName(), static_cast<int>(pid));
        return;
    }
    if (db_clientdesc) {
        const TPacketGDGuildAddMember packet = {
            pid, static_cast<std::uint32_t>(GetID()), static_cast<BYTE>(grade)
        };
        db_clientdesc->DBPacket(21, 0, &packet, sizeof(packet));
    }
}

CGuild::CGuild(DWORD guildId)
{
    Load(guildId);
}

void CGuild::Initialize()
{
    m_data = TGuildData();
    m_data.level = 1;
    m_general_count = 0;
    m_iMemberCountBonus = 0;
    for (bool& usable : abSkillUsable)
        usable = true;
}

void CGuild::Load(DWORD guildId)
{
    Initialize();
    sys_log(0, "GUILD: loading guild id %lu", guildId);
    m_data.guild_id = guildId;

    DBManager& db = DBManager::instance();
    const char* postfix = get_table_postfix();
    db.FuncQuery([guildId](SQLMsg* msg) {
        if (CGuild* guild = CGuildManager::instance().FindGuild(guildId))
            guild->LoadGuildData(msg);
    },
        "SELECT master, level, exp, name, skill_point, skill, sp, ladder_point, win, draw, loss, gold FROM guild%s WHERE id = %lu",
        postfix, guildId);
    db.FuncQuery([guildId](SQLMsg* msg) {
        if (CGuild* guild = CGuildManager::instance().FindGuild(guildId))
            guild->LoadGuildGradeData(msg);
    },
        "SELECT grade, name, auth+0 FROM guild_grade%s WHERE guild_id = %lu", postfix, guildId);
    db.FuncQuery([guildId](SQLMsg* msg) {
        if (CGuild* guild = CGuildManager::instance().FindGuild(guildId))
            guild->LoadGuildMemberData(msg);
    },
        "SELECT pid, grade, is_general, offer, level, job, name FROM guild_member%s, player%s WHERE guild_id = %lu and pid = id",
        postfix, postfix, guildId);
}

void CGuild::UpdateSkill(BYTE points, BYTE* levels)
{
    m_data.skill_point = points;
    std::memcpy(m_data.abySkill, levels, sizeof(m_data.abySkill));
    ComputeGuildPoints();
}

// 0808e900
void CGuild::UseSkill(DWORD vnum, CHARACTER* character, DWORD targetPid)
{
    const auto skillVnum = static_cast<std::uint32_t>(vnum);
    if (!character || !(GetMemberAuth(character->GetPlayerID()) & 8))
        return;

    sys_log(0, "GUILD_USE_SKILL : cname(%s), skill(%d)", character->GetName(), static_cast<int>(skillVnum));
    if (!character->CanMove() || skillVnum < GUILD_SKILL_START || skillVnum > GUILD_SKILL_END + 1)
        return;

    CSkillProto* skill = CSkillManager::instance().Get(skillVnum);
    if (!skill) {
        _sys_err("UseSkill", 0x4bf, "There is no such guild skill by number %u", static_cast<unsigned>(skillVnum));
        return;
    }

    const BYTE level = GetSkillLevel(skillVnum);
    if (!level || ((skill->dwFlag & SKILL_FLAG_SELFONLY) && character->FindAffect(skill->dwVnum, 0)))
        return;

    if (character->IsAffectFlag(28))
        character->RemoveAffect(215);
    if (character->IsAffectFlag(19))
        character->RemoveAffect(34);

    const double k = static_cast<double>(level) / skill->bMaxLevel;
    skill->kSPCostPoly.SetVar("k", k);
    const auto truncate = [](long double value) -> std::int32_t {
        if (!(value > -2147483649.0L && value < 2147483648.0L))
            return INT32_MIN;
        return static_cast<std::int32_t>(value);
    };
    const auto cost = truncate(skill->kSPCostPoly.Eval());
    const int availablePower = m_data.power;
    if (availablePower < cost) {
        character->ChatPacket(1, LC_TEXT("<\261\346\265\345> \277\353\275\305\267\302\300\314 \272\316\301\267\307\325\264\317\264\331. (%d, %d)"), availablePower, cost);
        return;
    }

    skill->kCooldownPoly.SetVar("k", k);
    const auto cooldown = truncate(skill->kCooldownPoly.Eval());
    const auto powerDelta = static_cast<std::int32_t>(0U - static_cast<std::uint32_t>(cost));
    const auto index = skillVnum - GUILD_SKILL_START;
    if (!abSkillUsable[index]) {
        character->ChatPacket(1, "%s", LC_TEXT("<\261\346\265\345> \304\360\305\270\300\323\300\314 \263\241\263\252\301\366 \276\312\276\306 \261\346\265\345 \275\272\305\263\300\273 \273\347\277\353\307\322 \274\366 \276\370\275\300\264\317\264\331."));
        return;
    }
    const TPacketGuildUseSkill cooldownPacket = {
        static_cast<std::uint32_t>(GetID()), static_cast<std::uint32_t>(skill->dwVnum),
        static_cast<std::uint32_t>(cooldown)
    };
    db_clientdesc->DBPacket(0x2f, 0, &cooldownPacket, sizeof(cooldownPacket));
    abSkillUsable[index] = false;
    if (test_server) {
        const int power = m_data.power;
        character->ChatPacket(1, LC_TEXT("<\261\346\265\345> %d \275\272\305\263\300\273 \273\347\277\353\307\324 (%d, %d) to %lu"), static_cast<int>(skillVnum), power, cost, static_cast<unsigned long>(targetPid));
    }

    if (skillVnum == GUILD_SKILL_TELEPORT) {
        SendDBSkillUpdate(powerDelta);
        if (CHARACTER* target = CHARACTER_MANAGER::instance().FindByPID(targetPid)) {
            character->WarpSet(target->GetX(), target->GetY(), 0);
            return;
        }

        CCI* remote = m_memberP2POnline.find(targetPid) == m_memberP2POnline.end()
            ? nullptr : P2P_MANAGER::instance().FindByPID(targetPid);
        if (!remote || !remote->pkDesc) {
            character->ChatPacket(1, "%s", LC_TEXT("<\261\346\265\345> \273\363\264\353\260\241 \277\302\266\363\300\316 \273\363\305\302\260\241 \276\306\264\325\264\317\264\331."));
            return;
        }
        if (remote->bChannel != g_bChannel) {
            character->ChatPacket(1, LC_TEXT("<\261\346\265\345> \273\363\264\353\260\241 %d \303\244\263\316\277\241 \300\326\275\300\264\317\264\331. (\307\366\300\347 \303\244\263\316 %d)"), static_cast<int>(remote->bChannel), static_cast<int>(g_bChannel));
            return;
        }

        char request[9] = {0x0c};
        const auto source = static_cast<std::uint32_t>(character->GetPlayerID());
        const auto target = static_cast<std::uint32_t>(targetPid);
        std::memcpy(request + 1, &source, sizeof(source));
        std::memcpy(request + 5, &target, sizeof(target));
        remote->pkDesc->Packet(request, sizeof(request));
        if (test_server)
            character->ChatPacket(3, "%s", "sent find position packet for guild teleport");
        return;
    }

    if (!UnderAnyWar(3)) {
        character->ChatPacket(1, "%s", LC_TEXT("<\261\346\265\345> \261\346\265\345 \275\272\305\263\300\272 \261\346\265\345\300\374 \301\337\277\241\270\270 \273\347\277\353\307\322 \274\366 \300\326\275\300\264\317\264\331."));
        return;
    }
    SendDBSkillUpdate(powerDelta);
    for (CHARACTER* member : m_memberOnline) {
        member->RemoveAffect(skillVnum);
        character->ComputeSkill(skillVnum, member, m_data.abySkill[index]);
    }
}

void CGuild::SkillUsableChange(DWORD vnum, bool usable)
{
    const std::uint32_t index = static_cast<std::uint32_t>(vnum) - 151u;
    if (index >= sizeof(abSkillUsable) / sizeof(abSkillUsable[0]))
        return;
    abSkillUsable[index] = usable;
    sys_log(0, "CGuild::SkillUsableChange(guild=%s, skill=%d, usable=%d)",
            GetName(), static_cast<int>(vnum), usable);
}

void CGuild::SaveSkill()
{
    DBManager& db = DBManager::instance();
    char text[2 * sizeof(m_data.abySkill) + 1]{};
    db.EscapeString(text, m_data.abySkill, sizeof(m_data.abySkill));
    db.Query("UPDATE guild%s SET sp = %d, skill_point=%d, skill='%s' WHERE id = %lu",
             get_table_postfix(), m_data.power, static_cast<int>(m_data.skill_point),
             text, static_cast<unsigned long>(m_data.guild_id));
}

void CGuild::SetWarRecords(int wins, int draws, int losses)
{
    m_data.win = wins;
    m_data.draw = draws;
    m_data.loss = losses;
}

void CGuild::SendDBSkillUpdate(int amount)
{
    if (!db_clientdesc)
        return;
    TPacketGuildSkillUpdate packet{};
    packet.guild_id = GetID();
    packet.amount = amount;
    packet.skill_point = m_data.skill_point;
    std::memcpy(packet.skill_levels, m_data.abySkill, sizeof(packet.skill_levels));
    db_clientdesc->DBPacket(19, 0, &packet, sizeof(packet));
}

void CGuild::SkillLevelUp(DWORD vnum)
{
    const auto skillVnum = static_cast<std::uint32_t>(vnum);
    const std::uint32_t index = skillVnum - 151u;
    if (index >= sizeof(m_data.abySkill))
        return;
    CSkillProto* skill = CSkillManager::instance().Get(skillVnum);
    if (!skill) {
        _sys_err("SkillLevelUp", 0x480, "There is no such guild skill by number %u", static_cast<unsigned>(skillVnum));
        return;
    }
    BYTE& level = m_data.abySkill[index];
    if (level >= skill->bMaxLevel || !m_data.skill_point)
        return;

    --m_data.skill_point;
    ++level;
    ComputeGuildPoints();
    SaveSkill();
    SendDBSkillUpdate(0);
    for (CHARACTER* member : m_memberOnline)
        SendSkillInfoPacket(member);
    const int loggedType = skill->bType;
    const int loggedLevel = level;
    const int loggedVnum = static_cast<std::int32_t>(skill->dwVnum);
    sys_log(0, "Guild SkillUp: %s %d level %d type %d", m_data.name, loggedVnum, loggedLevel, loggedType);
}

bool CGuild::OfferExp(CHARACTER* character, long amount)
{
    const std::uint32_t pid = character->GetPlayerID();
    TGuildMember* member = GetMember(pid);
    if (!member || amount < 0 || amount > INT32_MAX ||
        static_cast<std::uint64_t>(amount) + m_data.exp > UINT32_MAX)
        return false;
    if (character->GetExp() < static_cast<std::uint32_t>(amount)) {
        character->ChatPacket(1, "%s", locale_find("\074\261\346\265\345\076\040\301\246\260\370\307\317\260\355\300\332\040\307\317\264\302\040\260\346\307\350\304\241\260\241\040\263\262\300\272\040\260\346\307\350\304\241\272\270\264\331\040\270\271\275\300\264\317\264\331\056", g_iUseLocale));
        return false;
    }

    character->PointChange(3, -static_cast<int>(amount), false, false);
    const TPacketGuildExpUpdate experience = {
        static_cast<std::uint32_t>(GetID()), static_cast<std::int32_t>(amount / 100)
    };
    if (db_clientdesc)
        db_clientdesc->DBPacket(20, 0, &experience, sizeof(experience));
    GuildPointChange(3, experience.amount, true);
    member->offer_exp += experience.amount;
    SendMemberData(*member);

    SaveMember(character->GetPlayerID());
    if (db_clientdesc) {
        TPacketGuildChangeMemberData packet{};
        packet.guild_id = GetID();
        packet.pid = character->GetPlayerID();
        packet.offer = member->offer_exp;
        packet.level = character->GetLevel();
        packet.grade = member->grade;
        db_clientdesc->DBPacket(24, 0, &packet, sizeof(packet));
    }
    return true;
}

void CGuild::SaveMember(DWORD pid)
{
    const TGuildMember* member = GetMember(pid);
    if (!member)
        return;
    DBManager::instance().Query(
        "UPDATE guild_member%s SET grade = %d, offer = %lu, is_general = %d WHERE pid = %lu and guild_id = %lu",
        get_table_postfix(), static_cast<int>(member->grade), static_cast<unsigned long>(member->offer_exp),
        static_cast<int>(member->is_general), static_cast<unsigned long>(pid), static_cast<unsigned long>(GetID()));
}

void CGuild::SaveLevel()
{
    DBManager::instance().Query(
        "UPDATE guild%s SET level=%d, exp=%lu, skill_point=%d WHERE id = %lu",
        get_table_postfix(), static_cast<int>(m_data.level), static_cast<unsigned long>(m_data.exp),
        static_cast<int>(m_data.skill_point), static_cast<unsigned long>(GetID()));
}

void CGuild::SkillRecharge()
{
    // The original recharge callback returns immediately.
}

// 0808f150
static std::uint32_t __guild_levelup_exp(int level)
{
    return (LC_IsYMIR() ? guild_exp_table : guild_exp_table2)[level];
}

void CGuild::GuildPointChange(BYTE type, int amount, bool save)
{
    if (type == 7) {
        ChangeGuildPower(amount, save);
        return;
    }
    if (type != 3)
        return;

    const std::int64_t changed = static_cast<std::int64_t>(m_data.exp) + amount;
    m_data.exp = changed < 0 ? 0 : static_cast<std::uint32_t>(changed);

    if (changed >= 0)
        m_data.level = std::clamp<int>(m_data.level, 1, 20);
    while (changed >= 0 && m_data.exp >= __guild_levelup_exp(m_data.level)) {
        if (m_data.level == 20) {
            m_data.exp = 0;
            break;
        }
        m_data.exp -= __guild_levelup_exp(m_data.level);
        ++m_data.skill_point;
        ++m_data.level;
        ComputeGuildPoints();
        const auto refill = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(m_data.max_power) - static_cast<std::uint32_t>(m_data.power));
        ChangeGuildPower(refill, false);
        if (save)
            ChangeLadderPoint(1000);
        for (CHARACTER* member : m_memberOnline)
            SendGuildInfoPacket(member);
    }

    TPacketGCGuild header{};
    header.header = 0x4b;
    header.size = 9;
    header.subheader = 10;
    const std::uint32_t experience = m_data.exp;
    TEMP_BUFFER buffer;
    buffer.write(&header, sizeof(header));
    buffer.write(&m_data.level, sizeof(m_data.level));
    buffer.write(&experience, sizeof(experience));
    Packet(buffer.read_peek(), buffer.size());
    if (save)
        SaveLevel();
}

void CGuild::ChangeGuildPower(int amount, bool save)
{
    const auto power = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(m_data.power) + static_cast<std::uint32_t>(amount));
    m_data.power = power;
    const int maxPower = m_data.max_power;
    if (power > maxPower)
        m_data.power = maxPower;
    if (save)
        SaveSkill();
    for (CHARACTER* member : m_memberOnline)
        SendSkillInfoPacket(member);
}

void CGuild::ComputeGuildPoints()
{
    const int maxPower = 300 + 100 * m_data.level;
    const bool exceedsMax = m_data.power > maxPower;
    m_data.max_power = maxPower;
    if (exceedsMax)
        m_data.power = maxPower;
}

void CGuild::LoadGuildData(SQLMsg* message)
{
    SQLResult* result = message->Get();
    MYSQL_ROW row = result && result->iNumRows > 0 ? mysql_fetch_row(result->pSQLResult) : 0;
    if (!row)
    {
        _sys_err("LoadGuildData", 582, "Query failed: getting guild data %s", message->stQuery.c_str());
        return;
    }

    m_data.master_pid = std::strtoul(row[0], 0, 10);
    m_data.level = static_cast<BYTE>(std::strtoul(row[1], 0, 10));
    m_data.exp = std::strtoul(row[2], 0, 10);
    std::strncpy(m_data.name, row[3], sizeof(m_data.name) - 1);
    m_data.name[sizeof(m_data.name) - 1] = '\0';
    m_data.skill_point = static_cast<BYTE>(std::strtoul(row[4], 0, 10));
    std::memset(m_data.abySkill, 0, sizeof(m_data.abySkill));
    if (row[5])
    {
        const unsigned long length = mysql_fetch_lengths(result->pSQLResult)[5];
        const unsigned long size = sizeof(m_data.abySkill);
        std::memcpy(m_data.abySkill, row[5], length < size ? length : size);
    }
    m_data.power = static_cast<int>(std::strtoul(row[6], 0, 10));
    m_data.ladder_point = std::atoi(row[7]);
    if (m_data.ladder_point < 0)
        m_data.ladder_point = 0;
    m_data.win = std::atoi(row[8]);
    m_data.draw = std::atoi(row[9]);
    m_data.loss = std::atoi(row[10]);
    m_data.gold = std::atoi(row[11]);
    ComputeGuildPoints();
}

void CGuild::LoadGuildGradeData(SQLMsg* message)
{
    SQLResult* result = message->Get();
    if (!result || result->iNumRows != 15)
    {
        _sys_err("LoadGuildGradeData", 559, "Query failed: getting guild grade data");
        return;
    }

    while (MYSQL_ROW row = mysql_fetch_row(result->pSQLResult))
    {
        const BYTE grade = static_cast<BYTE>(std::atoi(row[0]));
        if (grade < 1 || grade > 15)
            continue;
        sys_log(0, "GuildGradeLoad %s", row[1]);
        TGuildGrade& target = m_data.grade_array[grade - 1];
        std::strncpy(target.grade_name, row[1], sizeof(target.grade_name) - 1);
        target.grade_name[sizeof(target.grade_name) - 1] = '\0';
        target.auth_flag = static_cast<BYTE>(std::strtoul(row[2], 0, 10));
    }
}

void CGuild::LoadGuildMemberData(SQLMsg* message)
{
    SQLResult* result = message->Get();
    if (!result || result->iNumRows <= 0)
        return;

    m_general_count = 0;
    sys_log(0, "GUILD_MEMBER: %s total %d members", m_data.name, result->iNumRows);
    while (MYSQL_ROW row = mysql_fetch_row(result->pSQLResult))
    {
        const DWORD pid = std::strtoul(row[0], 0, 10);
        const BYTE grade = static_cast<BYTE>(std::strtoul(row[1], 0, 10));
        const BYTE general = row[2] && row[2][0] == '1';
        const DWORD offer = std::strtoul(row[3], 0, 10);
        const BYTE level = static_cast<BYTE>(std::strtoul(row[4], 0, 10));
        const BYTE job = static_cast<BYTE>(std::strtoul(row[5], 0, 10));
        if (general)
            ++m_general_count;
        m_member.insert(std::make_pair(pid, TGuildMember(pid, grade, general, job, level, offer, row[6])));
        CGuildManager::instance().Link(pid, this);
    }
}

void CGuild::Packet(const void* data, int size)
{
    for (CHARACTER* ch : m_memberOnline)
        if (DESC* desc = ch->GetDesc())
            desc->Packet(data, size);
}

void CGuild::LoginMember(CHARACTER* ch)
{
    const DWORD pid = ch->GetPlayerID();
    if (m_member.find(pid) == m_member.end())
    {
        _sys_err("LoginMember", 254, "GUILD %s[%d] is not a memeber of guild.", ch->GetName(), static_cast<int>(pid));
        return;
    }

    ch->SetGuild(this);
    for (CHARACTER* member : m_memberOnline)
        SendLoginPacket(member, ch);
    m_memberOnline.insert(ch);
    SendAllGradePacket(ch);
    SendGuildInfoPacket(ch);
    SendListPacket(ch);
    SendSkillInfoPacket(ch);
    SendEnemyGuild(ch);
}

BYTE CGuild::GetMemberAuth(DWORD pid)
{
    const TGuildMember* member = GetMember(pid);
    if (!member || member->grade < 1 || member->grade > 15)
        return 0;
    return m_data.grade_array[member->grade - 1].auth_flag;
}

// 0808f930
CHARACTER* CGuild::GetMasterCharacter()
{
    const DWORD masterPid = GetMasterPID();
    return CHARACTER_MANAGER::instance().FindByPID(masterPid);
}

// 0808ff20
void CGuild::RecvMoneyChange(int gold)
{
    m_data.gold = gold;
    const TPacketGCGuild header = {0x4b, 8, 0x14};
    const std::int32_t total = gold;
    for (CHARACTER* member : m_memberOnline) {
        if (DESC* desc = member->GetDesc()) {
            desc->BufferedPacket(&header, sizeof(header));
            desc->Packet(&total, sizeof(total));
        }
    }
}

// 0808ffc0
void CGuild::RecvWithdrawMoneyGive(int gold)
{
    CHARACTER* master = GetMasterCharacter();
    if (master) {
        master->PointChange(POINT_GOLD, gold, false, false);
        sys_log(0, "GUILD: WITHDRAW %s:%lu player %s[%lu] gold %d", GetName(), GetID(),
                master->GetName(), master->GetPlayerID(), gold);
    }
    const TPacketGDGuildMoneyWithdrawGiveReply reply = {
        static_cast<std::uint32_t>(GetID()), gold, static_cast<BYTE>(master != nullptr)
    };
    db_clientdesc->DBPacket(0x35, 0, &reply, sizeof(reply));
}

void CGuild::RequestWithdrawMoney(CHARACTER* ch, int gold)
{
    const char* key = nullptr;
    if (!ch->CanDeposit())
        key = "<\261\346\265\345> \300\341\275\303\310\304\277\241 \300\314\277\353\307\330\301\326\275\312\275\303\277\300";
    else if (ch->GetPlayerID() != GetMasterPID())
        key = "<\261\346\265\345> \261\346\265\345 \261\335\260\355\277\241\274\261 \261\346\265\345\300\345\270\270 \303\342\261\335\307\322 \274\366 \300\326\275\300\264\317\264\331.";
    else if (gold > m_data.gold)
        key = "<\261\346\265\345> \260\241\301\366\260\355 \300\326\264\302 \265\267\300\314 \272\316\301\267\307\325\264\317\264\331.";
    if (key)
    {
        ch->ChatPacket(1, "%s", locale_find(key, g_iUseLocale));
        return;
    }

    const TPacketGDGuildMoney packet = {static_cast<std::uint32_t>(GetID()), gold};
    db_clientdesc->DBPacket(0x34, 0, &packet, sizeof(packet));
    ch->UpdateDepositPulse();
}

void CGuild::AddComment(CHARACTER* ch, const std::string& comment)
{
    if (comment.size() > 50)
        return;
    DBManager& db = DBManager::instance();
    char name[51];
    char content[101];
    db.EscapeString(name, ch->GetName(), std::strlen(ch->GetName()));
    db.EscapeString(content, comment.c_str(), comment.size());
    const DWORD guildId = GetID();
    const DWORD playerId = ch->GetPlayerID();
    db.FuncAfterQuery([guildId, playerId]() {
        CHARACTER* player = CHARACTER_MANAGER::instance().FindByPID(playerId);
        CGuild* guild = CGuildManager::instance().FindGuild(guildId);
        if (player && guild && player->GetGuild() == guild)
            guild->RefreshCommentForce(player);
    }, "INSERT INTO guild_comment%s(guild_id, name, notice, content, time) VALUES(%lu, '%s', %d, '%s', NOW())",
       get_table_postfix(), guildId, name, !comment.empty() && comment.front() == '!' ? 1 : 0, content);
}

void CGuild::DeleteComment(CHARACTER* ch, DWORD id)
{
    const TGuildMember* member = GetMember(ch->GetPlayerID());
    if (!member)
        return;
    DBManager& db = DBManager::instance();
    std::unique_ptr<SQLMsg> message;
    if (member->grade == 1)
        message.reset(db.DirectQuery("DELETE FROM guild_comment%s WHERE id = %lu AND guild_id = %lu",
                                    get_table_postfix(), id, GetID()));
    else
    {
        char name[51];
        db.EscapeString(name, ch->GetName(), std::strlen(ch->GetName()));
        message.reset(db.DirectQuery("DELETE FROM guild_comment%s WHERE id = %lu AND guild_id = %lu AND name = '%s'",
                                    get_table_postfix(), id, GetID(), name));
    }

    SQLResult* result = message ? message->Get() : nullptr;
    if (result && result->iAffectedRows > 0)
        RefreshCommentForce(ch);
    else
        ch->ChatPacket(1, "%s", locale_find("<\261\346\265\345> \273\350\301\246\307\322 \274\366 \276\370\264\302 \261\333\300\324\264\317\264\331.", g_iUseLocale));
}

void CGuild::RefreshComment(CHARACTER* ch)
{
    RefreshCommentForce(ch);
}

void CGuild::RefreshCommentForce(CHARACTER* ch)
{
    constexpr unsigned maxComments = 12;
    constexpr unsigned recordSize = 80;
    const std::unique_ptr<SQLMsg> message(DBManager::instance().DirectQuery(
        "SELECT id, name, content FROM guild_comment%s WHERE guild_id = %lu ORDER BY notice DESC, id DESC LIMIT %d",
        get_table_postfix(), GetID(), static_cast<int>(maxComments)));
    SQLResult* result = message ? message->Get() : nullptr;
    DESC* desc = ch->GetDesc();
    if (!desc)
        return;

    BYTE packet[5 + maxComments * recordSize] = {};
    BYTE count = 0;
    if (result && result->pSQLResult && mysql_num_fields(result->pSQLResult) >= 3)
    {
        while (count < maxComments)
        {
            MYSQL_ROW row = mysql_fetch_row(result->pSQLResult);
            if (!row)
                break;
            BYTE* record = packet + 5 + count * recordSize;
            const std::uint32_t id = row[0] ? std::strtoul(row[0], nullptr, 10) : 0;
            std::memcpy(record, &id, sizeof(id));
            if (row[1])
                std::strncpy(reinterpret_cast<char*>(record + 4), row[1], 24);
            if (row[2])
                std::strncpy(reinterpret_cast<char*>(record + 29), row[2], 50);
            ++count;
        }
    }
    const unsigned size = 5 + count * recordSize;
    packet[0] = 75;
    packet[1] = static_cast<BYTE>(size);
    packet[2] = static_cast<BYTE>(size >> 8);
    packet[3] = 9;
    packet[4] = count;
    if (count == 0) {
        desc->Packet(packet, size);
        return;
    }

    desc->BufferedPacket(packet, 4);
    desc->BufferedPacket(packet + 4, 1);
    for (unsigned i = 0; i < count; ++i) {
        const BYTE* record = packet + 5 + i * recordSize;
        desc->BufferedPacket(record, 4);
        desc->BufferedPacket(record + 4, 25);
        if (i + 1 < count)
            desc->BufferedPacket(record + 29, 51);
        else
            desc->Packet(record + 29, 51);
    }
}

namespace
{
struct FSendChangeGrade
{
    TPacketGuild p;

    void operator()() const
    {
        db_clientdesc->DBPacket(23, 0, &p, sizeof(p));
    }
};
}

void CGuild::SendGradeName(BYTE grade)
{
    constexpr std::size_t nameSize = sizeof(TGuildGrade::grade_name);
    static_assert(nameSize == 9, "Guild grade name wire size");
    BYTE packet[5 + nameSize] = {75, 14, 0, 6, grade};
    std::strncpy(reinterpret_cast<char*>(packet + 5), m_data.grade_array[grade - 1].grade_name, nameSize - 1);
    Packet(packet, sizeof(packet));
}

void CGuild::SendGradeAuth(BYTE grade)
{
    BYTE packet[6] = {75, 6, 0, 7, grade, m_data.grade_array[grade - 1].auth_flag};
    Packet(packet, sizeof(packet));
}

void CGuild::ChangeGradeName(BYTE grade, const char* name)
{
    if (grade == 1)
        return;
    if (grade == 0 || grade > 15) {
        _sys_err("ChangeGradeName", 0x2fb, "Wrong guild grade value %d", static_cast<int>(grade));
        return;
    }
    if (!name || !name[0])
        return;
    constexpr std::size_t maxLength = sizeof(TGuildGrade::grade_name) - 1;
    const std::size_t length = std::strlen(name);
    if (length > maxLength)
        return;
    DBManager& db = DBManager::instance();
    const DWORD guildId = GetID();
    char escaped[2 * maxLength + 1];
    db.EscapeString(escaped, name, length);
    db.FuncAfterQuery(FSendChangeGrade{{static_cast<std::uint32_t>(guildId), grade}},
        "UPDATE guild_grade%s SET name = '%s' where guild_id = %lu and grade = %d",
        get_table_postfix(), escaped, guildId, grade);
    std::strcpy(m_data.grade_array[grade - 1].grade_name, name);
    SendGradeName(grade);
}

void CGuild::ChangeGradeAuth(BYTE grade, BYTE auth)
{
    if (grade == 1)
        return;
    if (grade == 0 || grade > 15) {
        _sys_err("ChangeGradeAuth", 0x32a, "Wrong guild grade value %d", static_cast<int>(grade));
        return;
    }
    const DWORD guildId = GetID();
    DBManager::instance().FuncAfterQuery(FSendChangeGrade{{static_cast<std::uint32_t>(guildId), grade}},
        "UPDATE guild_grade%s SET auth = %d where guild_id = %lu and grade = %d",
        get_table_postfix(), auth, guildId, grade);
    m_data.grade_array[grade - 1].auth_flag = auth;
    SendGradeAuth(grade);
}

void CGuild::P2PChangeGrade(BYTE grade)
{
    if (grade < 1 || grade > 15)
        return;
    const DWORD guildId = GetID();
    DBManager::instance().FuncQuery([guildId](SQLMsg* message) {
        if (CGuild* guild = CGuildManager::instance().FindGuild(guildId))
            guild->__P2PUpdateGrade(message);
    }, "SELECT grade, name, auth+0 FROM guild_grade%s WHERE guild_id = %lu and grade = %d",
       get_table_postfix(), guildId, grade);
}

void CGuild::__P2PUpdateGrade(SQLMsg* message)
{
    SQLResult* result = message ? message->Get() : nullptr;
    if (!result || !result->pSQLResult || !result->iNumRows ||
        mysql_num_fields(result->pSQLResult) < 3)
        return;
    MYSQL_ROW row = mysql_fetch_row(result->pSQLResult);
    if (!row || !row[0] || !row[1] || !row[2])
        return;

    const int grade = std::atoi(row[0]);
    if (grade < 1 || grade > 15)
        return;
    const char* name = row[1];
    const int auth = std::atoi(row[2]);
    TGuildGrade& record = m_data.grade_array[grade - 1];
    if (std::strncmp(record.grade_name, name, 8)) {
        std::strncpy(record.grade_name, name, 8);
        record.grade_name[8] = '\0';
        SendGradeName(static_cast<BYTE>(grade));
    }
    if (record.auth_flag != auth) {
        record.auth_flag = static_cast<BYTE>(auth);
        SendGradeAuth(static_cast<BYTE>(grade));
    }
}

void CGuild::ChangeMemberGrade(DWORD pid, BYTE grade)
{
    TGuildMember* member = GetMember(pid);
    if (!member || grade < 2 || grade > 15)
        return;
    member->grade = grade;

    BYTE packet[9] = {75, 9, 0, 11};
    const std::uint32_t wirePid = pid;
    std::memcpy(packet + 4, &wirePid, sizeof(wirePid));
    packet[8] = grade;
    for (CHARACTER* online : m_memberOnline) {
        if (DESC* desc = online->GetDesc()) {
            desc->BufferedPacket(packet, 4);
            desc->BufferedPacket(packet + 4, 4);
            desc->Packet(packet + 8, 1);
        }
    }
    SaveMember(pid);

    const TPacketGuildChangeMemberData db = {
        static_cast<std::uint32_t>(GetID()), wirePid, static_cast<std::uint32_t>(member->offer_exp),
        member->level, grade
    };
    db_clientdesc->DBPacket(24, 0, &db, sizeof(db));
}

bool CGuild::ChangeMemberGeneral(DWORD pid, BYTE isGeneral)
{
    isGeneral = isGeneral != 0;
    if (isGeneral && m_general_count >= 1)
        return false;
    TGuildMember* member = GetMember(pid);
    if (!member || member->is_general == isGeneral)
        return true;
    const auto delta = isGeneral ? 1u : 0xffffffffu;
    m_general_count = static_cast<std::int32_t>(static_cast<std::uint32_t>(m_general_count) + delta);
    member->is_general = isGeneral;

    BYTE packet[9] = {75, 9, 0, 13};
    const std::uint32_t wirePid = pid;
    std::memcpy(packet + 4, &wirePid, sizeof(wirePid));
    packet[8] = isGeneral;
    for (CHARACTER* online : m_memberOnline) {
        if (DESC* desc = online->GetDesc()) {
            desc->BufferedPacket(packet, 4);
            desc->BufferedPacket(packet + 4, 4);
            desc->Packet(packet + 8, 1);
        }
    }
    SaveMember(pid);
    return true;
}

namespace
{
struct GuildInvitation
{
    std::uint32_t invitee;
    std::uint32_t guild;
};

long GuildInviteEvent(LPEVENT event)
{
    const auto& invitation = *static_cast<GuildInvitation*>(event->info);
    if (CGuild* guild = CGuildManager::instance().FindGuild(invitation.guild))
    {
        sys_log(0, "GuildInviteEvent %s", guild->GetName());
        guild->InviteDeny(invitation.invitee);
    }
    return 0;
}

}

CGuild::GuildJoinErrCode CGuild::VerifyGuildJoinableCondition(CHARACTER* invitee)
{
    CGuildManager& manager = CGuildManager::instance();
    const auto withdrawalTime = get_global_time();
    const int lastWithdrawal = invitee->GetQuestFlag("guild_manage.new_withdraw_time");
    const auto sinceWithdrawal = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(withdrawalTime) - static_cast<std::uint32_t>(lastWithdrawal));
    if (sinceWithdrawal < manager.GetWithdrawDelay())
        return GuildJoinErrCode::WithdrawPenalty;
    const auto disbandTime = get_global_time();
    const int lastDisband = invitee->GetQuestFlag("guild_manage.new_disband_time");
    const auto sinceDisband = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(disbandTime) - static_cast<std::uint32_t>(lastDisband));
    if (sinceDisband < manager.GetDisbandDelay())
        return GuildJoinErrCode::DisbandPenalty;
    if (invitee->GetGuild())
        return GuildJoinErrCode::AlreadyJoined;

    const int count = GetMemberCount();
    const int maximum = GetMaxMemberCount();
    if (count >= maximum) {
        sys_log(0, "GuildName = %s, GetMemberCount() = %d, GetMaxMemberCount() = %d (32 + MAX(level(%d)-10, 0) * 2 + bonus(%d)",
                GetName(), count, maximum, static_cast<int>(GetLevel()), m_iMemberCountBonus);
        return GuildJoinErrCode::GuildFull;
    }
    return GuildJoinErrCode::None;
}

bool CGuild::CanJoinGuild(CHARACTER& invitee, CHARACTER& recipient)
{
    auto& quests = quest::CQuestManager::instance();
    switch (VerifyGuildJoinableCondition(&invitee)) {
        case GuildJoinErrCode::None:
            return true;
        case GuildJoinErrCode::WithdrawPenalty:
            recipient.ChatPacket(1, locale_find("<\261\346\265\345> \305\273\305\360\307\321 \310\304 %d\300\317\300\314 \301\366\263\252\301\366 \276\312\300\272 \273\347\266\367\300\272 \261\346\265\345\277\241 \303\312\264\353\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale), quests.GetEventFlag("guild_withdraw_delay"));
            return false;
        case GuildJoinErrCode::DisbandPenalty:
            recipient.ChatPacket(1, locale_find("<\261\346\265\345> \261\346\265\345\270\246 \307\330\273\352\307\321 \301\366 %d\300\317\300\314 \301\366\263\252\301\366 \276\312\300\272 \273\347\266\367\300\272 \261\346\265\345\277\241 \303\312\264\353\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale), quests.GetEventFlag("guild_disband_delay"));
            return false;
        case GuildJoinErrCode::AlreadyJoined:
            recipient.ChatPacket(1, "%s", locale_find("<\261\346\265\345> \273\363\264\353\271\346\300\314 \300\314\271\314 \264\331\270\245 \261\346\265\345\277\241 \274\323\307\330\300\326\275\300\264\317\264\331.", g_iUseLocale));
            return false;
        case GuildJoinErrCode::GuildFull:
            recipient.ChatPacket(1, "%s", locale_find("<\261\346\265\345> \303\326\264\353 \261\346\265\345\277\370 \274\366\270\246 \303\312\260\372\307\337\275\300\264\317\264\331.", g_iUseLocale));
            return false;
    }
    return false;
}

void CGuild::Invite(CHARACTER* inviter, CHARACTER* invitee)
{
    const char* error = nullptr;
    if (invitee->IsBlockMode(4))
        error = "<\261\346\265\345> \273\363\264\353\271\346\300\314 \261\346\265\345 \303\312\264\353 \260\305\272\316 \273\363\305\302\300\324\264\317\264\331.";
    else if (!(GetMemberAuth(inviter->GetPlayerID()) & 1))
        error = "<\261\346\265\345> \261\346\265\345\277\370\300\273 \303\312\264\353\307\322 \261\307\307\321\300\314 \276\370\275\300\264\317\264\331.";
    else if (inviter->GetEmpire() != invitee->GetEmpire())
        error = "<\261\346\265\345> \264\331\270\245 \301\246\261\271 \273\347\266\367\300\273 \261\346\265\345\277\241 \303\312\264\353\307\322 \274\366 \276\370\275\300\264\317\264\331.";
    if (error)
    {
        inviter->ChatPacket(1, "%s", locale_find(error, g_iUseLocale));
        return;
    }
    if (!CanJoinGuild(*invitee, *inviter))
        return;

    const DWORD pid = invitee->GetPlayerID();
    DESC* desc = invitee->GetDesc();
    if (!desc || m_GuildInviteEventMap.find(pid) != m_GuildInviteEventMap.end())
        return;

    void* data;
    const DWORD infoId = __AllocEventInfo(sizeof(GuildInvitation), &data);
    auto* invitation = static_cast<GuildInvitation*>(data);
    invitation->invitee = static_cast<std::uint32_t>(pid);
    invitation->guild = static_cast<std::uint32_t>(GetID());
    const auto delay = static_cast<std::int32_t>(static_cast<std::uint32_t>(passes_per_sec) * 10U);
    LPEVENT event = event_create_ex(GuildInviteEvent, infoId, delay,
                                   const_cast<char*>("guild.cpp"), 0x773);
    if (!event)
        return;
    m_GuildInviteEventMap.emplace(pid, event);

    const TPacketGCGuild packet = {75, 21, 14};
    const std::uint32_t id = GetID();
    TEMP_BUFFER buffer;
    buffer.write(&packet, sizeof(packet));
    buffer.write(&id, sizeof(id));
    buffer.write(m_data.name, 13);
    const int size = buffer.size();
    const void* payload = buffer.read_peek();
    if (DESC* inviteeDesc = invitee->GetDesc())
        inviteeDesc->Packet(payload, size);
}

void CGuild::InviteAccept(CHARACTER* invitee)
{
    const auto event = m_GuildInviteEventMap.find(invitee->GetPlayerID());
    if (event == m_GuildInviteEventMap.end())
    {
        sys_log(0, "GuildInviteAccept from not invited character(invite guild: %s, invitee: %s)",
                GetName(), invitee->GetName());
        return;
    }
    event_cancel(&event->second);
    m_GuildInviteEventMap.erase(event);
    if (CanJoinGuild(*invitee, *invitee))
        RequestAddMember(invitee, 15);
}

void CGuild::InviteDeny(DWORD pid)
{
    const auto event = m_GuildInviteEventMap.find(pid);
    if (event == m_GuildInviteEventMap.end())
    {
        sys_log(0, "GuildInviteDeny from not invited character(invite guild: %s, invitee PID: %d)",
                GetName(), static_cast<int>(pid));
        return;
    }
    event_cancel(&event->second);
    m_GuildInviteEventMap.erase(event);
}

bool CGuild::ChargeSP(CHARACTER* ch, int amount)
{
    if (amount < 0 || static_cast<std::int64_t>(amount) * 100 > ch->GetGold())
        return false;
    const std::int64_t maxPower = m_data.max_power;
    const std::int64_t power = m_data.power;
    const auto capacity = std::max<std::int64_t>(0, maxPower - power);
    amount = static_cast<int>(std::min<std::int64_t>(amount, capacity));
    const int cost = amount * 100;
    ch->PointChange(POINT_GOLD, -cost, false, false);
    DBManager::instance().SendMoneyLog(5, 1, -cost);
    SendDBSkillUpdate(amount);
    ch->ChatPacket(1, locale_find("<\261\346\265\345> %lu\300\307 \277\353\275\305\267\302\300\273 \310\270\272\271\307\317\277\264\275\300\264\317\264\331.", g_iUseLocale), static_cast<unsigned long>(amount));
    return true;
}

void CGuild::RequestDepositMoney(CHARACTER* ch, int gold)
{
    if (!ch->CanDeposit())
    {
        ch->ChatPacket(1, locale_find("\074\261\346\265\345\076\040\300\341\275\303\310\304\277\241\040\300\314\277\353\307\330\301\326\275\312\275\303\277\300", g_iUseLocale));
        return;
    }
    if (gold > ch->GetGold())
        return;

    ch->PointChange(POINT_GOLD, static_cast<std::int32_t>(0u - static_cast<std::uint32_t>(gold)), false, false);
    const TPacketGDGuildMoney packet = {static_cast<std::uint32_t>(GetID()), gold};
    db_clientdesc->DBPacket(0x33, 0, &packet, sizeof(packet));
    char detail[64];
    std::snprintf(detail, sizeof(detail), "%lu %s", GetID(), GetName());
    LogManager::instance().CharLog(ch->GetPlayerID(), ch->GetX(), ch->GetY(), gold, "GUILD_DEPOSIT", detail);
    ch->UpdateDepositPulse();
    sys_log(0, "GUILD: DEPOSIT %s:%lu player %s[%lu] gold %d",
        GetName(), GetID(), ch->GetName(), ch->GetPlayerID(), gold);
}

void CGuild::RequestDisband(DWORD pid)
{
    if (GetMasterPID() != pid)
        return;
    const TPacketGuild packet = {static_cast<std::uint32_t>(GetID()), 0};
    db_clientdesc->DBPacket(0x19, 0, &packet, sizeof(packet));
    building::CManager::instance().ClearLandByGuildID(GetID());
}

bool CGuild::RequestRemoveMember(DWORD pid)
{
    const auto member = m_member.find(pid);
    if (member == m_member.end() || member->second.grade == 1)
        return false;
    const TPacketGuild packet = {static_cast<std::uint32_t>(GetID()), static_cast<std::uint32_t>(pid)};
    db_clientdesc->DBPacket(0x16, 0, &packet, sizeof(packet));
    return true;
}

void CGuild::P2PLoginMember(DWORD pid)
{
    if (m_member.find(pid) == m_member.end()) {
        _sys_err("P2PLoginMember", 237, "GUILD [%d] is not a memeber of guild.", static_cast<int>(pid));
        return;
    }

    m_memberP2POnline.insert(pid);
    for (std::set<CHARACTER*>::iterator it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
        SendLoginPacket(*it, pid);
}

void CGuild::P2PLogoutMember(DWORD pid)
{
    if (m_member.find(pid) == m_member.end()) {
        _sys_err("P2PLogoutMember", 281, "GUILD [%d] is not a memeber of guild.", static_cast<int>(pid));
        return;
    }

    m_memberP2POnline.erase(pid);
    for (std::set<CHARACTER*>::iterator it = m_memberOnline.begin(); it != m_memberOnline.end(); ++it)
        SendLogoutPacket(*it, pid);
}

void CGuild::SendLoginPacket(CHARACTER* character, CHARACTER* login)
{
    SendLoginPacket(character, login->GetPlayerID());
}

void CGuild::SendLoginPacket(CHARACTER* character, DWORD pid)
{
    if (character->GetDesc()) {
        TPacketGCGuild packet;
        packet.header = 75;
        packet.size = 8;
        packet.subheader = 0;
        const std::uint32_t wirePid = pid;
        TEMP_BUFFER buffer;
        buffer.write(&packet, sizeof(packet));
        buffer.write(&wirePid, sizeof(wirePid));
        const int size = buffer.size();
        const void* data = buffer.read_peek();
        character->GetDesc()->Packet(data, size);
    }
}

void CGuild::SendLogoutPacket(CHARACTER* character, CHARACTER* logout)
{
    SendLogoutPacket(character, logout->GetPlayerID());
}

void CGuild::SendLogoutPacket(CHARACTER* character, DWORD pid)
{
    if (character->GetDesc()) {
        TPacketGCGuild packet;
        packet.header = 75;
        packet.size = 8;
        packet.subheader = 1;
        const std::uint32_t wirePid = pid;
        TEMP_BUFFER buffer;
        buffer.write(&packet, sizeof(packet));
        buffer.write(&wirePid, sizeof(wirePid));
        const int size = buffer.size();
        const void* data = buffer.read_peek();
        character->GetDesc()->Packet(data, size);
    }
}

void CGuild::Chat(const char* text)
{
    P2PChat(text);
    BYTE header[6] = {6, 0};
    const auto guild = static_cast<std::uint32_t>(GetID());
    std::memcpy(header + 2, &guild, sizeof(guild));
    char payload[92] = {};
    std::strncpy(payload + 6, text, 85);
    P2P_MANAGER::instance().Send(header, sizeof(header));
    P2P_MANAGER::instance().Send(payload, sizeof(payload));
}

namespace {
struct FGuildChat
{
    const char* text;

    void operator()(LPCHARACTER member)
    {
        member->ChatPacket(4, "%s", text);
    }
};
}

void CGuild::P2PChat(const char* text)
{
    FGuildChat send{text};
    for (LPCHARACTER member : m_memberOnline)
        send(member);
}

void CGuild::LogoutMember(CHARACTER* character)
{
    DWORD pid=character->GetPlayerID();
    if (m_member.find(pid)==m_member.end()) {
        _sys_err("LogoutMember", 299, "GUILD %s[%d] is not a memeber of guild.",
                 character->GetName(), static_cast<std::int32_t>(pid));
        return;
    }
    m_memberOnline.erase(character);
    std::set<CHARACTER*>::iterator it=m_memberOnline.begin();
    while (it!=m_memberOnline.end()) {
        SendLogoutPacket(*it, character);
        ++it;
    }
}

void CGuild::SetMemberCountBonus(int bonus)
{
    m_iMemberCountBonus = bonus;
    sys_log(0, "GUILD_IS_FULL_BUG : Bonus set to %d(val:%d)", bonus, bonus);
}

// 0808b360
void CGuild::AddMember(TPacketDGGuildMember* packet)
{
    const DWORD pid = packet->dwPID;
    auto existing = m_member.find(pid);
    if (existing == m_member.end()) {
        m_member.emplace(pid, TGuildMember(pid, packet->bGrade, packet->isGeneral,
                                          packet->bJob, packet->bLevel, packet->dwOffer, packet->szName));
    } else {
        TGuildMember& member = existing->second;
        member.pid = pid;
        member.grade = packet->bGrade;
        member.job = packet->bJob;
        member.offer_exp = packet->dwOffer;
        member.is_general = packet->isGeneral;
    }

    CGuildManager::instance().Link(pid, this);
    SendListOneToAll(pid);

    CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(pid);
    const auto offer = static_cast<unsigned long>(packet->dwOffer);
    const auto level = static_cast<unsigned>(packet->bLevel);
    const auto job = static_cast<unsigned>(packet->bJob);
    const auto grade = static_cast<unsigned>(packet->bGrade);
    const auto loggedPid = static_cast<unsigned long>(packet->dwPID);
    sys_log(0, "GUILD: AddMember PID %lu, grade %u, job %u, level %u, offer %lu, name %s ptr %p",
            loggedPid, grade, job, level, offer, packet->szName, static_cast<void*>(character));
    if (character)
        LoginMember(character);
    else
        P2PLoginMember(pid);
}

// 0808bec0, 0808bee0
void CGuild::SendListOneToAll(CHARACTER* character)
{
    SendListOneToAll(character->GetPlayerID());
}

void CGuild::SendListOneToAll(DWORD pid)
{
    TGuildMember* member = GetMember(pid);
    if (!member)
        return;

    const TPacketGCGuild packet = {0x4b, 42, 2};
    for (CHARACTER* online : m_memberOnline) {
        if (DESC* desc = online->GetDesc()) {
            TEMP_BUFFER buffer;
            buffer.write(&packet, sizeof(packet));
            WriteGuildMember(buffer, *member);
            const int size = buffer.size();
            const void* data = buffer.read_peek();
            desc->Packet(data, size);
        }
    }
}

// 0808bcf0
void CGuild::SendOnlineRemoveOnePacket(DWORD pid)
{
    const TPacketGCGuild packet = {0x4b, 8, 5};
    const std::uint32_t wirePid = pid;
    TEMP_BUFFER buffer;
    buffer.write(&packet, sizeof(packet));
    buffer.write(&wirePid, sizeof(wirePid));
    const int size = buffer.size();
    const void* data = buffer.read_peek();
    Packet(data, size);
}

// 0808b760
bool CGuild::RemoveMember(DWORD pid)
{
    sys_log(0, "Receive Guild P2P RemoveMember");
    const auto member = m_member.find(pid);
    if (member == m_member.end() || member->second.grade == 1)
        return false;

    if (member->second.is_general)
        --m_general_count;
    m_member.erase(member);
    SendOnlineRemoveOnePacket(pid);
    CGuildManager::instance().Unlink(pid);
    if (CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(pid)) {
        m_memberOnline.erase(character);
        character->SetGuild(nullptr);
    }
    return true;
}

// 0808daa0
void CGuild::Disband()
{
    sys_log(0, "GUILD: Disband %s:%lu", GetName(), GetID());
    for (CHARACTER* character : m_memberOnline) {
        character->SetGuild(nullptr);
        SendOnlineRemoveOnePacket(character->GetPlayerID());
        const auto withdrawalTime = static_cast<std::int32_t>(get_global_time());
        character->SetQuestFlag("guild_manage.new_withdraw_time", withdrawalTime);
    }
    for (const auto& member : m_member)
        CGuildManager::instance().Unlink(member.first);
}

// 0808b000
CGuild::~CGuild()
{
    for (auto& invite : m_GuildInviteEventMap)
        event_cancel(&invite.second);
}

// 0808f700
void CGuild::ChangeMemberData(DWORD pid, DWORD offer, BYTE level, BYTE grade)
{
    TGuildMember* member = GetMember(pid);
    if (!member)
        return;

    member->level = level;
    member->offer_exp = offer;
    member->grade = grade;
    SendMemberData(*member);
}

void CGuild::SendMemberData(TGuildMember& member)
{
    const TPacketGCGuild packet = {0x4b, 17, 2};
    const std::uint32_t pid = member.pid;
    const std::uint32_t offer = member.offer_exp;
    member._dummy = 0;
    BYTE wire[13];
    std::memcpy(wire, &pid, sizeof(pid));
    wire[4] = member.grade;
    wire[5] = member.is_general;
    wire[6] = member.job;
    wire[7] = member.level;
    std::memcpy(wire + 8, &offer, sizeof(offer));
    wire[12] = member._dummy;

    for (CHARACTER* online : m_memberOnline) {
        if (DESC* desc = online->GetDesc()) {
            desc->BufferedPacket(&packet, sizeof(packet));
            desc->Packet(wire, sizeof(wire));
        }
    }
}
