#include "char.h"
#include "desc.h"
#include "packet.h"
#include "party.h"
#include "questmanager.h"
#include "questpc.h"
#include "../../libthecore/include/log.h"

#include <cfenv>
#include <cmath>
#include <cstdint>
#include <limits>
#include <sstream>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

namespace quest {
void combine_lua_string(lua_State* state, std::ostringstream& output);

namespace {
std::int32_t PartyFlagValue(lua_Number value)
{
    if (!(value > -2147483649.0 && value < 2147483648.0)) {
        std::feraiseexcept(FE_INVALID);
        return INT32_MIN;
    }
    return static_cast<std::int32_t>(value);
}

CParty* CurrentParty()
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    return character ? character->GetParty() : nullptr;
}

void SendCinematicPacket(CHARACTER* character, const packet_script& packet, const char* data, std::size_t len)
{
    DESC* desc = character->GetDesc();
    sys_log(0, "CINEMASEND_TRY %s %p", character->GetName(), static_cast<void*>(desc));
    if (desc) {
        sys_log(0, "CINEMASEND %s", character->GetName());
        desc->BufferedPacket(&packet, sizeof(packet));
        desc->Packet(data, len);
    }
}
}

struct FCinematicSender
{
    const char* str;
    packet_script packet;
    std::size_t len;

    void operator()(CHARACTER* character)
    {
        SendCinematicPacket(character, packet, str, len);
    }
};

struct FRunCinematicSender
{
    std::string data;
    packet_script packet;

    void operator()(CHARACTER* character)
    {
        SendCinematicPacket(character, packet, data.data(), data.size());
    }
};

namespace {
void SendCinematic(CParty& party, const std::string& script, bool run)
{
    if (script.size() > std::numeric_limits<WORD>::max() - sizeof(packet_script))
        return;
    const packet_script packet = {
        0x2d, static_cast<WORD>(script.size() + sizeof(packet_script)), 5,
        static_cast<WORD>(script.size())
    };
    if (run)
        party.ForEachNearMember(FRunCinematicSender{script, packet});
    else
        party.ForEachNearMember(FCinematicSender{script.data(), packet, script.size()});
}
}

struct FPartyChat
{
    int iChatType;
    const char* str;

    void operator()(CHARACTER* character)
    {
        character->ChatPacket(static_cast<BYTE>(iChatType), "%s", str);
    }
};

struct FPartyClearReady
{
    void operator()(CHARACTER* character)
    {
        character->RemoveAffect(212);
    }
};

namespace {
int PartyChat(lua_State* state, BYTE type)
{
    std::ostringstream message;
    combine_lua_string(state, message);
    if (CParty* party = CurrentParty()) {
        const std::string text = message.str();
        party->ForEachOnlineMember(FPartyChat{type, text.c_str()});
    }
    return 0;
}
}

int party_is_leader(lua_State* state)
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    CParty* party = character ? character->GetParty() : nullptr;
    bool isLeader = false;
    if (party) {
        const auto leaderId = static_cast<std::uint32_t>(party->GetLeaderPID());
        const auto playerId = static_cast<std::uint32_t>(character->GetPlayerID());
        isLeader = leaderId == playerId;
    }
    lua_pushboolean(state, isLeader);
    return 1;
}

int party_is_party(lua_State* state)
{
    lua_pushboolean(state, CurrentParty() != nullptr);
    return 1;
}

int party_set_flag(lua_State* state)
{
    CParty* party = CurrentParty();
    if (party && lua_isstring(state, 1) && lua_isnumber(state, 2)) {
        const auto value = PartyFlagValue(lua_tonumber(state, 2));
        party->SetFlag(lua_tostring(state, 1), value);
    }
    return 0;
}

int party_get_flag(lua_State* state)
{
    CParty* party = CurrentParty();
    lua_pushnumber(state, party && lua_isstring(state, 1) ? party->GetFlag(lua_tostring(state, 1)) : 0);
    return 1;
}

namespace {
struct FSetQuestFlag
{
    std::string flagname;
    int value;

    void operator()(CHARACTER* member)
    {
        if (member->GetDesc()) {
            if (PC* pc = CQuestManager::instance().GetPCForce(member->GetPlayerID()))
                pc->SetFlag(flagname, value, false);
        }
    }
};
}


int party_set_quest_flag(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    CHARACTER* character = manager.GetCurrentCharacterPtr();
    if (!character || !manager.GetCurrentPC() || !lua_isstring(state, 1))
        return 0;

    std::string name = manager.GetCurrentQuestName();
    name += '.';
    name += lua_tostring(state, 1);
    const auto value = PartyFlagValue(std::rint(lua_tonumber(state, 2)));
    FSetQuestFlag apply{name, value};
    if (CParty* party = character->GetParty())
        party->ForEachOnlineMember(apply);
    else
        apply(character);
    return 0;
}

int party_chat(lua_State* state) { return PartyChat(state, 0); }
int party_syschat(lua_State* state) { return PartyChat(state, 1); }

int party_get_near_count(lua_State* state)
{
    CParty* party = CurrentParty();
    lua_pushnumber(state, party ? static_cast<std::uint32_t>(party->GetNearMemberCount()) : 0u);
    return 1;
}

int party_show_cinematic(lua_State* state)
{
    if (lua_isstring(state, 1)) {
        const char* script = lua_tostring(state, 1);
        sys_log(0, "CINEMA %s", script);
        CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
        CParty* party = character ? character->GetParty() : nullptr;
        if (party) {
            party->Update();
            const char* payload = lua_tostring(state, 1);
            if (CParty* updatedParty = character->GetParty())
                SendCinematic(*updatedParty, payload, false);
        }
    }
    return 0;
}

int party_run_cinematic(lua_State* state)
{
    if (lua_isstring(state, 1)) {
        const char* script = lua_tostring(state, 1);
        sys_log(0, "RUN_CINEMA %s", script);
        CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
        CParty* party = character ? character->GetParty() : nullptr;
        if (party) {
            party->Update();
            const char* payload = lua_tostring(state, 1);
            const std::string command = std::string("[RUN_CINEMA value;") + payload + "]";
            if (CParty* updatedParty = character->GetParty())
                SendCinematic(*updatedParty, command, true);
        }
    }
    return 0;
}

int party_get_max_level(lua_State* state)
{
    CParty* party = CurrentParty();
    lua_pushnumber(state, party ? party->GetMemberMaxLevel() : 1);
    return 1;
}

int party_clear_ready(lua_State*)
{
    if (CParty* party = CurrentParty())
        party->ForEachNearMember(FPartyClearReady{});
    return 0;
}

// 0x08146700, table at 0x082285cc.
void RegisterPartyFunctionTable()
{
    luaL_reg functions[] = {
        {"is_leader", party_is_leader},
        {"is_party", party_is_party},
        {"setf", party_set_flag},
        {"getf", party_get_flag},
        {"setqf", party_set_quest_flag},
        {"chat", party_chat},
        {"syschat", party_syschat},
        {"get_near_count", party_get_near_count},
        {"show_cinematic", party_show_cinematic},
        {"run_cinematic", party_run_cinematic},
        {"get_max_level", party_get_max_level},
        {"clear_ready", party_clear_ready},
        {nullptr, nullptr}
    };
    CQuestManager::instance().AddLuaFunctionTable("party", functions);
}
}
