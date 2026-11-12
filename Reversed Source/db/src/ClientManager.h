#ifndef __INC_CLIENTMANAGER_H__
#define __INC_CLIENTMANAGER_H__

#include <cstdint>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>
#include "../../common/building.h"
#include "../../common/length.h"
#include "../../common/singleton.h"
#include "../../libthecore/include/fdwatch.h"
#include "DBProtocol.h"
#include "NetBase.h"
#include "Peer.h"
#include "LoginData.h"
#include "DBManager.h"

class CPlayerTableCache;
class CItemCache;
class CItemPriceListTableCache;

class CClientManager : public CNetBase, public singleton<CClientManager>
{
public:
    typedef std::list<CPeer*> TPeerList;
    typedef std::map<DWORD, CPlayerTableCache*> TPlayerTableCacheMap;
    typedef std::map<DWORD, CItemCache*> TItemCacheMap;
    typedef std::set<CItemCache*> TItemCacheSet;
    typedef std::map<DWORD, TItemCacheSet*> TItemCacheSetPtrMap;
    typedef std::map<DWORD, CItemPriceListTableCache*> TItemPriceListCacheMap;

    CClientManager();
    ~CClientManager();

    bool Initialize();
    void MainLoop();
    void Quit();
    time_t GetCurrentTime();
    void SetPlayerIDStart(int start);
    DWORD GainItemID();
    DWORD GetItemID();
    void SendGuildSkillUsable(DWORD guildID, DWORD skillVnum, bool usable);
    void SendNotice(const char* format, ...);
    void SetCacheFlushCountLimit(int limit);
    CPeer* GetPeer(DWORD handle);
    CPeer* GetAnyPeer();

    CLoginData* GetLoginData(DWORD key);
    CLoginData* GetLoginDataByLogin(const char* login);
    CLoginData* GetLoginDataByAID(DWORD accountID);
    CPlayerTableCache* GetPlayerCache(DWORD playerID);

    void ForwardPacket(BYTE header, const void* data, int size, BYTE channel = 0, CPeer* except = NULL);

    template <class Function>
    Function for_each_peer(Function function)
    {
        for (TPeerList::iterator it = m_peerList.begin(); it != m_peerList.end(); ++it)
            function(*it);
        return function;
    }

private:
    struct ClientHandleInfo
    {
        DWORD dwHandle;
        DWORD account_id;
        DWORD player_id;
        BYTE account_index;
        char login[LOGIN_MAX_LEN + 1];
        char safebox_password[7];
        char ip[MAX_HOST_LENGTH + 1];
        TAccountTable* pAccountTable;
        TSafeboxTable* pSafebox;

        explicit ClientHandleInfo(DWORD handle, DWORD pid = 0)
            : dwHandle(handle), account_id(0), player_id(pid), account_index(0),
              login{}, safebox_password{}, ip{}, pAccountTable(nullptr), pSafebox(nullptr)
        {
        }

        ~ClientHandleInfo()
        {
            if (pSafebox)
            {
                delete pSafebox;
                pSafebox = NULL;
            }
        }
    };

    struct TPartyInfo
    {
        BYTE bRole;
        BYTE bLevel;

        TPartyInfo() : bRole(0), bLevel(0) {}
    };

    struct tag_LogoutPlayerInfo
    {
        int pid;
        time_t time;
    };

    typedef std::map<DWORD, TPartyInfo> TPartyMember;
    typedef std::map<DWORD, TPartyMember> TPartyMap;
    typedef std::map<std::string, long> TEventFlagMap;
    typedef std::map<std::string, CLoginData*> TLogonAccountMap;

