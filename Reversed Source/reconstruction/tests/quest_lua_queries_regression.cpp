// Whole original CQM state queries over actual supplied Lua5.0.3 C SDK; test owns real VM lifetime.
#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <ext/hash_map>
#define private public
#include "../../game/src/questmanager.h"
#undef private
#include "../../game/src/char.h"
#include "desc_fixture.h"
#include "../../game/src/config.h"
#include "../../game/src/locale_service.h"
extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <fenv.h>
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
static quest::CQuestManager* manager;
static lua_State *primary,*alternate;
static int errorLine,errorCount,debugCount,debugValue,debugTop,mode,metaCalls;
static std::string errorQuest,errorState,debugQuest,debugState;
static int errorIndex;
extern "C" void quest_lua_error(int line,const char* quest,const char* state,int index){
 assert(line==939||line==957);++errorCount;errorLine=line;errorQuest=quest;errorState=state?state:"";errorIndex=index;
 // Diagnostic occurs with failed global value still on originalstack; following settop reloads ownerL.
 assert(lua_gettop(manager->L)==3);
 if(mode==1){manager->L=alternate;mode=0;}
}
extern "C" void quest_lua_debug(int value,int top,const char* quest,const char* state){
 ++debugCount;debugValue=value;debugTop=top;debugQuest=quest;debugState=state;assert(lua_gettop(manager->L)==top);
 if(mode==2){manager->L=alternate;mode=0;}
}
static void Prefix(lua_State* L,int count=2){lua_settop(L,0);lua_pushnumber(L,111);lua_pushstring(L,"sentinel");if(count==3)lua_pushnumber(L,222);assert(lua_gettop(L)==count);}
static void Top(lua_State* L){assert(lua_gettop(L)==2&&lua_tonumber(L,1)==111&&!std::strcmp(lua_tostring(L,2),"sentinel"));}
static void Reset(){manager->L=primary;Prefix(primary);Prefix(alternate);mode=0;errorLine=errorCount=debugCount=0;test_server=0;}
static void Number(lua_State* L,const char* key,double value){lua_pushstring(L,key);lua_pushnumber(L,value);lua_settable(L,-3);}
static void String(lua_State* L,int key,const char* value){lua_pushnumber(L,key);lua_pushstring(L,value);lua_settable(L,-3);}
static int IndexMetamethod(lua_State* L){
 ++metaCalls;assert(lua_gettop(L)==2);
 if(mode==3){assert(!std::strcmp(lua_tostring(L,2),"metastate"));lua_pushnumber(L,26);manager->L=alternate;return 1;}
 if(mode==4){assert(lua_tonumber(L,2)==77);lua_pushstring(L,"from-primary");manager->L=alternate;return 1;}
 lua_pushnumber(L,17);return 1;
}
static void MetaTable(lua_State* L,const char* name){lua_newtable(L);lua_newtable(L);lua_pushstring(L,"__index");lua_pushcfunction(L,IndexMetamethod);lua_settable(L,-3);lua_setmetatable(L,-2);lua_setglobal(L,name);}
static const std::string wrongQuest("wrong"),wrongState("x");
static int ProtectedName(lua_State* L){const char* value=manager->GetQuestStateName(wrongQuest,1);if(value)lua_pushstring(L,value);else lua_pushnil(L);return 1;}
static int ProtectedIndex(lua_State* L){lua_pushnumber(L,manager->GetQuestStateIndex(wrongQuest,wrongState));return 1;}
static int FirstBinding(lua_State* L) {
    lua_pushnumber(L, 11);
    return 1;
}

static int SecondBinding(lua_State* L) {
    lua_pushnumber(L, 22);
    return 1;
}

static int globalWrites;
static int StoreBindingGlobal(lua_State* L) {
    assert(lua_gettop(L) == 3 && lua_istable(L, 1) && lua_istable(L, 3));
    assert(std::strcmp(lua_tostring(L, 2), "bindings") == 0);
    ++globalWrites;
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_rawset(L, 1);
    return 0;
}

static int reverseErrors;
extern "C" void quest_lua_reverse_error(const char* message) {
    assert(std::strcmp(message, "QUEST wrong quest state file for quest missing_reverse") == 0);
    assert(lua_gettop(primary) == 3 && lua_isnil(primary, -1));
    ++reverseErrors;
}

