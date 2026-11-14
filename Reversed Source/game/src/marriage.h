#ifndef MCNCC_GAME_MARRIAGE_H
#define MCNCC_GAME_MARRIAGE_H
#include "../../common/singleton.h"
#include "../../libthecore/include/typedef.h"
#include <map>
#include <set>
#include <utility>
#include "event.h"
#include <cstdint>
#include <ctime>
#include <string>
class CHARACTER;
namespace marriage {
struct TWeddingInfo { std::uint32_t dwMapIndex; };
struct TMarriage {
    std::uint32_t pid1, pid2;
    int love_point;
    time_t marry_time;
    CHARACTER* ch1;
    CHARACTER* ch2;
    bool bSave, is_married;
    std::string name1, name2;
    TWeddingInfo* pWeddingInfo;
    bool isLastNear;
    BYTE byLastLovePoint;
    LPEVENT eventNearCheck;

    ~TMarriage();
    DWORD GetOther(DWORD pid) const { const auto id = static_cast<std::uint32_t>(pid); return id == pid1 ? pid2 : (id == pid2 ? pid1 : 0); }
    void SetMarried();
    void WarpToWeddingMap(DWORD pid);
    void RequestEndWedding();
    bool IsNear();
    bool IsOnline() { return ch1 && ch2; }
    int GetMarriagePoint();
    int GetMarriageGrade();
    int GetBonus(DWORD vnum, bool share, CHARACTER* character);
    void Update(DWORD points);
    void Save();
    void Logout(DWORD pid);
    void StopNearCheckEvent();
    void StartNearCheckEvent();
    void NearCheck();
    void Login(CHARACTER* character);
};
class CManager : public singleton<CManager> {
public:
    CManager();
    virtual ~CManager();
    bool Initialize();
    void Destroy();
    void Login(CHARACTER* character);
    void Logout(CHARACTER* character);
    TMarriage* Get(DWORD pid);
    bool IsMarriageUniqueItem(DWORD vnum);
    bool IsMarried(DWORD pid);
    bool IsEngaged(DWORD pid);
    bool IsEngagedOrMarried(DWORD pid);
    void Add(DWORD first, DWORD second, time_t marriedAt, const char* firstName, const char* secondName);
    void Update(DWORD first, DWORD second, long totalPoints, BYTE married);
    void Remove(DWORD first, DWORD second);
    void WeddingReady(DWORD first, DWORD second, DWORD map);
    void WeddingStart(DWORD first, DWORD second);
    void WeddingEnd(DWORD first, DWORD second);
    void RequestUpdate(DWORD first, DWORD second, int points, BYTE married);
    void RequestAdd(DWORD first, DWORD second, const char* firstName, const char* secondName);
    void RequestRemove(DWORD first, DWORD second);
    void RequestEndWedding(DWORD first, DWORD second);
    template <class Callback> Callback for_each_wedding(Callback callback)
    {
        for (const auto& couple : m_setWedding) {
            if (TMarriage* record = Get(couple.first))
                callback(record);
        }
        return callback;
    }
private:
    std::set<TMarriage*> m_Marriages;
    std::map<std::uint32_t, TMarriage*> m_MarriageByPID;
    std::set<std::pair<std::uint32_t, std::uint32_t> > m_setWedding;
};
}
#endif