    bool InsertLogonAccount(const char* login, DWORD handle, const char* ip);
    bool DeleteLogonAccount(const char* login, DWORD handle);
    bool FindLogonAccount(const char* login);
    void QUERY_LOGIN_BY_KEY(CPeer*, DWORD handle, TPacketGDLoginByKey*);
    void RESULT_LOGIN_BY_KEY(CPeer* peer, SQLMsg* message);
    void RESULT_PLAYER_INDEX_CREATE(CPeer* peer, SQLMsg* message);
    void QUERY_LOGOUT(CPeer*, DWORD handle, const char* payload);
    void RESULT_LOGIN(CPeer* peer, SQLMsg* message);
    void QUERY_CHANGE_NAME(CPeer*, DWORD handle, TPacketGDChangeName*);
    void SendLoginToBilling(CLoginData*, bool);
    void InsertLogoutPlayer(int pid);

    void QUERY_PARTY_CREATE(CPeer*, TPacketPartyCreate*);
    void QUERY_PARTY_DELETE(CPeer*, TPacketPartyDelete*);
    void QUERY_PARTY_ADD(CPeer*, TPacketPartyAdd*);
    void QUERY_PARTY_REMOVE(CPeer*, TPacketPartyRemove*);
    void QUERY_PARTY_STATE_CHANGE(CPeer*, TPacketPartyStateChange*);
    void QUERY_PARTY_SET_MEMBER_LEVEL(CPeer*, TPacketPartySetMemberLevel*);
    void GuildCreate(CPeer*, DWORD);
    void GuildChangeGrade(CPeer*, TPacketGuild*);
    void GuildAddMember(CPeer*, TPacketGDGuildAddMember*);
    void GuildRemoveMember(CPeer*, TPacketGuild*);
    void GuildSkillUpdate(CPeer*, TPacketGuildSkillUpdate*);
    void GuildExpUpdate(CPeer*, TPacketGuildExpUpdate*);
    void GuildChangeMemberData(CPeer*, TPacketGuildChangeMemberData*);
    void GuildDisband(CPeer*, TPacketGuild*);
    void GuildWar(CPeer*, TPacketGuildWar*);
    void GuildWarScore(CPeer*, TPacketGuildWarScore*);
    void GuildChangeLadderPoint(TPacketGuildLadderPoint*);
    void GuildUseSkill(TPacketGuildUseSkill*);

    void PutPlayerCache(TPlayerTable*);
    void QUERY_PLAYER_LOAD(CPeer*, DWORD, TPlayerLoadPacket*);
    void DeleteLogoutPlayer(int pid);
    TItemCacheSet* GetItemCacheSet(DWORD owner);
    CItemCache* GetItemCache(DWORD id);
    void QUERY_PLAYER_SAVE(CPeer*, DWORD, TPlayerTable*);
    void RESULT_PLAYER_LOAD(CPeer*, MYSQL_RES*, ClientHandleInfo*);
    void RESULT_ITEM_LOAD(CPeer*, MYSQL_RES*, DWORD, DWORD);
    void RESULT_AFFECT_LOAD(CPeer*, MYSQL_RES*, DWORD);
    void RESULT_QUEST_LOAD(CPeer*, MYSQL_RES*, DWORD, DWORD);
    void RESULT_COMPOSITE_PLAYER(CPeer*, SQLMsg*, DWORD);
    void CreateItemCacheSet(DWORD);
    void PutItemCache(TPlayerItem*, bool);
    void QUERY_ADD_AFFECT(CPeer*, TPacketGDAddAffect*);
    void QUERY_REMOVE_AFFECT(CPeer*, TPacketGDRemoveAffect*);

