#include "cstdio"
#include "char_manager.h"
#include "cmd.h"
#include "db.h"
#include "priv_manager.h"
#include "fishing.h"
#include "mining.h"
#include <cmath>
#include <limits>
#include <cstring>
#include <dirent.h>
#include <cstdint>
#include "constants.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "guild.h"
#include "guild_manager.h"
#include "../../libthecore/include/hangul.h"
#include "OXEvent.h"
#include "buffer_manager.h"
#include "desc.h"
#include "packet.h"
#include "p2p.h"
#include "regen.h"
#include "sectree_manager.h"
#include "utils.h"
#include <ctime>
#include <climits>
#include <cstdio>
#include <fenv.h>
#include <math.h>
#include "char.h"
#include "locale_service.h"
#include "../../libthecore/include/utils.h"
#include <sstream>
#include "config.h"
#include "../../libthecore/include/log.h"
// Original questlua.cpp closed methods only; full VM/startup/RunState remains pending.
#include "questmanager.h"
#include "questevent.h"
extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
#include "../../liblua/include/lualib.h"
}
void SendNotice(const char* text);
void CHARACTER_AddGotoInfo(const std::string& name, BYTE empire, int map, DWORD x, DWORD y);
void CHARACTER_SetBGMVolumeEnable();
void CHARACTER_AddBGMInfo(unsigned mapIndex, const char* name, float volume);

namespace quest {
bool CQuestManager::LoadQuestLuaFiles()
{
    char settings[256];
    char questlib[256];
    char locale[256];
    std::snprintf(settings, 255, "%s/settings.lua", LocaleService_GetBasePath().c_str());
    std::snprintf(questlib, 255, "%s/questlib.lua", LocaleService_GetQuestPath().c_str());
    if (LC_IsGermany() || LC_IsEurope())
        std::snprintf(locale, 255, "%s/locale.lua", g_stQuestDir.c_str());
    else
        std::snprintf(locale, 255, "%s/locale_%s.lua", g_stQuestDir.c_str(), g_stLocale.c_str());

    struct Script {
        const char* label;
        const char* path;
        int line;
    };
    const Script scripts[] = {
        {"LoadSettings", settings, 0x792},
        {"LoadQuestlib", questlib, 0x79a},
        {"LoadQuestLocale", locale, 0x7a2}
    };
    for (const Script& script : scripts) {
        const int result = lua_dofile(L, script.path);
        sys_log(0, "%s(%s), returns %d", script.label, script.path, result);
        if (result) {
            QuestError("InitializeLua", script.line, "%s(%s) FAILURE", script.label, script.path);
            return false;
        }
    }

    for (const std::string& objectDir : g_setQuestObjectDir) {
        const std::string stateDir = objectDir + "/state/";
        DIR* directory = opendir(stateDir.c_str());
        if (!directory)
            continue;
        unsigned index = 0;
        while (const dirent* entry = readdir(directory)) {
            if (entry->d_name[0] == '.')
                continue;
            const std::string name(entry->d_name);
            const std::string path = stateDir + name;
            RegisterQuest(name, ++index);
            const int result = lua_dofile(L, path.c_str());
            sys_log(0, "QUEST: loading %s, returns %d", path.c_str(), result);
            BuildStateIndexToName(name.c_str());
        }
        closedir(directory);
    }

    lua_setgcthreshold(L, 0);
    lua_newtable(L);
    lua_setglobal(L, "__codecache");
    return true;
}

//0805cd80: originalNRVOdirectretstorage. Defaultcounters remain indeterminate; runtime not forced.
QuestState CQuestManager::OpenState(const std::string& quest_name,int state_index) {
    (void)quest_name;
    QuestState qs;
    qs.args=0;
    qs.st=state_index;
    qs.co=lua_newthread(L);
    qs.ico=luaL_ref(L,LUA_REGISTRYINDEX);
    return qs;
}
//0805d000: CURRENT ownerL registry, co used only as guard; no coroutine/VM close or fieldreset.
void CQuestManager::CloseState(QuestState& qs) {
    if(qs.co) {
        luaL_unref(L,LUA_REGISTRYINDEX,qs.ico);
        qs.co=0;
    }
}
//0805cd10: qs unused; only script append and complete actualSendScript, no stateclose.
void CQuestManager::GotoEndState(QuestState& qs) {
    (void)qs;
    AddScript(std::string("[DONE]"));
    SendScript();
}
}

// Complete original questlua.cpp yield/script bridges; RunState and Confirm remain separate.
namespace quest {
//0805cc90: write suspendBEFOREscriptappend/SendScript; no Lua stack mutation.
void CQuestManager::GotoPauseState(QuestState& qs){qs.suspend_state=1;AddScript(std::string("[NEXT]"));SendScript();}
//0805cc10
void CQuestManager::GotoInputState(QuestState& qs){qs.suspend_state=3;AddScript(std::string("[INPUT]"));SendScript();}
//0805cb90
void CQuestManager::GotoSelectItemState(QuestState& qs){qs.suspend_state=5;AddScript(std::string("[SELECT_ITEM]"));SendScript();}
//0805c1d0: full table traversal, invalidentry diagnostics AND normal complete script path.
void CQuestManager::GotoSelectState(QuestState& qs){
    lua_checkstack(qs.co,1);
    int n=luaL_getn(qs.co,-1);
    qs.args=static_cast<short>(n);
    std::ostringstream os;
    os<<"[QUESTION ";
    for(int i=1;i<=n;++i){
        lua_rawgeti(qs.co,-1,i);
        if(lua_isstring(qs.co,-1)){
            if(i!=1)os<<"|";
            os<<i<<";"<<lua_tostring(qs.co,-1);
        }else{
            const char* type=lua_typename(qs.co,-1); // Original -1 typecode bug, NOTlua_type repair.
            CQuestManager::instance().QuestError("GotoSelectState",2021,"SELECT wrong data %s",type);
            CQuestManager::instance().QuestError("GotoSelectState",2022,"here");
        }
        lua_pop(qs.co,1);
    }
    os<<"]";
    AddScript(os.str());
    qs.suspend_state=2;
    if(test_server)sys_log(0,"%s",m_strScript.c_str());
    SendScript();
}
}

namespace quest {
// Original 0805a5e0. The final global assignment honors Lua metamethods.
void CQuestManager::AddLuaFunctionTable(const char* name, luaL_reg* functions) {
    lua_newtable(L);
    for (; functions->name; ++functions) {
        lua_pushstring(L, functions->name);
        lua_pushcfunction(L, functions->func);
        lua_rawset(L, -3);
    }
    lua_setglobal(L, name);
}

// Original 0805a690. Lua's number/string predicates include coercible values.
void CQuestManager::BuildStateIndexToName(const char* quest_name) {
    const int top = lua_gettop(L);
    lua_getglobal(L, quest_name);
    if (lua_isnil(L, -1)) {
        CQuestManager::instance().QuestError("BuildStateIndexToName", 1740,
                                            "QUEST wrong quest state file for quest %s", quest_name);
    } else {
        lua_pushnil(L);
        while (lua_next(L, -2)) {
            if (lua_isstring(L, -2) && lua_isnumber(L, -1)) {
                lua_pushvalue(L, -2);
                lua_rawset(L, -4);
            } else {
                lua_pop(L, 1);
            }
        }
    }
    lua_settop(L, top);
}
}