static void TestStartupHelpers(quest::CQuestManager& qm) {
    Reset();
    lua_pushvalue(primary, LUA_GLOBALSINDEX);
    lua_newtable(primary);
    lua_pushstring(primary, "__newindex");
    lua_pushcfunction(primary, StoreBindingGlobal);
    lua_rawset(primary, -3);
    lua_setmetatable(primary, -2);
    lua_pop(primary, 1);

    luaL_reg bindings[] = {
        {"call", FirstBinding}, {"call", SecondBinding}, {0, 0}, {"ignored", FirstBinding}
    };
    qm.AddLuaFunctionTable("bindings", bindings);
    assert(globalWrites == 1);
    Top(primary);
    lua_getglobal(primary, "bindings");
    lua_pushstring(primary, "call");
    lua_rawget(primary, -2);
    assert(lua_pcall(primary, 0, 1, 0) == 0 && lua_tonumber(primary, -1) == 22);
    lua_pop(primary, 1);
    lua_pushstring(primary, "ignored");
    lua_rawget(primary, -2);
    assert(lua_isnil(primary, -1));
    lua_pop(primary, 2);

    lua_pushvalue(primary, LUA_GLOBALSINDEX);
    lua_pushnil(primary);
    lua_setmetatable(primary, -2);
    lua_pop(primary, 1);
    qm.AddLuaFunctionTable("bindings", bindings + 2);
    lua_getglobal(primary, "bindings");
    assert(lua_istable(primary, -1));
    lua_pushnil(primary);
    assert(lua_next(primary, -2) == 0);
    lua_pop(primary, 1);
    Top(primary);

    // Existing reverse slots avoid adding a field during Lua's table iteration.
    lua_newtable(primary);
    Number(primary, "start", 8);
    String(primary, 8, "old");
    lua_pushstring(primary, "decimal");
    lua_pushstring(primary, "007");
    lua_rawset(primary, -3);
    lua_pushstring(primary, "007");
    lua_pushboolean(primary, 0);
    lua_rawset(primary, -3);
    lua_setglobal(primary, "reverse");
    qm.BuildStateIndexToName("reverse");
    assert(std::strcmp(qm.GetQuestStateName("reverse", 8), "start") == 0);
    lua_getglobal(primary, "reverse");
    lua_pushstring(primary, "007");
    lua_rawget(primary, -2);
    assert(std::strcmp(lua_tostring(primary, -1), "decimal") == 0);
    lua_pop(primary, 2);
    Top(primary);
    qm.BuildStateIndexToName("missing_reverse");
    assert(reverseErrors == 1);
    Top(primary);
}

namespace quest {
int _get_locale(lua_State*);
int _number(lua_State*);
int _say(lua_State*);
int _raw_script(lua_State*);
int _set_skin(lua_State*);
int _get_server_timer_arg(lua_State*);
int _getnpcid(lua_State*);
int _is_test_server(lua_State*);
int _is_allow_map(lua_State*);
}

static int callbackErrors;
extern "C" void quest_lua_callback_error(const char* function, int line, const char* message) {
    if (std::strcmp(function, "_raw_script") == 0) {
        assert(line == 706 && std::strcmp(message, "QUEST wrong argument: questname: example") == 0);
    } else {
        assert(std::strcmp(function, "_set_skin") == 0);
        assert(line == 739 && std::strcmp(message, "QUEST wrong skin index") == 0);
    }
    ++callbackErrors;
}

