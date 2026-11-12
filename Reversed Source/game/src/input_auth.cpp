#include "input.h"
#include "config.h"
#include "db.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "locale_service.h"
#include "passpod.h"
#include "matrix_for_runup.h"
#include "packet.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"
#include <cctype>
#include <cstring>
CInputAuth::CInputAuth() {}
CInputAuth::~CInputAuth() {}
BYTE CInputAuth::GetType() { return 8; }

namespace {
bool FN_IS_VALID_LOGIN_STRING(const char* login)
{
    if (std::strlen(login) < 2)
        return false;
    for (const unsigned char* ch = reinterpret_cast<const unsigned char*>(login); *ch; ++ch) {
        if (std::isdigit(*ch) || std::isalpha(*ch))
            continue;
        if (LC_IsCanada() && std::strchr(" !#$%&()*-.@^_", *ch))
            continue;
        return false;
    }
    return true;
}
}

bool Login_IsInChannelService(const char* login)
{
    return *login == '[';
}

// 2009 packet 0x6f, password SQL authentication.
void CInputAuth::Login(DESC* desc, const char* data)
{
    TPacketCGLogin3 request;
    std::memcpy(&request, data, sizeof(request));
    request.login[sizeof(request.login) - 1] = '\0';
    request.passwd[sizeof(request.passwd) - 1] = '\0';

    // The original trim helper inspects the byte preceding an empty destination.
    char normalized[32] = {};
    trim_and_lower(normalized + 1, request.login, sizeof(request.login) - 1);
    std::memcpy(request.login, normalized + 1, sizeof(request.login));
    const auto loginLength = std::strlen(request.login);
    sys_log(0, "InputAuth::Login : %s(%d) desc %p", request.login, static_cast<int>(loginLength), desc);
    if (!FN_IS_VALID_LOGIN_STRING(request.login)) {
        sys_log(0, "InputAuth::Login : IS_NOT_VALID_LOGIN_STRING(%s) desc %p", request.login, desc);
        LoginFailure(desc, "NOID");
        return;
    }
    if (g_bNoMoreClient) {
        LoginFailure(desc, "SHUTDOWN");
        return;
    }

    DESC_MANAGER& descriptors = DESC_MANAGER::instance();
    if (descriptors.FindByLoginName(request.login)) {
        LoginFailure(desc, "ALREADY");
        return;
    }
    if (LC_IsJapan() && !test_server) {
        if (!g_NetmarbleDBDesc || !g_NetmarbleDBDesc->IsPhase(11) || loginLength > 16)
            LoginFailure(desc, "NOTAVAIL");
        else if (!NetmarbleJP_RequestLogin(desc, request))
            LoginFailure(desc, "ALREADY");
        return;
    }

    const DWORD loginKey = descriptors.CreateLoginKey(desc);
    sys_log(0, "InputAuth::Login : key %lu login %s", loginKey, request.login);
    DBManager& db = DBManager::instance();
    char escapedLogin[61];
    char escapedPassword[33];
    db.EscapeString(escapedLogin, request.login, loginLength);
    db.EscapeString(escapedPassword, request.passwd, std::strlen(request.passwd));

    const char* query;
    if (LC_IsJapan()) {
        query = "SELECT PASSWORD('%s'), password, securitycode, social_id, id, status, availDt - NOW() > 0,"
                "UNIX_TIMESTAMP(silver_expire),UNIX_TIMESTAMP(gold_expire),UNIX_TIMESTAMP(safebox_expire),"
                "UNIX_TIMESTAMP(fish_mind_expire),UNIX_TIMESTAMP(marriage_fast_expire),"
                "UNIX_TIMESTAMP(money_drop_rate_expire) FROM account WHERE login='%s'";
    } else if (LC_IsCIBN()) {
        query = "SELECT PASSWORD('%s'),password,securitycode,social_id,id,status,"
                "availDt - NOW() > 0 FROM account WHERE login='%s'";
    } else if (Login_IsInChannelService(escapedLogin)) {
        sys_log(0, "ChannelServiceLogin [%s]", escapedLogin);
        query = "SELECT '%s',password,securitycode,social_id,id,status,availDt - NOW() > 0,"
                "UNIX_TIMESTAMP(silver_expire),UNIX_TIMESTAMP(gold_expire),UNIX_TIMESTAMP(safebox_expire),"
                "UNIX_TIMESTAMP(autoloot_expire),UNIX_TIMESTAMP(fish_mind_expire),"
                "UNIX_TIMESTAMP(marriage_fast_expire),UNIX_TIMESTAMP(money_drop_rate_expire),"
                "UNIX_TIMESTAMP(create_time) FROM account WHERE login='%s'";
    } else {
        query = "SELECT PASSWORD('%s'),password,securitycode,social_id,id,status,availDt - NOW() > 0,"
                "UNIX_TIMESTAMP(silver_expire),UNIX_TIMESTAMP(gold_expire),UNIX_TIMESTAMP(safebox_expire),"
                "UNIX_TIMESTAMP(autoloot_expire),UNIX_TIMESTAMP(fish_mind_expire),"
                "UNIX_TIMESTAMP(marriage_fast_expire),UNIX_TIMESTAMP(money_drop_rate_expire),"
                "UNIX_TIMESTAMP(create_time) FROM account WHERE login='%s'";
    }
    db.ReturnQuery(2, loginKey, new TPacketCGLogin3(request), query, escapedPassword, escapedLogin);
}

