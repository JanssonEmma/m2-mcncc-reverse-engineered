#include "char.h"
#include "config.h"
#include "log.h"
#include "../../common/singleton.h"
#include "../../libthecore/include/log.h"
#include <vector>

// Original itemlog.cpp owner. The old ring advances the pivot before insertion.
struct cItemLog : public singleton<cItemLog>
{
    typedef std::vector<int> VEC_ITEMID;

    cItemLog();
    virtual ~cItemLog();
    bool Insert(int id);
    bool Find(int id);
    int GetCount() { return m_nCount; }
    void Print();

private:
    int __GetNewPivot();
    int __GetPivot();
    void __SetPivot(int pivot);
    void __IncPivot();

    VEC_ITEMID m_vec_itemid;
    int m_nPivot;
    int m_nCount;
};

cItemLog::cItemLog()
{
    m_vec_itemid.insert(m_vec_itemid.begin(), 10000, 0);
    m_nPivot = 0;
    m_nCount = 0;
}

cItemLog::~cItemLog() {}

int cItemLog::__GetNewPivot()
{
    __IncPivot();
    if (__GetPivot() > 9999)
        __SetPivot(0);
    return __GetPivot();
}

int cItemLog::__GetPivot() { return m_nPivot; }
void cItemLog::__SetPivot(int pivot) { m_nPivot = pivot; }
void cItemLog::__IncPivot() { ++m_nPivot; }

bool cItemLog::Insert(int id)
{
    if (Find(id))
        return false;
    m_vec_itemid[__GetNewPivot()] = id;
    if (m_nCount < 10000)
        ++m_nCount;
    else
        m_nCount = 10000;
    return true;
}

bool cItemLog::Find(int id)
{
    for (int i = 0; i < 10000 && i < m_nCount; ++i) {
        if (m_vec_itemid[i] == id)
            return true;
    }
    return false;
}

void cItemLog::Print()
{
    sys_log(0, "===========================");
    for (VEC_ITEMID::iterator it = m_vec_itemid.begin(); it != m_vec_itemid.end(); ++it)
        sys_log(0, "item log id %d", *it);
    sys_log(0, "===========================");
}

cItemLog* g_pItemLog = 0;

void InitItemLog()
{
    if (!g_pItemLog)
        g_pItemLog = new cItemLog;
}

void ReleaseLog()
{
    delete g_pItemLog;
}

bool IsSelledItem(CHARACTER* character, int id)
{
    if (cItemLog::instance().Insert(id)) {
        if (test_server) {
            sys_log(0, "[HACK_ITEM] Added Item list name %s  PlayerID%d ItemID%d",
                    character->GetName(), static_cast<int>(character->GetPlayerID()), id);
            character->ChatPacket(1, "Added itemlog list name %s Player id %d itemid %d",
                                  character->GetName(), static_cast<int>(character->GetPlayerID()), id);
        }
        return true;
    }

    sys_log(0, "[HACK_ITEM] PlayerID %d , x %d y %d , itemid %d",
            static_cast<int>(character->GetPlayerID()), static_cast<int>(character->GetX()),
            static_cast<int>(character->GetY()), id);
    const char* name = character->GetName();
    LogManager::instance().ItemLog(character->GetPlayerID(), character->GetX(), character->GetY(),
                                   id, "HACK ITEM", name);
    return false;
}

void PrintItemLog()
{
    cItemLog::instance().Print();
}