static void TestDirectStartupCallbacks(quest::CQuestManager& qm) {
    Reset();
    lua_pushcfunction(primary, quest::_say);
    lua_pushstring(primary, "one");
    lua_pushboolean(primary, 1);
    lua_pushnumber(primary, 23);
    lua_pushlstring(primary, "two\0ignored", 11);
    assert(lua_pcall(primary, 4, 0, 0) == 0);
    assert(qm.m_strScript == "one23two[ENTER]");

    lua_pushcfunction(primary, quest::_raw_script);
    lua_pushstring(primary, "unused");
    lua_pushnumber(primary, 17);
    assert(lua_pcall(primary, 2, 0, 0) == 0);
    assert(qm.m_strScript == "one23two[ENTER]17");
    quest::PC pc;
    pc.m_stCurQuest = "example";
    qm.m_pCurrentPC = &pc;
    lua_pushcfunction(primary, quest::_raw_script);
    lua_pushboolean(primary, 0);
    assert(lua_pcall(primary, 1, 0, 0) == 0);
    qm.m_pCurrentPC = 0;

    const int rounding = fegetround();
    assert(fesetround(FE_UPWARD) == 0);
    lua_pushcfunction(primary, quest::_set_skin);
    lua_pushnumber(primary, 1.25);
    assert(lua_pcall(primary, 1, 0, 0) == 0 && qm.m_iCurrentSkin == 2);
    assert(fesetround(rounding) == 0);
    lua_pushcfunction(primary, quest::_set_skin);
    lua_pushnil(primary);
    assert(lua_pcall(primary, 1, 0, 0) == 0 && qm.m_iCurrentSkin == 2);
    assert(callbackErrors == 2);

    lua_pushcfunction(primary, quest::_number);
    lua_pushnumber(primary, -3.75);
    lua_pushstring(primary, "-3.25");
    assert(lua_pcall(primary, 2, 1, 0) == 0 && lua_tonumber(primary, -1) == -3);
    lua_pop(primary, 1);
    lua_pushcfunction(primary, quest::_number);
    lua_pushboolean(primary, 0);
    assert(lua_pcall(primary, 1, 1, 0) == 0 && lua_tonumber(primary, -1) == 0);
    lua_pop(primary, 1);

    qm.m_dwServerTimerArg = 0xffffffffUL;
    lua_pushcfunction(primary, quest::_get_server_timer_arg);
    assert(lua_pcall(primary, 0, 1, 0) == 0 && lua_tonumber(primary, -1) == 4294967295.0);
    lua_pop(primary, 1);
    qm.m_mapNPCNameID["7"] = 0x80000000U;
    lua_pushcfunction(primary, quest::_getnpcid);
    lua_pushnumber(primary, 7);
    assert(lua_pcall(primary, 1, 1, 0) == 0 && lua_tonumber(primary, -1) == 2147483648.0);
    lua_pop(primary, 1);

    lua_pushcfunction(primary, quest::_get_locale);
    assert(lua_pcall(primary, 0, 1, 0) == 0 && std::strcmp(lua_tostring(primary, -1), g_stLocale.c_str()) == 0);
    lua_pop(primary, 1);
    test_server = -1;
    lua_pushcfunction(primary, quest::_is_test_server);
    assert(lua_pcall(primary, 0, 1, 0) == 0 && lua_isboolean(primary, -1) && lua_toboolean(primary, -1));
    lua_pop(primary, 1);
    test_server = 0;
    map_allow_add(321);
    lua_pushcfunction(primary, quest::_is_allow_map);
    lua_pushnumber(primary, 321.75);
    assert(lua_pcall(primary, 1, 1, 0) == 0 && lua_type(primary, -1) == LUA_TNUMBER &&
           lua_tonumber(primary, -1) == 1);
    lua_pop(primary, 1);
    Top(primary);
}

namespace quest {
int _left_image(lua_State*);
int _top_image(lua_State*);
int _time_to_str(lua_State*);
int _chat(lua_State*);
int _cmdchat(lua_State*);
int _syschat(lua_State*);
int _notice(lua_State*);
}

