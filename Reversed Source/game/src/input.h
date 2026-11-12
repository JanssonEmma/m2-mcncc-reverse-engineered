// Partial old input API: base frame dispatch and closed processor recovered.
#ifndef MCNCC_GAME_INPUT_H
#define MCNCC_GAME_INPUT_H
#include "packet_info.h"
#include <netinet/in.h>
#include <cstddef>
class DESC;
class CHARACTER;
void LoginFailure(DESC* desc, const char* status);
class CInputProcessor {
public:
    CInputProcessor();
    virtual ~CInputProcessor();
    virtual bool Process(DESC* desc, const void* data, int bytes, int& consumed);
    virtual BYTE GetType()=0;
    void BindPacketInfo(CPacketInfo* info);
    void Pong(DESC* desc);
    void CRCReport(CHARACTER* character, const char* data);
    void Version(CHARACTER* character, const char* data);
    void Handshake(DESC* desc, const char* data);
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data)=0;
    CPacketInfo* m_pPacketInfo;
    int m_iBufferLeft;
};
class CInputClose : public CInputProcessor {
public:
    virtual BYTE GetType();
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data);
};
// Complete old concrete processor data/virtual interface; Analyze behavior is pending.
// Append before input.h's final #endif. Default constructors for data-less
// Login/Main/Dead remain implicit, matching the target's lack of emitted symbols.
class CInputHandshake : public CInputProcessor {
public:
    CInputHandshake();
    virtual ~CInputHandshake();
    virtual BYTE GetType();
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data);
    void GuildMarkLogin(DESC* desc, const char* data);
    CPacketInfo* m_pMainPacketInfo;
};
class CInputLogin : public CInputProcessor {
public:
    virtual ~CInputLogin();
    virtual BYTE GetType();
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data);
    void Login(DESC* desc, const char* data);
    void LoginByKey(DESC* desc, const char* data);
    void CharacterCreate(DESC* desc, const char* data);
    void GuildMarkCRCList(DESC* desc, const char* data);
    void GuildMarkIDXList(DESC* desc, const char* data);
    int GuildMarkUpload(DESC* desc, const char* data, int bytes);
    int GuildSymbolUpload(DESC* desc, const char* data, int bytes);
    void GuildSymbolCRC(DESC* desc, const char* data);
    void CharacterSelect(DESC* desc, const char* data);
    void Entergame(DESC* desc, const char* data);
    void CharacterDelete(DESC* desc, const char* data);
    void Empire(DESC* desc, const char* data);
    void ChangeName(DESC* desc, const char* data);
};
class CInputMain : public CInputProcessor {
public:
    virtual ~CInputMain();
    void Exchange(CHARACTER* ch, const char* data);
    void OnClick(CHARACTER* character, const char* data);
    void PartyInvite(CHARACTER* character, const char* data);
    void PartyInviteAnswer(CHARACTER* character, const char* data);
    void PartyRemove(CHARACTER* character, const char* data);
    void PartySetState(CHARACTER* character, const char* data);
    void PartyUseSkill(CHARACTER* character, const char* data);
    void PartyParameter(CHARACTER* character, const char* data);
    void Fishing(CHARACTER* character, const char* data);
    void Shoot(CHARACTER* character, const char* data);
    void UseSkill(CHARACTER* character, const char* data);
    void FlyTarget(CHARACTER* character, const char* data, BYTE header);
    void Attack(CHARACTER* character, const char* data);
    void Target(CHARACTER* character, const char* data);
    void ItemMove(CHARACTER* character, const char* data);
    void ItemPickup(CHARACTER* character, const char* data);
    void QuickslotAdd(CHARACTER* character, const char* data);
    void QuickslotDelete(CHARACTER* character, const char* data);
    void QuickslotSwap(CHARACTER* character, const char* data);
    void Position(CHARACTER* character, const char* data);
    void ItemDrop(CHARACTER* character, const char* data);
    void ItemDrop2(CHARACTER* character, const char* data);
    void QuestInputString(CHARACTER* character, const char* data);
    void QuestConfirm(CHARACTER* character, const char* data);
    void ScriptButton(CHARACTER* character, const char* data);
    void ScriptSelectItem(CHARACTER* character, const char* data);
    void ScriptAnswer(CHARACTER* character, const char* data);
    void Warp(CHARACTER* character, const char* data);
    void ItemUse(CHARACTER* character, const char* data);
    int Shop(CHARACTER* character, const char* data, int bytes);
    void Hack(CHARACTER* character, const char* data);
    int Guild(CHARACTER* character, const char* data, int bytes);
    void AnswerMakeGuild(CHARACTER* character, const char* data);
    void SafeboxCheckin(CHARACTER* character, const char* data);
    void SafeboxCheckout(CHARACTER* character, const char* data, bool mall);
    void SafeboxItemMove(CHARACTER* character, const char* data);
    int MyShop(CHARACTER* character, const char* data, int bytes);
    void ItemToItem(CHARACTER* character, const char* data);
    void Roulette(CHARACTER* character, const char* data);
    void ItemGive(CHARACTER* character, const char* data);
    void Refine(CHARACTER* character, const char* data);
    int Chat(CHARACTER* character, const char* data, int bytes);
    void Move(CHARACTER* character, const char* data);
    int Whisper(CHARACTER* character, const char* data, int bytes);
    int Messenger(CHARACTER* character, const char* data, int bytes);
    int SMS(CHARACTER* character, const char* data, int bytes);
    int SyncPosition(CHARACTER* character, const char* data, int bytes);
    virtual BYTE GetType();
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data);
};
class CInputDead : public CInputMain {
public:
    virtual ~CInputDead();
    virtual BYTE GetType();
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data);
};
class CInputAuth : public CInputProcessor {
public:
    CInputAuth();
    virtual ~CInputAuth();
    void PasspodAnswer(DESC* desc, const char* data);
    void Login(DESC* desc, const char* data);
    void MatrixCard(DESC* desc, const char* data);
    void MatrixAnswer(DESC* desc, const char* data);
    void RunupMatrixAnswer(DESC* desc, const char* data);
    virtual BYTE GetType();
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data);
};