    void AddGuildPriv(TPacketGiveGuildPriv*);
    void AddEmpirePriv(TPacketGiveEmpirePriv*);
    void AddCharacterPriv(TPacketGiveCharacterPriv*);
    void MoneyLog(TPacketMoneyLog*);
    void GuildDepositMoney(TPacketGDGuildMoney*);
    void GuildWithdrawMoney(CPeer*, TPacketGDGuildMoney*);
    void GuildWithdrawMoneyGiveReply(TPacketGDGuildMoneyWithdrawGiveReply*);
    void GuildWarBet(TPacketGDGuildWarBet*);
    void MarriageAdd(TPacketMarriageAdd*);
    void MarriageUpdate(TPacketMarriageUpdate*);
    void MarriageRemove(TPacketMarriageRemove*);
    void WeddingRequest(TPacketWeddingRequest*);
    void WeddingReady(TPacketWeddingReady*);
    void WeddingEnd(TPacketWeddingEnd*);
    void BreakMarriage(CPeer*, void*);
    void BlockException(TPacketBlockException*);
    void QUERY_FLUSH_CACHE(CPeer*, const char*);
    void QUERY_ITEM_FLUSH(CPeer*, const char*);
    void QUERY_ITEM_SAVE(CPeer*, const char*);
    void QUERY_ITEM_DESTROY(CPeer*, const char*);
    void QUERY_QUEST_SAVE(CPeer*, TQuestTable*, DWORD);
    void InsertLoginData(CLoginData*);
    void DeleteLoginData(CLoginData*);
    void QUERY_AUTH_LOGIN(CPeer*, DWORD, TPacketGDAuthLogin*);
    void BillingExpire(TPacketBillingExpire*);
    void BillingCheck(const char*);
    void SendAllLoginToBilling();
    void SendAllGuildSkillRechargePacket();
    void QUERY_EMPIRE_SELECT(CPeer*, DWORD, TEmpireSelectPacket*);
    void QUERY_SETUP(CPeer*, DWORD, const char*);
    void QUERY_BOOT(CPeer*, TPacketGDBoot*);
    void QUERY_PLAYER_COUNT(CPeer*, TPlayerCountPacket*);
    bool __IsItemRangeCollision(CPeer*, DWORD start, DWORD end);
    DWORD __GetMaxItemID(CPeer*, DWORD start, DWORD end);
    void SendPartyOnSetup(CPeer*);
    bool __GetAdminInfo(const char*, std::vector<tAdminInfo>*);
    bool __GetHostInfo(std::vector<std::string>*);
    void CreateObject(TPacketGDCreateObject*);
    void DeleteObject(DWORD);
    void UpdateLand(std::uint32_t const* fields);
    void ReloadAdmin(CPeer*, TPacketReloadAdmin*);
    void QUERY_RELOAD_PROTO();
    void QUERY_SMS(CPeer*, TPacketGDSMS*);
    void VCard(TPacketGDVCard*);
    void BlockChat(TPacketBlockChat*);
    void QUERY_SAFEBOX_LOAD(CPeer*, DWORD, TSafeboxLoadPacket*, bool);
    void QUERY_SAFEBOX_CHANGE_SIZE(CPeer*, DWORD, TSafeboxChangeSizePacket*);
    void QUERY_SAFEBOX_CHANGE_PASSWORD(CPeer*, DWORD, TSafeboxChangePasswordPacket*);
    void QUERY_SAFEBOX_SAVE(CPeer*, TSafeboxTable*);
    CItemPriceListTableCache* GetItemPriceListCache(DWORD);
    void PutItemPriceListCache(const TItemPriceListTable*);
    void MyshopPricelistUpdate(const TPacketMyshopPricelistHeader*);
    void MyshopPricelistRequest(CPeer*, DWORD, DWORD);
    void LoadEventFlag();
    void SetEventFlag(TPacketSetEventFlag*);
    void SendEventFlagsOnSetup(CPeer*);