namespace quest {

// The original i386 casts use a truncating signed 32-bit x87 conversion.
static int LuaTruncateInt(lua_Number value) {
    if (!(value > -2147483649.0 && value < 2147483648.0)) {
        feraiseexcept(FE_INVALID);
        return INT_MIN;
    }
    return static_cast<int>(value);
}

// 08052b40
void combine_lua_string(lua_State* state, std::ostringstream& output) {
    const int count = lua_gettop(state);
    for (int index = 1; index <= count; ++index) {
        if (lua_isstring(state, index)) {
            output << lua_tostring(state, index);
        } else if (lua_isnumber(state, index)) {
            char text[20];
            std::sprintf(text, "%.14g\n", lua_tonumber(state, index));
            output << text;
        }
    }
}

// 08053e70
int _get_locale(lua_State* state) {
    lua_pushstring(state, g_stLocale.c_str());
    return 1;
}

// 08053eb0
int _number(lua_State* state) {
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2)) {
        lua_pushnumber(state, 0);
        return 1;
    }
    const int last = LuaTruncateInt(lua_tonumber(state, 2));
    const int first = LuaTruncateInt(lua_tonumber(state, 1));
    lua_pushnumber(state, number_ex(first, last, const_cast<char*>("questlua.cpp"), 315));
    return 1;
}

// 08055b70
int _say(lua_State* state) {
    std::ostringstream output;
    combine_lua_string(state, output);
    const std::string script = output.str() + "[ENTER]";
    CQuestManager::Instance().AddScript(script);
    return 0;
}

// 08055df0
int _raw_script(lua_State* state) {
    if (test_server)
        sys_log(0, "_raw_script : %s ", lua_tostring(state, -1));
    if (lua_isstring(state, -1)) {
        const std::string script(lua_tostring(state, -1));
        CQuestManager::Instance().AddScript(script);
    } else {
        const char* name = CQuestManager::instance().GetCurrentQuestName().c_str();
        CQuestManager::instance().QuestError("_raw_script", 706,
                                             "QUEST wrong argument: questname: %s", name);
    }
    return 0;
}

// 08056270
int _set_skin(lua_State* state) {
    if (lua_isnumber(state, -1)) {
        const double rounded = rint(lua_tonumber(state, -1));
        CQuestManager& manager = CQuestManager::Instance();
        manager.SetSkinStyle(LuaTruncateInt(rounded));
    } else {
        CQuestManager::instance().QuestError("_set_skin", 739, "QUEST wrong skin index");
    }
    return 0;
}

// 08056490
int _get_server_timer_arg(lua_State* state) {
    lua_pushnumber(state, CQuestManager::instance().GetServerTimerArg());
    return 1;
}

// 08057ac0
int _getnpcid(lua_State* state) {
    const char* name = lua_tostring(state, -1);
    CQuestManager& manager = CQuestManager::instance();
    lua_pushnumber(state, manager.FindNPCIDByName(std::string(name)));
    return 1;
}

// 08057b90
int _is_test_server(lua_State* state) {
    lua_pushboolean(state, test_server);
    return 1;
}

// 0805a4b0
int _is_allow_map(lua_State* state) {
    if (test_server) {
        CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
        const bool allowed = map_allow_find(LuaTruncateInt(lua_tonumber(state, 1)));
        const int map = LuaTruncateInt(lua_tonumber(state, 1));
        character->ChatPacket(1, "allow map %d %d", map, static_cast<int>(allowed));
    }
    const bool allowed = map_allow_find(LuaTruncateInt(lua_tonumber(state, 1)));
    lua_pushnumber(state, allowed);
    return 1;
}


// 08055fb0
int _left_image(lua_State* state) {
    if (lua_isstring(state, -1)) {
        const std::string image(lua_tostring(state, -1));
        const std::string script = "[LEFTIMAGE src;" + image + "]";
        CQuestManager::Instance().AddScript(script);
    }
    return 0;
}

// 08056110
int _top_image(lua_State* state) {
    if (lua_isstring(state, -1)) {
        const std::string image(lua_tostring(state, -1));
        const std::string script = "[TOPIMAGE src;" + image + "]";
        CQuestManager::Instance().AddScript(script);
    }
    return 0;
}

// 08057120
int _time_to_str(lua_State* state) {
    const std::time_t instant = static_cast<std::time_t>(LuaTruncateInt(lua_tonumber(state, -1)));
    lua_pushstring(state, std::asctime(std::gmtime(&instant)));
    return 1;
}

// 08057180
int _chat(lua_State* state) {
    std::ostringstream output;
    combine_lua_string(state, output);
    const std::string message = output.str();
    CQuestManager::Instance().GetCurrentCharacterPtr()->ChatPacket(0, "%s", message.c_str());
    return 0;
}

// 080573d0
int _cmdchat(lua_State* state) {
    std::ostringstream output;
    combine_lua_string(state, output);
    const std::string message = output.str();
    CQuestManager::Instance().GetCurrentCharacterPtr()->ChatPacket(5, "%s", message.c_str());
    return 0;
}

// 08057620
int _syschat(lua_State* state) {
    std::ostringstream output;
    combine_lua_string(state, output);
    const std::string message = output.str();
    CQuestManager::Instance().GetCurrentCharacterPtr()->ChatPacket(1, "%s", message.c_str());
    return 0;
}

// 08057870
int _notice(lua_State* state) {
    std::ostringstream output;
    combine_lua_string(state, output);
    const std::string message = output.str();
    CQuestManager::Instance().GetCurrentCharacterPtr()->ChatPacket(2, "%s", message.c_str());
    return 0;
}

}

