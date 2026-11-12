#include "stdafx.h"
#include "SqlFormat.h"
#include "Marriage.h"
#include "Main.h"
#include "DBManager.h"
#include "ClientManager.h"

#include <cstdint>
#include <memory>

namespace marriage
{

    namespace
    {
        enum
        {
            MARRIAGE_ADD_HEADER = 150,
            MARRIAGE_UPDATE_HEADER = 151,
            MARRIAGE_REMOVE_HEADER = 152,
            WEDDING_READY_HEADER = 154,
            WEDDING_START_HEADER = 155,
            WEDDING_END_HEADER = 156
        };

#pragma pack(push, 1)
        struct MarriageAddPacket
        {
            std::uint32_t dwPID1;
            std::uint32_t dwPID2;
            std::int32_t tMarryTime;
            char szName1[25];
            char szName2[25];
        };

        struct WeddingPairPacket
        {
            std::uint32_t dwPID1;
            std::uint32_t dwPID2;
        };
#pragma pack(pop)
    }

    const DWORD WEDDING_LENGTH = 60 * 60; // sec
    bool operator < (const TWedding& lhs, const TWedding& rhs)
    {
        return lhs.dwTime < rhs.dwTime;
    }

    bool operator > (const TWedding& lhs, const TWedding& rhs)
    {
        return lhs.dwTime > rhs.dwTime;
    }

    bool operator > (const TWeddingInfo &lhs, const TWeddingInfo& rhs)
    {
        return lhs.dwTime > rhs.dwTime;
    }

    using namespace std;

    CManager::CManager()
    {
    }

    CManager::~CManager()
    {
    }