static void TestDirectOutputCallbacks(quest::CQuestManager& qm) {
    Reset();
    qm.m_strScript.clear();
    lua_pushcfunction(primary, quest::_left_image);
    lua_pushboolean(primary, 1);
    lua_pushlstring(primary, "pic\0ignored", 11);
    assert(lua_pcall(primary, 2, LUA_MULTRET, 0) == 0);
    Top(primary);
    assert(qm.m_strScript == "[LEFTIMAGE src;pic]");

    lua_pushcfunction(primary, quest::_top_image);
    lua_pushstring(primary, "ignored");
    lua_pushnumber(primary, 67);
    assert(lua_pcall(primary, 2, LUA_MULTRET, 0) == 0);
    Top(primary);
    assert(qm.m_strScript == "[LEFTIMAGE src;pic][TOPIMAGE src;67]");

    lua_pushcfunction(primary, quest::_left_image);
    lua_pushstring(primary, "ignored");
    lua_pushboolean(primary, 1);
    assert(lua_pcall(primary, 2, LUA_MULTRET, 0) == 0);
    Top(primary);
    assert(qm.m_strScript == "[LEFTIMAGE src;pic][TOPIMAGE src;67]");

    CHARACTER character;
    DescOutputProbe descriptor;
    LPFDWATCH watch = fdwatch_new(16);
    LPBUFFER output = buffer_new(0, 8192);
    descriptor.Attach(watch, 5, output);
    descriptor.SetPacketState(5);
    character.BindDesc(&descriptor);
    qm.m_pCurrentCharacter = &character;
    const lua_CFunction functions[] = {quest::_chat, quest::_cmdchat, quest::_syschat, quest::_notice};
    const BYTE types[] = {0, 5, 1, 2};
    for (unsigned int i = 0; i < sizeof(types) / sizeof(types[0]); ++i) {
        lua_pushcfunction(primary, functions[i]);
        lua_pushstring(primary, "hello% ");
        lua_pushboolean(primary, 1);
        lua_pushnumber(primary, 23);
        lua_pushlstring(primary, "x\0ignored", 9);
        assert(lua_pcall(primary, 4, LUA_MULTRET, 0) == 0);
        Top(primary);
        const BYTE* bytes = static_cast<const BYTE*>(buffer_read_peek(output));
        assert(buffer_size(output) == 20 && bytes[0] == 4 && bytes[1] == 20 &&
               bytes[2] == 0 && bytes[3] == types[i]);
        assert(std::memcmp(bytes + 9, "hello% 23x", 11) == 0);
        buffer_reset(output);
    }
    lua_pushcfunction(primary, quest::_notice);
    assert(lua_pcall(primary, 0, LUA_MULTRET, 0) == 0);
    Top(primary);
    const BYTE* empty = static_cast<const BYTE*>(buffer_read_peek(output));
    assert(buffer_size(output) == 10 && empty[0] == 4 && empty[3] == 2 && empty[9] == 0);
    buffer_reset(output);
    character.BindDesc(0);
    lua_pushcfunction(primary, quest::_chat);
    lua_pushstring(primary, "discarded by real ChatPacket without a descriptor");
    assert(lua_pcall(primary, 1, LUA_MULTRET, 0) == 0 && !buffer_size(output));
    Top(primary);
    qm.m_pCurrentCharacter = 0;
    descriptor.Attach(0, -1, 0);
    buffer_delete(output);
    fdwatch_delete(watch);

    const int rounding = fegetround();
    assert(fesetround(FE_UPWARD) == 0);
    lua_pushcfunction(primary, quest::_time_to_str);
    lua_pushnumber(primary, 0);
    lua_pushnumber(primary, 946684799.75);
    assert(lua_pcall(primary, 2, LUA_MULTRET, 0) == 0);
    assert(fesetround(rounding) == 0);
    assert(lua_gettop(primary) == 3 &&
           std::strcmp(lua_tostring(primary, -1), "Fri Dec 31 23:59:59 1999\n") == 0);
    lua_pop(primary, 1);
    Top(primary);
}

bool DropEvent_CharStone_SetValue(const std::string&, int);
static std::vector<std::string> stoneFormats;
static std::vector<double> stoneValues;
extern "C" void quest_lua_char_stone_log(const char* format, double value) {
    stoneFormats.push_back(format);
    stoneValues.push_back(value);
}

static void CheckCharStoneLogs(const char* toggle, int alive, double low, double middle,
                               double high, int range) {
    unsigned int offset = 0;
    assert(stoneValues.size() == stoneFormats.size());
    if (toggle) {
        assert(stoneFormats.size() == 6 && stoneFormats[0] == toggle);
        offset = 1;
    }
    assert(stoneFormats.size() == offset + 5);
    const char* formats[] = {
        "dropevent.drop_char_stone: %d", "dropevent.drop_char_stone.percent_lv01_10: %f",
        "dropevent.drop_char_stone.percent_lv11_30: %f", "dropevent.drop_char_stone.percent_lv31_MX: %f",
        "dropevent.drop_char_stone.level_range: %d"
    };
    const double values[] = {static_cast<double>(alive), low, middle, high, static_cast<double>(range)};
    for (unsigned int i = 0; i < 5; ++i) {
        assert(stoneFormats[offset + i] == formats[i]);
        assert(stoneValues[offset + i] == values[i]);
    }
    stoneFormats.clear();
    stoneValues.clear();
}

