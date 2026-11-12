#ifndef __INC_ITEM_AWARD_H
#define __INC_ITEM_AWARD_H

#include <map>
#include <set>
#include <string>

#include "../../common/length.h"
#include "../../common/singleton.h"
#include "../../common/tables.h"
#include "../../libsql/AsyncSQL.h"
#include "DBProtocol.h"

typedef struct SItemAward
{
    DWORD dwID;
    char szLogin[LOGIN_MAX_LEN + 1];
    DWORD dwVnum;
    DWORD dwCount;
    DWORD dwSocket0;
    DWORD dwSocket1;
    DWORD dwSocket2;
    char szWhy[ITEM_AWARD_WHY_MAX_LEN + 1];
    bool bTaken;
    bool bMall;
} TItemAward;

class ItemAwardManager : public singleton<ItemAwardManager>
{
public:
    ItemAwardManager();
    virtual ~ItemAwardManager();

    void RequestLoad();
    void Load(SQLMsg* pMsg);
    std::set<TItemAward*>* GetByLogin(const char* login);
    void Taken(DWORD awardID, DWORD itemID);

private:
    std::map<DWORD, TItemAward*> m_map_award;
    std::map<std::string, std::set<TItemAward*> > m_map_kSetAwardByLogin;
};

#endif
