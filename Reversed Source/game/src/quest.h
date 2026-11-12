//Complete old quest script/state record data; behavior recovery is partial.

#ifndef MCNCC_GAME_QUEST_H

#define MCNCC_GAME_QUEST_H

#include <string>

#include <vector>

#include "../../libthecore/include/typedef.h"

struct lua_State;

namespace quest {

enum EQuestConfirmType { CONFIRM_NO, CONFIRM_YES, CONFIRM_TIMEOUT };

struct AStateScriptType {
    std::vector<char> m_code;
    int GetSize() const { return m_code.size(); }
    const char* GetCode() const { return &m_code[0]; }
};

struct AArgScript {

    std::string arg;

    std::vector<char> when_condition;

    AStateScriptType script;

    unsigned int quest_index;

    int state_index;

    // Original quest.h99 inline default constructor: only implicit STL member construction.
    // quest_index/state_index retain their original indeterminate values until initialized.
    AArgScript() {}

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

    // Original quest.h inline default ctor C1 0805d230 (139 bytes).

    // Preserve indeterminate _clock_value/_counter_value; do NOT copy newer defaults.

    QuestState():co(0),ico(0),args(0),suspend_state(0),iIndex(0),bStart(false),st(-1) {}

    // Existing complete field declaration generates the original memberwise copy

    // semantics corresponding to C1(const QuestState&) 080633e0. No explicit copy

    // declaration is needed or proposed by this bounded integration candidate.

};

}

#endif