namespace quest {
// 0805cde0. Unknown yields take the original error/end path.
namespace {
DWORD ReadLuaDword(lua_State* state, int index = 1)
{
    const lua_Number value = lua_tonumber(state, index);
    if (!(value >= -9223372036854775808.0 && value < 9223372036854775808.0))
        return 0;
    return static_cast<std::uint32_t>(static_cast<std::int64_t>(value));
}
}

int _set_bgm_volume_enable(lua_State*)
{
    CHARACTER_SetBGMVolumeEnable();
    return 0;
}

int _add_bgm_info(lua_State* state)
{
    if (lua_isnumber(state, 1) && lua_isstring(state, 2)) {
        const auto map = static_cast<std::uint32_t>(LuaTruncateInt(lua_tonumber(state, 1)));
        const char* name = lua_tostring(state, 2);
        if (name) {
            const float volume = lua_isnumber(state, 3) ? static_cast<float>(lua_tonumber(state, 3)) : 0.02f;
            CHARACTER_AddBGMInfo(map, name, volume);
        }
    }
    return 0;
}

int _add_ox_quiz(lua_State* state)
{
    const int level = LuaTruncateInt(lua_tonumber(state, 1));
    const char* question = lua_tostring(state, 2);
    const bool answer = lua_toboolean(state, 3) != 0;
    if (!COXEventManager::instance().AddQuiz(static_cast<BYTE>(level), question, answer))
        sys_log(0, "OXEVENT : Cannot add quiz. %d %s %d", level, question ? question : "", answer);

    // Preserve the original Lua stack result.
    return 1;
}

int _find_pc(lua_State* state)
{
    CHARACTER* found = nullptr;
    if (lua_isstring(state, 1))
        found = CHARACTER_MANAGER::instance().FindPC(lua_tostring(state, 1));
    else
        CQuestManager::instance().QuestError("_find_pc", 0x214, "invalid argument");

    const auto vid = found ? static_cast<std::int32_t>(static_cast<DWORD>(found->GetVID())) : 0;
    lua_pushnumber(state, vid);
    return 1;
}

int _pc_name(lua_State* state)
{
    CHARACTER* found = nullptr;
    if (lua_isnumber(state, 1))
        found = CHARACTER_MANAGER::instance().Find(ReadLuaDword(state));
    else
        CQuestManager::instance().QuestError("_pc_name", 0x245, "invalid argument");

    lua_pushstring(state, found ? found->GetName() : "");
    return 1;
}

int _find_npc_by_vnum(lua_State* state)
{
    DWORD vid = 0;
    auto& quests = CQuestManager::instance();
    if (!lua_isnumber(state, 1)) {
        quests.QuestError("_find_npc_by_vnum", 600, "invalid argument");
    } else if (CHARACTER* character = quests.GetCurrentCharacterPtr()) {
        const auto* candidates = CHARACTER_MANAGER::instance().GetCharacterSetByRaceNum(ReadLuaDword(state));
        for (CHARACTER* candidate : *candidates) {
            if (candidate->GetMapIndex() == character->GetMapIndex()) {
                vid = static_cast<DWORD>(candidate->GetVID());
                break;
            }
        }
    }
    lua_pushnumber(state, vid);
    return 1;
}

int _get_quest_state(lua_State* state)
{
    if (!lua_isstring(state, 1))
        return 0;

    PC* player = CQuestManager::instance().GetCurrentPC();
    if (!player) {
        if (test_server)
            sys_log(0, "PC == 0 ");
        return 1;
    }

    const std::string name = std::string(lua_tostring(state, 1)) + ".__status";
    const int value = player->GetFlag(name);
    lua_pushnumber(state, value);
    if (test_server)
        sys_log(0, "Get_quest_state name %s value %d", name.c_str(), value);
    return 1;
}

int _do_command(lua_State* state)
{
    if (lua_isstring(state, 1))
        interpret_command(CQuestManager::instance().GetCurrentCharacterPtr(), lua_tostring(state, 1));
    return 0;
}

int _print(lua_State* state)
{
    if (lua_isstring(state, 1))
        sys_log(0, "%s", lua_tostring(state, 1));
    return 0;
}

int _under_han(lua_State* state)
{
    lua_pushboolean(state, lua_isstring(state, 1) && under_han(lua_tostring(state, 1)));
    return 1;
}

int _get_guildid_byname(lua_State* state)
{
    CGuild* guild = nullptr;
    if (lua_isstring(state, 1))
        guild = CGuildManager::instance().FindGuildByName(lua_tostring(state, 1));
    else
        CQuestManager::instance().QuestError("_get_guildid_byname", 0x1f8,
                                             "_get_guildid_byname() - invalud argument");
    lua_pushnumber(state, guild ? guild->GetID() : 0);
    return 1;
}

int _block_chat(lua_State* state)
{
    if (!lua_isstring(state, 1) || !lua_isnumber(state, 2)) {
        CQuestManager::instance().QuestError("_block_chat", 0x277, "invalid argument type");
        return 0;
    }

    const char* name = lua_tostring(state, 1);
    const int duration = LuaTruncateInt(lua_tonumber(state, 2));
    sys_log(0, "BLOCK CHAT %s %d", name, duration);
    CHARACTER* issuer = CQuestManager::instance().GetCurrentCharacterPtr();
    const char* response;
    if (CHARACTER* target = CHARACTER_MANAGER::instance().FindPC(name)) {
        if (issuer != target) {
            target->AddAffect(0xdf, 0, 0, 0, duration, 0, true, false);
            return 0;
        }
        response = "cannot block chatting myself";
    } else if (P2P_MANAGER::instance().Find(name)) {
        BYTE packet[30] = {0x16};
        std::strncpy(reinterpret_cast<char*>(packet + 1), name, 24);
        const auto wireDuration = static_cast<std::int32_t>(duration);
        std::memcpy(packet + 26, &wireDuration, sizeof(wireDuration));
        P2P_MANAGER::instance().Send(packet, sizeof(packet));
        response = "Block chat requested.";
    } else {
        response = "There is no character by that name";
    }
    if (issuer)
        issuer->ChatPacket(1, "%s", response);
    return 0;
}

int _add_goto_info(lua_State* state)
{
    const char* name = lua_tostring(state, 1);
    const auto empire = lua_tonumber(state, 2);
    const auto map = lua_tonumber(state, 3);
    const auto x = lua_tonumber(state, 4);
    const auto y = lua_tonumber(state, 5);
    if (name) {
        CHARACTER_AddGotoInfo(name, static_cast<BYTE>(LuaTruncateInt(empire)), LuaTruncateInt(map),
                              static_cast<std::uint32_t>(LuaTruncateInt(x)),
                              static_cast<std::uint32_t>(LuaTruncateInt(y)));
    }
    return 0;
}

int _find_pc_cond(lua_State* state)
{
    CHARACTER* found = nullptr;
    auto& quests = CQuestManager::instance();
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2) || !lua_isnumber(state, 3)) {
        quests.QuestError("_find_pc_cond", 0x223, "invalid argument");
    } else {
        if (CHARACTER* current = quests.GetCurrentCharacterPtr()) {
            const int minimum = LuaTruncateInt(lua_tonumber(state, 1));
            const int maximum = LuaTruncateInt(lua_tonumber(state, 2));
            const auto jobs = static_cast<unsigned>(ReadLuaDword(state, 3));
            const long map = current->GetMapIndex();
            if (test_server)
                sys_log(0, "find_pc_cond map=%ld, job=%u, level=%d~%d", map, jobs, minimum, maximum);
            found = CHARACTER_MANAGER::instance().FindSpecifyPC(jobs, map, current, minimum, maximum);
        }
    }
    const auto vid = found ? static_cast<std::int32_t>(static_cast<DWORD>(found->GetVID())) : 0;
    lua_pushnumber(state, vid);
    return 1;
}

int _item_name(lua_State* state)
{
    const TItemTable* item = lua_isnumber(state, 1)
        ? ITEM_MANAGER::instance().GetTable(ReadLuaDword(state)) : nullptr;
    lua_pushstring(state, item ? item->szLocaleName : "");
    return 1;
}

int _mob_name(lua_State* state)
{
    const CMob* mob = lua_isnumber(state, 1)
        ? CMobManager::instance().Get(ReadLuaDword(state)) : nullptr;
    lua_pushstring(state, mob ? mob->m_table.szLocaleName : "");
    return 1;
}

int _mob_vnum(lua_State* state)
{
    const CMob* mob = lua_isstring(state, 1)
        ? CMobManager::instance().Get(lua_tostring(state, 1), false) : nullptr;
    lua_pushnumber(state, mob ? mob->m_table.dwVnum : 0);
    return 1;
}

int _get_global_time(lua_State* state)
{
    lua_pushnumber(state, get_global_time());
    return 1;
}

static int SetServerTimer(lua_State* state, bool loop)
{
    auto& quests = CQuestManager::instance();
    const int count = lua_gettop(state);
    if ((count != 2 && count != 3) || !lua_isstring(state, 1) || !lua_isnumber(state, 2)
        || (count == 3 && !lua_isnumber(state, 3))) {
        quests.QuestError(loop ? "_set_server_loop_timer" : "_set_server_timer", loop ? 0x31e : 0x304,
                          "QUEST set_server_timer argument count wrong.");
        return 0;
    }

    const char* name = lua_tostring(state, 1);
    const lua_Number seconds = lua_tonumber(state, 2);
    const auto arg = count == 3 ? static_cast<unsigned>(ReadLuaDword(state, 3)) : 0;
    const auto npc = quests.LoadTimerScript(name);
    quests.AddServerTimer(name, arg, quest_create_server_timer_event(name, seconds, npc, loop, arg));
    return 0;
}

int _set_server_timer(lua_State* state) { return SetServerTimer(state, false); }
int _set_server_loop_timer(lua_State* state) { return SetServerTimer(state, true); }

int _clear_server_timer(lua_State* state)
{
    if (const char* name = lua_tostring(state, 1)) {
        CQuestManager::instance().ClearServerTimer(name,
            ReadLuaDword(state, 2));
    }
    return 0;
}

