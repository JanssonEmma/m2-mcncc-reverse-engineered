//Complete old PC data declaration (target108 bytes); quest execution is pending.
#ifndef MCNCC_GAME_QUESTPC_H
#define MCNCC_GAME_QUESTPC_H
#include "quest.h"
#include "event.h"
#include <map>
namespace quest {
struct RewardData {
    enum RewardType { REWARD_TYPE_NONE=0, REWARD_TYPE_EXP=1, REWARD_TYPE_ITEM=2 } type;
    DWORD value1;
    int value2;
    RewardData(RewardType type,DWORD value1,int value2=0);
};
class PC {
public:
    PC();
    ~PC();
private:
    struct TQuestStateChangeInfo { DWORD quest_idx; int prev_state; int next_state; };
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
