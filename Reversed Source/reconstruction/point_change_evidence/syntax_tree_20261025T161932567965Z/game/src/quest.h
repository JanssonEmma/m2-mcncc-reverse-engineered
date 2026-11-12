//Complete old quest script/state record data; behavior recovery is partial.
#ifndef MCNCC_GAME_QUEST_H
#define MCNCC_GAME_QUEST_H
#include <string>
#include <vector>
#include "../../libthecore/include/typedef.h"
struct lua_State;
namespace quest {
struct AStateScriptType { std::vector<char> m_code; };
struct AArgScript {
    std::string arg;
    std::vector<char> when_condition;
    AStateScriptType script;
    unsigned int quest_index;
    int state_index;
    AArgScript();
};
struct QuestState {
    lua_State* co;
    int ico;
    short args;
    BYTE suspend_state;
    int iIndex;
    bool bStart;
    int st;
    std::string _title;
    std::string _clock_name;
    std::string _counter_name;
    int _clock_value;
    int _counter_value;
    std::string _icon_file;
    std::vector<AArgScript*> chat_scripts;
    QuestState();
};
}
#endif
