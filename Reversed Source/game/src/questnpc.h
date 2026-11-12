//Complete old NPC data declaration; the target has20 event slots.
#ifndef MCNCC_GAME_QUESTNPC_H
#define MCNCC_GAME_QUESTNPC_H
#include "questpc.h"
namespace quest {
class NPC {
public:
    typedef std::map<int,AStateScriptType> AQuestScriptType;
    typedef std::map<unsigned int,AQuestScriptType> QuestMapType;
    typedef std::map<int,std::vector<AArgScript> > AArgQuestScriptType;
    typedef std::map<unsigned int,AArgQuestScriptType> ArgQuestMapType;
    NPC();
    ~NPC();
    bool OnClick(PC& pc);
    bool OnTakeItem(PC& pc);
    bool OnUseItem(PC& pc, bool receiveAll);
    bool HasChat();
    bool OnChat(PC& pc);
    bool OnButton(PC& player, unsigned index);
    bool OnInfo(PC& player, unsigned index);
    void Set(unsigned int vnum, const std::string& script_name);
    bool OnLevelUp(PC& pc);
    bool OnKill(PC& pc);
    bool OnPartyKill(PC& pc);
    bool OnUnmount(PC& pc);
    bool OnAttrIn(PC& pc);
    bool OnTarget(PC& pc, DWORD index, const char* name, const char* verb, bool& result);
    bool OnAttrOut(PC& pc);
    bool OnLogin(PC& pc, const char* quest);
    bool OnLogout(PC& pc);
    bool OnTimer(PC& pc);
    bool OnServerTimer(PC& pc);
    bool HandleEvent(PC& pc, int event);
    bool HandleReceiveAllNoWaitEvent(PC& pc, int event);

    bool OnEnterState(PC& pc, DWORD index, int state);
    bool OnLeaveState(PC& pc, DWORD index, int state);
    bool OnLetter(PC& pc, DWORD index, int state);
    bool HandleReceiveAllEvent(PC& pc, int event);
    template<class MAP, class MATCH, class MISS>
    void MatchingQuest(PC& pc, MAP& quests, MATCH& match, MISS& miss);
protected:
    bool ExecuteEventScript(PC& pc, int event, DWORD index, int state);
    void LoadStateScript(int event_index, const char* filename, const char* script_name);
    unsigned int m_vnum;
    QuestMapType m_mapOwnQuest[20];
    ArgQuestMapType m_mapOwnArgQuest[20];
};
}
#endif