bool MatrixCardCheck(const char* matrix, const char* answer, unsigned long rows, unsigned cols);

namespace {
void MatrixAnswerFailed(DESC* desc)
{
    sys_log(0, "MATRIX_CARD: wrong answer: %s", desc->GetAccountTable().login);
    LoginFailure(desc, "WRONGMAT");
    if (!desc->CheckMatrixTryCount()) {
        LoginFailure(desc, "QUIT");
        desc->SetPhase(0);
    }
}
}

void CInputAuth::MatrixCard(DESC* desc, const char* data)
{
    if (!desc->GetAccountTable().id || desc->m_stMatrixCode.empty() || !desc->m_stMatrixCardID.empty()) {
        _sys_err("MatrixCard", 0xed, "HEADER_CG_MATRIX_CARD received without a matrix challenge");
        return;
    }

    char answer[9] = {};
    std::memcpy(answer, data + 1, sizeof(answer) - 1);
    if (!MatrixCardCheck(desc->m_stMatrixCode.c_str(), answer,
                         desc->GetMatrixRows(), static_cast<unsigned>(desc->GetMatrixCols()))) {
        MatrixAnswerFailed(desc);
        return;
    }

    desc->m_stMatrixCode.clear();
    DBManager::instance().SendAuthLogin(desc);
}

void CInputAuth::RunupMatrixAnswer(DESC* desc, const char* data)
{
    if (!desc->GetAccountTable().id || desc->m_stMatrixCardID.empty() || desc->m_stMatrixCode.empty()) {
        _sys_err("RunupMatrixAnswer", 0x131, "HEADER_CG_MATRIX_CARD received without a Runup challenge");
        return;
    }

    char answer[5] = {};
    std::memcpy(answer, data + 1, sizeof(answer) - 1);
    if (!MatrixDBManager::instance().CheckMatrix(
            desc->m_stMatrixCardID.c_str(), desc->m_stMatrixCode.c_str(), answer)) {
        MatrixAnswerFailed(desc);
        return;
    }

    sys_log(0, "MATRIX_CARD: success: %s", desc->GetAccountTable().login);
    desc->m_stMatrixCardID.clear();
    desc->m_stMatrixCode.clear();
    DBManager::instance().SendAuthLogin(desc);
}

void CInputAuth::PasspodAnswer(DESC* desc, const char* data)
{
    const TAccountTable& account = desc->GetAccountTable();
    if (!account.id || !desc->IsPasspodPending()) {
        _sys_err("PasspodAnswer", 0x154, "HEADER_CG_PASSPOD_ANSWER received without a Passpod challenge");
        return;
    }

    char answer[10] = {};
    std::memcpy(answer, data + 1, sizeof(answer) - 1);
    const int result = CPasspod::instance().ConfirmPasspod(account.login, answer);
    if (result == 0) {
        desc->SetPasspodPending(false);
        sys_log(0, "PASSPOD: success: %s", account.login);
        DBManager::instance().SendAuthLogin(desc);
        return;
    }

    constexpr const char* failures[] = {"PASERR1", "PASERR2", "PASERR3", "PASERR4", "PASERR5"};
    sys_log(0, "PASSPOD: wrong answer: %s ret_code %d", account.login, result);
    LoginFailure(desc, failures[result - 1]);
    if (!desc->CheckMatrixTryCount()) {
        desc->SetPasspodPending(false);
        LoginFailure(desc, "QUIT");
        desc->SetPhase(0);
    }
}

int CInputAuth::Analyze(DESC* desc, BYTE header, const char* data)
{
    if (test_server)
        sys_log(0, " InputAuth Analyze Header[%d] ", static_cast<int>(header));
    switch (header) {
        case 0x6c: MatrixCard(desc, data); break;
        case 0x6f: Login(desc, data); break;
        case 0xc9: RunupMatrixAnswer(desc, data); break;
        case 0xca: PasspodAnswer(desc, data); break;
        case 0xfe: Pong(desc); break;
        default:
            _sys_err("Analyze", 0x123, "This phase does not handle this header %d (0x%x)(phase: AUTH)",
                     static_cast<unsigned>(header), static_cast<unsigned>(header));
            break;
    }
    return 0;
}