class CInputP2P : public CInputProcessor {
public:
    CInputP2P();
    virtual ~CInputP2P();
    virtual BYTE GetType();
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data);
    void Login(DESC* desc, const char* data);
    void Logout(DESC* desc, const char* data);
    void Setup(DESC* desc, const char* data);
    int Relay(DESC* desc, const char* data, int bytes);
    int Notice(DESC* desc, const char* data, int bytes);
    int Guild(DESC* desc, const char* data, int bytes);
    void Disconnect(const char* data);
    void Shout(const char* data);
    void MessengerAdd(const char* data);
    void MessengerRemove(const char* data);
    void FindPosition(DESC* desc, const char* data);
    void WarpCharacter(const char* data);
    void MessengerMobile(const char* data);
    void GuildWarZoneMapIndex(const char* data);
    void Transfer(const char* data);
    void XmasWarpSanta(const char* data);
    void XmasWarpSantaReply(const char* data);
    void LoginPing(DESC* desc, const char* data);
    void BlockChat(const char* data);
    void ForkedMapInfo(const char* data);
};

struct TPacketCGLogin3;
bool NetmarbleJP_RequestLogin(DESC* desc, const TPacketCGLogin3& request);
void NetmarbleJP_QueryAccount(DWORD key, TPacketCGLogin3* request);

// These internal request markers never arrive from the 0x6f client packet.
enum : BYTE { NETMARBLE_ACCOUNT_LOOKUP = 0, NETMARBLE_ACCOUNT_CREATED = 1 };

class CInputNetmarble : public CInputProcessor {
public:
    bool Process(DESC* desc, const void* data, int bytes, int& consumed) override;
    BYTE GetType() override;
    void SendFirstPacket(DESC* desc);
    void Reset();
protected:
    int Analyze(DESC* desc, BYTE header, const char* data) override;
};

class CInputTeen : public CInputProcessor {
public:
    bool Process(DESC* desc, const void* data, int bytes, int& consumed) override;
    BYTE GetType() override;
    void SetStep(int step);
protected:
    int Analyze(DESC* desc, BYTE header, const char* data) override;
private:
    bool ProcessHandshake(DESC* desc, const void* data, int bytes, int& consumed);
    bool ProcessMain(DESC* desc, const void* data, int bytes, int& consumed);
    int m_step = 0;
};