    bool CManager::Initialize()
    {
        char szQuery[1024];
        static const char* c_szDeleteQuery = "DELETE FROM marriage WHERE is_married = 0";
        static const char* c_szQuery =
            "SELECT pid1, pid2, love_point, time, is_married, p1.name, p2.name FROM marriage, player%s as p1, player%s as p2 WHERE p1.id = pid1 AND p2.id = pid2";

        dbsql::snprintf(szQuery, sizeof(szQuery), c_szQuery, GetTablePostfix(), GetTablePostfix());

        unique_ptr<SQLMsg> pmsg_delete(CDBManager::instance().DirectQuery(c_szDeleteQuery));
        unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));

        SQLResult * pRes = pmsg->Get();

        if (pRes->iNumRows > 0)
        {
            for (int uiRow = 0; uiRow != pRes->iNumRows; ++uiRow)
            {
                MYSQL_ROW row = mysql_fetch_row(pRes->pSQLResult);

                DWORD pid1 = static_cast<DWORD>(atoi(row[0]));
                DWORD pid2 = static_cast<DWORD>(atoi(row[1]));
                int love_point = atoi(row[2]);
                DWORD time = strtoul(row[3], NULL, 10);
                BYTE is_married = static_cast<BYTE>(atoi(row[4]));
                const char* name1 = row[5];
                const char* name2 = row[6];

                TMarriage* pMarriage = new TMarriage(pid1, pid2, love_point, time, is_married, name1, name2);
                m_Marriages.insert(pMarriage);
                m_MarriageByPID.insert(make_pair(pid1, pMarriage));
                m_MarriageByPID.insert(make_pair(pid2, pMarriage));
            }
        }
        return true;
    }

    TMarriage* CManager::Get(DWORD dwPlayerID)
    {
        std::map<DWORD, TMarriage*>::iterator it = m_MarriageByPID.find(dwPlayerID);

        if (it != m_MarriageByPID.end())
            return it->second;

        return NULL;
    }

    void Align(DWORD& rPID1, DWORD& rPID2)
    {
        if (rPID1 > rPID2)
            std::swap(rPID1, rPID2);
    }

    void CManager::Add(DWORD dwPID1, DWORD dwPID2, const char* szName1, const char* szName2)
    {
        DWORD now = CClientManager::instance().GetCurrentTime();
        if (IsMarried(dwPID1) || IsMarried(dwPID2))
        {
            sys_err("cannot marry already married character. %d - %d", static_cast<int>(dwPID1), static_cast<int>(dwPID2));
            return;
        }

        Align(dwPID1, dwPID2);

        char szQuery[513];
        dbsql::snprintf(szQuery, 512, "INSERT INTO marriage(pid1, pid2, love_point, time) VALUES (%lu, %lu, 0, %lu)", dwPID1, dwPID2, now);

        unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));

        SQLResult* res = pmsg->Get();
        if (res->iAffectedRows <= 0)
        {
            sys_err("cannot insert marriage");
            return;
        }

        sys_log(0, "MARRIAGE ADD %lu %lu", dwPID1, dwPID2);

        TMarriage* pMarriage = new TMarriage(dwPID1, dwPID2, 0, now, 0, szName1, szName2);
        m_Marriages.insert(pMarriage);
        m_MarriageByPID.insert(make_pair(dwPID1, pMarriage));
        m_MarriageByPID.insert(make_pair(dwPID2, pMarriage));

        MarriageAddPacket p;
        p.dwPID1 = dwPID1;
        p.dwPID2 = dwPID2;
        p.tMarryTime = static_cast<std::int32_t>(now);
        strncpy(p.szName1, szName1, 24);
        p.szName1[24] = '\0';
        strncpy(p.szName2, szName2, 24);
        p.szName2[24] = '\0';
        CClientManager::instance().ForwardPacket(MARRIAGE_ADD_HEADER, &p, sizeof(p));
    }

    void CManager::Update(DWORD dwPID1, DWORD dwPID2, long lLovePoint, BYTE byMarried)
    {
        TMarriage* pMarriage = Get(dwPID1);
        if (!pMarriage || pMarriage->GetOther(dwPID1) != dwPID2)
        {
            sys_err("not under marriage : %lu %lu", dwPID1, dwPID2);
            return;
        }

        Align(dwPID1, dwPID2);

        char szQuery[513];
        dbsql::snprintf(szQuery, 512, "UPDATE marriage SET love_point = %ld, is_married = %d WHERE pid1 = %lu AND pid2 = %lu",
                lLovePoint, byMarried, pMarriage->pid1, pMarriage->pid2);

        unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));

        SQLResult* res = pmsg->Get();
        if (res->iAffectedRows <= 0)
        {
            sys_err("cannot update marriage : PID:%lu %lu", dwPID1, dwPID2);
            return;
        }

        sys_log(0, "MARRIAGE UPDATE PID:%lu %lu LP:%lu ST:%d", dwPID1, dwPID2, static_cast<unsigned long>(lLovePoint), byMarried);
        pMarriage->love_point = static_cast<int>(lLovePoint);
        pMarriage->is_married = byMarried;

        TPacketMarriageUpdate p;
        p.dwPID1 = dwPID1;
        p.dwPID2 = dwPID2;
        p.lLovePoint = pMarriage->love_point;
        p.byMarried = pMarriage->is_married;
        CClientManager::instance().ForwardPacket(MARRIAGE_UPDATE_HEADER, &p, sizeof(p));
    }

    void CManager::Remove(DWORD dwPID1, DWORD dwPID2)
    {
        TMarriage* pMarriage = Get(dwPID1);

        if (pMarriage)
        {
            sys_log(0, "Break Marriage pid1 %d pid2 %d Other %d",
                    static_cast<int>(dwPID1), static_cast<int>(dwPID2), static_cast<int>(pMarriage->GetOther(dwPID1)));
        }
        if (!pMarriage || pMarriage->GetOther(dwPID1) != dwPID2)
        {
            std::map<DWORD, TMarriage*>::iterator it = m_MarriageByPID.begin();

            for (; it != m_MarriageByPID.end(); ++it)
            {
                sys_log(0, "Marriage List pid1 %d pid2 %d",
                        static_cast<int>(it->second->pid1), static_cast<int>(it->second->pid2));
            }
            sys_err("not under marriage : PID:%lu %lu", dwPID1, dwPID2);
            return;
        }

        Align(dwPID1, dwPID2);

        char szQuery[513];
        dbsql::snprintf(szQuery, 512, "DELETE FROM marriage WHERE pid1 = %lu AND pid2 = %lu", dwPID1, dwPID2);

        unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));

        SQLResult* res = pmsg->Get();
        if (res->iAffectedRows <= 0)
        {
            sys_err("cannot delete marriage : PID:%lu %lu", dwPID1, dwPID2);
            return;
        }

        sys_log(0, "MARRIAGE REMOVE PID:%lu %lu", dwPID1, dwPID2);

        m_Marriages.erase(pMarriage);
        m_MarriageByPID.erase(dwPID1);
        m_MarriageByPID.erase(dwPID2);

        WeddingPairPacket p;
        p.dwPID1 = dwPID1;
        p.dwPID2 = dwPID2;
        CClientManager::instance().ForwardPacket(MARRIAGE_REMOVE_HEADER, &p, sizeof(p));

        delete pMarriage;
    }

    void CManager::EngageToMarriage(DWORD dwPID1, DWORD dwPID2)
    {
        TMarriage* pMarriage = Get(dwPID1);
        if (!pMarriage || pMarriage->GetOther(dwPID1) != dwPID2)
        {
            sys_err("not under marriage : PID:%lu %lu", dwPID1, dwPID2);
            return;
        }

        if (pMarriage->is_married)
        {
            sys_err("already married, cannot change engage to marry : PID:%lu %lu", dwPID1, dwPID2);
            return;
        }

        Align(dwPID1, dwPID2);

        char szQuery[513];
        dbsql::snprintf(szQuery, 512, "UPDATE marriage SET is_married = 1 WHERE pid1 = %lu AND pid2 = %lu",
                pMarriage->pid1, pMarriage->pid2);

        unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));

        SQLResult* res = pmsg->Get();
        if (res->iAffectedRows <= 0)
        {
            sys_err("cannot change engage to marriage : PID:%lu %lu", dwPID1, dwPID2);
            return;
        }

        sys_log(0, "MARRIAGE ENGAGE->MARRIAGE PID:%lu %lu", dwPID1, dwPID2);
        pMarriage->is_married = 1;

        TPacketMarriageUpdate p;
        p.dwPID1 = dwPID1;
        p.dwPID2 = dwPID2;
        p.lLovePoint = pMarriage->love_point;
        p.byMarried = pMarriage->is_married;
        CClientManager::instance().ForwardPacket(MARRIAGE_UPDATE_HEADER, &p, sizeof(p));
    }

    void CManager::OnSetup(CPeer* peer)
    {

        for (std::set<TMarriage*>::iterator it = m_Marriages.begin(); it != m_Marriages.end(); ++it)
        {
            TMarriage* pMarriage = *it;

            {
                MarriageAddPacket p;
                p.dwPID1 = pMarriage->pid1;
                p.dwPID2 = pMarriage->pid2;
                p.tMarryTime = static_cast<std::int32_t>(pMarriage->time);
                strncpy(p.szName1, pMarriage->name1.c_str(), 24);
                p.szName1[24] = '\0';
                strncpy(p.szName2, pMarriage->name2.c_str(), 24);
                p.szName2[24] = '\0';
                peer->EncodeHeader(MARRIAGE_ADD_HEADER, 0, sizeof(p));
                peer->Encode(&p, sizeof(p));
            }

            {
                TPacketMarriageUpdate p;
                p.dwPID1 = pMarriage->pid1;
                p.dwPID2 = pMarriage->pid2;
                p.lLovePoint = pMarriage->love_point;
                p.byMarried = pMarriage->is_married;
                peer->EncodeHeader(MARRIAGE_UPDATE_HEADER, 0, sizeof(p));
                peer->Encode(&p, sizeof(p));
            }
        }

        for (std::map<std::pair<DWORD, DWORD>, TWedding>::iterator it = m_mapRunningWedding.begin(); it != m_mapRunningWedding.end(); ++it)
        {
            const TWedding& t = it->second;

            TPacketWeddingReady p;
            p.dwPID1 = t.dwPID1;
            p.dwPID2 = t.dwPID2;
            p.dwMapIndex = t.dwMapIndex;

            peer->EncodeHeader(WEDDING_READY_HEADER, 0, sizeof(p));
            peer->Encode(&p, sizeof(p));

            // Peer setup sends the ready payload for both headers.
            peer->EncodeHeader(WEDDING_START_HEADER, 0, sizeof(p));
            peer->Encode(&p, sizeof(p));
        }
    }

    void CManager::ReadyWedding(DWORD dwMapIndex, DWORD dwPID1, DWORD dwPID2)
    {
        DWORD dwStartTime = CClientManager::instance().GetCurrentTime();
        m_pqWeddingStart.push(TWedding(dwStartTime + 5, dwMapIndex, dwPID1, dwPID2));
    }

    void CManager::EndWedding(DWORD dwPID1, DWORD dwPID2)
    {
        std::map<std::pair<DWORD, DWORD>, TWedding>::iterator it = m_mapRunningWedding.find(make_pair(dwPID1, dwPID2));
        if (it == m_mapRunningWedding.end())
        {
            sys_err("try to end wedding %lu %lu", dwPID1, dwPID2);
            return;
        }

        TWedding& w = it->second;

        WeddingPairPacket p;
        p.dwPID1 = w.dwPID1;
        p.dwPID2 = w.dwPID2;
        CClientManager::instance().ForwardPacket(WEDDING_END_HEADER, &p, sizeof(p));
        m_mapRunningWedding.erase(it);
    }

    void CManager::Update()
    {
        DWORD now = CClientManager::instance().GetCurrentTime();

        if (!m_pqWeddingEnd.empty())
        {
            while (!m_pqWeddingEnd.empty() && m_pqWeddingEnd.top().dwTime <= now)
            {
                TWeddingInfo wi = m_pqWeddingEnd.top();
                m_pqWeddingEnd.pop();

                std::map<std::pair<DWORD, DWORD>, TWedding>::iterator it = m_mapRunningWedding.find(make_pair(wi.dwPID1, wi.dwPID2));
                if (it == m_mapRunningWedding.end())
                    continue;

                TWedding& w = it->second;

                WeddingPairPacket p;
                p.dwPID1 = w.dwPID1;
                p.dwPID2 = w.dwPID2;
                CClientManager::instance().ForwardPacket(WEDDING_END_HEADER, &p, sizeof(p));
                m_mapRunningWedding.erase(it);

                // Preserve the original lookup through the erased wedding entry.
                std::map<DWORD, TMarriage*>::iterator it_marriage = m_MarriageByPID.find(w.dwPID1);

                if (it_marriage != m_MarriageByPID.end())
                {
                    TMarriage* pMarriage = it_marriage->second;
                    if (!pMarriage->is_married)
                    {
                        Remove(pMarriage->pid1, pMarriage->pid2);
                    }
                }
            }
        }
        if (!m_pqWeddingStart.empty())
        {
            while (!m_pqWeddingStart.empty() && m_pqWeddingStart.top().dwTime <= now)
            {
                TWedding w = m_pqWeddingStart.top();
                m_pqWeddingStart.pop();

                WeddingPairPacket p;
                p.dwPID1 = w.dwPID1;
                p.dwPID2 = w.dwPID2;
                CClientManager::instance().ForwardPacket(WEDDING_START_HEADER, &p, sizeof(p));

                w.dwTime += WEDDING_LENGTH;
                m_pqWeddingEnd.push(TWeddingInfo(w.dwTime, w.dwPID1, w.dwPID2));
                m_mapRunningWedding.insert(make_pair(make_pair(w.dwPID1, w.dwPID2), w));
            }
        }
    }
}
