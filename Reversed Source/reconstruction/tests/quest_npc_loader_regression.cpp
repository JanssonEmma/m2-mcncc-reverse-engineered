// Exercise the whole loader with real Lua tables and real files.
#include <map>
#include <set>
#include <list>
#include <queue>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../game/src/config.h"
extern "C" {
#include "../../liblua/include/lua.h"
}
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int max_bytes_written, current_bytes_written, total_bytes_written;
void ShutdownOnFatalError() { std::abort(); }
extern "C" void desc_test_allow_clock(void);
extern "C" int loader_loading_logs(void);
extern "C" int loader_index_errors(void);
extern "C" int loader_name_errors(void);

static quest::NPC* activeNpc;
static int lookupCount;
static const unsigned int questIndex = 42;
static const char filename[] = "quest_npc_loader_fixture.tmp";

static int Lookup(lua_State* vm) {
    assert(std::strcmp(lua_tostring(vm, 2), "start") == 0);
    ++lookupCount;
    if (lookupCount == 1) {
        assert(activeNpc->m_mapOwnQuest[19].empty() && loader_loading_logs() == 0);
        lua_pushnumber(vm, 3);
    } else {
        assert(lookupCount == 2 && loader_loading_logs() == 1);
        assert(activeNpc->m_mapOwnQuest[19].count(questIndex) == 1);
        lua_pushnumber(vm, 8);
    }
    return 1;
}

static void WriteFixture(const char* data, std::size_t length) {
    std::FILE* file = std::fopen(filename, "wb");
    assert(file);
    assert(std::fwrite(data, 1, length, file) == length);
    assert(std::fclose(file) == 0);
}

static std::string Bytes(const std::vector<char>& data) {
    return std::string(data.begin(), data.end());
}

int main() {
    desc_test_allow_clock();
    test_server = 0;
    quest::CQuestManager manager;
    quest::NPC npc;
    activeNpc = &npc;
    manager.m_hmQuestName["q"] = questIndex;
    lua_State* vm = lua_open();
    assert(vm);
    manager.L = vm;

    lua_newtable(vm);
    lua_newtable(vm);
    lua_pushstring(vm, "__index");
    lua_pushcfunction(vm, Lookup);
    lua_settable(vm, -3);
    lua_setmetatable(vm, -2);
    lua_setglobal(vm, "q");

    const char payload[] = {'a', 0, 'b', '\n', 'c'};
    const std::string expected(payload, sizeof(payload));
    WriteFixture(payload, sizeof(payload));
    npc.LoadStateScript(19, filename, "q.start");
    assert(lookupCount == 2 && lua_gettop(vm) == 0);
    quest::NPC::AQuestScriptType& plain = npc.m_mapOwnQuest[19][questIndex];
    assert(plain.size() == 1 && plain.count(8) == 1);
    assert(Bytes(plain.find(8)->second.m_code) == expected);

    lua_newtable(vm);
    lua_pushstring(vm, "start");
    lua_pushnumber(vm, 8);
    lua_settable(vm, -3);
    lua_setglobal(vm, "q");
    npc.LoadStateScript(19, filename, "q.start");
    assert(Bytes(plain.find(8)->second.m_code) == expected + expected);

    // Seed both scalars before copying. No loader call below grows the vector.
    quest::AArgScript seed;
    seed.quest_index = 100;
    seed.state_index = 200;
    seed.arg = "old";
    std::vector<quest::AArgScript>& arguments = npc.m_mapOwnArgQuest[0][questIndex][8];
    arguments.push_back(seed);
    npc.LoadStateScript(0, filename, "q.start.0.when");
    npc.LoadStateScript(0, filename, "q.start.0.when");
    assert(Bytes(arguments[0].when_condition) == expected + expected);
    npc.LoadStateScript(0, filename, "q.start.0.arg");
    assert(arguments[0].arg == std::string(payload, 3));
    assert(arguments[0].quest_index == 100 && arguments[0].state_index == 200);
    npc.LoadStateScript(0, filename, "q.start.0.unknown");
    npc.LoadStateScript(0, filename, "q.start.0");
    npc.LoadStateScript(0, filename, "q.start.0.script.extra");
    assert(loader_index_errors() == 1 && loader_name_errors() == 1);
    assert(arguments.size() == 1 && arguments[0].script.m_code.empty());
    npc.LoadStateScript(2, filename, "q.start.-1.unknown");
    assert(npc.m_mapOwnArgQuest[2][questIndex].count(8) == 1);
    assert(npc.m_mapOwnArgQuest[2][questIndex][8].empty());
    assert(arguments[0].quest_index == 100 && arguments[0].state_index == 200);
    npc.LoadStateScript(0, filename, "q.start.0.script");
    assert(Bytes(arguments[0].script.m_code) == expected);
    assert(arguments[0].quest_index == questIndex && arguments[0].state_index == 8);
    assert(lua_gettop(vm) == 0);
    assert(std::remove(filename) == 0);
    npc.LoadStateScript(1, filename, "q.start");
    assert(npc.m_mapOwnQuest[1][questIndex].count(8) == 1);
    assert(npc.m_mapOwnQuest[1][questIndex][8].m_code.empty());
    std::puts("NPC state-script loading passed");
}