static int SetNamedTimer(lua_State* state, bool loop)
{
    auto& quests = CQuestManager::instance();
    if (lua_gettop(state) != 2 || !lua_isstring(state, 1) || !lua_isnumber(state, 2)) {
        quests.QuestError(loop ? "_set_named_loop_timer" : "_set_named_timer", loop ? 0x33e : 0x329,
                          "QUEST set_timer argument count wrong.");
        return 0;
    }

    CHARACTER* character = quests.GetCurrentCharacterPtr();
    PC* player = quests.GetCurrentPC();
    if (character && player) {
        const char* name = lua_tostring(state, -2);
        const lua_Number seconds = lua_tonumber(state, -1);
        const auto npc = quests.LoadTimerScript(name);
        player->AddTimer(name, quest_create_timer_event(name, character->GetPlayerID(), seconds, npc, loop));
    }
    return 0;
}

int _set_timer(lua_State* state)
{
    auto& quests = CQuestManager::instance();
    if (lua_gettop(state) != 1 || !lua_isnumber(state, 1)) {
        quests.QuestError("_set_timer", 0x2ec, "QUEST invalid argument.");
    } else if (CHARACTER* character = quests.GetCurrentCharacterPtr()) {
        quest_create_timer_event("", character->GetPlayerID(), lua_tonumber(state, 1), 0, false);
    }
    return 0;
}

int _set_named_timer(lua_State* state) { return SetNamedTimer(state, false); }

int _timer(lua_State* state)
{
    return lua_gettop(state) == 1 ? _set_timer(state) : _set_named_timer(state);
}

int _set_named_loop_timer(lua_State* state) { return SetNamedTimer(state, true); }

int _clear_named_timer(lua_State* state)
{
    auto& quests = CQuestManager::instance();
    if (lua_gettop(state) != 1 || !lua_isstring(state, 1))
        quests.QuestError("_clear_named_timer", 0x366, "QUEST set_timer argument count wrong.");
    else if (PC* player = quests.GetCurrentPC())
        player->RemoveTimer(lua_tostring(state, 1));
    return 0;
}

int _set_quest_state(lua_State* state)
{
    if (!lua_isstring(state, 1) || !lua_isstring(state, 2))
        return 0;
    auto& quests = CQuestManager::instance();
    QuestState* running = quests.GetCurrentState();
    if (!running || running->co != state) {
        sys_log(0, "running thread != current thread???");
        return luaL_error(state, "running thread != current thread???");
    }

    if (PC* player = quests.GetCurrentPC()) {
        const std::string name = lua_tostring(state, 1);
        const std::string next = lua_tostring(state, 2);
        if (test_server)
            sys_log(0, "set_state %s %s ", name.c_str(), next.c_str());
        if (name == player->GetCurrentQuestName()) {
            const char* stateName = lua_tostring(state, -1);
            if (!stateName)
                return 0;
            running->st = quests.GetQuestStateIndex(name, stateName);
            player->SetCurrentQuestStateName(stateName);
        } else {
            player->SetQuestState(name, next);
        }
    }
    return 0;
}

int _notice_all(lua_State* state)
{
    std::ostringstream stream;
    combine_lua_string(state, stream);
    const std::string text = stream.str();
    BYTE header[5] = {4};
    const auto length = static_cast<std::uint32_t>(text.size() + 1);
    std::memcpy(header + 1, &length, sizeof(length));
    TEMP_BUFFER buffer;
    buffer.write(header, sizeof(header));
    buffer.write(text.c_str(), length);
    P2P_MANAGER::instance().Send(buffer.read_peek(), buffer.size());
    SendNotice(text.c_str());
    if (!lua_gettop(state))
        lua_pushnil(state);
    return 1;
}

namespace {
template<class Func>
void ForEachQuestMapCharacter(long mapIndex, Func func)
{
    if (SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(mapIndex)) {
        for (const auto& entry : *map) {
            entry.second->ForEachEntity([&](LPENTITY entity) {
                if (entity->IsType(0))
                    func(static_cast<CHARACTER*>(entity));
            });
        }
    }
}


enum class WarpFilter { All, OtherEmpire, OwnEmpire };

struct warp_all_to_village_event_info {
    int fromMap;
};

struct warp_all_to_village_except_my_empire_event_info {
    int fromMap, empire;
};

struct warp_all_to_map_event_info {
    int fromMap, toMap, x, y;
};

struct warp_all_to_map_empire_event_info {
    int empire, fromMap, toMap, x, y;
};

void WarpQuestMapToVillage(int fromMap, WarpFilter filter, int ownEmpire)
{
    ForEachQuestMapCharacter(fromMap, [&](CHARACTER* character) {
        if (!character->GetDesc())
            return;
        const BYTE empire = character->GetEmpire();
        if (empire < 1 || empire > 3) {
            const bool all = filter == WarpFilter::All;
            CQuestManager::instance().QuestError("operator()", all ? 0x477 : 0x4d3,
                all ? "Unkonwn Empire %s %d " : "[WARP_TO_VILLAGE]Unkonwn Empire %s %d ",
                character->GetName(), static_cast<int>(character->GetMapIndex()));
        } else if (filter == WarpFilter::OtherEmpire && empire == ownEmpire) {
            if (test_server)
                sys_log(0, "[WARP_TO_VILLAGE]My Empire %s %d ",
                        character->GetName(), static_cast<int>(character->GetMapIndex()));
        } else {
            character->WarpSet(g_start_position[empire][0], g_start_position[empire][1], 0);
        }
    });
    if (test_server)
        sys_log(0, "Warp All to Village");
}

void WarpQuestMap(int fromMap, int toMap, int x, int y, WarpFilter filter, int ownEmpire)
{
    if (filter == WarpFilter::All)
        sys_log(0, "Warp_All_to_Map_1 x %d y %d mapindex %d ", x, y, toMap);
    else if (filter == WarpFilter::OtherEmpire)
        sys_log(0, "Warp_All_to_Map_1");

    ForEachQuestMapCharacter(fromMap, [&](CHARACTER* character) {
        if (!character->GetDesc())
            return;
        const BYTE empire = character->GetEmpire();
        if ((filter == WarpFilter::OtherEmpire && empire == ownEmpire)
            || (filter == WarpFilter::OwnEmpire && empire != ownEmpire))
            return;
        if (test_server && filter == WarpFilter::All)
            sys_log(0, "Warp_All_to_Map_1 x %d y %d mapindex %d ", x, y, toMap);
        character->WarpSet(x, y, toMap);
    });
    if (test_server && filter != WarpFilter::OwnEmpire)
        sys_log(0, "Warp_All_to_Map");
}

}

EVENTFUNC(warp_all_to_village_event)
{
    const auto& info = *static_cast<warp_all_to_village_event_info*>(event->info);
    WarpQuestMapToVillage(info.fromMap, WarpFilter::All, 0);
    return 0;
}

EVENTFUNC(warp_all_to_village_except_my_empire_event)
{
    const auto& info = *static_cast<warp_all_to_village_except_my_empire_event_info*>(event->info);
    WarpQuestMapToVillage(info.fromMap, WarpFilter::OtherEmpire, info.empire);
    return 0;
}

EVENTFUNC(warp_all_to_map_event)
{
    const auto& info = *static_cast<warp_all_to_map_event_info*>(event->info);
    WarpQuestMap(info.fromMap, info.toMap, info.x, info.y, WarpFilter::All, 0);
    return 0;
}

EVENTFUNC(warp_all_to_map_except_my_empire_event)
{
    const auto& info = *static_cast<warp_all_to_map_empire_event_info*>(event->info);
    WarpQuestMap(info.fromMap, info.toMap, info.x, info.y, WarpFilter::OtherEmpire, info.empire);
    return 0;
}

