// Complete old CItem data/virtual interface; nonvirtual API recovery is partial.
#ifndef MCNCC_GAME_ITEM_H
#define MCNCC_GAME_ITEM_H
#include "entity.h"
#include "event.h"
#include "../../common/tables.h"
class CHARACTER;
struct packet_item_use;
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
    void UpdatePacket();
    long GetValue(DWORD idx);
    bool IsPCBangItem();
    int GetRefineLevel();
    DWORD GetRefineFromVnum();
    void UsePacketEncode(CHARACTER* character, CHARACTER* victim, packet_item_use* packet);
    bool CreateSocket(BYTE slot, BYTE gold);
    bool IsPolymorphItem();
    long FindApplyValue(BYTE type);
    void SetSockets(const long* sockets);
    void SetSocket(int index,long value);
    void ApplyAddon(int iAddonType);
    bool RemoveAttributeType(BYTE bType);
    void AlterToSocketItem(int iSocketCount);
    int GetAttributeSetIndex();
    bool HasAttr(BYTE bApply);
    int GetAttributeCount();
    int FindAttribute(BYTE bType);
    void SetAttribute(int i,BYTE bType,short sValue);
    void AddAttribute(BYTE bApply,short sValue);

    void StartUniqueExpireEvent();
    void StopUniqueExpireEvent();
    void SetUniqueExpireEvent(LPEVENT event);
    int GetSocketCount();
    bool AddSocket();
    bool IsAccessoryForSocket();
    bool CanPutInto(CItem* item);
    void SetAccessorySocketMaxGrade(int maxGrade);
// Public API additions inside CItem. Raw old inline getters; do not clamp.
int GetAccessorySocketGrade() { return m_alSockets[0]; }
int GetAccessorySocketMaxGrade() { return m_alSockets[1]; }
int GetAccessorySocketDownGradeTime() { return m_alSockets[2]; }
void SetAccessorySocketGrade(int iGrade);
void SetAccessorySocketDownGradeTime(DWORD time);

    bool AddToCharacter(CHARACTER* character,BYTE cell);
    bool DistanceValid(CHARACTER* character);
    bool IsOwnership(CHARACTER* character);
    void SetOwnership(CHARACTER* character,int seconds = 10);
    void SetOwnershipEvent(LPEVENT event);
    bool AddToGround(long mapIndex, const PIXEL_POSITION& position);
    void StartDestroyEvent();
    void SetDestroyEvent(LPEVENT event);
    CItem* RemoveFromGround();
    void SetCell(CHARACTER* character,BYTE cell) { m_pOwner=character; m_bCell=cell; }
    BYTE GetSize() { return m_pProto ? m_pProto->bSize : 0; }
    BYTE GetType() { return m_pProto ? m_pProto->bType : 0; }
    BYTE GetSubType() { return m_pProto ? m_pProto->bSubType : 0; }
    // Old inline signatures and field accesses corroborated by persistence callers.
    BYTE GetWindow() { return m_bWindow; }
    void SetWindow(BYTE value) { m_bWindow=value; }
    DWORD GetID() { return m_dwID; }
    void SetID(DWORD value) { m_dwID=value; }
    DWORD GetVID() { return m_dwVID; }
    void SetVID(DWORD value) { m_dwVID=value; }
    BYTE GetCell() { return m_bCell; }
    bool IsEquipped() const { return m_bEquipped; }
    bool CanUsedBy(CHARACTER* character);
    bool CheckItemUseLevel(int level);
    bool IsExchanging() { return m_bExchanging; }
    void SetExchanging(bool on);
    DWORD GetCount() { return m_dwCount; }
    CHARACTER* GetOwner() { return m_pOwner; }
    const char* GetName() { return m_pProto ? m_pProto->szLocaleName : 0; }
    const TItemTable* GetProto() { return m_pProto; }
    DWORD GetVnum() { return m_pProto ? m_pProto->dwVnum : 0; }
    int GetSpecialGroup();
    const long* GetSockets() { return m_alSockets; }
    const TPlayerItemAttribute* GetAttributes() { return m_aAttr; }
    bool GetSkipSave() { return m_bSkipSave; }
    void SetSkipSave(bool value) { m_bSkipSave=value; }
    DWORD GetLastOwnerPID() { return m_dwLastOwnerPID; }
    void AddFlag(long bit);
    void RemoveFlag(long bit);
    long GetFlag() { return m_lFlag; }
    int GetGold();
    int GetShopBuyPrice();
    void ModifyPoints(bool add);
    bool SetCount(DWORD count);
    CItem* RemoveFromCharacter();
    int FindEquipCell(CHARACTER* character);
    bool EquipTo(CHARACTER* character, BYTE wearCell);
    bool Unequip();
    void StartAccessorySocketExpireEvent();
    void StopAccessorySocketExpireEvent();
    void SetAccessorySocketExpireEvent(LPEVENT event);
    void AccessorySocketDegrade();
    void AddAttr(BYTE bApply,BYTE bLevel);
    void PutAttributeWithLevel(BYTE bLevel);
    void PutAttribute(int* aiAttrPercentTable);
    void AlterToMagicItem();
    void ChangeAttribute(const int* aiChangeProb = 0);
    void AddAttribute();
    void ClearAttribute();
    bool RemoveAttributeAt(int index);
    void SetAttributes(const TPlayerItemAttribute* c_pAttribute);
    void CopyAttributeTo(CItem* pItem);
    bool HasRareAttr(BYTE bApply);
    int GetRareAttrCount();
    bool ChangeRareAttribute();
    bool AddRareAttribute();

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
