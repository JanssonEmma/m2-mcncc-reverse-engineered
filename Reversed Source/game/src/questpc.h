//Complete old PC data declaration (target108 bytes); quest execution is pending.
#ifndef MCNCC_GAME_QUESTPC_H
#define MCNCC_GAME_QUESTPC_H
#include "quest.h"
#include "event.h"
#include <map>
class CHARACTER;
namespace quest {
struct RewardData {
    enum RewardType { REWARD_TYPE_NONE=0, REWARD_TYPE_EXP=1, REWARD_TYPE_ITEM=2 } type;
    DWORD value1;
    int value2;
    // Original questpc.h inline RewardData(RewardType,DWORD,int=0).
    // Inline call-site instances 08060f34 and 0806109d; no standalone original export.
    RewardData(RewardType kind,DWORD first,int second=0):type(kind),value1(first),value2(second) {}
};
class PC {
public:
    typedef std::map<unsigned int,QuestState> QuestInfo;
    typedef QuestInfo::iterator QuestInfoIterator;
    PC();
    ~PC();
    void Destroy();
    void SetID(DWORD);
    bool HasQuest(const std::string&);
    QuestState& GetQuest(const std::string&);
    QuestInfoIterator quest_begin() { return m_QuestInfo.begin(); }
    QuestInfoIterator quest_end() { return m_QuestInfo.end(); }
    QuestInfoIterator quest_find(DWORD index) { return m_QuestInfo.find(index); }
    bool IsRunning() { return m_RunningQuestState!=0; }
    void EndRunning();
    void CancelRunning();
    QuestState* GetRunningQuestState() { return m_RunningQuestState; }
    void SetQuest(const std::string&,QuestState&);
    void SetCurrentQuestStateName(const std::string&);
    void SetQuestState(const std::string&,const std::string&);
    void SetQuestState(const std::string&,int);
    void ClearQuest(const std::string&);
    void SetFlag(const std::string&,int,bool);
    int GetFlag(const std::string&);
    bool DeleteFlag(const std::string&);
    const std::string& GetCurrentQuestName();
    int GetCurrentQuestIndex();
    void RemoveTimer(const std::string&);
    void RemoveTimerNotCancel(const std::string&);
    void AddTimer(const std::string&,LPEVENT);
    void ClearTimer();
    void SetCurrentQuestStartFlag();
    void SetCurrentQuestDoneFlag();
    void SetQuestTitle(const std::string&,const std::string&);
    void SetCurrentQuestTitle(const std::string&);
    void SetCurrentQuestClockName(const std::string&);
    void SetCurrentQuestClockValue(int);
    void SetCurrentQuestCounterName(const std::string&);
    void SetCurrentQuestCounterValue(int);
    void SetCurrentQuestIconFile(const std::string&);
    // Old nonconst inline read witnessed by LevelUp.
    bool IsLoaded() { return m_bLoaded; }
// Original questpc.h116 inline; input_db.cpp080e9b5e..080e9b62.
// Build is a SEPARATE original caller instruction080e9b65, not part of this method.
    void SetLoaded() { m_bLoaded=true; }
    void Build();
    void Save();
    bool HasReward() { return !m_vRewardData.empty() || m_bIsGivenReward; }
    void Reward(CHARACTER*);
    void GiveItem(const std::string&,DWORD,int);
    void GiveExp(const std::string&,DWORD);
    void SetSendDoneFlag() { m_bShouldSendDone=true; }
    bool GetAndResetDoneFlag() { bool previous=m_bShouldSendDone; m_bShouldSendDone=false; return previous; }
    void SendFlagList(CHARACTER*);
    void SetQuestState(const char*,const char*);
    void SetConfirmWait(DWORD id) { m_dwWaitConfirmFromPID=id; m_bConfirmWait=true; }
    void ClearConfirmWait() { m_bConfirmWait=false; }
    bool IsConfirmWait();
    bool IsConfirmWait(DWORD id) { return m_bConfirmWait && id==m_dwWaitConfirmFromPID; }

private:
    typedef std::map<std::string,int> TFlagMap;
    typedef std::map<std::string,LPEVENT> TTimerMap;
    void AddQuestStateChange(const std::string&,int,int);
    void DoQuestStateChange();
    void SetSendFlag(int);
    void ClearSendFlag();
    void SaveFlag(const std::string&,int);
    void ClearCurrentQuestBeginFlag();
    void SetCurrentQuestBeginFlag();
    int GetCurrentQuestBeginFlag();
    void SendQuestInfoPakcet();

    struct TQuestStateChangeInfo { DWORD quest_idx; int prev_state; int next_state; TQuestStateChangeInfo(DWORD index,int previous,int next):quest_idx(index),prev_state(previous),next_state(next) {} };
    std::vector<TQuestStateChangeInfo> m_QuestStateChange;
    std::vector<RewardData> m_vRewardData;
    bool m_bIsGivenReward;
    bool m_bShouldSendDone;
    DWORD m_dwID;
    std::map<unsigned int,QuestState> m_QuestInfo;
    QuestState* m_RunningQuestState;
    std::string m_stCurQuest;
    std::map<std::string,int> m_FlagMap;
    std::map<std::string,int> m_FlagSaveMap;
    std::map<std::string,LPEVENT> m_TimerMap;
    int m_iSendToClient;
    bool m_bLoaded;
    int m_iLastState;
    DWORD m_dwWaitConfirmFromPID;
    bool m_bConfirmWait;
};
}
#endif