static void TestCharStoneFlag() {
    assert(!DropEvent_CharStone_SetValue("drop_char_stone.unknown", 5) && stoneFormats.empty());
    assert(DropEvent_CharStone_SetValue("drop_char_stone", 0));
    CheckCharStoneLogs("dropevent.drop_char_stone = off", 0, 1, 2, 3, 10);
    assert(DropEvent_CharStone_SetValue("drop_char_stone", -9));
    CheckCharStoneLogs("dropevent.drop_char_stone = on", 1, 1, 2, 3, 10);
    assert(DropEvent_CharStone_SetValue("drop_char_stone.percent_lv01_10", 16777217));
    CheckCharStoneLogs(0, 1, 167772.17, 2, 3, 10);
    assert(DropEvent_CharStone_SetValue("drop_char_stone.percent_lv11_30", -25));
    CheckCharStoneLogs(0, 1, 167772.17, -0.25, 3, 10);
    assert(DropEvent_CharStone_SetValue("drop_char_stone.percent_lv31_MX", 125));
    CheckCharStoneLogs(0, 1, 167772.17, -0.25, 1.25, 10);
    assert(DropEvent_CharStone_SetValue("drop_char_stone.level_range", -4));
    CheckCharStoneLogs(0, 1, 167772.17, -0.25, 1.25, -4);
    assert(!DropEvent_CharStone_SetValue("DROP_CHAR_STONE", 0) && stoneFormats.empty());
}

