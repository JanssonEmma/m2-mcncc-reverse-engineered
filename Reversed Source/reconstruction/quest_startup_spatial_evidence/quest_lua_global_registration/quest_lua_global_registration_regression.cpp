#include <map>
#include <set>
#include <list>
#include <queue>
#include <stack>
#include <vector>
#include <string>
#include <limits>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/questmanager.h"
#include "../../game/src/char.h"
#include "../../game/src/log.h"
#undef protected
#undef private
#include "../../game/src/config.h"
#include "desc_fixture.h"
#include "sql_connection_fixture.h"
extern "C" {
#include "../../liblua/include/lua.h"
}
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace quest { void RegisterGlobalFunctionTable(lua_State*); }
int max_bytes_written, current_bytes_written, total_bytes_written;
void ShutdownOnFatalError() { std::abort(); }
extern "C" void desc_test_allow_clock(void);

static std::vector<std::string> globalWrites;
static std::vector<std::string> information;
static std::vector<std::string> diagnostics;

extern "C" void quest_global_lua_log(const char* quest, const char* player, const char* message)
{
    assert(!std::strcmp(quest, "log_quest") && !std::strcmp(player, "log_player"));
    information.push_back(message);
}

extern "C" void quest_global_lua_error(const char* message)
{
    diagnostics.push_back(message);
}

static int StoreGlobal(lua_State* state)
{
    assert(lua_gettop(state) == 3 && lua_istable(state, 1));
    assert(lua_type(state, 2) == LUA_TSTRING && lua_iscfunction(state, 3));
    globalWrites.push_back(lua_tostring(state, 2));
    lua_pushvalue(state, 2);
    lua_pushvalue(state, 3);
    lua_rawset(state, 1);
    return 0;
}

static void CheckPrefix(lua_State* state)
{
    assert(lua_gettop(state) == 1 && lua_type(state, 1) == LUA_TSTRING);
    assert(!std::strcmp(lua_tostring(state, 1), "stack-prefix"));
}

static void Call(lua_State* state, int argumentCount)
{
    // LUA_MULTRET verifies the callback returns no Lua values.
    assert(lua_pcall(state, argumentCount, LUA_MULTRET, 0) == 0);
    CheckPrefix(state);
}

static void Register(lua_State* state)
{
    lua_pushstring(state, "stack-prefix");
    lua_pushnumber(state, 73);
    lua_setglobal(state, "unrelated");
    lua_pushvalue(state, LUA_GLOBALSINDEX);
    lua_newtable(state);
    lua_pushstring(state, "__newindex");
    lua_pushcfunction(state, StoreGlobal);
    lua_rawset(state, -3);
    lua_setmetatable(state, -2);
    lua_pop(state, 1);

    quest::RegisterGlobalFunctionTable(state);
    assert(globalWrites.size() == 3 && globalWrites[0] == "sys_err" &&
           globalWrites[1] == "sys_log" && globalWrites[2] == "char_log");
    CheckPrefix(state);
    lua_getglobal(state, "unrelated");
    assert(lua_tonumber(state, -1) == 73);
    lua_pop(state, 1);
    lua_pushvalue(state, LUA_GLOBALSINDEX);
    lua_pushnil(state);
    lua_setmetatable(state, -2);
    lua_pop(state, 1);
}

static void Packet(LPBUFFER output, BYTE type, const char* text)
{
    const BYTE* bytes = static_cast<const BYTE*>(buffer_read_peek(output));
    const unsigned int expected = static_cast<unsigned int>(std::strlen(text)) + 10;
    assert(buffer_size(output) >= static_cast<int>(expected));
    assert(bytes[0] == 4 && bytes[3] == type);
    assert((static_cast<unsigned int>(bytes[1]) | (static_cast<unsigned int>(bytes[2]) << 8)) == expected);
    assert(!std::memcmp(bytes + 9, text, expected - 9));
    buffer_read_proceed(output, expected);
}

static void InfoCall(lua_State* state, double level, const char* text)
{
    lua_getglobal(state, "sys_log");
    lua_pushnumber(state, level);
    lua_pushstring(state, text);
    Call(state, 2);
}

static void ErrorCall(lua_State* state, const char* text)
{
    lua_getglobal(state, "sys_err");
    lua_pushstring(state, text);
    Call(state, 1);
}

struct AuditRuntime : SqlRuntime {
    CAsyncSQL* sql;
    int lockDepth;
    int posted;
    AuditRuntime() : sql(0), lockDepth(0), posted(0) {}

    virtual int SemInit(sem_t* sem, int shared, unsigned int count) {
        assert(sem && !shared && !count);
        *sem = reinterpret_cast<sem_t>(1);
        return 0;
    }
    virtual int SemDestroy(sem_t* sem) { assert(sem && *sem); return 0; }
    virtual int MutexInit(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr) {
        assert(mutex && !attr);
        std::memset(mutex, 0, sizeof(*mutex));
        return 0;
    }
    virtual int Create(pthread_t*, const pthread_attr_t* attr, void* (*entry)(void*), void* arg) {
        assert(!attr && entry == AsyncSQLThread && arg == sql);
        return 11; // Existing real-queue tests stop at the native thread-create boundary.
    }
    virtual void Close(MYSQL* handle) { assert(handle == sql->GetSQLHandle()); }
    virtual int MutexDestroy(pthread_mutex_t* mutex) { assert(mutex && !lockDepth); return 0; }
    virtual unsigned long Escape(MYSQL* handle, char* to, const char* from, unsigned long count) {
        assert(handle == sql->GetSQLHandle());
        unsigned long used = 0;
        for (unsigned long i = 0; i < count; ++i) {
            if (from[i] == '\'' || from[i] == '\\')
                to[used++] = '\\';
            to[used++] = from[i];
        }
        to[used] = 0;
        return used;
    }
    virtual int MutexLock(pthread_mutex_t* mutex) {
        assert(mutex == sql->m_mtxQuery && !lockDepth);
        ++lockDepth;
        return 0;
    }
    virtual int SemPost(sem_t* sem) {
        assert(sem && *sem && lockDepth == 1 && sql->m_queue_query.size() == 1);
        ++posted;
        return 0;
    }
    virtual int MutexUnlock(pthread_mutex_t* mutex) {
        assert(mutex == sql->m_mtxQuery && lockDepth == 1);
        --lockDepth;
        return 0;
    }
};