EVENTFUNC(warp_all_to_map_my_empire_event)
{
    const auto& info = *static_cast<warp_all_to_map_empire_event_info*>(event->info);
    WarpQuestMap(info.fromMap, info.toMap, info.x, info.y, WarpFilter::OwnEmpire, info.empire);
    return 0;
}

namespace {
template<class Info>
void QueueQuestWarp(long (*callback)(LPEVENT), const Info& values, int seconds, int line)
{
    Info* info;
    const DWORD id = __AllocEventInfo(sizeof(*info), reinterpret_cast<void**>(&info));
    *info = values;
    const std::int32_t delay = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(seconds) * static_cast<std::uint32_t>(passes_per_sec));
    event_create_ex(callback, id, delay, const_cast<char*>("questlua.cpp"), line);
}

int ScheduleQuestWarp(lua_State* state, bool village, WarpFilter filter, int line)
{
    const auto integer = [state](int index) { return LuaTruncateInt(lua_tonumber(state, index)); };
    if (test_server && village)
        sys_log(0, filter == WarpFilter::All ? "Warp_all_to_village" : "Warp_all_to_village_except_my_empire");
    const int map = integer(1);
    const int destination = village ? 0 : integer(2);
    const int x = village ? 0 : integer(3);
    const int y = village ? 0 : integer(4);
    const int seconds = integer(village ? 2 : 5);
    CHARACTER* character = filter == WarpFilter::All ? nullptr : CQuestManager::instance().GetCurrentCharacterPtr();
    if (filter == WarpFilter::All || character) {
        if (village) {
            if (filter == WarpFilter::All) {
                QueueQuestWarp(warp_all_to_village_event, warp_all_to_village_event_info{map}, seconds, line);
            } else {
                QueueQuestWarp(warp_all_to_village_except_my_empire_event,
                    warp_all_to_village_except_my_empire_event_info{map, character->GetEmpire()}, seconds, line);
            }
            const char* text = locale_find(filter == WarpFilter::All ? "\300\341\275\303\310\304\040\270\360\265\316\040\270\266\300\273\267\316\040\300\314\265\277\265\313\264\317\264\331\056" : "\300\341\275\303\310\304\040\307\330\264\347\040\270\266\300\273\300\272\040\040\270\266\300\273\267\316\040\300\314\265\277\265\313\264\317\264\331\056", g_iUseLocale);
            ForEachQuestMapCharacter(map, [text](CHARACTER* player) {
                if (player->GetDesc())
                    player->ChatPacket(2, "%s", text);
            });
        } else {
            const char* operation = "_warp_all_in_map";
            if (filter == WarpFilter::All) {
                QueueQuestWarp(warp_all_to_map_event,
                    warp_all_to_map_event_info{map, destination, x, y}, seconds, line);
            } else {
                const bool own = filter == WarpFilter::OwnEmpire;
                operation = own ? "_warp_all_in_map_t_my_empire" : "_warp_all_in_map_except_my_empire";
                QueueQuestWarp(own ? warp_all_to_map_my_empire_event : warp_all_to_map_except_my_empire_event,
                    warp_all_to_map_empire_event_info{character->GetEmpire(), map, destination, x, y}, seconds, line);
            }
            if (test_server)
                sys_log(0, "%s From %d To %d x %d y %d Sec %d ", operation, map, destination, x, y, seconds);
        }
    }
    if (!lua_gettop(state))
        lua_pushnil(state);
    return 1;
}

}

int _warp_all_to_village(lua_State* state)
{
    return ScheduleQuestWarp(state, true, WarpFilter::All, 0x4b6);
}

int _warp_all_to_village_except_my_empire(lua_State* state)
{
    return ScheduleQuestWarp(state, true, WarpFilter::OtherEmpire, 0x509);
}

int _warp_all_in_map(lua_State* state)
{
    return ScheduleQuestWarp(state, false, WarpFilter::All, 0x564);
}

int _warp_all_in_map_except_my_empire(lua_State* state)
{
    return ScheduleQuestWarp(state, false, WarpFilter::OtherEmpire, 0x62d);
}

int _warp_all_in_map_my_empire(lua_State* state)
{
    return ScheduleQuestWarp(state, false, WarpFilter::OwnEmpire, 0x68d);
}

int _warp_to_village(lua_State* state)
{
    if (test_server)
        sys_log(0, "Warp_all_to_village");
    if (CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr()) {
        const BYTE empire = character->GetEmpire();
        const long* position = g_start_position[empire < 4 ? empire : 0];
        character->WarpSet(position[0], position[1], 0);
    }
    if (!lua_gettop(state))
        lua_pushnil(state);
    return 1;
}

int _notice_in_map(lua_State* state)
{
    const int map = LuaTruncateInt(lua_tonumber(state, 1));
    const char* text = lua_tostring(state, 2);
    if (text) {
        ForEachQuestMapCharacter(map, [text](CHARACTER* character) {
            if (character->GetDesc())
                character->ChatPacket(2, "%s", text);
        });
    }
    return 0;
}

int _say_in_map(lua_State* state)
{
    const int map = LuaTruncateInt(lua_tonumber(state, 1));
    const char* text = lua_tostring(state, 2);
    if (!text)
        return 0;
    const int style = LuaTruncateInt(lua_tonumber(state, 3));
    const std::string script = std::string(text) + "[ENTER][DONE]";
    packet_script packet = {};
    if (script.size() > 0xffffu - sizeof(packet))
        return 0;
    packet.header = 45;
    packet.skin = style == 2 ? 5 : style != 0;
    packet.src_size = static_cast<WORD>(script.size());
    packet.size = packet.src_size + sizeof(packet);
    TEMP_BUFFER buffer;
    buffer.write(&packet, sizeof(packet));
    buffer.write(script.data(), packet.src_size);
    ForEachQuestMapCharacter(map, [&](CHARACTER* character) {
        if (DESC* desc = character->GetDesc())
            desc->Packet(buffer.read_peek(), buffer.size());
    });
    if (test_server)
        sys_log(0, "[SAY_IN_MAP] Script %s %d size %d window %d", script.c_str(),
                packet.src_size, buffer.size(), packet.skin);
    return 0;
}

int _regen_in_map(lua_State* state)
{
    const int mapIndex = LuaTruncateInt(lua_tonumber(state, 1));
    const char* path = lua_tostring(state, 2);
    if (!path)
        return 0;
    if (SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(mapIndex)) {
        sys_log(0, "[REGEN_LOAD_IN_MAP] Map %d File %s ", mapIndex, path);
        regen_load_in_file(path, mapIndex, map->m_setting.iBaseX, map->m_setting.iBaseY);
    } else {
        sys_log(0, "[REGEN_LOAD_IN_MAP]not open map %d ", mapIndex);
    }
    return 0;
}

int _kill_all_in_map(lua_State* state)
{
    const int map = LuaTruncateInt(lua_tonumber(state, 1));
    ForEachQuestMapCharacter(map, [](CHARACTER* character) {
        if (!character->GetDesc())
            character->Dead(nullptr, false);
    });
    return 0;
}

static unsigned CurrentQuestEmpire()
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    const BYTE empire = character ? character->GetEmpire() : 0;
    return empire < 4 ? empire : 0;
}

int _get_start_mapindex(lua_State* state)
{
    lua_pushnumber(state, g_start_map[CurrentQuestEmpire()]);
    return 1;
}

int _get_start_pos_X(lua_State* state)
{
    lua_pushnumber(state, g_start_position[CurrentQuestEmpire()][0]);
    return 1;
}

int _get_start_pos_Y(lua_State* state)
{
    lua_pushnumber(state, g_start_position[CurrentQuestEmpire()][1]);
    return 1;
}