int main(){
 assert(sizeof(lua_Number)==8&&sizeof(int)==4);desc_test_allow_clock();quest::CQuestManager qm;manager=&qm;assert(!qm.L);primary=lua_open();alternate=lua_open();assert(primary&&alternate);manager->L=primary;
 // Real table values: reverse numeric keys, forward named values and convertible numeric strings.
 lua_newtable(primary);String(primary,0,"start");String(primary,-17,"negative");String(primary,INT_MAX,"highest");Number(primary,"start",0);Number(primary,"negative",-17);Number(primary,"fraction",3.5);Number(primary,"minus",-2.5);Number(primary,"up",1.25);Number(primary,"down",1.75);
 lua_pushstring(primary,"numeric-string");lua_pushstring(primary,"12.5");lua_settable(primary,-3);lua_pushnumber(primary,9);lua_pushboolean(primary,1);lua_settable(primary,-3);lua_pushnumber(primary,18);lua_pushnumber(primary,23);lua_settable(primary,-3);lua_setglobal(primary,"q");
 Reset();assert(!std::strcmp(qm.GetQuestStateName("q",0),"start"));Top(primary);const char* borrowed=qm.GetQuestStateName("q",-17);assert(!std::strcmp(borrowed,"negative")&&borrowed==qm.GetQuestStateName("q",-17));Top(primary);assert(!std::strcmp(qm.GetQuestStateName("q",INT_MAX),"highest"));Top(primary);
 assert(!qm.GetQuestStateName("q",999)&&!qm.GetQuestStateName("q",9));Top(primary);std::string numeric=qm.GetQuestStateName("q",18);assert(numeric=="23");Top(primary);assert(!errorCount&&!debugCount);
 const int oldRound=fegetround();assert(fesetround(FE_TONEAREST)==0);assert(qm.GetQuestStateIndex("q","fraction")==4&&qm.GetQuestStateIndex("q","minus")==-2&&qm.GetQuestStateIndex("q","numeric-string")==12);Top(primary);
 assert(fesetround(FE_UPWARD)==0);assert(qm.GetQuestStateIndex("q","up")==2);assert(fesetround(FE_DOWNWARD)==0);assert(qm.GetQuestStateIndex("q","down")==1);assert(fesetround(oldRound)==0);
 assert(qm.GetQuestStateIndex("q","absent")==0&&qm.GetQuestStateIndex("q","start")==0&&qm.GetQuestStateIndex("q","negative")==-17);Top(primary);assert(!errorCount);
 // Both std::string arguments enter Lua as original C strings, truncating at embeddedNUL.
 std::string binaryQuest("q\0ignored",9),binaryState("negative\0ignored",16);assert(qm.GetQuestStateIndex(binaryQuest,binaryState)==-17);assert(!std::strcmp(qm.GetQuestStateName(binaryQuest,0),"start"));Top(primary);
 Reset();assert(!std::strcmp(qm.GetQuestStateName("missing",-1),""));assert(errorCount==1&&errorLine==939&&errorQuest=="missing"&&errorIndex==-1);Top(primary);assert(qm.GetQuestStateIndex("missing","state")==0);assert(errorCount==2&&errorLine==957&&errorQuest=="missing"&&errorState=="state");Top(primary);
 // Missingnil diagnostics then restore on whichever manager.L the callback leaves active.
 Reset();Prefix(alternate,3);mode=1;assert(qm.GetQuestStateIndex("missing","state")==0&&manager->L==alternate&&mode==0);Top(alternate);assert(lua_gettop(primary)==3);Prefix(primary);
 Reset();Prefix(alternate,3);mode=1;assert(!std::strcmp(qm.GetQuestStateName("missing",4),"")&&manager->L==alternate&&mode==0);Top(alternate);assert(lua_gettop(primary)==3);Prefix(primary);
 // Debug logging runs after stack restoration, retaining old format's x/v argument inversion.
 Reset();test_server=1;assert(qm.GetQuestStateIndex("q","negative")==-17);assert(debugCount==1&&debugValue==-17&&debugTop==2&&debugQuest=="q"&&debugState=="negative");Top(primary);mode=2;assert(qm.GetQuestStateIndex("q","start")==0&&manager->L==alternate&&debugCount==2&&mode==0);Top(primary);Top(alternate);
 // Actual __index C closures exercise real Lua lookup dispatch, not a table/query mock.
 Reset();MetaTable(primary,"meta");metaCalls=0;assert(qm.GetQuestStateIndex("meta","any")==17&&metaCalls==1);Top(primary);
 // A Lua metamethod can change the manager ownerL. Later tonumber/tostring/settop reload it.
 Reset();lua_pushnumber(alternate,91);mode=3;assert(qm.GetQuestStateIndex("meta","metastate")==91&&manager->L==alternate&&metaCalls==2);Top(alternate);assert(lua_gettop(primary)==4&&lua_type(primary,3)==LUA_TTABLE&&lua_tonumber(primary,4)==26);Prefix(primary);
 Reset();lua_pushstring(alternate,"from-alternate");mode=4;std::string changed=qm.GetQuestStateName("meta",77);assert(changed=="from-alternate"&&manager->L==alternate&&metaCalls==3);Top(alternate);assert(lua_gettop(primary)==4&&lua_type(primary,3)==LUA_TTABLE&&!std::strcmp(lua_tostring(primary,4),"from-primary"));Prefix(primary);
 // Non-table globals preserve actual Lua errors. Test-owned protected calls catch; app methods don't.
 Reset();lua_pushnumber(primary,5);lua_setglobal(primary,"wrong");lua_pushcfunction(primary,ProtectedName);int result=lua_pcall(primary,0,1,0);assert(result!=0&&lua_gettop(primary)==3&&!errorCount);lua_pop(primary,1);Top(primary);lua_pushcfunction(primary,ProtectedIndex);result=lua_pcall(primary,0,1,0);assert(result!=0&&lua_gettop(primary)==3&&!errorCount);lua_pop(primary,1);Top(primary);
 TestStartupHelpers(qm);
 TestDirectStartupCallbacks(qm);
 TestDirectOutputCallbacks(qm);
 TestCharStoneFlag();
 manager->L=0;lua_close(alternate);lua_close(primary);manager=0;primary=alternate=0;test_server=0;std::puts("whole CQM Lua state queries passed over supplied real Lua5.0.3 SDK; original5.0 revision/VM startup equivalence unclaimed");return 0;
}
