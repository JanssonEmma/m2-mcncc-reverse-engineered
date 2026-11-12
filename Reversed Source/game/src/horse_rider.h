// Full data declaration and virtual interface; no unrecovered no-op bodies.
#ifndef MCNCC_HORSE_RIDER_H
#define MCNCC_HORSE_RIDER_H
#include "../../common/tables.h"
#include "event.h"
const int HORSE_MAX_LEVEL=30;
struct THorseStat {
    int iMinLevel,iNPCRace,iMaxHealth,iMaxStamina;
    int iST,iDX,iHT,iIQ,iDamMean,iDamMin,iDamMax,iArmor;
};
extern THorseStat c_aHorseStat[HORSE_MAX_LEVEL+1];
long horse_stamina_consume_event(LPEVENT event);
long horse_stamina_regen_event(LPEVENT event);
class CHARACTER;
class CHorseRider {
public:
    CHorseRider();
    virtual ~CHorseRider();
    bool HasHorse();
    BYTE GetHorseLevel();
    BYTE GetHorseGrade();
    short GetHorseHealth();
    short GetHorseStamina();
    short GetHorseMaxHealth();
    short GetHorseMaxStamina();
    int GetHorseST();
    int GetHorseDX();
    int GetHorseHT();
    int GetHorseIQ();
    int GetHorseArmor();
    virtual bool ReviveHorse();
    void FeedHorse();
    virtual void HorseDie();
    bool IsHorseRiding();
    void ResetHorseHealthDropTime();
    virtual void SetHorseLevel(int level);
    void EnterHorse();
    virtual void SendHorseInfo();
    virtual void ClearHorseInfo();
    virtual void UpdateRideTime(int interval);
protected:
    void SetHorseData(const THorseInfo& info);
    const THorseInfo& GetHorseData();
    void UpdateHorseDataByLogoff(DWORD logoffTime);
    virtual bool StartRiding();
    virtual bool StopRiding();
    virtual DWORD GetMyHorseVnum();
private:
    void UpdateHorseStamina(int stamina, bool send = true);
    void StartStaminaConsumeEvent();
    void StartStaminaRegenEvent();
    void UpdateHorseHealth(int health, bool send = true);
    void CheckHorseHealthDropTime(bool send = true);
    void Initialize();
    void Destroy();
    friend long horse_stamina_consume_event(LPEVENT event);
    friend void do_horse_set_stat(CHARACTER*, char*, int, int);
    friend long horse_stamina_regen_event(LPEVENT event);
    THorseInfo m_Horse;
    LPEVENT m_eventStaminaRegen;
    LPEVENT m_eventStaminaConsume;
};

// Old non-const signatures; ordinary field/stat access follows the recorded data types.
inline bool CHorseRider::HasHorse() { return m_Horse.bLevel!=0; }
inline BYTE CHorseRider::GetHorseLevel() { return m_Horse.bLevel; }
inline short CHorseRider::GetHorseHealth() { return m_Horse.sHealth; }
inline short CHorseRider::GetHorseStamina() { return m_Horse.sStamina; }
inline bool CHorseRider::IsHorseRiding() { return m_Horse.bRiding!=0; }
inline const THorseInfo& CHorseRider::GetHorseData() { return m_Horse; }
inline int CHorseRider::GetHorseST() { return c_aHorseStat[m_Horse.bLevel].iST; }
inline int CHorseRider::GetHorseDX() { return c_aHorseStat[m_Horse.bLevel].iDX; }
inline int CHorseRider::GetHorseHT() { return c_aHorseStat[m_Horse.bLevel].iHT; }
inline int CHorseRider::GetHorseIQ() { return c_aHorseStat[m_Horse.bLevel].iIQ; }
inline int CHorseRider::GetHorseArmor() { return c_aHorseStat[m_Horse.bLevel].iArmor; }
#endif


