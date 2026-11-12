#include "char.h"
#include "config.h"
#include "db.h"
#include "guild_manager.h"
#include "marriage.h"
#include <cstdlib>
#include <memory>

namespace {
MYSQL_ROW FirstRow(SQLMsg* message)
{
    SQLResult* result = message ? message->Get() : nullptr;
    return result && result->iNumRows > 0 && result->pSQLResult
        ? mysql_fetch_row(result->pSQLResult) : nullptr;
}
}

int CHARACTER::ChangeEmpire(BYTE empire)
{
    if (GetEmpire() == empire)
        return 1;

    DBManager& db = DBManager::instance();
    const DWORD pid = GetPlayerID();
    const char* postfix = get_table_postfix();
    std::unique_ptr<SQLMsg> account(db.DirectQuery(
        "SELECT id, pid1, pid2, pid3, pid4 FROM player_index%s WHERE pid1=%lu OR pid2=%lu OR pid3=%lu OR pid4=%lu AND empire=%u",
        postfix, pid, pid, pid, pid, static_cast<unsigned int>(GetEmpire())));
    MYSQL_ROW row = FirstRow(account.get());
    if (!row)
        return 0;

    DWORD players[4];
    for (unsigned int index = 0; index < 4; ++index)
        players[index] = row[index + 1] ? std::atoi(row[index + 1]) : 0;
    account.reset();

    for (DWORD player : players)
    {
        std::unique_ptr<SQLMsg> membership(db.DirectQuery(
            "SELECT guild_id FROM guild_member%s WHERE pid=%lu", postfix, player));
        row = FirstRow(membership.get());
        if (row && row[0] && CGuildManager::instance().FindGuild(static_cast<DWORD>(std::atoi(row[0]))))
            return 2;
    }
    for (DWORD player : players)
        if (marriage::CManager::instance().IsEngagedOrMarried(player))
            return 3;

    std::unique_ptr<SQLMsg> update(db.DirectQuery(
        "UPDATE player_index%s SET empire=%u WHERE pid1=%lu OR pid2=%lu OR pid3=%lu OR pid4=%lu AND empire=%u",
        postfix, static_cast<unsigned int>(empire), pid, pid, pid, pid,
        static_cast<unsigned int>(GetEmpire())));
    const SQLResult* result = update ? update->Get() : nullptr;
    if (!result || result->iAffectedRows <= 0)
        return 0;
    SetChangeEmpireCount();
    return 999;
}

DWORD CHARACTER::GetAID()
{
    const DWORD pid = GetPlayerID();
    std::unique_ptr<SQLMsg> account(DBManager::instance().DirectQuery(
        "SELECT id FROM player_index%s WHERE pid1=%lu OR pid2=%lu OR pid3=%lu OR pid4=%lu AND empire=%u",
        get_table_postfix(), pid, pid, pid, pid, static_cast<unsigned int>(GetEmpire())));
    const MYSQL_ROW row = FirstRow(account.get());
    return row && row[0] ? static_cast<DWORD>(std::atoi(row[0])) : 0;
}

int CHARACTER::GetChangeEmpireCount()
{
    const DWORD account = GetAID();
    if (!account)
        return 0;
    std::unique_ptr<SQLMsg> message(DBManager::instance().DirectQuery(
        "SELECT change_count FROM change_empire WHERE account_id = %lu", account));
    const MYSQL_ROW row = FirstRow(message.get());
    return row && row[0] ? std::atoi(row[0]) : 0;
}

void CHARACTER::SetChangeEmpireCount()
{
    const DWORD account = GetAID();
    if (!account)
        return;

    const int count = GetChangeEmpireCount();
    DBManager& db = DBManager::instance();
    if (count == 0)
    {
        std::unique_ptr<SQLMsg> insert(db.DirectQuery("INSERT INTO change_empire VALUES(%lu, %d, NOW())", account, 1));
    }
    else
    {
        const int next = static_cast<int>(static_cast<unsigned int>(count) + 1);
        std::unique_ptr<SQLMsg> update(db.DirectQuery("UPDATE change_empire SET change_count=%d WHERE account_id=%lu",
            next, account));
    }
}
