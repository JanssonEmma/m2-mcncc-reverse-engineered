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

    const char* roots[] = {"npc_set_fixture_a", "npc_set_fixture_b"};
    const char* events[] = {"alpha", "beta"};
    for (int root = 0; root < 2; ++root) {
        const std::string base = roots[root];
        MakeDirectory(base);
        MakeDirectory(base + "/script");
        for (int event = 0; event < 2; ++event) {
            const std::string folder = base + "/script/" + events[event];
            MakeDirectory(folder);
            WriteFile(folder + "/q.start", static_cast<char>('A' + event * 2 + root));
            WriteFile(folder + "/.ignored", 'X');
            WriteFile(folder + "/cVs-extra", 'Y');
        }
    }
    g_setQuestObjectDir.clear();
    g_setQuestObjectDir.insert(roots[1]);
    g_setQuestObjectDir.insert(roots[0]);
    g_setQuestObjectDir.insert("npc_set_fixture_missing");
    npc.Set(314, "script");
    assert(npc.m_vnum == 314 && npc_set_loaded_count() == 4);
    assert(npc.m_mapOwnQuest[6].empty());
    const std::vector<char>& code = npc.m_mapOwnQuest[5][42][8].m_code;
    assert(std::string(code.begin(), code.end()) == "ABCD");

    for (int root = 0; root < 2; ++root) {
        const std::string base = roots[root];
        for (int event = 0; event < 2; ++event) {
            const std::string folder = base + "/script/" + events[event];
            assert(std::remove((folder + "/q.start").c_str()) == 0);
            assert(std::remove((folder + "/.ignored").c_str()) == 0);
            assert(std::remove((folder + "/cVs-extra").c_str()) == 0);
            RemoveDirectory(folder);
        }
        RemoveDirectory(base + "/script");
        RemoveDirectory(base);
    }
    g_setQuestObjectDir.clear();
    assert(lua_gettop(vm) == 0);
    std::puts("NPC directory traversal passed");
}