    DWORD GetUserCount();
    void SendTime();
    void VCardProcess();
    void AddPeer(socket_t);
    void RemovePeer(CPeer*);
    void ProcessPackets(CPeer*);
    void UpdatePlayerCache();
    void UpdateItemCache();
    void UpdateLogoutPlayer();
    void UpdateItemPriceListCache();
    void FlushPlayerCacheSet(int);
    void FlushItemCacheSet(DWORD);
    void UpdateItemCacheSet(int);
    bool DeleteItemCache(DWORD);
    void RESULT_SAFEBOX_LOAD(CPeer*, SQLMsg*);
    void RESULT_SAFEBOX_CHANGE_SIZE(CPeer*, SQLMsg*);
    void RESULT_SAFEBOX_CHANGE_PASSWORD(CPeer*, SQLMsg*);
    void RESULT_SAFEBOX_CHANGE_PASSWORD_SECOND(CPeer*, SQLMsg*);
    void QUERY_HIGHSCORE_REGISTER(CPeer*, TPacketGDHighscore*);
    void RESULT_HIGHSCORE_REGISTER(CPeer*, SQLMsg*);
    void __QUERY_PLAYER_CREATE(CPeer*, DWORD, TPlayerCreatePacket*);
    void __QUERY_PLAYER_DELETE(CPeer*, DWORD, TPlayerDeletePacket*);
    void __RESULT_PLAYER_DELETE(CPeer*, SQLMsg*);
    void RESULT_PRICELIST_LOAD(CPeer*, SQLMsg*);
    void RESULT_PRICELIST_LOAD_FOR_UPDATE(SQLMsg*);
    int Process();
    int AnalyzeQueryResult(SQLMsg*);
    void Destroy();
    bool InitializeLocalization();
    bool InitializeNowItemID();
    bool InitializeTables();
    bool InitializeMobTable();
    bool InitializeItemTable();
    bool InitializeQuestItemTable();
    bool InitializeSkillTable();
    bool InitializeRefineTable();
    bool InitializeShopTable();
    bool InitializeBanwordTable();
    bool InitializeItemAttrTable();
    bool InitializeItemRareTable();
    bool InitializeLandTable();
    bool InitializeObjectProto();
    bool InitializeObjectTable();

    int m_looping;
    std::set<std::string> m_allowedPeerAddresses;
    std::string m_authPeerAddress;
    socket_t m_fdAccept;
    TPeerList m_peerList;
    CPeer* m_pkAuthPeer;
    std::map<DWORD, CLoginData*> m_map_pkLoginData;
    std::map<std::string, CLoginData*> m_map_pkLoginDataByLogin;
    std::map<DWORD, CLoginData*> m_map_pkLoginDataByAID;
    int m_iPlayerIDStart;
    int m_iPlayerDeleteLevelLimit;
    bool m_bChinaEventServer;
    TLogonAccountMap m_map_kLogonAccount;
    std::vector<TMobTable> m_vec_mobTable;
    std::vector<TItemTable> m_vec_itemTable;
    std::map<DWORD, TItemTable*> m_map_itemTableByVnum;
    int m_iShopTableSize;
    TShopTable* m_pShopTable;
    int m_iRefineTableSize;
    TRefineTable* m_pRefineTable;
    std::vector<TSkillTable> m_vec_skillTable;
    std::vector<TBanwordTable> m_vec_banwordTable;
    std::vector<TItemAttrTable> m_vec_itemAttrTable;
    std::vector<TItemAttrTable> m_vec_itemRareTable;
    std::vector<building::TLand> m_vec_kLandTable;
    std::vector<building::TObjectProto> m_vec_kObjectProto;
    std::map<DWORD, building::TObject*> m_map_pkObjectTable;
    std::queue<TPacketGDVCard> m_queue_vcard;
    bool m_bShutdowned;
    TPlayerTableCacheMap m_map_playerCache;
    TItemCacheMap m_map_itemCache;
    TItemCacheSetPtrMap m_map_pkItemCacheSetPtr;
    TItemPriceListCacheMap m_mapItemPriceListCache;
    std::map<BYTE, TPartyMap> m_map_pkChannelParty;
    TEventFlagMap m_map_lEventFlag;
    BYTE m_bLastHeader;
    int m_iCacheFlushCount;
    int m_iCacheFlushCountLimit;
    DWORD m_dwItemIDStart;
    DWORD m_dwItemIDStop;
    DWORD m_dwNowItemID;
    std::vector<tLocale> m_vec_Locale;
    std::map<int, tag_LogoutPlayerInfo*> m_map_logout;
};
#endif