void RegisterAffectFunctionTable();
namespace {
int PrivilegeLuaInt(lua_State* state, int argument)
{
    const auto value = std::trunc(lua_tonumber(state, argument));
    return value >= std::numeric_limits<int>::min() && value <= std::numeric_limits<int>::max()
        ? static_cast<int>(value) : std::numeric_limits<int>::min();
}

void AppendPrivilegeDescription(std::ostringstream& text, BYTE type, int value, time_t endTime)
{
    if (text.tellp() > std::streampos(0))
        text << ", ";
    const auto seconds = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(endTime) - static_cast<std::uint32_t>(get_global_time()));
    text << GetPrivName(type) << " : " << value << "% ("
         << static_cast<double>(seconds) / 3600.0 << " hours)\n";
}

int PushPrivilegeDescription(lua_State* state, const std::ostringstream& text)
{
    const std::string description = text.str();
    lua_pushstring(state, description.empty() ? "None!\n" : description.c_str());
    return 1;
}
}

int _give_empire_privilege(lua_State* state)
{
    const int empire = PrivilegeLuaInt(state, 1);
    const int type = PrivilegeLuaInt(state, 2);
    const int value = PrivilegeLuaInt(state, 3);
    const int duration = PrivilegeLuaInt(state, 4);
    sys_log(0, "_give_empire_privileage(empire=%d, type=%d, value=%d, time=%d)",
            empire, type, value, duration);
    CPrivManager::instance().RequestGiveEmpirePriv(static_cast<BYTE>(empire), static_cast<BYTE>(type),
                                                  value, duration);
    return 0;
}

int _give_guild_privilege(lua_State* state)
{
    const int guild = PrivilegeLuaInt(state, 1);
    const int type = PrivilegeLuaInt(state, 2);
    const int value = PrivilegeLuaInt(state, 3);
    const int duration = PrivilegeLuaInt(state, 4);
    sys_log(0, "_give_guild_privileage(empire=%d, type=%d, value=%d, time=%d)",
            guild, type, value, duration);
    CPrivManager::instance().RequestGiveGuildPriv(static_cast<std::uint32_t>(guild), static_cast<BYTE>(type),
                                                 value, duration);
    return 0;
}

int _give_char_privilege(lua_State* state)
{
    if (CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr())
        CPrivManager::instance().RequestGiveCharacterPriv(character->GetPlayerID(),
            static_cast<BYTE>(PrivilegeLuaInt(state, 1)), PrivilegeLuaInt(state, 2));
    return 0;
}

int _get_empire_privilege(lua_State* state)
{
    int value = 0;
    if (lua_isnumber(state, 1) && lua_isnumber(state, 2))
        value = CPrivManager::instance().GetPrivByEmpire(static_cast<BYTE>(PrivilegeLuaInt(state, 1)),
                                                        static_cast<BYTE>(PrivilegeLuaInt(state, 2)));
    lua_pushnumber(state, value);
    return 1;
}

int _get_guild_privilege(lua_State* state)
{
    int value = 0;
    if (lua_isnumber(state, 1) && lua_isnumber(state, 2))
        value = CPrivManager::instance().GetPrivByGuild(static_cast<std::uint32_t>(PrivilegeLuaInt(state, 1)),
                                                       static_cast<BYTE>(PrivilegeLuaInt(state, 2)));
    lua_pushnumber(state, value);
    return 1;
}

int _get_empire_privilege_string(lua_State* state)
{
    const BYTE empire = static_cast<BYTE>(PrivilegeLuaInt(state, 1));
    std::ostringstream text;
    auto& privileges = CPrivManager::instance();
    for (BYTE type = 1; type < 5; ++type) {
        const auto* entry = privileges.GetPrivByEmpireEx(empire, type);
        if (entry && entry->m_value)
            AppendPrivilegeDescription(text, type, entry->m_value, entry->m_end_time_sec);
    }
    return PushPrivilegeDescription(state, text);
}

int _get_guild_privilege_string(lua_State* state)
{
    const DWORD guild = static_cast<std::uint32_t>(PrivilegeLuaInt(state, 1));
    std::ostringstream text;
    auto& privileges = CPrivManager::instance();
    for (BYTE type = 1; type < 5; ++type) {
        const auto* entry = privileges.GetPrivByGuildEx(guild, type);
        if (entry && entry->value)
            AppendPrivilegeDescription(text, type, entry->value, entry->end_time_sec);
    }
    return PushPrivilegeDescription(state, text);
}

namespace {
BYTE QuestRefinementCell(lua_State* state)
{
    const auto value = std::trunc(lua_tonumber(state, -1));
    if (value < -32768.0 || value > 32767.0 || !std::isfinite(value))
        return 0;
    return static_cast<BYTE>(static_cast<short>(value));
}
}

int _refine_pick(lua_State* state)
{
    LPCHARACTER character = CQuestManager::instance().GetCurrentCharacterPtr();
    LPITEM pick = character ? character->GetItem(QuestRefinementCell(state)) : nullptr;
    lua_pushnumber(state, mining::RealRefinePick(character, pick));
    return 1;
}

int _fish_real_refine_rod(lua_State* state)
{
    LPCHARACTER character = CQuestManager::instance().GetCurrentCharacterPtr();
    LPITEM rod = character ? character->GetItem(QuestRefinementCell(state)) : nullptr;
    lua_pushnumber(state, fishing::RealRefineRod(character, rod));
    return 1;
}

namespace {
std::uint32_t MobSpawnDword(lua_Number number)
{
    if (number < -9223372036854775808.0 || number >= 9223372036854775808.0 || !std::isfinite(number))
        return 0;
    return static_cast<std::uint32_t>(static_cast<std::int64_t>(number));
}

int SpawnQuestMob(lua_State* state, bool group)
{
    const char* function = group ? "mob_spawn_group" : "mob_spawn";
    auto& quests = CQuestManager::instance();
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2) || !lua_isnumber(state, 3) ||
        !lua_isnumber(state, 4) || (group && !lua_isnumber(state, 6))) {
        quests.QuestError(function, group ? 0xea : 0xa7, "invalid argument");
        if (!group)
            return 0;
        lua_pushnumber(state, 0);
        return 1;
    }

    const DWORD vnum = MobSpawnDword(lua_tonumber(state, 1));
    const int localX = LuaTruncateInt(lua_tonumber(state, 2));
    const int localY = LuaTruncateInt(lua_tonumber(state, 3));
    const float radius = static_cast<float>(lua_tonumber(state, 4)) * 100.0f;
    const bool aggressive = lua_toboolean(state, 5) != 0;
    std::uint32_t count = lua_isnumber(state, 6) ? MobSpawnDword(lua_tonumber(state, 6)) : 1;
    if (!count)
        count = 1;
    else if (count > 10) {
        quests.QuestError(function, group ? 0xfa : 0xb6, "count bigger than 10");
        count = 10;
    }

    CHARACTER* character = quests.GetCurrentCharacterPtr();
    PC* pc = quests.GetCurrentPC();
    const long mapIndex = character ? character->GetMapIndex() : 0;
    SECTREE_MAP* map = character ? SECTREE_MANAGER::instance().GetMap(mapIndex) : nullptr;
    bool spawned = false;
    if (map && pc && std::isfinite(radius)) {
        const DWORD questIndex = static_cast<DWORD>(pc->GetCurrentQuestIndex());
        const std::int64_t baseX = static_cast<std::int64_t>(localX) * 100 + map->m_setting.iBaseX;
        const std::int64_t baseY = static_cast<std::int64_t>(localY) * 100 + map->m_setting.iBaseY;
        auto& characters = CHARACTER_MANAGER::instance();
        for (std::uint32_t i = 0; i < count; ++i) {
            for (int attempt = 0; attempt < 8; ++attempt) {
                constexpr float pi = 3.1415927410125732421875f;
                const int angleRoll = number_ex(0, 999, const_cast<char*>("questlua.cpp"), group ? 0x109 : 0xc5);
                const float angle = static_cast<float>(angleRoll * 2.0L * pi / 1000.0L);
                const int distanceRoll = number_ex(0, 999, const_cast<char*>("questlua.cpp"), group ? 0x10a : 0xc6);
                const float distance = static_cast<float>(distanceRoll * static_cast<long double>(radius) / 1000.0L);
                const std::int64_t x = baseX + LuaTruncateInt(static_cast<double>(std::cos(angle)) * distance);
                const std::int64_t y = baseY + LuaTruncateInt(static_cast<double>(std::sin(angle)) * distance);
                if (x < INT_MIN || x > INT_MAX || y < INT_MIN || y > INT_MAX)
                    continue;

                CHARACTER* mob;
                if (group)
                    mob = characters.SpawnGroup(vnum, mapIndex, x, y, x, y, nullptr, aggressive, nullptr);
                else
                    mob = characters.SpawnMob(vnum, mapIndex, x, y, 0, false, -1, true);
                if (!mob)
                    continue;
                if (!group && aggressive)
                    mob->SetAggressive();
                mob->SetQuestByVnum(questIndex);
                if (!spawned) {
                    spawned = true;
                    lua_pushnumber(state, static_cast<DWORD>(mob->GetVID()));
                }
                break;
            }
        }
    }
    if (!spawned)
        lua_pushnumber(state, 0);
    return 1;
}
}