class CInputUDP : public CInputProcessor {
public:
    CInputUDP();
    virtual ~CInputUDP();
    virtual bool Process(DESC* desc, const void* data, int bytes, int& consumed);
    virtual BYTE GetType();
    void SetSockAddr(sockaddr_in& address) { m_SockAddr = address; }
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data);
    void Handshake(DESC* desc, const char* data);
    void StateChecker(const char* data);
    sockaddr_in m_SockAddr;
};

class CInputDB : public CInputProcessor {
public:
    ~CInputDB() override;
    bool Process(DESC* desc, const void* data, int bytes, int& consumed) override;
    BYTE GetType() override;

protected:
    int Analyze(DESC* desc, BYTE header, const char* data) override;
    void Boot(const char* data);
    bool ItemLoad(DESC* desc, const char* data);
    bool QuestLoad(DESC* desc, const char* data);
    bool ReloadProto(const char* data);
    bool VCard(const char* data);
    bool BillingRepair(const char* data);
    bool BillingLogin(const char* data);
    bool ReloadAdmin(const char* data);
    bool AffectLoad(DESC* desc, const char* data);
    void EmpireSelect(DESC* desc, const char* data);
    void ChangeName(DESC* desc, const char* data);
    bool SafeboxLoad(DESC* desc, const char* data);
    bool MallLoad(DESC* desc, const char* data);
    bool SafeboxChangeSize(DESC* desc, const char* data);
    void SafeboxWrongPassword(DESC* desc);
    bool SafeboxChangePasswordAnswer(DESC* desc, const char* data);
    bool LoadStorage(DESC* desc, const char* data, bool mall);
    void LoginSuccess(DWORD handle, const char* data);
    void PlayerLoad(DESC* desc, const char* data);
    void PlayerCreateSuccess(DESC* desc, const char* data);
    void PlayerCreateFailure(DESC* desc, BYTE type);
    void PlayerDeleteSuccess(DESC* desc, const char* data);
    void PlayerDeleteFail(DESC* desc);

private:
    void GuildAddMember(const char* data);
    void GuildChangeMemberData(const char* data);
    void GuildRemoveMember(const char* data);
    void GuildDisband(const char* data);
    bool MarriageAdd(const char* data);
    bool MarriageUpdate(const char* data);
    bool MarriageRemove(const char* data);
    bool WeddingRequest(const char* data);
    bool WeddingReady(const char* data);
    bool WeddingStart(const char* data);
    bool WeddingEnd(const char* data);
    bool CreateObject(const char* data);
    bool DeleteObject(const char* data);
    bool UpdateLand(const char* data);
    void PartyCreate(DWORD leader);
    void PartyDelete(DWORD leader);
    void PartyAdd(DWORD leader, DWORD member, BYTE role);
    void PartyRemove(DWORD member);
    void PartyStateChange(DWORD leader, DWORD member, BYTE role, bool apply);
    void PartySetMemberLevel(DWORD leader, DWORD member, BYTE level);
    void GuildSkillRecharge();
    bool GuildSkillUpdate(const char* data);
    bool GuildExpUpdate(const char* data);
    bool GuildLoad(const char* data);
    bool GuildLadder(const char* data);
    bool GuildSkillUsableChange(const char* data);
    bool ChangeEmpirePriv(const char* data);
    bool ChangeGuildPriv(const char* data);
    bool ChangeCharacterPriv(const char* data);
    bool MoneyLog(const char* data);
    bool Notice(const char* data);
    bool BillingExpire(const char* data);
    bool BillingCheck(const char* data);
    bool MyshopPricelistRes(const char* data);
    bool GuildChangeGrade(const char* data);
    bool GuildWar(const char* data);
    bool GuildWarScore(const char* data);
    bool GuildWarReserveAdd(const char* data);
    bool GuildWarReserveDelete(const char* data);
    bool GuildWarBet(const char* data);
    void GuildMoneyChange(DWORD guildId, int gold);
    void GuildWithdrawMoney(DWORD guildId, int gold);
    bool AddBlockCountryIp(const char* data);
    bool BlockException(const char* data);
    bool Time(const char* data);
    bool LoginAlready(DESC* desc, const char* data);
    bool AuthLogin(DESC* desc, const char* data);
    bool SetEventFlag(const char* data);
    bool MapLocations(const char* data);
    bool P2P(const char* data);
    void FinishBoot(DWORD maxItemId);
    DWORD m_dwHandle;
    std::size_t m_payloadSize;
};

#endif
