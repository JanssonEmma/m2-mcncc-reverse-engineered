// Exercise the original directory traversal through the real state-script loader.
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
#include <climits>
#include <cstdio>
#include <cstdlib>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

int max_bytes_written, current_bytes_written, total_bytes_written;
void ShutdownOnFatalError() { std::abort(); }
extern "C" void desc_test_allow_clock(void);
extern "C" int npc_set_loaded_count(void);
extern "C" void npc_set_expect_tree(const char* name);

static void MakeDirectory(const std::string& path) {
#ifdef _WIN32
    assert(_mkdir(path.c_str()) == 0);
#else
    assert(mkdir(path.c_str(), 0700) == 0);
#endif
}

static void RemoveDirectory(const std::string& path) {
#ifdef _WIN32
    assert(_rmdir(path.c_str()) == 0);
#else
    assert(rmdir(path.c_str()) == 0);
#endif
}

static void WriteFile(const std::string& path, char byte) {
    std::FILE* file = std::fopen(path.c_str(), "wb");
    assert(file);
    assert(std::fputc(byte, file) == byte);
    assert(std::fclose(file) == 0);
}

static const char* roots[] = {"npc_set_fixture_a", "npc_set_fixture_b"};
static const char* events[] = {"alpha", "beta"};

static void WriteTree(const std::string& name) {
    for (int root = 0; root < 2; ++root) {
        const std::string base = roots[root];
        if (name == "script") MakeDirectory(base);
        MakeDirectory(base + "/" + name);
        for (int event = 0; event < 2; ++event) {
            const std::string folder = base + "/" + name + "/" + events[event];
            MakeDirectory(folder);
            WriteFile(folder + "/q.start", static_cast<char>('A' + event * 2 + root));
            WriteFile(folder + "/.ignored", 'X');
            WriteFile(folder + "/cVs-extra", 'Y');
        }
    }
}

static void RemoveTree(const std::string& name) {
    for (int root = 0; root < 2; ++root) {
        const std::string base = roots[root];
        for (int event = 0; event < 2; ++event) {
            const std::string folder = base + "/" + name + "/" + events[event];
            assert(std::remove((folder + "/q.start").c_str()) == 0);
            assert(std::remove((folder + "/.ignored").c_str()) == 0);
            assert(std::remove((folder + "/cVs-extra").c_str()) == 0);
            RemoveDirectory(folder);
        }
        RemoveDirectory(base + "/" + name);
        if (name == "script") RemoveDirectory(base);
    }
}

int main() {
    desc_test_allow_clock();
    test_server = 0;
    quest::CQuestManager manager;
    quest::NPC npc;
    manager.m_hmQuestName["q"] = 42;
    manager.m_mapEventName["beta"] = 5;
    manager.m_mapEventName["alpha"] = 5;
    manager.m_mapEventName["missing"] = 6;
    lua_State* vm = lua_open();
    assert(vm);
    manager.L = vm;
    lua_newtable(vm);
    lua_pushstring(vm, "start");
    lua_pushnumber(vm, 8);
    lua_settable(vm, -3);
    lua_setglobal(vm, "q");

    WriteTree("script");
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert(roots[1]);
    g_setQuestObjectDir.insert(roots[0]);
    g_setQuestObjectDir.insert("npc_set_fixture_missing");
    npc.Set(314, "script");
    assert(npc.m_vnum == 314 && npc_set_loaded_count() == 4);
    assert(npc.m_mapOwnQuest[6].empty());
    const std::vector<char>& code = npc.m_mapOwnQuest[5][42][8].m_code;
    assert(std::string(code.begin(), code.end()) == "ABCD");

    npc_set_expect_tree("script");
    manager.m_mapNPC[UINT_MAX].m_vnum = 1;
    assert(manager.LoadTimerScript("script") == UINT_MAX);
    assert(manager.m_mapNPC[UINT_MAX].m_vnum == UINT_MAX);
    assert(manager.LoadTimerScript("script") == UINT_MAX && npc_set_loaded_count() == 4);
    assert(manager.LoadTimerScript("missing") == UINT_MAX - 1);
    assert(manager.m_mapTimerID.size() == 2);

    manager.RegisterNPCVnum(315);
    assert(manager.m_registeredNPCVnum.count(315) == 1 && manager.m_mapNPC.count(315) == 0);
    MakeDirectory(std::string(roots[0]) + "/315");
    manager.RegisterNPCVnum(315);
    assert(manager.m_mapNPC.count(315) == 0);
    RemoveDirectory(std::string(roots[0]) + "/315");

    WriteTree("314");
    npc_set_expect_tree("314");
    manager.RegisterNPCVnum(314);
    assert(manager.m_registeredNPCVnum.count(314) == 1 && npc_set_loaded_count() == 8);
    const std::vector<char>& registered = manager.m_mapNPC[314].m_mapOwnQuest[5][42][8].m_code;
    assert(std::string(registered.begin(), registered.end()) == "ABCDABCD");
    assert(manager.m_mapNPCNameID.empty());
    manager.RegisterNPCVnum(314);
    assert(npc_set_loaded_count() == 8);
    RemoveTree("314");
    RemoveTree("script");
    g_setQuestObjectDir.clear();
    assert(lua_gettop(vm) == 0);
    std::puts("NPC directory traversal passed");
}