// 08053330
int mob_spawn(lua_State* state)
{
    return SpawnQuestMob(state, false);
}

// 080538e0
int mob_spawn_group(lua_State* state)
{
    return SpawnQuestMob(state, true);
}

// 08052ea0
int member_chat(lua_State* state)
{
    CHARACTER* member = CQuestManager::instance().GetCurrentPartyMember();
    if (member) {
        std::ostringstream message;
        combine_lua_string(state, message);
        member->ChatPacket(0, "%s", message.str().c_str());
    }
    return 0;
}

// 080532a0
int member_set_ready(lua_State*)
{
    if (CHARACTER* member = CQuestManager::instance().GetCurrentPartyMember())
        member->AddAffect(0xd4, POINT_NONE, 0, 7, 0xffff, 0, true, false);
    return 0;
}

// 08053240
int member_clear_ready(lua_State*)
{
    if (CHARACTER* member = CQuestManager::instance().GetCurrentPartyMember())
        member->RemoveAffect(static_cast<DWORD>(0xd4));
    return 0;
}

// 08052d20
int highscore_register(lua_State* state)
{
    const char* board = lua_tostring(state, 1);
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    if (!board || !character)
        return 0;

    auto* info = new THighscoreRegisterQueryInfo{};
    std::strncpy(info->board, board, 20);
    info->board[20] = '\0';
    info->pid = character->GetPlayerID();
    info->value = LuaTruncateInt(lua_tonumber(state, 2));
    info->descending = LuaTruncateInt(lua_tonumber(state, 3)) != 0;

    char escaped[41]{};
    auto& database = DBManager::instance();
    if (database.EscapeString(escaped, info->board, std::strlen(info->board)) >= sizeof(escaped)) {
        delete info;
        return 0;
    }
    database.ReturnQuery(4, info->pid, info,
                         "SELECT value FROM highscore%s WHERE board='%s' AND pid=%lu",
                         get_table_postfix(), escaped, static_cast<unsigned long>(info->pid));
    return 1;
}

// 08052be0
int highscore_show(lua_State* state)
{
    const char* board = lua_tostring(state, 1);
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    if (!board || !character)
        return 0;

    const std::size_t length = std::strlen(board);
    if (length > 1000)
        return 0;
    std::string escaped(length * 2 + 1, '\0');
    auto& database = DBManager::instance();
    if (database.EscapeString(&escaped[0], board, length) >= escaped.size())
        return 0;

    const char* postfix = get_table_postfix();
    const char* order = LuaTruncateInt(lua_tonumber(state, 2)) != 0 ? "DESC" : "";
    database.ReturnQuery(5, character->GetPlayerID(), nullptr,
                         "SELECT h.pid, p.name, h.value FROM highscore%s as h, player%s as p "
                         "WHERE h.board = '%s' AND h.pid = p.id ORDER BY h.value %s LIMIT 10",
                         postfix, postfix, escaped.c_str(), order);
    return 0;
}

void RegisterBuildingFunctionTable();
void RegisterDungeonFunctionTable();
void RegisterGameFunctionTable();
void RegisterGuildFunctionTable();
void RegisterHorseFunctionTable();
void RegisterITEMFunctionTable();
void RegisterMarriageFunctionTable();
void RegisterNPCFunctionTable();
int quest_setstate(lua_State* state);
void RegisterPartyFunctionTable();
void RegisterPCFunctionTable();
void RegisterQuestFunctionTable();
void RegisterTargetFunctionTable();
void RegisterArenaFunctionTable();
void RegisterForkedFunctionTable();
void RegisterOXEventFunctionTable();
void RegisterGlobalFunctionTable(lua_State* state);

