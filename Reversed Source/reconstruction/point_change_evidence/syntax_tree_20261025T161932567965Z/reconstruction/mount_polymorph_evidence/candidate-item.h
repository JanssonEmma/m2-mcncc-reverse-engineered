// Complete old CItem data/virtual interface; nonvirtual API recovery is partial.
#ifndef MCNCC_GAME_ITEM_H
#define MCNCC_GAME_ITEM_H
#include "entity.h"
#include "event.h"
#include "../../common/tables.h"
class CHARACTER;
class CItem : public CEntity {
public:
    CItem();
    virtual ~CItem();
    virtual void EncodeInsertPacket(LPENTITY entity);
    virtual void EncodeRemovePacket(LPENTITY entity);
    void Initialize();
    void Destroy();
    void SetProto(TItemTable* table);
    void Save();
    void ModifyPoints(bool add);
    void UpdatePacket();
    long GetValue(DWORD idx);
    void SetSockets(long* sockets);
    void SetSocket(int index,long value);
    void StopUniqueExpireEvent();
    int GetSocketCount();
    bool IsAccessoryForSocket();
    void SetAccessorySocketMaxGrade(int maxGrade);
// Public API additions inside CItem. Raw old inline getters; do not clamp.
int GetAccessorySocketGrade() { return m_alSockets[0]; }
int GetAccessorySocketMaxGrade() { return m_alSockets[1]; }
int GetAccessorySocketDownGradeTime() { return m_alSockets[2]; }
void SetAccessorySocketGrade(int iGrade);
void SetAccessorySocketDownGradeTime(DWORD time);

    bool AddToCharacter(CHARACTER* character,BYTE cell);
    bool IsOwnership(CHARACTER* character);
    void SetOwnership(CHARACTER* character,int seconds = 10);
    void SetOwnershipEvent(LPEVENT event);
    CItem* RemoveFromGround();
    void SetCell(CHARACTER* character,BYTE cell) { m_pOwner=character; m_bCell=cell; }
    BYTE GetSize() { return m_pProto ? m_pProto->bSize : 0; }
    // Old inline signatures and field accesses corroborated by persistence callers.
    BYTE GetWindow() { return m_bWindow; }
    void SetWindow(BYTE value) { m_bWindow=value; }
    DWORD GetID() { return m_dwID; }
    void SetID(DWORD value) { m_dwID=value; }
    DWORD GetVID() { return m_dwVID; }
    void SetVID(DWORD value) { m_dwVID=value; }
    BYTE GetCell() { return m_bCell; }
    DWORD GetCount() { return m_dwCount; }
    CHARACTER* GetOwner() { return m_pOwner; }
    const char* GetName() { return m_pProto ? m_pProto->szLocaleName : 0; }
    const TItemTable* GetProto() { return m_pProto; }
    DWORD GetVnum() { return m_pProto ? m_pProto->dwVnum : 0; }
    const long* GetSockets() { return m_alSockets; }
    const TPlayerItemAttribute* GetAttributes() { return m_aAttr; }
    bool GetSkipSave() { return m_bSkipSave; }
    void SetSkipSave(bool value) { m_bSkipSave=value; }
    DWORD GetLastOwnerPID() { return m_dwLastOwnerPID; }
    long GetFlag() { return m_lFlag; }
private:
    const TItemTable * m_pProto;
    CHARACTER * m_pOwner;
    BYTE m_bWindow;
    DWORD m_dwID;
    bool m_bEquipped;
    DWORD m_dwVID;
    BYTE m_bCell;
    DWORD m_dwCount;
    long int m_lFlag;
    DWORD m_dwLastOwnerPID;
    bool m_bExchanging;
    long int m_alSockets[3];
    TPlayerItemAttribute m_aAttr[7];
    LPEVENT m_pkDestroyEvent;
    LPEVENT m_pkUniqueExpireEvent;
    LPEVENT m_pkAccessorySocketExpireEvent;
    LPEVENT m_pkOwnershipEvent;
    DWORD m_dwOwnershipPID;
    bool m_bSkipSave;
};
#endif
