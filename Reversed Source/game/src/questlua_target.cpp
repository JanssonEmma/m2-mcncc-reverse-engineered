#include "questmanager.h"
#include "char.h"
#include "sectree_manager.h"
#include "target.h"
#include "../../liblua/include/lauxlib.h"
#include "../../libthecore/include/log.h"
#include <climits>
#include <cstring>

namespace quest {
namespace {
int TargetNumber(lua_State* state, int argument)
{
    const lua_Number value=lua_tonumber(state,argument);
    // FISTP with x87 rounding set to truncate.
    if (!(value>-2147483649.0 && value<2147483648.0)) return INT_MIN;
    return static_cast<int>(value);
}

int TargetCoordinate(long base, int cell)
{
    unsigned int bits=static_cast<unsigned int>(base)+static_cast<unsigned int>(cell)*100;
    int value;
    std::memcpy(&value,&bits,sizeof(value));
    return value;
}
}

int target_pos(lua_State* state)
{
    CQuestManager& manager=CQuestManager::instance();
    CHARACTER* ch=manager.GetCurrentCharacterPtr();
    int index=manager.GetCurrentPC()->GetCurrentQuestIndex();
    if (!lua_isstring(state,1) || !lua_isnumber(state,2) || !lua_isnumber(state,3)) {
        _sys_err("target_pos",0x13,"invalid argument, name: %s, quest_index %d",ch->GetName(),index);
        return 0;
    }
    PIXEL_POSITION base;
    if (!SECTREE_MANAGER::instance().GetMapBasePositionByMapIndex(ch->GetMapIndex(),base)) {
        _sys_err("target_pos",0x1b,"cannot find base position in this map %ld",ch->GetMapIndex());
        return 0;
    }
    int x=TargetCoordinate(base.x,TargetNumber(state,2));
    int y=TargetCoordinate(base.y,TargetNumber(state,3));
    int send=lua_isnumber(state,6) ? TargetNumber(state,6) : 1;
    const char* description=lua_isstring(state,5) ? lua_tostring(state,5) : 0;
    int map=TargetNumber(state,4);
    CTargetManager::instance().CreateTarget(ch->GetPlayerID(),index,lua_tostring(state,1),TARGET_TYPE_POS,x,y,map,description,send);
    return 1;
}

int target_vid(lua_State* state)
{
    CQuestManager& manager=CQuestManager::instance();
    CHARACTER* ch=manager.GetCurrentCharacterPtr();
    int index=manager.GetCurrentPC()->GetCurrentQuestIndex();
    if (!lua_isstring(state,1) || !lua_isnumber(state,2)) {
        _sys_err("target_vid",0x35,"invalid argument, name: %s, quest_index %d",ch->GetName(),index);
        return 0;
    }
    int send=lua_isnumber(state,4) ? TargetNumber(state,4) : 1;
    const char* description=lua_isstring(state,3) ? lua_tostring(state,3) : 0;
    int map=ch->GetMapIndex();
    int vid=TargetNumber(state,2);
    CTargetManager::instance().CreateTarget(ch->GetPlayerID(),index,lua_tostring(state,1),TARGET_TYPE_VID,vid,0,map,description,send);
    return 1;
}

int target_delete(lua_State* state)
{
    CQuestManager& manager=CQuestManager::instance();
    CHARACTER* ch=manager.GetCurrentCharacterPtr();
    int index=manager.GetCurrentPC()->GetCurrentQuestIndex();
    if (!lua_isstring(state,1)) {
        _sys_err("target_delete",0x4e,"invalid argument, name: %s, quest_index %d",ch->GetName(),index);
        return 0;
    }
    CTargetManager::instance().DeleteTarget(ch->GetPlayerID(),index,lua_tostring(state,1));
    return 1;
}

int target_clear(lua_State*)
{
    CQuestManager& manager=CQuestManager::instance();
    CHARACTER* ch=manager.GetCurrentCharacterPtr();
    int index=manager.GetCurrentPC()->GetCurrentQuestIndex();
    CTargetManager::instance().DeleteTarget(ch->GetPlayerID(),index,0);
    return 1;
}

int target_id(lua_State* state)
{
    CQuestManager& manager=CQuestManager::instance();
    CHARACTER* ch=manager.GetCurrentCharacterPtr();
    DWORD index=manager.GetCurrentPC()->GetCurrentQuestIndex();
    if (!lua_isstring(state,1)) {
        _sys_err("target_id",0x67,"invalid argument, name: %s, quest_index %lu",ch->GetName(),index);
        lua_pushnumber(state,0);
        return 0;
    }
    LPEVENT event=CTargetManager::instance().GetTargetEvent(ch->GetPlayerID(),index,lua_tostring(state,1));
    int vid=0;
    if (event) {
        TargetInfo* info=static_cast<TargetInfo*>(event_info(event));
        if (info->iType==TARGET_TYPE_VID) vid=info->iArg1;
    }
    lua_pushnumber(state,vid);
    return 1;
}

void RegisterTargetFunctionTable()
{
    luaL_reg functions[]={
        {"pos",target_pos},
        {"vid",target_vid},
        {"npc",target_vid},
        {"pc",target_vid},
        {"delete",target_delete},
        {"clear",target_clear},
        {"id",target_id},
        {0,0}
    };
    CQuestManager::instance().AddLuaFunctionTable("target",functions);
}
}
