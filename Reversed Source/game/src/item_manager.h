// Complete old manager data; nonvirtual API recovery remains partial.
#ifndef MCNCC_GAME_ITEM_MANAGER_H
#define MCNCC_GAME_ITEM_MANAGER_H
#include <cstdint>
#include <vector>
#include <map>
#include <set>
#include "../../common/singleton.h"
#include "../../common/tables.h"
int GetDropPerKillPct(int minimum, int fallback, int deltaPercent, const char* flag);
bool DropEvent_CharStone_SetValue(const std::string& name, int value);
class CItem;
class CHARACTER;
class CDropItemGroup {
public:
    struct SDropItemGroupInfo {
        DWORD dwVnum;
        DWORD dwPct;
        int iCount;
    };

    CDropItemGroup(DWORD vnum, DWORD mobVnum, const std::string& name)
        : m_dwVnum(vnum), m_dwMobVnum(mobVnum), m_stName(name)
    {
    }

    const std::vector<SDropItemGroupInfo>& GetVector() { return m_vec_items; }

    void AddItem(DWORD vnum, DWORD probability, int count)
    {
        const SDropItemGroupInfo item = { vnum, probability, count };
        m_vec_items.push_back(item);
    }

private:
    DWORD m_dwVnum;
    DWORD m_dwMobVnum;
    std::string m_stName;
    std::vector<SDropItemGroupInfo> m_vec_items;
};
class CSpecialItemGroup {
public:
    enum EGiveType { NONE, GOLD, EXP, MOB, SLOW, DRAIN_HP, POISON, MOB_GROUP };

    struct CSpecialItemInfo {
        DWORD vnum;
        int count;
        int rare;
    };

    explicit CSpecialItemGroup(DWORD vnum) : m_dwVnum(vnum) {}
    const CSpecialItemInfo* SelectItem() const;

    void AddItem(DWORD vnum, int count, int weight, int rare)
    {
        if (!weight)
            return;
        const CSpecialItemInfo item = { vnum, count, rare };
        const auto previous = m_vecProbs.empty() ? 0U : static_cast<std::uint32_t>(m_vecProbs.back());
        m_vecProbs.push_back(static_cast<std::int32_t>(previous + static_cast<std::uint32_t>(weight)));
        m_vecItems.push_back(item);
    }

private:
    DWORD m_dwVnum;
    std::vector<int> m_vecProbs;
    std::vector<CSpecialItemInfo> m_vecItems;
};
class CMobItemGroup {
public:
    struct SMobItemGroupInfo {
        DWORD dwItemVnum;
        int iCount;
        int iRarePct;
    };

    CMobItemGroup(DWORD mob, int killDrop, const std::string& name)
        : m_dwMobVnum(mob), m_iKillDrop(killDrop), m_stName(name) {}

    void AddItem(DWORD vnum, int count, int weight, int rare)
    {
        const SMobItemGroupInfo item = { vnum, count, rare };
        const auto previous = m_vecProbs.empty() ? 0U : static_cast<std::uint32_t>(m_vecProbs.back());
        m_vecProbs.push_back(static_cast<std::int32_t>(previous + static_cast<std::uint32_t>(weight)));
        m_vecItems.push_back(item);
    }

    const SMobItemGroupInfo* SelectItem() const;
    int GetKillPerDrop() const { return m_iKillDrop; }
    bool IsEmpty() const { return m_vecProbs.empty(); }

private:
    DWORD m_dwMobVnum;
    int m_iKillDrop;
    std::string m_stName;
    std::vector<int> m_vecProbs;
    std::vector<SMobItemGroupInfo> m_vecItems;
};

class CLevelItemGroup {
public:
    struct SLevelItemGroupInfo {
        DWORD dwVNum;
        DWORD dwPct;
        int iCount;
    };

    explicit CLevelItemGroup(DWORD limit) : m_dwLevelLimit(limit) {}
    DWORD GetLevelLimit() { return m_dwLevelLimit; }
    const std::vector<SLevelItemGroupInfo>& GetVector() { return m_vec_items; }
    void AddItem(DWORD vnum, DWORD probability, int count)
    {
        const SLevelItemGroupInfo item = { vnum, probability, count };
        m_vec_items.push_back(item);
    }

private:
    DWORD m_dwLevelLimit;
    std::string m_stName;
    std::vector<SLevelItemGroupInfo> m_vec_items;
};

class CBuyerThiefGlovesItemGroup {
public:
    struct SThiefGroupInfo {
        DWORD dwVnum;
        DWORD dwPct;
        int iCount;
    };

    CBuyerThiefGlovesItemGroup(DWORD vnum, DWORD mob, const std::string& name)
        : m_dwVnum(vnum), m_dwMobVnum(mob), m_stName(name) {}
    const std::vector<SThiefGroupInfo>& GetVector() { return m_vec_items; }
    void AddItem(DWORD vnum, DWORD probability, int count)
    {
        const SThiefGroupInfo item = { vnum, probability, count };
        m_vec_items.push_back(item);
    }

private:
    DWORD m_dwVnum;
    DWORD m_dwMobVnum;
    std::string m_stName;
    std::vector<SThiefGroupInfo> m_vec_items;
};
class ITEM_MANAGER : public singleton<ITEM_MANAGER> {
    friend class CItem;
    friend class CRoulette;
public:
    ITEM_MANAGER();
    virtual ~ITEM_MANAGER();
    bool Initialize(TItemTable* table, int size);
    bool GetVnum(const char* name, DWORD& vnum);
    bool GetVnumByOriginalName(const char* name, DWORD& vnum);
    void Destroy();
    void DelayedSave(CItem* item);
    void FlushDelayedSave(CItem* item);
    CItem* CreateItem(DWORD vnum, DWORD count, DWORD id, bool tryMagic, int rarePercent);
    void DestroyItem(CItem* item);
    void RemoveItem(CItem* item, const char* reason);
    void SaveSingleItem(CItem* item);
    void Update();
    void GracefulShutdown();
    CItem* Find(DWORD id);
    CItem* FindByVID(DWORD vid);
    DWORD GetRefineFromVnum(DWORD vnum);
    TItemTable* GetTable(DWORD vnum);
    DWORD GetNewID();
    bool SetMaxItemID(DWORD id);
    bool CreateDropItem(CHARACTER* mob, CHARACTER* killer, std::vector<CItem*>& drops);
    bool ReadCommonDropItemFile(const char* filename);
    bool ReadEtcDropItemFile(const char* filename);
    bool ReadDropItemGroup(const char* filename);
    bool ReadSpecialDropItemFile(const char* filename);
    CSpecialItemGroup* GetSpecialItemGroup(DWORD vnum);
    bool ReadMonsterDropItemGroup(const char* filename);
protected:
    void CreateQuestDropItem(CHARACTER* mob, CHARACTER* killer, std::vector<CItem*>& drops, int delta, int range);
    int RealNumber(DWORD vnum);
private:
    std::vector<TItemTable> m_vec_prototype;
    std::map<DWORD,DWORD> m_map_ItemRefineFrom;
    int m_iTopOfTable;
    std::map<DWORD,CItem*> m_VIDMap;
    std::uint32_t m_dwVIDCount;
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
