#ifndef MCNCC_GAME_OXEVENT_H
#define MCNCC_GAME_OXEVENT_H
#include "../../common/singleton.h"
#include "event.h"
#include <cstdint>
#include <map>
#include <set>
#include <vector>
struct tag_Quiz {
    char level;
    char Quiz[256];
    bool answer;
};
class CHARACTER;

enum OXEventStatus {
    OXEVENT_FINISH = 0,
    OXEVENT_OPEN = 1,
    OXEVENT_CLOSE = 2,
    OXEVENT_QUIZ = 3,
    OXEVENT_ERR = 0xff
};

class COXEventManager : public singleton<COXEventManager> {
public:
    bool Initialize();
    void Destroy();
    OXEventStatus GetStatus();
    void SetStatus(OXEventStatus status);
    bool CloseEvent();
    bool ShowQuizList(CHARACTER* character);
    bool LogWinner();
    void ClearQuiz();
    unsigned GetAttenderCount() const { return m_map_attender.size(); }
    bool Quiz(unsigned char level, int timeLimit);
    bool CheckAnswer(bool answer);
    void WarpToAudience();
    bool GiveItemToAttender(DWORD vnum, unsigned char count);
    friend long oxevent_timer(LPEVENT event);
    bool AddQuiz(unsigned char level, const char* question, bool answer);
    bool Enter(CHARACTER* character);
    bool EnterAttender(CHARACTER* character);
    bool EnterAudience(CHARACTER* character);
private:
    std::map<std::uint32_t, std::uint32_t> m_map_char;
    std::map<std::uint32_t, std::uint32_t> m_map_attender;
    std::map<std::uint32_t, std::uint32_t> m_map_miss;
    std::vector<std::vector<tag_Quiz> > m_vec_quiz;
    std::set<LPEVENT> m_timedEvents;
};
#endif