bool CQuestManager::InitializeLua()
{
    L = lua_open();
    if (!L)
        return false;
    luaopen_base(L);
    luaopen_table(L);
    luaopen_string(L);
    luaopen_math(L);
    luaopen_io(L);
    luaopen_debug(L);

    RegisterAffectFunctionTable();
    RegisterBuildingFunctionTable();
    RegisterDungeonFunctionTable();
    RegisterGameFunctionTable();
    RegisterGuildFunctionTable();
    RegisterHorseFunctionTable();
    RegisterITEMFunctionTable();
    RegisterMarriageFunctionTable();
    RegisterNPCFunctionTable();
    RegisterPartyFunctionTable();
    RegisterPCFunctionTable();
    RegisterQuestFunctionTable();
    RegisterTargetFunctionTable();
    RegisterArenaFunctionTable();
    RegisterForkedFunctionTable();
    RegisterOXEventFunctionTable();
    luaL_reg memberFunctions[] = {
        {"chat", member_chat},
        {"set_ready", member_set_ready},
        {"clear_ready", member_clear_ready},
        {nullptr, nullptr}
    };
    AddLuaFunctionTable("member", memberFunctions);
    luaL_reg highscoreFunctions[] = {
        {"register", highscore_register},
        {"show", highscore_show},
        {nullptr, nullptr}
    };
    AddLuaFunctionTable("highscore", highscoreFunctions);
    luaL_reg mobFunctions[] = {
        {"spawn", mob_spawn},
        {"spawn_group", mob_spawn_group},
        {nullptr, nullptr}
    };
    AddLuaFunctionTable("mob", mobFunctions);
    RegisterGlobalFunctionTable(L);
    const luaL_reg globals[] = {
        {"set_state", quest_setstate},
        {"setskin", _set_skin},
        {"set_skin", _set_skin},
        {"time_to_str", _time_to_str},
        {"say", _say},
        {"chat", _chat},
        {"cmdchat", _cmdchat},
        {"syschat", _syschat},
        {"notice", _notice},
        {"get_locale", _get_locale},
        {"setleftimage", _left_image},
        {"settopimage", _top_image},
        {"server_timer", _set_server_timer},
        {"clear_server_timer", _clear_server_timer},
        {"server_loop_timer", _set_server_loop_timer},
        {"get_server_timer_arg", _get_server_timer_arg},
        {"timer", _timer},
        {"loop_timer", _set_named_loop_timer},
        {"cleartimer", _clear_named_timer},
        {"getnpcid", _getnpcid},
        {"is_test_server", _is_test_server},
        {"raw_script", _raw_script},
        {"number", _number},
        {"set_bgm_volume_enable", _set_bgm_volume_enable},
        {"add_bgm_info", _add_bgm_info},
        {"add_goto_info", _add_goto_info},
        {"__refine_pick", _refine_pick},
        {"add_ox_quiz", _add_ox_quiz},
        {"__fish_real_refine_rod", _fish_real_refine_rod},
        {"__give_char_priv", _give_char_privilege},
        {"__give_empire_priv", _give_empire_privilege},
        {"__give_guild_priv", _give_guild_privilege},
        {"__get_empire_priv_string", _get_empire_privilege_string},
        {"__get_empire_priv", _get_empire_privilege},
        {"__get_guild_priv_string", _get_guild_privilege_string},
        {"__get_guildid_byname", _get_guildid_byname},
        {"__get_guild_priv", _get_guild_privilege},
        {"__block_chat", _block_chat},
        {"item_name", _item_name},
        {"mob_name", _mob_name},
        {"mob_vnum", _mob_vnum},
        {"get_time", _get_global_time},
        {"get_global_time", _get_global_time},
        {"print", _print},
        {"command", _do_command},
        {"find_pc", _find_pc},
        {"find_pc_cond", _find_pc_cond},
        {"find_pc_by_name", _find_pc},
        {"pc_name", _pc_name},
        {"find_npc_by_vnum", _find_npc_by_vnum},
        {"set_quest_state", _set_quest_state},
        {"get_quest_state", _get_quest_state},
        {"under_han", _under_han},
        {"notice_all", _notice_all},
        {"warp_all_to_village", _warp_all_to_village},
        {"warp_all_to_village_except_my_empire", _warp_all_to_village_except_my_empire},
        {"warp_to_village", _warp_to_village},
        {"say_in_map", _say_in_map},
        {"warp_all_in_map", _warp_all_in_map},
        {"notice_in_map", _notice_in_map},
        {"kill_all_in_map", _kill_all_in_map},
        {"warp_all_except_my_empire_in_map", _warp_all_in_map_except_my_empire},
        {"warp_all_my_empire_in_map", _warp_all_in_map_my_empire},
        {"regen_in_map", _regen_in_map},
        {"get_start_mapindex", _get_start_mapindex},
        {"get_start_pos_X", _get_start_pos_X},
        {"get_start_pos_Y", _get_start_pos_Y},
        {"is_allow_map", _is_allow_map}
    };
    for (const luaL_reg& entry : globals) {
        lua_pushcfunction(L, entry.func);
        lua_setglobal(L, entry.name);
    }
    return LoadQuestLuaFiles();
}

bool CQuestManager::RunState(QuestState& qs) {
    m_bError=false;
    m_CurrentRunningState=&qs;
    int result=lua_resume(qs.co,qs.args);
    if (result==0) {
        if (lua_gettop(qs.co)==0) { GotoEndState(qs); return false; }
        if (!strcmp(lua_tostring(qs.co,1),"select")) { GotoSelectState(qs);return true; }
        if (!strcmp(lua_tostring(qs.co,1),"wait")) { GotoPauseState(qs);return true; }
        if (!strcmp(lua_tostring(qs.co,1),"input")) { GotoInputState(qs);return true; }
        if (!strcmp(lua_tostring(qs.co,1),"confirm")) { GotoConfirmState(qs);return true; }
        if (!strcmp(lua_tostring(qs.co,1),"select_item")) { GotoSelectItemState(qs);return true; }
    } else instance().QuestError("RunState",0x896,"LUA_ERROR: %s",lua_tostring(qs.co,1));
    WriteRunningStateToSyserr();
    m_bError=true;
    GotoEndState(qs);
    return false;
}
}

namespace quest {
struct confirm_timeout_event_info {
    DWORD dwWaitPID;
    DWORD dwReplyPID;
};

// 0805c650
long confirm_timeout_event(LPEVENT event)
{
    confirm_timeout_event_info* info = static_cast<confirm_timeout_event_info*>(event->info);
    if (CHARACTER_MANAGER::instance().FindByPID(info->dwWaitPID))
        CQuestManager::instance().Confirm(info->dwWaitPID, CONFIRM_TIMEOUT, 0);
    return 0;
}

// 0805c720. The VID uses x87 truncation to 64 bits before the low word is read.
void CQuestManager::GotoConfirmState(QuestState& qs)
{
    qs.suspend_state = 4;
    const lua_Number target = lua_tonumber(qs.co, -3);
    DWORD vid = 0;
    if (target >= -9223372036854775808.0 && target < 9223372036854775808.0)
        vid = static_cast<unsigned int>(static_cast<long long>(target));
    else
        feraiseexcept(FE_INVALID);
    const char* message = lua_tostring(qs.co, -2);
    const int timeout = LuaTruncateInt(lua_tonumber(qs.co, -1));
    sys_log(0, "GotoConfirmState vid %lu msg '%s', timeout %d", vid, message, timeout);
    CHARACTER* targetCharacter = CHARACTER_MANAGER::instance().Find(vid);
    if (targetCharacter && targetCharacter->GetDesc())
        targetCharacter->ConfirmWithMsg(message, timeout, GetCurrentCharacterPtr()->GetPlayerID());
    GetCurrentPC()->SetConfirmWait(targetCharacter && targetCharacter->GetDesc() ? targetCharacter->GetPlayerID() : 0);
    std::ostringstream script;
    script << "[CONFIRM_WAIT timeout;" << timeout << "]";
    AddScript(script.str());
    SendScript();
    void* rawInfo = nullptr;
    const DWORD eventId = __AllocEventInfo(sizeof(confirm_timeout_event_info), &rawInfo);
    auto* info = static_cast<confirm_timeout_event_info*>(rawInfo);
    info->dwWaitPID = GetCurrentCharacterPtr()->GetPlayerID();
    info->dwReplyPID = targetCharacter && targetCharacter->GetDesc() ? targetCharacter->GetPlayerID() : 0;
    const unsigned int rawDelay = static_cast<unsigned int>(timeout) * static_cast<unsigned int>(passes_per_sec);
    int delay;
    std::memcpy(&delay, &rawDelay, sizeof(delay));
    event_create_ex(confirm_timeout_event, eventId, delay, const_cast<char*>("questlua.cpp"), 0x830);
}
}

namespace quest {
// 080527f0
std::string ScriptToString(const std::string& script)
{
    CQuestManager& manager = CQuestManager::instance();
    lua_State* state = manager.L;
    const int top = lua_gettop(state);
    const int error = lua_dobuffer(state, ("return " + script).c_str(),
                                   script.size() + 7, "ScriptToString");
    std::string result;
    if (!error) {
        if (lua_isstring(state, -1))
            result = lua_tostring(state, -1);
    } else {
        CQuestManager::instance().QuestError("ScriptToString", 0x40,
            "LUA ScriptRunError (code:%d src:[%s])", error, script.c_str());
    }
    lua_settop(state, top);
    return result;
}
}

namespace quest {
bool IsScriptTrue(const char* code, int size)
{
    if (!size) return true;
    CQuestManager& manager=CQuestManager::instance();
    lua_State* state=manager.L;
    int top=lua_gettop(state);
    int error=lua_dobuffer(state,code,size,"IsScriptTrue");
    int result=lua_toboolean(state,-1);
    if (error) {
        char format[100];
        snprintf(format,sizeof(format),"LUA ScriptRunError (code:%%d src:[%%%ds])",size);
        manager.QuestError("IsScriptTrue",0x53,format,error,code);
    }
    lua_settop(state,top);
    return result!=0;
}
}
