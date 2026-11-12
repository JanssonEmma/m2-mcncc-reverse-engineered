#ifndef MCNCC_GAME_EXCHANGE_H
#define MCNCC_GAME_EXCHANGE_H
#include "../../libthecore/include/typedef.h"

class CHARACTER;
class CGrid;
class CItem;

class CExchange {
public:
    explicit CExchange(CHARACTER* owner);
    ~CExchange();
    bool Accept(bool accept=true);
    void Cancel();
    bool AddGold(long gold);
    bool AddItem(BYTE cell, BYTE display);
    bool RemoveItem(BYTE slot);
    CHARACTER* GetOwner() { return m_pOwner; }
    CExchange* GetCompany() { return m_pCompany; }
    bool GetAcceptStatus() { return m_bAccept; }
    void SetCompany(CExchange* company) { m_pCompany=company; }
private:
    bool Done();
    bool Check(int* count);
    bool CheckSpace();

    CExchange* m_pCompany;
    CHARACTER* m_pOwner;
    BYTE m_abItemPos[12];
    CItem* m_apItems[12];
    BYTE m_abItemDisplayPos[12];
    bool m_bAccept;
    long m_lGold;
    CGrid* m_pGrid;
};
#endif
