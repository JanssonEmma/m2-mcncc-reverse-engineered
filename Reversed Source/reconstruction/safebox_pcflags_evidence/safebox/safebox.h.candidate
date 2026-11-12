// Complete original safebox.h logical class/data/API; destructor remains declared until real removal SCC closes.
#ifndef MCNCC_GAME_SAFEBOX_H
#define MCNCC_GAME_SAFEBOX_H
#include "../../libthecore/include/typedef.h"
class CHARACTER;class CItem;class CGrid;
typedef CHARACTER* LPCHARACTER;
typedef CItem* LPITEM;
class CSafebox {
public:
    CSafebox(LPCHARACTER pkChrOwner,int iSize,DWORD dwGold);
    ~CSafebox();
    bool Add(DWORD dwPos,LPITEM pkItem);
    LPITEM Get(DWORD dwPos);
    LPITEM Remove(DWORD dwPos);
    void ChangeSize(int iSize);
    bool MoveItem(BYTE bCell,BYTE bDestCell,BYTE count);
    LPITEM GetItem(BYTE bCell);
    void Save();
    bool IsEmpty(DWORD dwPos,BYTE bSize);
    bool IsValidPosition(DWORD dwPos);
    void SetWindowMode(BYTE bWindowMode);
protected:
    void __Destroy();
    LPCHARACTER m_pkChrOwner;
    LPITEM m_pkItems[135];
    CGrid* m_pkGrid;
    int m_iSize;
    long m_lGold;
    BYTE m_bWindowMode;
};
#endif
