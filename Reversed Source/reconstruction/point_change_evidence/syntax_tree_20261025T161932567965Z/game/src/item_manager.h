// Complete old manager data; nonvirtual API recovery remains partial.
#ifndef MCNCC_GAME_ITEM_MANAGER_H
#define MCNCC_GAME_ITEM_MANAGER_H
#include <vector>
#include <map>
#include <set>
#include "../../common/singleton.h"
#include "../../common/tables.h"
class CItem;
class CDropItemGroup;
class CSpecialItemGroup;
class CMobItemGroup;
class CLevelItemGroup;
class CBuyerThiefGlovesItemGroup;
class ITEM_MANAGER : public singleton<ITEM_MANAGER> {
public:
    ITEM_MANAGER();
    virtual ~ITEM_MANAGER();
    void Destroy();
    void DelayedSave(CItem* item);
    void FlushDelayedSave(CItem* item);
    void SaveSingleItem(CItem* item);
    void Update();
    void GracefulShutdown();
    CItem* Find(DWORD id);
    CItem* FindByVID(DWORD vid);
    DWORD GetRefineFromVnum(DWORD vnum);
private:
    std::vector<TItemTable> m_vec_prototype;
    std::map<DWORD,DWORD> m_map_ItemRefineFrom;
    int m_iTopOfTable;
    std::map<DWORD,CItem*> m_VIDMap;
    DWORD m_dwVIDCount;
    DWORD m_dwCurrentID;
    std::set<CItem*> m_set_pkItemForDelayedSave;
    std::map<DWORD,CItem*> m_map_pkItemByID;
    std::map<DWORD,DWORD> m_map_dwEtcItemDropProb;
    std::map<DWORD,CDropItemGroup*> m_map_pkDropItemGroup;
    std::map<DWORD,CSpecialItemGroup*> m_map_pkSpecialItemGroup;
    std::map<DWORD,CMobItemGroup*> m_map_pkMobItemGroup;
    std::map<DWORD,CLevelItemGroup*> m_map_pkLevelItemGroup;
    std::map<DWORD,CBuyerThiefGlovesItemGroup*> m_map_pkGloveItemGroup;
    std::map<DWORD,int> m_ItemToSpecialGroup;
    std::map<DWORD,TItemTable> m_map_vid;
};
#endif
