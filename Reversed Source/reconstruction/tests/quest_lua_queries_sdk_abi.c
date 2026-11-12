/* Real supplied SDK target32 ABI against original binary DWARF; revision equality is not claimed. */
#include "../../liblua/include/lua.h"
#include "../../liblua/src/lstate.h"
#include <stddef.h>
typedef char q_lua_number8[sizeof(lua_Number)==8?1:-1];
typedef char q_lua_pointer4[sizeof(void*)==4?1:-1];
typedef char q_lua_State_size[sizeof(lua_State)==92?1:-1];
typedef char q_lua_State_next[offsetof(lua_State,next)==0?1:-1];
typedef char q_lua_State_tt[offsetof(lua_State,tt)==4?1:-1];
typedef char q_lua_State_marked[offsetof(lua_State,marked)==5?1:-1];
typedef char q_lua_State_top[offsetof(lua_State,top)==8?1:-1];
typedef char q_lua_State_base[offsetof(lua_State,base)==12?1:-1];
typedef char q_lua_State_l_G[offsetof(lua_State,l_G)==16?1:-1];
typedef char q_lua_State_ci[offsetof(lua_State,ci)==20?1:-1];
typedef char q_lua_State_stack_last[offsetof(lua_State,stack_last)==24?1:-1];
typedef char q_lua_State_stack[offsetof(lua_State,stack)==28?1:-1];
typedef char q_lua_State_stacksize[offsetof(lua_State,stacksize)==32?1:-1];
typedef char q_lua_State_end_ci[offsetof(lua_State,end_ci)==36?1:-1];
typedef char q_lua_State_base_ci[offsetof(lua_State,base_ci)==40?1:-1];
typedef char q_lua_State_size_ci[offsetof(lua_State,size_ci)==44?1:-1];
typedef char q_lua_State_nCcalls[offsetof(lua_State,nCcalls)==46?1:-1];
typedef char q_lua_State_hookmask[offsetof(lua_State,hookmask)==48?1:-1];
typedef char q_lua_State_allowhook[offsetof(lua_State,allowhook)==49?1:-1];
typedef char q_lua_State_hookinit[offsetof(lua_State,hookinit)==50?1:-1];
typedef char q_lua_State_basehookcount[offsetof(lua_State,basehookcount)==52?1:-1];
typedef char q_lua_State_hookcount[offsetof(lua_State,hookcount)==56?1:-1];
typedef char q_lua_State_hook[offsetof(lua_State,hook)==60?1:-1];
typedef char q_lua_State__gt[offsetof(lua_State,_gt)==64?1:-1];
typedef char q_lua_State_openupval[offsetof(lua_State,openupval)==76?1:-1];
typedef char q_lua_State_gclist[offsetof(lua_State,gclist)==80?1:-1];
typedef char q_lua_State_errorJmp[offsetof(lua_State,errorJmp)==84?1:-1];
typedef char q_lua_State_errfunc[offsetof(lua_State,errfunc)==88?1:-1];
typedef char q_lua_TObject_size[sizeof(TObject)==12?1:-1];
typedef char q_lua_TObject_tt[offsetof(TObject,tt)==0?1:-1];
typedef char q_lua_TObject_value[offsetof(TObject,value)==4?1:-1];
typedef char q_CallInfo_size[sizeof(CallInfo)==24?1:-1];
typedef char q_CallInfo_base[offsetof(CallInfo,base)==0?1:-1];
typedef char q_CallInfo_top[offsetof(CallInfo,top)==4?1:-1];
typedef char q_CallInfo_state[offsetof(CallInfo,state)==8?1:-1];
typedef char q_CallInfo_u[offsetof(CallInfo,u)==12?1:-1];
