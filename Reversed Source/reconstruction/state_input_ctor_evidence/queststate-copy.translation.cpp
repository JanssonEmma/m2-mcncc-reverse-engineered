// Evidence translation ONLY: implicit memberwise copy remains appropriate in
// the full record. This file is not an added production definition.
quest::QuestState::QuestState(const QuestState& other):
 co(other.co),ico(other.ico),args(other.args),suspend_state(other.suspend_state),
 iIndex(other.iIndex),bStart(other.bStart),st(other.st),_title(other._title),
 _clock_name(other._clock_name),_counter_name(other._counter_name),
 _clock_value(other._clock_value),_counter_value(other._counter_value),
 _icon_file(other._icon_file),chat_scripts(other.chat_scripts) {}
// chat_scripts copies POINTERS, never AArgScript pointees; no Lua retain/close.
// Copying a default state before assigning its two counters retains original
// indeterminate-value behavior. Tests must set both before any copy.
