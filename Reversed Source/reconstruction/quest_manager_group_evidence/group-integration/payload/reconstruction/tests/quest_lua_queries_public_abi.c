/* Actual supplied SDK PUBLIC API checks; original internal92/12 differs nativeMinGW target. */
#include "../../liblua/include/lua.h"
typedef char q_lua_number8[sizeof(lua_Number)==8?1:-1];
typedef char q_lua_int4[sizeof(int)==4?1:-1];
typedef char q_lua_registry[(LUA_REGISTRYINDEX==-10000)?1:-1];
typedef char q_lua_globals[(LUA_GLOBALSINDEX==-10001)?1:-1];
typedef char q_lua_nil0[(LUA_TNIL==0)?1:-1];
typedef char q_lua_number3[(LUA_TNUMBER==3)?1:-1];
typedef char q_lua_string4[(LUA_TSTRING==4)?1:-1];
typedef char q_lua_table5[(LUA_TTABLE==5)?1:-1];
typedef char q_lua_thread8[(LUA_TTHREAD==8)?1:-1];
/* Correct actual C declaration types are checked by normal compiler pointer assignment. */
static void q_api_types(void){
 lua_State* (*open_state)(void)=lua_open;
 void (*close_state)(lua_State*)=lua_close;
 int (*gettop)(lua_State*)=lua_gettop;
 void (*settop)(lua_State*,int)=lua_settop;
 void (*pushstring)(lua_State*,const char*)=lua_pushstring;
 void (*pushnumber)(lua_State*,lua_Number)=lua_pushnumber;
 void (*gettable)(lua_State*,int)=lua_gettable;
 int (*type)(lua_State*,int)=lua_type;
 lua_Number (*tonumber)(lua_State*,int)=lua_tonumber;
 const char* (*tostring)(lua_State*,int)=lua_tostring;
 (void)open_state;(void)close_state;(void)gettop;(void)settop;(void)pushstring;(void)pushnumber;(void)gettable;(void)type;(void)tonumber;(void)tostring;
}
