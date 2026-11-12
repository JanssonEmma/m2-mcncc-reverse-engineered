#include "char.h"
#include "desc.h"
#include "packet.h"
#include "party.h"
#include "questmanager.h"
#include "questpc.h"
#include "../../libthecore/include/log.h"

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
    if (!(value > -2147483649.0 && value < 2147483648.0))
        return INT32_MIN;
    return static_cast<std::int32_t>(value);
}

CParty* CurrentParty()
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    return character ? character->GetParty() : nullptr;
}

void SendCinematic(CParty& party, const std::string& script)
{
    if (script.size() > std::numeric_limits<WORD>::max() - sizeof(packet_script))
        return;
    packet_script packet = {
        0x2d, static_cast<WORD>(script.size() + sizeof(packet_script)), 5,
        static_cast<WORD>(script.size())
    };
    party.ForEachNearMember([&](CHARACTER* character) {
        DESC* desc = character->GetDesc();
        sys_log(0, "CINEMASEND_TRY %s %p", character->GetName(), static_cast<void*>(desc));
        if (desc) {
            sys_log(0, "CINEMASEND %s", character->GetName());
            desc->BufferedPacket(&packet, sizeof(packet));
            desc->Packet(script.data(), script.size());
        }
    });
}

int PartyChat(lua_State* state, BYTE type)
{
    std::ostringstream message;
    combine_lua_string(state, message);
    if (CParty* party = CurrentParty()) {
        const std::string text = message.str();
        party->ForEachOnlineMember([&](CHARACTER* character) {
            character->ChatPacket(type, "%s", text.c_str());
        });
    }
    return 0;
}
}

int party_is_leader(lua_State* state)
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    CParty* party = character ? character->GetParty() : nullptr;
    lua_pushboolean(state, party && party->GetLeaderPID() == character->GetPlayerID());
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

int party_set_quest_flag(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    CHARACTER* character = manager.GetCurrentCharacterPtr();
    if (!character || !manager.GetCurrentPC() || !lua_isstring(state, 1))
        return 0;

    const std::string name = manager.GetCurrentQuestName() + "." + lua_tostring(state, 1);
    const auto value = PartyFlagValue(std::rint(lua_tonumber(state, 2)));
    const auto apply = [&](CHARACTER* member) {
        if (member->GetDesc()) {
            if (PC* pc = manager.GetPCForce(member->GetPlayerID()))
                pc->SetFlag(name, value, false);
        }
    };
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
    lua_pushnumber(state, party ? party->GetNearMemberCount() : 0);
    return 1;
}

int party_show_cinematic(lua_State* state)
{
    if (lua_isstring(state, 1)) {
        const char* script = lua_tostring(state, 1);
        sys_log(0, "CINEMA %s", script);
        if (CParty* party = CurrentParty()) {
            party->Update();
            SendCinematic(*party, lua_tostring(state, 1));
        }
    }
    return 0;
}

int party_run_cinematic(lua_State* state)
{
    if (lua_isstring(state, 1)) {
        const char* script = lua_tostring(state, 1);
        sys_log(0, "RUN_CINEMA %s", script);
        if (CParty* party = CurrentParty()) {
            party->Update();
            SendCinematic(*party, std::string("[RUN_CINEMA value;") + lua_tostring(state, 1) + "]");
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
        party->ForEachNearMember([](CHARACTER* character) { character->RemoveAffect(212); });
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