static void ExpectAudit(LogManager& logger, const char* suffix)
{
    assert(logger.m_sql.m_queue_query.size() == 1);
    SQLMsg* message = logger.m_sql.m_queue_query.front();
    assert(message->stQuery == std::string("INSERT DELAYED INTO log_global (type, time, who, x, y, what, how, hint) VALUES('CHARACTER', NOW(), 87, 4294967271, 30, ") + suffix);
    assert(message->m_pkSQL == logger.m_sql.GetSQLHandle() && !message->bReturn);
    delete message;
    logger.m_sql.m_queue_query.pop();
}

int main()
{
    desc_test_allow_clock();
    assert(sizeof(DWORD) == 4);
    quest::CQuestManager manager;
    lua_State* state = lua_open();
    assert(state);
    manager.L = state;
    Register(state);

    quest::PC pc;
    pc.m_stCurQuest = "log_quest";
    CHARACTER character;
    character.m_stName = "log_player";
    character.m_dwPlayerID = 87;
    character.SetXYZ(-25, 30, 0);
    LPBUFFER output = buffer_new(0, 16384);
    LPFDWATCH watch = fdwatch_new(16);
    DescOutputProbe descriptor;
    descriptor.Attach(watch, 5, output);
    descriptor.SetPacketState(5);
    character.BindDesc(&descriptor);

    test_server = 0;
    manager.m_pCurrentPC = 0;
    manager.m_pCurrentCharacter = &character;
    InfoCall(state, 0, "without-pc");
    ErrorCall(state, "without-pc");
    manager.m_pCurrentPC = &pc;
    manager.m_pCurrentCharacter = 0;
    InfoCall(state, 0, "without-character");
    ErrorCall(state, "without-character");
    manager.m_pCurrentCharacter = &character;
    lua_getglobal(state, "sys_log");
    lua_pushboolean(state, 1);
    lua_pushstring(state, "wrong-level-type");
    Call(state, 2);
    lua_getglobal(state, "sys_log");
    lua_pushnumber(state, 0);
    lua_pushboolean(state, 1);
    Call(state, 2);
    lua_getglobal(state, "sys_err");
    lua_pushboolean(state, 1);
    Call(state, 1);
    InfoCall(state, 1, "suppressed");
    assert(information.empty() && diagnostics.empty() && !buffer_size(output));

    lua_getglobal(state, "sys_log");
    lua_pushstring(state, "0.25");
    lua_pushnumber(state, 42);
    Call(state, 2);
    assert(information.size() == 1 && information.back() == "42");
    Packet(output, 1, "QUEST_SYSLOG 42");
    InfoCall(state, std::numeric_limits<double>::quiet_NaN(), "unordered");
    assert(information.size() == 2 && information.back() == "unordered");
    Packet(output, 1, "QUEST_SYSLOG unordered");
    ErrorCall(state, "ordinary-error");
    assert(diagnostics.size() == 1 && diagnostics.back() == "from Quest log_quest Player log_player msg : ordinary-error");
    Packet(output, 1, "QUEST_SYSERR ordinary-error");
    assert(!buffer_size(output));

    test_server = 1;
    InfoCall(state, 2, "test-server");
    assert(information.size() == 3 && information.back() == "test-server");
    Packet(output, 1, "QUEST_SYSLOG test-server");
    ErrorCall(state, "test-error");
    assert(diagnostics.size() == 2 && diagnostics.back() == "from Quest log_quest Player log_player msg : test-error");
    Packet(output, 3, "error occor on [_syserr:62]");
    Packet(output, 3, "from Quest log_quest Player log_player msg : test-error");
    Packet(output, 1, "QUEST_SYSERR test-error");
    assert(!buffer_size(output));
    test_server = 0;

    AuditRuntime runtime;
    sql_test_runtime = &runtime;
    {
        LogManager logger;
        runtime.sql = &logger.m_sql;
        assert(logger.Connect("host", "user", "password", "database"));
        g_table_postfix = "_global";
        lua_getglobal(state, "char_log");
        lua_pushstring(state, "-1.75");
        lua_pushnumber(state, 25);
        lua_pushstring(state, "a'b\\c");
        Call(state, 3);
        ExpectAudit(logger, "4294967295, '25', 'a\\'b\\\\c')");
        lua_getglobal(state, "char_log");
        lua_pushboolean(state, 1);
        lua_pushboolean(state, 1);
        lua_pushboolean(state, 1);
        Call(state, 3);
        ExpectAudit(logger, "0, '', '')");
        assert(runtime.posted == 2 && !runtime.lockDepth);
    }
    sql_test_runtime = 0;
    character.BindDesc(0);
    descriptor.Attach(0, -1, 0);
    buffer_delete(output);
    fdwatch_delete(watch);
    manager.m_pCurrentCharacter = 0;
    manager.m_pCurrentPC = 0;
    std::puts("whole Lua global registrar and callbacks passed through real VM, CHAR, DESC and CharLog");
    return 0;
}
