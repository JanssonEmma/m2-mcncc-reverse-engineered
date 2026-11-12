#include "char.h"
#include "char_manager.h"
#include "config.h"
#include "exchange.h"
#include "banword.h"
#include "building.h"
#include "cmd.h"
#include "desc.h"
#include "desc_manager.h"
#include "empire_text_convert.h"
#include "guild.h"
#include "guild_manager.h"
#include "db.h"
#include "utils.h"
#include "gm.h"
#include "packet.h"
#include <algorithm>
#include <string>
#include "locale_service.h"
#include "log.h"
#include "p2p.h"
#include "messenger_manager.h"
#include "party.h"
#include "buffer_manager.h"
#include "shop_manager.h"
#include "safebox.h"
#include "item.h"
#include "roulette.h"
#include "item_manager.h"
#include "desc_client.h"
#include <cstdint>
#include <cstdio>
#include "locale.hpp"
#include "questmanager.h"
#include "../../libthecore/include/main.h"
#include "../../libthecore/include/log.h"
#include <cstring>
// Recovered lifecycle/type methods; application Analyze remains declaration-only.
#include "input.h"
void SendShout(const char* text, BYTE empire);

int CInputMain::Chat(CHARACTER* character, const char* data, int bytes)
{
    const int size = static_cast<BYTE>(data[1]);
    if (size < 3) {
        character->GetDesc()->SetPhase(0);
        return -1;
    }
    // The sequence byte follows the complete variable-length packet.
    if (bytes <= size)
        return -1;

    const int extra = size - 3;
    char input[253] = {};
    std::memcpy(input, data + 3, extra);
    if (input[0] == '/') {
        interpret_command(character, input + 1);
        return extra;
    }
    if (character->IncreaseChatCounter() > 9) {
        sys_log(0, "CHAT_HACK: %s", character->GetName());
        character->GetDesc()->SetPhase(0);
        return extra;
    }
    if (character->FindAffect(0xdf)) {
        character->ChatPacket(1, "%s", locale_find("\303\244\306\303\040\261\335\301\366\040\273\363\305\302\300\324\264\317\264\331\056", g_iUseLocale));
        return extra;
    }

    if (!LC_IsJapan())
        CBanwordManager::instance().ConvertString(input);
    char message[86] = {};
    std::snprintf(message, 85, "%s : %s", character->GetName(), input);
    const BYTE type = static_cast<BYTE>(data[2]);
    if (type == 6) {
        const int minimumLevel = g_iUseLocale ? 15 : 3;
        if (character->GetLevel() < minimumLevel) {
            character->ChatPacket(1, locale_find("\277\334\304\241\261\342\264\302\040\267\271\272\247\040\045\144\040\300\314\273\363\270\270\040\273\347\277\353\040\260\241\264\311\040\307\325\264\317\264\331\056", g_iUseLocale), minimumLevel);
            return extra;
        }
        const auto pulse = static_cast<std::uint32_t>(thecore_heart->pulse);
        const auto last = static_cast<std::uint32_t>(character->GetLastShoutPulse());
        if (static_cast<std::int32_t>(pulse - last) >= passes_per_sec * 15) {
            character->SetLastShoutPulse(pulse);
            BYTE packet[88] = {8, character->GetEmpire()};
            std::memcpy(packet + 2, message, sizeof(message));
            char escaped[171];
            auto& log = LogManager::instance();
            log.EscapeString(escaped, packet + 2, sizeof(packet) - 3);
            log.Query("INSERT INTO shout_log%s VALUES(NOW(), %d,%d,'%s')", get_table_postfix(),
                      g_bChannel, character->GetEmpire(), escaped);
            P2P_MANAGER::instance().Send(packet, sizeof(packet));
            SendShout(message, character->GetEmpire());
        }
        return extra;
    }

    const auto length = std::strlen(message) + 1;
    BYTE header[9] = {4, 0, 0, type};
    const auto wireSize = static_cast<std::uint16_t>(sizeof(header) + length);
    const auto vid = static_cast<std::uint32_t>(static_cast<DWORD>(character->GetVID()));
    std::memcpy(header + 1, &wireSize, sizeof(wireSize));
    std::memcpy(header + 4, &vid, sizeof(vid));
    sys_log(0, "CHAT: %s", message);
    if (type == 0) {
        const BYTE empire = character->GetGMLevel() || character->IsEquipUniqueGroup(0x2729)
            ? 0 : character->GetEmpire();
        const auto nameLength = std::strlen(character->GetName());
        for (DESC* desc : DESC_MANAGER::instance().GetClientSet()) {
            CHARACTER* listener = desc->GetCharacter();
            if (!listener || listener->GetMapIndex() != character->GetMapIndex())
                continue;
            desc->BufferedPacket(header, sizeof(header));
            if (desc->GetEmpire() == empire || !empire || listener->GetGMLevel()
                || listener->IsEquipUniqueGroup(0x2729) || nameLength >= length) {
                desc->Packet(message, length);
            } else {
                char converted[86] = {};
                std::memcpy(converted, message, length);
                ConvertEmpireText(empire, converted + nameLength, listener->GetSkillPower(empire + 0x7d) * 2 + 10);
                desc->Packet(converted, length);
            }
        }
    } else if (type == 3) {
        if (CParty* party = character->GetParty()) {
            TEMP_BUFFER buffer;
            buffer.write(header, sizeof(header));
            buffer.write(message, length);
            party->ForEachOnlineMember([&](CHARACTER* member) {
                if (DESC* desc = member->GetDesc())
                    desc->Packet(buffer.read_peek(), buffer.size());
            });
        } else {
            character->ChatPacket(1, "%s", locale_find("\306\304\306\274\040\301\337\300\314\040\276\306\264\325\264\317\264\331\056", g_iUseLocale));
        }
    } else if (type == 4) {
        if (CGuild* guild = character->GetGuild())
            guild->Chat(message);
        else
            character->ChatPacket(1, "%s", locale_find("\261\346\265\345\277\241\040\260\241\300\324\307\317\301\366\040\276\312\276\322\275\300\264\317\264\331\056", g_iUseLocale));
    } else {
        sys_err("Unknown chat type %d", type);
    }
    return extra;
}

int CInputMain::Whisper(CHARACTER* sender, const char* data, int bytes)
{
    constexpr int headerSize = 28;
    std::uint16_t size;
    std::memcpy(&size, data + 1, sizeof(size));
    if (size < headerSize) {
        sender->GetDesc()->SetPhase(0);
        return -1;
    }
    if (bytes <= size)
        return -1;
    const int extra = size - headerSize;
    if (sender->FindAffect(0xdf)) {
        sender->ChatPacket(1, "%s", locale_find("\303\244\306\303\040\261\335\301\366\040\273\363\305\302\300\324\264\317\264\331\056", g_iUseLocale));
        return extra;
    }

    char targetName[25] = {};
    std::memcpy(targetName, data + 3, 24);
    CHARACTER* target = CHARACTER_MANAGER::instance().FindPC(targetName);
    if (target == sender)
        return extra;
    if (test_server)
        sys_log(0, "Whisper to %s from %s ", target ? target->GetName() : "Null", sender->GetName());

    DESC* recipient = target ? target->GetDesc() : nullptr;
    BYTE targetEmpire = target ? target->GetEmpire() : 0;
    if (!target) {
        if (CCI* peer = P2P_MANAGER::instance().Find(targetName)) {
            recipient = peer->pkDesc;
            targetEmpire = peer->bEmpire;
            if (test_server) {
                sys_log(0, "Whisper to Null from %s ( Channel %d Mapindex %d)", sender->GetName(),
                        peer->bChannel, peer->bMapIndex);
            }
        }
    }

    TPacketGCWhisper packet = {};
    static_assert(sizeof(packet) == 29, "whisper wire header");
    packet.bHeader = 0x22;
    packet.wSize = sizeof(packet);
    if (!recipient)
        packet.bType = 1;
    else if (sender->GetBlockMode() & 8)
        packet.bType = 3;
    else if (target && (target->GetBlockMode() & 8))
        packet.bType = 2;
    if (packet.bType) {
        std::memcpy(packet.szNameFrom, targetName, sizeof(targetName));
        if (DESC* desc = sender->GetDesc())
            desc->Packet(&packet, sizeof(packet));
        return extra;
    }

    std::string text(data + headerSize, extra);
    text.push_back(0);
    CBanwordManager::instance().ConvertString(&text[0]);
    const BYTE empire = sender->GetEmpire();
    if (g_bEmpireWhisper && empire && targetEmpire && empire != targetEmpire
        && !sender->IsEquipUniqueGroup(0x2729) && (!target || !target->IsEquipUniqueGroup(0x2729))
        && !sender->GetGMLevel() && !gm_get_level(targetName, nullptr)) {
        if (target)
            ConvertEmpireText(empire, &text[0], target->GetSkillPower(empire + 0x7d) * 2 + 10);
        else
            packet.bType = static_cast<BYTE>(~empire);
    }

    const auto length = std::min<std::size_t>(85, std::strlen(text.c_str()) + 1);
    std::strncpy(packet.szNameFrom, sender->GetName(), sizeof(packet.szNameFrom) - 1);
    packet.wSize += length;
    TEMP_BUFFER buffer;
    buffer.write(&packet, sizeof(packet));
    buffer.write(text.c_str(), length);
    if (!target)
        recipient->SetRelay(targetName);
    recipient->Packet(buffer.read_peek(), buffer.size());
    sys_log(0, "WHISPER: %s -> %s : %s", sender->GetName(), targetName, text.c_str());
    return extra;
}

int CInputMain::SyncPosition(CHARACTER* character, const char* data, int bytes)
{
    std::uint16_t size;
    std::memcpy(&size, data + 1, sizeof(size));
    sys_log(1, "SyncPosition %s wSize %d iBytes %d", character->GetName(), size, bytes);
    if (size < 3) {
        character->GetDesc()->SetPhase(0);
        return -1;
    }
    if (bytes <= size)
        return -1;

    const int extra = size - 3;
    if (extra % 12) {
        sys_err("CInputMain::SyncPosition : invalid packet length %d (name: %s)", size, character->GetName());
        return extra;
    }
    TEMP_BUFFER elements;
    const int count = extra / 12;
    for (int i = 0; i < count; ++i) {
        const char* row = data + 3 + i * 12;
        std::uint32_t vid;
        std::int32_t x, y;
        std::memcpy(&vid, row, sizeof(vid));
        std::memcpy(&x, row + 4, sizeof(x));
        std::memcpy(&y, row + 8, sizeof(y));
        CHARACTER* target = CHARACTER_MANAGER::instance().Find(vid);
        if (!target || target->GetCharType() == 1 || target->GetCharType() == 3)
            continue;

        sys_log(1, "%s: SYNC: %s %d %d cnt %d", character->GetName(), target->GetName(), x, y, count);
        if (target->SetSyncOwner(character, true)) {
            target->Sync(x, y);
            elements.write(row, 12);
        }
    }
    if (elements.size()) {
        BYTE header[3] = {5};
        const auto wireSize = static_cast<std::uint16_t>(sizeof(header) + elements.size());
        std::memcpy(header + 1, &wireSize, sizeof(wireSize));
        TEMP_BUFFER packet;
        packet.write(header, sizeof(header));
        packet.write(elements.read_peek(), elements.size());
        character->PacketAround(packet.read_peek(), packet.size(), character);
    }
    return extra;
}

void CInputMain::Move(CHARACTER* character, const char* data)
{
    if (!character->CanMove())
        return;

    const BYTE function = static_cast<BYTE>(data[1]);
    if (function >= 6 && function < 128) {
        sys_err("invalid move type: %s", character->GetName());
        return;
    }
    std::int32_t x, y;
    std::uint32_t clientTime;
    std::memcpy(&x, data + 4, sizeof(x));
    std::memcpy(&y, data + 8, sizeof(y));
    std::memcpy(&clientTime, data + 12, sizeof(clientTime));
    DESC* desc = character->GetDesc();
    const float rotation = static_cast<BYTE>(data[3]) * 5.0f;

    if (function == 1) {
        if (!character->GetLimitPoint(19))
            return;
        character->SetRotation(rotation);
        character->ResetStopTime();
        character->Goto(x, y);
    } else {
        if (function == 2 || function == 3) {
            character->OnMove(true);
        } else if (function & 0x80) {
            const unsigned motion = function & 0x7f;
            if (!character->IsUsableSkillMotion(motion)) {
                char message[256];
                std::snprintf(message, 255, "SKILL_HACK: name=%s, job=%d, group=%d, motion=%u",
                              character->GetName(), character->GetJob(), character->GetSkillGroup(), motion);
                sys_log(0, "%s", message);
                LogManager::instance().HackLog(message, desc->GetAccountTable().login,
                                                character->GetName(), desc->GetHostName());
                char file[] = "input_main.cpp";
                if (test_server) {
                    desc->DelayedDisconnect(number_ex(3, 3, file, 0x382));
                    character->ChatPacket(1, "%s", message);
                } else {
                    desc->DelayedDisconnect(number_ex(150, 500, file, 0x387));
                }
            }
            character->OnMove(false);
        }
        character->SetRotation(rotation);
        character->ResetStopTime();
        character->Move(x, y);
        character->Stop();
        character->StopStaminaConsume();
    }

    BYTE packet[24] = {3, function, static_cast<BYTE>(data[2]), static_cast<BYTE>(data[3])};
    const auto vid = static_cast<std::uint32_t>(static_cast<DWORD>(character->GetVID()));
    const auto duration = static_cast<std::uint32_t>(function == 1 ? character->GetMoveDuration() : 0);
    std::memcpy(packet + 4, &vid, sizeof(vid));
    std::memcpy(packet + 8, data + 4, 12);
    std::memcpy(packet + 20, &duration, sizeof(duration));
    character->PacketAround(packet, sizeof(packet), character);

    const auto now = static_cast<std::uint32_t>(get_dword_time());
    const auto base = static_cast<std::uint32_t>(desc->GetClientTime());
    if (now - base <= 5000)
        return;
    const auto elapsed = static_cast<std::int32_t>(now - base);
    const auto reported = static_cast<std::int32_t>(clientTime - base);
    character->SetSpeedHackMultipler(elapsed == 0 || reported <= elapsed
        ? 1.0f : static_cast<float>(static_cast<long double>(elapsed) / reported));
    const auto delta = static_cast<std::int32_t>(now - clientTime);
    if (delta >= 30000) {
        sys_log(0, "SPEEDHACK: slow timer name %s delta %d", character->GetName(), delta);
    } else if (delta < -(elapsed / 50)) {
        sys_log(0, "SPEEDHACK: DETECTED! %s (delta %d %d)", character->GetName(), delta, elapsed);
    } else {
        return;
    }
    desc->SetPhase(0);
}

// 08100770
void CInputMain::PartyInvite(CHARACTER* character, const char* data)
{
    if (character->GetArena()) {
        character->ChatPacket(1, "%s", LC_TEXT("\264\353\267\303\300\345\277\241\274\255 \273\347\277\353\307\317\275\307 \274\366 \276\370\275\300\264\317\264\331."));
        return;
    }
    std::uint32_t vid;
    std::memcpy(&vid, data + 1, sizeof(vid));
    CHARACTER* guest = CHARACTER_MANAGER::instance().Find(vid);
    if (guest && character->GetDesc() && guest->GetDesc()) {
        character->PartyInvite(guest);
        return;
    }
    _sys_err("PartyInvite", 0x502, "PARTY Cannot find invited character");
}

// 08100860
void CInputMain::PartyInviteAnswer(CHARACTER* character, const char* data)
{
    if (character->GetArena()) {
        character->ChatPacket(1, "%s", LC_TEXT("\264\353\267\303\300\345\277\241\274\255 \273\347\277\353\307\317\275\307 \274\366 \276\370\275\300\264\317\264\331."));
        return;
    }
    std::uint32_t vid;
    std::memcpy(&vid, data + 1, sizeof(vid));
    CHARACTER* leader = CHARACTER_MANAGER::instance().Find(vid);
    if (!leader) {
        character->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \306\304\306\274\277\344\303\273\300\273 \307\321 \304\263\270\257\305\315\270\246 \303\243\300\273\274\366 \276\370\275\300\264\317\264\331."));
        return;
    }
    if (data[5])
        leader->PartyInviteAccept(character);
    else
        leader->PartyInviteDeny(character->GetPlayerID());
}

// 08100b60
void CInputMain::PartyRemove(CHARACTER* character, const char* data)
{
    if (character->GetArena()) {
        character->ChatPacket(1, "%s", LC_TEXT("\264\353\267\303\300\345\277\241\274\255 \273\347\277\353\307\317\275\307 \274\366 \276\370\275\300\264\317\264\331."));
        return;
    }
    CPartyManager& parties = CPartyManager::instance();
    if (!parties.IsEnablePCParty()) {
        character->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \274\255\271\366 \271\256\301\246\267\316 \306\304\306\274 \260\374\267\303 \303\263\270\256\270\246 \307\322 \274\366 \276\370\275\300\264\317\264\331."));
        return;
    }
    if (character->GetDungeon()) {
        character->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \264\370\300\374 \276\310\277\241\274\255\264\302 \306\304\306\274\277\241\274\255 \303\337\271\346\307\322 \274\366 \276\370\275\300\264\317\264\331."));
        return;
    }

    CParty* party = character->GetParty();
    if (!party)
        return;
    std::uint32_t targetPid;
    std::memcpy(&targetPid, data + 1, sizeof(targetPid));
    const DWORD pid = character->GetPlayerID();
    const bool leader = party->GetLeaderPID() == pid;
    if (!leader && targetPid != pid) {
        character->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \264\331\270\245 \306\304\306\274\277\370\300\273 \305\273\305\360\275\303\305\263 \274\366 \276\370\275\300\264\317\264\331."));
        return;
    }

    if ((leader && targetPid == pid) || party->GetMemberCount() == 2) {
        parties.DeleteParty(party);
        return;
    }
    if (leader) {
        if (CHARACTER* target = CHARACTER_MANAGER::instance().FindByPID(targetPid))
            target->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \306\304\306\274\277\241\274\255 \303\337\271\346\264\347\307\317\274\314\275\300\264\317\264\331."));
    } else {
        character->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \306\304\306\274\277\241\274\255 \263\252\260\241\274\314\275\300\264\317\264\331."));
    }
    party->Quit(targetPid);
}

// 08100950
void CInputMain::PartySetState(CHARACTER* character, const char* data)
{
    if (!CPartyManager::instance().IsEnablePCParty()) {
        character->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \274\255\271\366 \271\256\301\246\267\316 \306\304\306\274 \260\374\267\303 \303\263\270\256\270\246 \307\322 \274\366 \276\370\275\300\264\317\264\331."));
        return;
    }
    CParty* party = character->GetParty();
    if (!party)
        return;
    const auto leaderPid = static_cast<std::uint32_t>(character->GetPlayerID());
    if (party->GetLeaderPID() != leaderPid) {
        character->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \270\256\264\365\270\270 \272\257\260\346\307\322 \274\366 \300\326\275\300\264\317\264\331."));
        return;
    }
    std::uint32_t pid;
    std::memcpy(&pid, data + 1, sizeof(pid));
    if (!party->IsMember(pid)) {
        character->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \273\363\305\302\270\246 \272\257\260\346\307\317\267\301\264\302 \273\347\266\367\300\314 \306\304\306\274\277\370\300\314 \276\306\264\325\264\317\264\331."));
        return;
    }

    const BYTE role = static_cast<BYTE>(data[5]);
    const bool enabled = data[6] != 0;
    sys_log(0, "PARTY SetRole pid %d to role %d state %s", static_cast<int>(pid), role,
            enabled ? "on" : "off");
    if (role == 0)
        return;
    if (role < 2 || role > 7) {
        _sys_err("PartySetState", 0x555, "wrong byRole in PartySetState Packet name %s state %d",
                 character->GetName(), role);
        return;
    }
    if (!party->SetRole(pid, role, enabled))
        return;
    const TPacketPartyStateChange packet = {leaderPid, pid, role, static_cast<BYTE>(data[6])};
    db_clientdesc->DBPacket(0x28, 0, &packet, sizeof(packet));
}

// 081013d0
void CInputMain::PartyUseSkill(CHARACTER* character, const char* data)
{
    CParty* party = character->GetParty();
    if (!party)
        return;
    if (party->GetLeaderPID() != character->GetPlayerID()) {
        character->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \306\304\306\274 \261\342\274\372\300\272 \306\304\306\274\300\345\270\270 \273\347\277\353\307\322 \274\366 \300\326\275\300\264\317\264\331."));
        return;
    }
    if (data[1] == 1) {
        party->HealParty();
    } else if (data[1] == 2) {
        std::uint32_t vid;
        std::memcpy(&vid, data + 2, sizeof(vid));
        if (CHARACTER* target = CHARACTER_MANAGER::instance().Find(vid))
            party->SummonToLeader(target->GetPlayerID());
        else
            character->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \274\322\310\257\307\317\267\301\264\302 \264\353\273\363\300\273 \303\243\300\273 \274\366 \276\370\275\300\264\317\264\331."));
    }
}

// 08101520
void CInputMain::PartyParameter(CHARACTER* character, const char* data)
{
    if (CParty* party = character->GetParty())
        party->SetParameter(static_cast<BYTE>(data[1]));
}

int CInputMain::Analyze(DESC* desc, BYTE header, const char* data)
{
    CHARACTER* character = desc->GetCharacter();
    if (!character) {
        _sys_err("Analyze", 0x8b5, "no character on desc");
        desc->SetPhase(0);
        return 0;
    }
    if (test_server && header != 7)
        sys_log(0, "CInputMain::Analyze() ==> Header [%d] ", header);
    switch (header) {
        case 0x02:
            if (!character->IsObserverMode())
                Attack(character, data);
            break;
        case 0x03: {
            const int size = static_cast<BYTE>(data[1]);
            if (test_server && size >= 3 && size < m_iBufferLeft)
                sys_log(0, "%.*s", size - 3, data + 3);
            return Chat(character, data, m_iBufferLeft);
        }
        case 0x07:
            Move(character, data);
            if (!*desc->GetClientVersion()) {
                _sys_err("Analyze", 0x8e1, "Version not recieved name %s", character->GetName());
                desc->SetPhase(0);
            }
            break;
        case 0x08: return SyncPosition(character, data, m_iBufferLeft);
        case 0x0c:
            if (!character->IsObserverMode())
                ItemDrop(character, data);
            break;
        case 0x14:
            if (!character->IsObserverMode())
                ItemDrop2(character, data);
            break;
        case 0x0d:
            if (!character->IsObserverMode())
                ItemMove(character, data);
            break;
        case 0x0f:
            if (!character->IsObserverMode())
                ItemPickup(character, data);
            break;
        case 0x10: QuickslotAdd(character, data); break;
        case 0x11: QuickslotDelete(character, data); break;
        case 0x12: QuickslotSwap(character, data); break;
        case 0x13: return Whisper(character, data, m_iBufferLeft);
        case 0x1a: OnClick(character, data); break;
        case 0x1b:
            if (!character->IsObserverMode())
                Exchange(character, data);
            break;
        case 0x1c: Position(character, data); break;
        case 0x1d: ScriptAnswer(character, data); break;
        case 0x1e: QuestInputString(character, data); break;
        case 0x1f: QuestConfirm(character, data); break;
        case 0x32: return Shop(character, data, m_iBufferLeft);
        case 0x33:
        case 0x35: FlyTarget(character, data, header); break;
        case 0x34:
            if (!character->IsObserverMode())
                UseSkill(character, data);
            break;
        case 0x36:
            if (!character->IsObserverMode())
                Shoot(character, data);
            break;
        case 0x37: return MyShop(character, data, m_iBufferLeft);
        case 0x3c:
            if (!character->IsObserverMode())
                ItemToItem(character, data);
            break;
        case 0x3d: Target(character, data); break;
        case 0x41:
            Warp(character, data);
            break;
        case 0x42: ScriptButton(character, data); break;
        case 0x72: ScriptSelectItem(character, data); break;
        case 0x0b:
            if (!character->IsObserverMode())
                ItemUse(character, data);
            break;
        case 0x43: return Messenger(character, data, m_iBufferLeft);
        case 0x45: SafeboxCheckout(character, data, true); break;
        case 0x46: SafeboxCheckin(character, data); break;
        case 0x47: SafeboxCheckout(character, data, false); break;
        case 0x48: PartyInvite(character, data); break;
        case 0x49: PartyInviteAnswer(character, data); break;
        case 0x4a: PartyRemove(character, data); break;
        case 0x4b: PartySetState(character, data); break;
        case 0x4c: PartyUseSkill(character, data); break;
        case 0x4d: SafeboxItemMove(character, data); break;
        case 0x4e: PartyParameter(character, data); break;
        case 0x50: return Guild(character, data, m_iBufferLeft);
        case 0x51: AnswerMakeGuild(character, data); break;
        case 0x52: Fishing(character, data); break;
        case 0x53:
            if (!character->IsObserverMode())
                ItemGive(character, data);
            break;
        case 0x60: Refine(character, data); break;
        case 0x67: CRCReport(character, data); break;
        case 0x6b: return SMS(character, data, m_iBufferLeft);
        case 0x69: Hack(character, data); break;
        case 0xfc: Handshake(desc, data); break;
        case 0xfd: Version(character, data); break;
        case 0xfe: Pong(desc); break;
    }
    return 0;
}

int CInputDead::Analyze(DESC* desc, BYTE header, const char* data)
{
    CHARACTER* character = desc->GetCharacter();
    if (!character) {
        _sys_err("Analyze", 0x9be, "no character on desc");
        return 0;
    }
    switch (header) {
        case 0x03: return Chat(character, data, m_iBufferLeft);
        case 0x13: return Whisper(character, data, m_iBufferLeft);
        case 0x69: Hack(character, data); break;
        case 0x6b: return SMS(character, data, m_iBufferLeft);
        case 0xfc: Handshake(desc, data); break;
        case 0xfe: Pong(desc); break;
    }
    return 0;
}

CInputMain::~CInputMain() {}
BYTE CInputMain::GetType() { return 3; }
CInputDead::~CInputDead() {}
BYTE CInputDead::GetType() { return 4; }

extern int g_nPortalLimitTime;

namespace {
bool ExchangeCooldownElapsed(int pulse, int start, unsigned int limit)
{
    unsigned int bits=static_cast<unsigned int>(pulse)-start;
    int delta, threshold;
    std::memcpy(&delta,&bits,4);
    std::memcpy(&threshold,&limit,4);
    return delta>=threshold;
}
}

void CInputMain::Exchange(CHARACTER* ch, const char* data)
{
    if (!ch->CanHandleItem(false,false)) return;
    const int pulse=thecore_pulse();
    unsigned int arg;
    std::memcpy(&arg,data+2,sizeof(arg));
    CHARACTER_MANAGER& manager=CHARACTER_MANAGER::instance();
    CHARACTER* victim=manager.Find(arg);
    const unsigned int limit=static_cast<unsigned int>(passes_per_sec)*g_nPortalLimitTime;
    if (victim && !ExchangeCooldownElapsed(pulse,victim->m_iSafeboxLoadTime,limit)) {
        victim->ChatPacket(1,locale_find("\260\305\267\241\040\310\304\040\045\144\303\312\040\300\314\263\273\277\241\040\303\242\260\355\270\246\040\277\255\274\366\040\276\370\275\300\264\317\264\331\056",g_iUseLocale),g_nPortalLimitTime);
        return;
    }
    BYTE sub=static_cast<BYTE>(data[1]);
    sys_log(0,"CInputMain()::Exchange()  SubHeader %d ",sub);
    if (!ExchangeCooldownElapsed(pulse,ch->m_iSafeboxLoadTime,limit)) {
        ch->ChatPacket(1,locale_find("\260\305\267\241\040\310\304\040\045\144\303\312\040\300\314\263\273\277\241\040\303\242\260\355\270\246\040\277\255\274\366\040\276\370\275\300\264\317\264\331\056",g_iUseLocale),g_nPortalLimitTime);
        return;
    }
    CExchange* exchange=ch->GetExchange();
    if (sub==0) {
        if (exchange) return;
        victim=manager.Find(arg);
        if (!victim) return;
        CHARACTER* blocked=!ExchangeCooldownElapsed(pulse,ch->m_iSafeboxLoadTime,limit) ? ch :
                           !ExchangeCooldownElapsed(pulse,victim->m_iSafeboxLoadTime,limit) ? victim : 0;
        if (blocked) {
            blocked->ChatPacket(1,locale_find("\303\242\260\355\270\246\040\277\254\310\304\040\045\144\303\312\040\300\314\263\273\277\241\264\302\040\260\305\267\241\270\246\040\307\322\274\366\040\276\370\275\300\264\317\264\331\056",g_iUseLocale),g_nPortalLimitTime);
            if (test_server)
                blocked->ChatPacket(1,"[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d",
                                    pulse,blocked->m_iSafeboxLoadTime,limit);
            return;
        }
        if (ch->GetGold()>=2000000000) {
            ch->ChatPacket(1,locale_find("\276\327\274\366\260\241\040\062\060\276\357\040\263\311\300\273\040\303\312\260\372\307\317\277\251\040\260\305\267\241\270\246\040\307\322\274\366\260\241\040\276\370\275\300\264\317\264\331\056\056",g_iUseLocale));
            _sys_err("Exchange",0x2e7,"[OVERFLOG_GOLD] START (%lu) id %lu name %s ",
                     static_cast<unsigned long>(static_cast<std::uint32_t>(ch->GetGold())),ch->GetPlayerID(),ch->GetName());
            return;
        }
        if (victim->GetDesc() && quest::CQuestManager::instance().GiveItemToPC(ch->GetPlayerID(),victim)) {
            sys_log(0,"Exchange canceled by quest %s %s",ch->GetName(),victim->GetName());
            return;
        }
        if (!ch->GetMyShop() && !ch->IsOpenSafebox() && !ch->GetShopOwner() && !ch->IsCubeOpen())
            ch->ExchangeStart(victim);
        else ch->ChatPacket(1,locale_find("\264\331\270\245\040\260\305\267\241\301\337\300\317\260\346\277\354\040\260\263\300\316\273\363\301\241\300\273\040\277\255\274\366\260\241\040\276\370\275\300\264\317\264\331\056",g_iUseLocale));
        return;
    }
    if (!exchange) return;
    switch (sub) {
        case 1:
            if (!exchange->GetCompany()->GetAcceptStatus()) exchange->AddItem(data[2],data[6]);
            break;
        case 2:
            if (!exchange->GetCompany()->GetAcceptStatus()) exchange->RemoveItem(data[2]);
            break;
        case 3: {
            CHARACTER* recipient=exchange->GetCompany()->GetOwner();
            if (static_cast<unsigned int>(recipient->GetGold())+arg>=2000000000U) {
                ch->ChatPacket(1,locale_find("\273\363\264\353\271\346\300\307\040\303\321\261\335\276\327\300\314\040\062\060\276\357\040\263\311\300\273\040\303\312\260\372\307\317\277\251\040\260\305\267\241\270\246\040\307\322\274\366\260\241\040\276\370\275\300\264\317\264\331\056\056",g_iUseLocale));
                _sys_err("Exchange",0x317,"[OVERFLOW_GOLD] ELK_ADD (%lu) id %lu name %s ",
                         static_cast<unsigned long>(static_cast<std::uint32_t>(recipient->GetGold())),recipient->GetPlayerID(),recipient->GetName());
            } else if (!exchange->GetCompany()->GetAcceptStatus()) {
                int gold;
                std::memcpy(&gold,&arg,4);
                exchange->AddGold(gold);
            }
            break;
        }
        case 4:
            sys_log(0,"CInputMain()::Exchange() ==> ACCEPT ",sub);
            exchange->Accept(true);
            break;
        case 5:
            exchange->Cancel();
            break;
    }
}

void CInputMain::Fishing(CHARACTER* character, const char* data)
{
    character->SetRotation(static_cast<BYTE>(data[1]) * 5.0f);
    character->fishing();
}

namespace {
// 080fd750
int GuildDepositLimit()
{
    return 10000000;
}
}

int CInputMain::Guild(CHARACTER* character, const char* data, int bytes)
{
    constexpr int headerSize = 2;
    if (bytes < headerSize)
        return -1;

    const BYTE command = static_cast<BYTE>(data[1]);
    constexpr int payloadSizes[] = {4, 4, 10, 2, 4, 0, 4, 0, 5, 8, 5, 5, 4, 4, 4};
    if (command >= sizeof(payloadSizes) / sizeof(payloadSizes[0]))
        return 0;

    const char* payload = data + headerSize;
    const int available = bytes - headerSize;
    int size = payloadSizes[command];
    if (command == 5)
    {
        if (available == 0)
            return -1;
        size = 1 + static_cast<BYTE>(payload[0]);
    }
    if (available < size)
        return -1;

    CGuild* guild = character->GetGuild();
    const auto notice = [character](const char* key) {
        character->ChatPacket(1, "%s", locale_find(key, g_iUseLocale));
    };
    const char* const noGuild = "<\261\346\265\345> \261\346\265\345\277\241 \274\323\307\330\300\326\301\366 \276\312\275\300\264\317\264\331.";
    switch (command)
    {
        case 1: {
            if (!guild)
            {
                notice(noGuild);
                break;
            }
            if (character->GetWarMap() && (LC_IsEurope() || LC_IsGermany()))
            {
                notice("<\261\346\265\345> \261\346\265\345\300\374 \301\337\277\241\264\302 \261\346\265\345 \277\370\300\273 \305\273\305\360\275\303\305\263\274\366\260\241 \276\370\275\300\264\317\264\331");
                break;
            }
            std::uint32_t targetPid;
            std::memcpy(&targetPid, payload, sizeof(targetPid));
            CHARACTER* target = CHARACTER_MANAGER::instance().FindByPID(targetPid);
            if (target && target->GetGuild() != guild)
            {
                notice("<\261\346\265\345> \273\363\264\353\271\346\300\314 \260\260\300\272 \261\346\265\345\260\241 \276\306\264\325\264\317\264\331.");
                break;
            }
            if (!(guild->GetMemberAuth(character->GetPlayerID()) & 2))
            {
                notice("<\261\346\265\345> \261\346\265\345\277\370\300\273 \260\255\301\246 \305\273\305\360 \275\303\305\263 \261\307\307\321\300\314 \276\370\275\300\264\317\264\331.");
                break;
            }
            if (target)
            {
                target->SetQuestFlag("guild_manage.new_withdraw_time", get_global_time());
                guild->RequestRemoveMember(target->GetPlayerID());
            }
            else
                notice(guild->RequestRemoveMember(targetPid) ? "<\261\346\265\345> \261\346\265\345\277\370\300\273 \260\255\301\246 \305\273\305\360 \275\303\304\327\275\300\264\317\264\331." : "<\261\346\265\345> \261\327\267\257\307\321 \273\347\266\367\300\273 \303\243\300\273 \274\366 \276\370\275\300\264\317\264\331.");
            break;
        }
        case 4: {
            if (!guild) {
                notice(noGuild);
                break;
            }
            std::uint32_t requested;
            std::memcpy(&requested, payload, sizeof(requested));
            const std::uint32_t amount = requested / 100 * 100;
            if (amount <= INT32_MAX && guild->OfferExp(character, amount))
                character->ChatPacket(1, locale_find("\074\261\346\265\345\076\040\045\154\165\300\307\040\260\346\307\350\304\241\270\246\040\305\365\300\332\307\317\277\264\275\300\264\317\264\331\056", g_iUseLocale), static_cast<unsigned long>(amount));
            else
                notice("\074\261\346\265\345\076\040\260\346\307\350\304\241\040\305\365\300\332\277\241\040\275\307\306\320\307\317\277\264\275\300\264\317\264\331\056");
            break;
        }
        case 5: {
            const BYTE length = static_cast<BYTE>(payload[0]);
            if (length > 50)
                break;
            if (!guild)
            {
                notice(noGuild);
                break;
            }
            const char* begin = payload + 1;
            if (length && *begin == '!' && !(guild->GetMemberAuth(character->GetPlayerID()) & 4))
            {
                notice("<\261\346\265\345> \260\370\301\366\261\333\300\273 \300\333\274\272\307\322 \261\307\307\321\300\314 \276\370\275\300\264\317\264\331.");
                break;
            }
            const char* end = static_cast<const char*>(std::memchr(begin, 0, length));
            guild->AddComment(character, std::string(begin, end ? end : begin + length));
            break;
        }
        case 6: {
            if (!guild)
            {
                notice(noGuild);
                break;
            }
            std::uint32_t id;
            std::memcpy(&id, payload, sizeof(id));
            guild->DeleteComment(character, id);
            break;
        }
        case 7:
            if (guild)
                guild->RefreshComment(character);
            else
                notice(noGuild);
            break;
        case 2:
        case 3: {
            if (!guild)
            {
                notice(noGuild);
                break;
            }
            const TGuildMember* member = guild->GetMember(character->GetPlayerID());
            const bool rename = command == 2;
            if (!member || member->grade != 1)
            {
                notice(rename ? "<\261\346\265\345> \301\367\300\247 \300\314\270\247\300\273 \272\257\260\346\307\322 \261\307\307\321\300\314 \276\370\275\300\264\317\264\331." : "<\261\346\265\345> \301\367\300\247 \261\307\307\321\300\273 \272\257\260\346\307\322 \261\307\307\321\300\314 \276\370\275\300\264\317\264\331.");
                break;
            }
            const BYTE grade = static_cast<BYTE>(payload[0]);
            if (grade == 1)
            {
                notice(rename ? "<\261\346\265\345> \261\346\265\345\300\345\300\307 \301\367\300\247 \300\314\270\247\300\272 \272\257\260\346\307\322 \274\366 \276\370\275\300\264\317\264\331." : "<\261\346\265\345> \261\346\265\345\300\345\300\307 \261\307\307\321\300\272 \272\257\260\346\307\322 \274\366 \276\370\275\300\264\317\264\331.");
                break;
            }
            if (rename)
            {
                char name[10] = {};
                std::memcpy(name, payload + 1, 9);
                if (name[8] || !check_name(name))
                    notice("<\261\346\265\345> \300\373\307\325\307\317\301\366 \276\312\300\272 \301\367\300\247 \300\314\270\247 \300\324\264\317\264\331.");
                else
                    guild->ChangeGradeName(grade, name);
            }
            else
                guild->ChangeGradeAuth(grade, static_cast<BYTE>(payload[1]));
            break;
        }
        case 8:
        case 10: {
            if (!guild)
            {
                notice(noGuild);
                break;
            }
            std::uint32_t pid;
            std::memcpy(&pid, payload, sizeof(pid));
            const BYTE value = static_cast<BYTE>(payload[4]);
            const TGuildMember* member = guild->GetMember(character->GetPlayerID());
            if (command == 8)
            {
                if (!member || member->grade != 1)
                    notice("<\261\346\265\345> \301\367\300\247\270\246 \272\257\260\346\307\322 \261\307\307\321\300\314 \276\370\275\300\264\317\264\331.");
                else if (pid == character->GetPlayerID())
                    notice("<\261\346\265\345> \261\346\265\345\300\345\300\307 \301\367\300\247\264\302 \272\257\260\346\307\322 \274\366 \276\370\275\300\264\317\264\331.");
                else if (value == 1)
                    notice("<\261\346\265\345> \261\346\265\345\300\345\300\270\267\316 \301\367\300\247\270\246 \272\257\260\346\307\322 \274\366 \276\370\275\300\264\317\264\331.");
                else
                    guild->ChangeMemberGrade(pid, value);
            }
            else if (!member || member->grade != 1)
                notice("<\261\346\265\345> \300\345\261\272\300\273 \301\366\301\244\307\322 \261\307\307\321\300\314 \276\370\275\300\264\317\264\331.");
            else if (!guild->ChangeMemberGeneral(pid, value))
                notice("<\261\346\265\345> \264\365\300\314\273\363 \300\345\274\366\270\246 \301\366\301\244\307\322 \274\366 \276\370\275\300\264\317\264\331.");
            break;
        }
        case 0: {
            std::uint32_t vid;
            std::memcpy(&vid, payload, sizeof(vid));
            CHARACTER* target = CHARACTER_MANAGER::instance().Find(vid);
            if (!target)
                notice("<\261\346\265\345> \261\327\267\257\307\321 \273\347\266\367\300\273 \303\243\300\273 \274\366 \276\370\275\300\264\317\264\331.");
            else if (character->GetDesc())
            {
                if (guild)
                    guild->Invite(character, target);
                else
                    notice(noGuild);
            }
            break;
        }
        case 11: {
            std::uint32_t id;
            std::memcpy(&id, payload, sizeof(id));
            if (CGuild* inviting = CGuildManager::instance().FindGuild(id))
            {
                if (payload[4])
                    inviting->InviteAccept(character);
                else
                    inviting->InviteDeny(character->GetPlayerID());
            }
            break;
        }
        case 9: {
            if (!guild)
            {
                notice(noGuild);
                break;
            }
            std::uint32_t skill, targetPid;
            std::memcpy(&skill, payload, sizeof(skill));
            std::memcpy(&targetPid, payload + 4, sizeof(targetPid));
            guild->UseSkill(skill, character, targetPid);
            break;
        }
        case 12: {
            if (!guild)
            {
                notice(noGuild);
                break;
            }
            std::int32_t amount;
            std::memcpy(&amount, payload, sizeof(amount));
            if (amount < 0 || static_cast<std::int64_t>(amount) * 100 > character->GetGold())
                notice("<\261\346\265\345> \265\267\300\314 \272\316\301\267\307\325\264\317\264\331.");
            else if (!guild->ChargeSP(character, amount))
                notice("<\261\346\265\345> \277\353\275\305\267\302 \310\270\272\271\277\241 \275\307\306\320\307\317\277\264\275\300\264\317\264\331.");
            break;
        }
        case 13:
        case 14: {
            if (!LC_IsJapan())
                break;
            if (!guild)
            {
                notice(noGuild);
                break;
            }

            std::int32_t gold;
            std::memcpy(&gold, payload, sizeof(gold));
            if (gold < 0)
            {
                notice("<\261\346\265\345> \300\337\270\370\265\310 \261\335\276\327\300\324\264\317\264\331.");
                break;
            }
            if (command == 13)
            {
                if (character->GetGold() < gold)
                    notice("<\261\346\265\345> \260\241\301\366\260\355 \300\326\264\302 \265\267\300\314 \272\316\301\267\307\325\264\317\264\331.");
                else
                    guild->RequestDepositMoney(character, std::min(gold, GuildDepositLimit()));
            }
            else
                guild->RequestWithdrawMoney(character, std::min(gold, 500000));
            break;
        }
    }
    return size;
}

void CInputMain::AnswerMakeGuild(CHARACTER* character, const char* data)
{
    if (character->GetGold() < 200000 || (LC_IsCIBN() && character->CountSpecifyItem(0x111d5) < 1))
        return;
    CGuildManager& manager = CGuildManager::instance();
    quest::CQuestManager& quests = quest::CQuestManager::instance();
    if (get_global_time() - character->GetQuestFlag("guild_manage.new_disband_time") < manager.GetDisbandDelay()) {
        character->ChatPacket(1, locale_find("\074\261\346\265\345\076\040\307\330\273\352\307\321\040\310\304\040\045\144\300\317\040\300\314\263\273\277\241\264\302\040\261\346\265\345\270\246\040\270\270\265\351\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale),
                              quests.GetEventFlag("guild_disband_delay"));
        return;
    }
    if (get_global_time() - character->GetQuestFlag("guild_manage.new_withdraw_time") < manager.GetWithdrawDelay()) {
        character->ChatPacket(1, locale_find("\074\261\346\265\345\076\040\305\273\305\360\307\321\040\310\304\040\045\144\300\317\040\300\314\263\273\277\241\264\302\040\261\346\265\345\270\246\040\270\270\265\351\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale),
                              quests.GetEventFlag("guild_withdraw_delay"));
        return;
    }
    if (character->GetGuild())
        return;
    TGuildCreateParameter parameter{};
    parameter.master = character;
    std::memcpy(parameter.name, data + 1, sizeof(parameter.name) - 1);
    if (!check_name(parameter.name)) {
        character->ChatPacket(1, "%s", locale_find("\300\373\307\325\307\317\301\366\040\276\312\300\272\040\261\346\265\345\040\300\314\270\247\040\300\324\264\317\264\331\056", g_iUseLocale));
        return;
    }
    if (!parameter.name[0] || !manager.CreateGuild(parameter)) {
        character->ChatPacket(1, "%s", locale_find("\074\261\346\265\345\076\040\261\346\265\345\040\273\375\274\272\277\241\040\275\307\306\320\307\317\277\264\275\300\264\317\264\331\056", g_iUseLocale));
        return;
    }
    character->ChatPacket(1, locale_find("\074\261\346\265\345\076\040\133\045\163\135\040\261\346\265\345\260\241\040\273\375\274\272\265\307\276\372\275\300\264\317\264\331\056", g_iUseLocale), parameter.name);
    character->PointChange(11, -200000, false, false);
    DBManager::instance().SendMoneyLog(5, character->GetPlayerID(), -200000);
    char hint[128];
    std::snprintf(hint, sizeof(hint), "GUILD_NAME %s MASTER %s", parameter.name, character->GetName());
    LogManager::instance().CharLog(character->GetPlayerID(), 0, 0, 0, "MAKE_GUILD", hint);
    if (g_iUseLocale != 0)
        character->RemoveSpecifyItem(0x111d5, 1);
}

void CInputMain::Hack(CHARACTER* character, const char* data)
{
    DESC* desc = character->GetDesc();
    if (!desc)
        return;
    char reason[129];
    std::memcpy(reason, data + 1, sizeof(reason) - 1);
    reason[sizeof(reason) - 1] = '\0';

    LogManager& logs = LogManager::instance();
    logs.HackLog(reason, desc->GetAccountTable().login, character->GetName(), desc->GetHostName());
    char escaped[257]{};
    const unsigned long length = logs.EscapeString(escaped, reason, std::strlen(reason));
    escaped[std::min<unsigned long>(length, sizeof(escaped) - 1)] = '\0';
    sys_log(0, "HACK_DETECT: %s %s", character->GetName(), escaped);
    if (std::strcmp(reason, "KERNEL DEBUGER") == 0 ||
        std::strcmp(reason, "DEBUGER") == 0 ||
        std::strcmp(reason, "SPEEDHACK Application") == 0)
        desc->DelayedDisconnect(number_ex(180, 300, const_cast<char*>("input_main.cpp"), 0x7f8));
}

int CInputMain::Shop(CHARACTER* character, const char* data, int bytes)
{
    if (bytes < 3)
        return -1;
    const BYTE subheader = static_cast<BYTE>(data[1]);
    if (test_server)
        sys_log(0, " CInputMain::Shop() ==> SubHeader %d", subheader);

    CShopManager& manager = CShopManager::instance();
    switch (subheader) {
    case 0:
        sys_log(1, "INPUT: %s SHOP: END", character->GetName());
        manager.StopShopping(character);
        return 0;
    case 1:
        if (bytes < 5)
            return -1;
        sys_log(1, "INPUT: %s SHOP: BUY %d", character->GetName(), static_cast<BYTE>(data[3]));
        manager.Buy(character, static_cast<BYTE>(data[3]));
        return 2;
    case 2:
        if (bytes < 4)
            return -1;
        sys_log(0, "INPUT: %s SHOP: SELL", character->GetName());
        manager.Sell(character, static_cast<BYTE>(data[2]), 0);
        return 1;
    case 3:
        if (bytes < 5)
            return -1;
        sys_log(0, "INPUT: %s SHOP: SELL2", character->GetName());
        manager.Sell(character, static_cast<BYTE>(data[2]), static_cast<BYTE>(data[3]));
        return 2;
    default:
        _sys_err("Shop", 0x28e, "CInputMain::Shop : Unknown subheader %d : %s",
                 subheader, character->GetName());
        return 0;
    }
}

namespace {
void LogStorageTransfer(CHARACTER* character, CItem* item, const char* operation)
{
    char hint[128];
    std::snprintf(hint, 127, "%s %lu", item->GetName(), static_cast<unsigned long>(item->GetCount()));
    LogManager::instance().ItemLog(character->GetPlayerID(), character->GetX(), character->GetY(),
                                  item->GetID(), operation, hint);
}
}

void CInputMain::SafeboxCheckin(CHARACTER* character, const char* data)
{
    if (!character->CanHandleItem(false, false))
        return;
    const BYTE source = static_cast<BYTE>(data[2]);
    const BYTE destination = static_cast<BYTE>(data[1]);
    CSafebox* storage = character->GetSafebox();
    CItem* item = character->GetItem(source);
    if (!storage || !item)
        return;
    if (!storage->IsEmpty(destination, item->GetSize())) {
        character->ChatPacket(1, "%s", locale_find("\074\303\242\260\355\076\040\277\305\261\346\040\274\366\040\276\370\264\302\040\300\247\304\241\300\324\264\317\264\331\056", g_iUseLocale));
        return;
    }
    if (item->GetVnum() == 0x11561) {
        character->ChatPacket(1, "%s", locale_find("\074\303\242\260\355\076\040\300\314\040\276\306\300\314\305\333\300\272\040\263\326\300\273\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return;
    }

    item->RemoveFromCharacter();
    character->SyncQuickslot(1, source, 0xff);
    if (!storage->Add(destination, item)) {
        item->AddToCharacter(character, source);
        return;
    }
    LogStorageTransfer(character, item, "SAFEBOX PUT");
}

void CInputMain::SafeboxCheckout(CHARACTER* character, const char* data, bool mall)
{
    if (!character->CanHandleItem(false, false))
        return;
    const BYTE source = static_cast<BYTE>(data[1]);
    const BYTE destination = static_cast<BYTE>(data[2]);
    CSafebox* storage = mall ? character->GetMall() : character->GetSafebox();
    CItem* item = storage ? storage->Get(source) : nullptr;
    if (!item || !character->IsEmptyItemGrid(destination, item->GetSize(), -1))
        return;
    if (!storage->Remove(source))
        return;
    if (!item->AddToCharacter(character, destination)) {
        storage->Add(source, item);
        return;
    }
    ITEM_MANAGER::instance().FlushDelayedSave(item);
    if (db_clientdesc) {
        const std::uint32_t id = item->GetID();
        db_clientdesc->DBPacket(0x23, 0, &id, sizeof(id));
    }
    LogStorageTransfer(character, item, mall ? "MALL GET" : "SAFEBOX GET");
}

void CInputMain::SafeboxItemMove(CHARACTER* character, const char* data)
{
    if (!character->CanHandleItem(false, false))
        return;
    if (CSafebox* storage = character->GetSafebox())
        storage->MoveItem(static_cast<BYTE>(data[1]), static_cast<BYTE>(data[2]), static_cast<BYTE>(data[3]));
}

int CInputMain::MyShop(CHARACTER* character, const char* data, int bytes)
{
    constexpr int headerSize = 35;
    if (bytes < headerSize)
        return -1;
    const BYTE count = static_cast<BYTE>(data[34]);
    const int payload = count * sizeof(TShopItemTable);
    if (bytes - headerSize < payload + 1)
        return -1;

    if (character->GetGold() >= 2000000000) {
        character->ChatPacket(1, "%s", locale_find("\274\322\300\257\040\265\267\300\314\040\062\060\276\357\263\311\300\273\040\263\321\276\356\040\260\305\267\241\270\246\040\307\333\274\366\260\241\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        sys_log(0, "MyShop ==> OverFlow Gold id %lu name %s ",
                static_cast<unsigned long>(character->GetPlayerID()), character->GetName());
    } else if (!character->IsStun() && !character->IsDead()) {
        if (character->GetExchange() || character->IsOpenSafebox() ||
            character->GetShopOwner() || character->IsCubeOpen()) {
            character->ChatPacket(1, "%s", locale_find("\264\331\270\245\040\260\305\267\241\301\337\300\317\260\346\277\354\040\260\263\300\316\273\363\301\241\300\273\040\277\255\274\366\260\241\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        } else {
            char sign[34]{};
            std::memcpy(sign, data + 1, sizeof(sign) - 1);
            TShopItemTable table[256];
            std::memcpy(table, data + headerSize, payload);
            sys_log(0, "MyShop count %d", count);
            character->OpenMyShop(sign, table, count);
        }
    }
    return payload;
}

void CInputMain::ItemToItem(CHARACTER* character, const char* data)
{
    if (character)
        character->UseItem(static_cast<BYTE>(data[1]), static_cast<BYTE>(data[2]));
}

void CInputMain::ItemUse(CHARACTER* character, const char* data)
{
    character->UseItem(static_cast<BYTE>(data[1]));
}

void CInputMain::Attack(CHARACTER* character, const char* data)
{
    character->GetDesc()->AssembleCRCMagicCube(data[6], data[7]);
    std::uint32_t vid;
    std::memcpy(&vid, data + 2, sizeof(vid));
    CHARACTER* victim = CHARACTER_MANAGER::instance().Find(vid);
    if (victim && victim != character && victim->GetCharType() != 1 && victim->GetCharType() != 3)
        character->Attack(victim, static_cast<BYTE>(data[1]));
}

void CInputMain::Target(CHARACTER* character, const char* data)
{
    std::uint32_t vid;
    std::memcpy(&vid, data + 1, sizeof(vid));
    if (building::CManager::instance().FindObjectByVID(vid)) {
        BYTE packet[6] = {0x3f};
        std::memcpy(packet + 1, &vid, sizeof(vid));
        character->GetDesc()->Packet(packet, sizeof(packet));
    } else {
        character->SetTarget(CHARACTER_MANAGER::instance().Find(vid));
    }
}

void CInputMain::ItemMove(CHARACTER* character, const char* data)
{
    if (character)
        character->MoveItem(static_cast<BYTE>(data[1]), static_cast<BYTE>(data[2]), static_cast<BYTE>(data[3]));
}

void CInputMain::ItemPickup(CHARACTER* character, const char* data)
{
    if (!character)
        return;
    std::uint32_t vid;
    std::memcpy(&vid, data + 1, sizeof(vid));
    character->PickupItem(vid);
}

void CInputMain::QuickslotAdd(CHARACTER* character, const char* data)
{
    TQuickslot slot;
    static_assert(sizeof(slot) == 2, "quickslot wire fields");
    std::memcpy(&slot, data + 2, sizeof(slot));
    character->SetQuickslot(static_cast<BYTE>(data[1]), slot);
}

void CInputMain::QuickslotDelete(CHARACTER* character, const char* data)
{
    character->DelQuickslot(static_cast<BYTE>(data[1]));
}

void CInputMain::QuickslotSwap(CHARACTER* character, const char* data)
{
    character->SwapQuickslot(static_cast<BYTE>(data[1]), static_cast<BYTE>(data[2]));
}

void CInputMain::Position(CHARACTER* character, const char* data)
{
    const BYTE position = static_cast<BYTE>(data[1]);
    if (position == 0)
        character->Standup();
    else if (position == 3 || position == 4)
        character->Sitdown(position == 4);
}

// 080fe350
void CInputMain::ItemDrop(CHARACTER* character, const char* data)
{
    if (!character)
        return;
    std::int32_t gold;
    std::memcpy(&gold, data + 2, sizeof(gold));
    if (gold)
        character->DropGold(gold);
    else
        character->DropItem(static_cast<BYTE>(data[1]), 0);
}

// 080fe390
void CInputMain::ItemDrop2(CHARACTER* character, const char* data)
{
    if (!character)
        return;
    std::int32_t gold;
    std::memcpy(&gold, data + 2, sizeof(gold));
    if (gold)
        character->DropGold(gold);
    else
        character->DropItem(static_cast<BYTE>(data[1]), static_cast<BYTE>(data[6]));
}

void CInputMain::QuestInputString(CHARACTER* character, const char* data)
{
    char message[65] = {};
    std::memcpy(message, data + 1, sizeof(message) - 1);
    const auto pid = character->GetPlayerID();
    sys_log(0, "QUEST InputString pid %lu msg %s", pid, message);
    quest::CQuestManager::instance().Input(pid, message);
}

void CInputMain::QuestConfirm(CHARACTER* character, const char* data)
{
    std::uint32_t pid;
    std::memcpy(&pid, data + 2, sizeof(pid));
    CHARACTER* waiting = CHARACTER_MANAGER::instance().FindByPID(pid);
    const auto answer = data[1] ? quest::CONFIRM_YES : quest::CONFIRM_NO;
    sys_log(0, "QuestConfirm from %s pid %lu name %s answer %d",
            character->GetName(), static_cast<unsigned long>(pid), waiting ? waiting->GetName() : "", answer);
    if (waiting)
        quest::CQuestManager::instance().Confirm(waiting->GetPlayerID(), answer, character->GetPlayerID());
}

void CInputMain::ScriptButton(CHARACTER* character, const char* data)
{
    std::uint32_t index;
    std::memcpy(&index, data + 1, sizeof(index));
    const auto pid = character->GetPlayerID();
    sys_log(0, "QUEST ScriptButton pid %d idx %u", static_cast<int>(pid), index);
    auto& quests = quest::CQuestManager::instance();
    quest::PC* player = quests.GetPCForce(pid);
    if (player && player->IsConfirmWait())
        quests.Confirm(pid, quest::CONFIRM_TIMEOUT, 0);
    else if (index & 0x80000000u)
        quests.QuestInfo(pid, index & 0x7fffffffu);
    else
        quests.QuestButton(pid, index);
}

void CInputMain::ScriptSelectItem(CHARACTER* character, const char* data)
{
    std::uint32_t selection;
    std::memcpy(&selection, data + 1, sizeof(selection));
    const auto pid = character->GetPlayerID();
    sys_log(0, "QUEST ScriptSelectItem pid %d answer %d", static_cast<int>(pid), static_cast<std::int32_t>(selection));
    quest::CQuestManager::instance().SelectItem(pid, selection);
}

// 080ffdb0
void CInputMain::ScriptAnswer(CHARACTER* character, const char* data)
{
    const DWORD pid = character->GetPlayerID();
    const BYTE answer = static_cast<BYTE>(data[1]);
    sys_log(0, "QUEST ScriptAnswer pid %d answer %d", static_cast<int>(pid), answer);
    auto& quests = quest::CQuestManager::instance();
    if (answer > 250)
        quests.Resume(pid);
    else
        quests.Select(pid, answer);
}

// 08100230
void CInputMain::Warp(CHARACTER* character, const char*)
{
    character->WarpEnd();
}

// 081025f0. The 2009 dispatcher has no route to this handler.
void CInputMain::Roulette(CHARACTER* character, const char* data)
{
    std::int32_t itemId, money;
    std::memcpy(&itemId, data, sizeof(itemId));
    std::memcpy(&money, data + sizeof(itemId), sizeof(money));

    if (character->GetGold() < itemId) {
        character->ChatPacket(1, "%s", locale_find("\xb1\xdd\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.", g_iUseLocale));
        return;
    }
    if (itemId == 0) {
        if (test_server)
            character->ChatPacket(1, "%s", locale_find("\xb8\xd3\xb4\xcf  \xb7\xea\xb7\xbf \xbd\xc3\xc0\xdb.", g_iUseLocale));
        CRoulette::instance().Roulette(character, money);
        return;
    }
    if (test_server)
        character->ChatPacket(1, "%s", locale_find("\xbe\xc6\xc0\xcc\xc5\xdb \xb7\xea\xb7\xbf \xbd\xc3\xc0\xdb.", g_iUseLocale));
    if (CItem* item = ITEM_MANAGER::instance().Find(static_cast<DWORD>(itemId)))
        CRoulette::instance().Roulette(character, item);
    else
        sys_log(0, "[Roulette] Invalid Item id[ %d ] Name[%s]", static_cast<int>(itemId), character->GetName());
}

void CInputMain::ItemGive(CHARACTER* character, const char* data)
{
    std::uint32_t vid;
    std::memcpy(&vid, data + 1, sizeof(vid));
    if (CHARACTER* target = CHARACTER_MANAGER::instance().Find(vid)) {
        character->GiveItem(target, static_cast<BYTE>(data[5]));
    } else {
        character->ChatPacket(1, locale_find("\276\306\300\314\305\333\300\273 \260\307\263\327\301\331 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
    }
}

void CInputMain::FlyTarget(CHARACTER* character, const char* data, BYTE header)
{
    std::uint32_t target;
    std::int32_t x;
    std::int32_t y;
    std::memcpy(&target, data + 1, sizeof(target));
    std::memcpy(&x, data + 5, sizeof(x));
    std::memcpy(&y, data + 9, sizeof(y));
    character->FlyTarget(target, x, y, header);
}

void CInputMain::Shoot(CHARACTER* character, const char* data)
{
    character->Shoot(static_cast<BYTE>(data[1]));
}

void CInputMain::OnClick(CHARACTER* character, const char* data)
{
    std::uint32_t vid;
    std::memcpy(&vid, data + 1, sizeof(vid));
    if (CHARACTER* target = CHARACTER_MANAGER::instance().Find(vid)) {
        target->OnClick(character);
    } else if (test_server) {
        _sys_err("OnClick", 0x29e, "CInputMain::OnClick %s.Click.NOT_EXIST_VID[%d]",
                 character->GetName(), static_cast<int>(vid));
    }
}

// 080ffb70
void CInputMain::UseSkill(CHARACTER* character, const char* data)
{
    std::uint32_t vnum;
    std::uint32_t vid;
    std::memcpy(&vnum, data + 1, sizeof(vnum));
    std::memcpy(&vid, data + 5, sizeof(vid));
    character->UseSkill(vnum, CHARACTER_MANAGER::instance().Find(vid), true);
}

// 08102330
void CInputMain::Refine(CHARACTER* character, const char* data)
{
    if (character->m_pkExchange || character->m_isOpenSafebox || character->m_pkChrShopOwner ||
        character->m_pkMyShop || character->m_pointsInstant.pCubeNpc) {
        character->ChatPacket(1, "%s", locale_find("\303\242\260\355,\260\305\267\241\303\242\265\356\300\314 \277\255\270\260 \273\363\305\302\277\241\274\255\264\302 \260\263\267\256\300\273 \307\322\274\366\260\241 \276\370\275\300\264\317\264\331", g_iUseLocale));
        character->ClearRefineMode();
        return;
    }

    const BYTE type = static_cast<BYTE>(data[2]);
    if (type == 255) {
        sys_log(0, "DoRefine cancel");
        character->ClearRefineMode();
        return;
    }

    const BYTE cell = static_cast<BYTE>(data[1]);
    if (cell > 90)
        return;
    CItem* item = character->GetItem(cell);
    if (!item)
        return;

    character->m_iRefineTime = thecore_pulse();
    sys_log(0, "DoRefine type %d item %s", type, item->GetName());
    switch (type) {
    case 0:
        sys_log(0, "DoRefine type 0 item %s", item->GetName());
        character->DoRefine(item, false);
        break;
    case 2:
    case 3:
    case 5:
    case 6:
        sys_log(0, "DoRefine type %d item %s", type, item->GetName());
        character->DoRefineWithScroll(item);
        break;
    case 4:
        if (character->GetQuestFlag("deviltower_zone.can_refine")) {
            character->DoRefine(item, true);
            character->SetQuestFlag("deviltower_zone.can_refine", 0);
        } else {
            character->ChatPacket(1, "\273\347\261\315 \305\270\277\366 \277\317\267\341 \272\270\273\363\300\272 \307\321\271\370\261\356\301\366 \273\347\277\353\260\241\264\311\307\325\264\317\264\331.");
        }
        break;
    }
}

// 080fe4a0
int CInputMain::Messenger(CHARACTER* character, const char* data, int bytes)
{
    if (bytes < 2)
        return -1;

    constexpr int nameSize = 24;
    constexpr BYTE blockInvite = 0x10;
    const BYTE subheader = static_cast<BYTE>(data[1]);
    const char* payload = data + 2;
    const int remaining = bytes - 2;
    const char* gmNotice = "<\270\336\275\305\301\256> \277\356\277\265\300\332\264\302 \270\336\275\305\301\256\277\241 \303\337\260\241\307\322 \274\366 \276\370\275\300\264\317\264\331.";
    const char* blockNotice = "\273\363\264\353\271\346\300\314 \270\336\275\305\301\256 \303\337\260\241 \260\305\272\316 \273\363\305\302\300\324\264\317\264\331.";

    switch (subheader) {
    case 0: {
        if (remaining < 4)
            return -1;
        std::uint32_t vid;
        std::memcpy(&vid, payload, sizeof(vid));
        CHARACTER* target = CHARACTER_MANAGER::instance().Find(vid);
        if (!target || character->IsObserverMode())
            return 4;
        if (target->GetBlockMode() & blockInvite) {
            character->ChatPacket(1, "%s", locale_find(blockNotice, g_iUseLocale));
            return 4;
        }
        DESC* targetDesc = target->GetDesc();
        if (!targetDesc)
            return 4;
        if (!character->GetGMLevel() && target->GetGMLevel())
            character->ChatPacket(1, "%s", locale_find(gmNotice, g_iUseLocale));
        else if (character->GetDesc() != targetDesc)
            MessengerManager::instance().RequestToAdd(character, target);
        return 4;
    }
    case 1:
    case 2: {
        if (remaining < nameSize)
            return -1;
        char name[nameSize + 1]{};
        std::memcpy(name, payload, nameSize);
        if (subheader == 2) {
            MessengerManager::instance().RemoveFromList(character->GetName(), name);
            return nameSize;
        }

        if (!character->GetGMLevel() && gm_get_level(name, nullptr)) {
            character->ChatPacket(1, "%s", locale_find(gmNotice, g_iUseLocale));
            return nameSize;
        }

        CHARACTER* target = CHARACTER_MANAGER::instance().FindPC(name);
        if (!target)
            character->ChatPacket(1, locale_find("%s \264\324\300\272 \301\242\274\323\265\307 \300\326\301\366 \276\312\275\300\264\317\264\331.", g_iUseLocale), name);
        else if (target != character) {
            if (target->GetBlockMode() & blockInvite)
                character->ChatPacket(1, "%s", locale_find(blockNotice, g_iUseLocale));
            else
                MessengerManager::instance().RequestToAdd(character, target);
        }
        return nameSize;
    }
    default:
        _sys_err("Messenger", 0x247, "CInputMain::Messenger : Unknown subheader %d : %s",
                 subheader, character->GetName());
        return 0;
    }
}

// 081027f0
int CInputMain::SMS(CHARACTER* character, const char* data, int bytes)
{
    constexpr int headerSize = 28;
    constexpr int fee = 2000;
    if (bytes < headerSize)
        return -1;

    std::uint16_t size;
    std::memcpy(&size, data + 1, sizeof(size));
    if (size < headerSize) {
        character->GetDesc()->SetPhase(0);
        return -1;
    }
    if (bytes < size)
        return -1;

    char recipient[25]{};
    std::memcpy(recipient, data + 3, sizeof(recipient) - 1);
    MessengerManager& messenger = MessengerManager::instance();
    const int state = messenger.GetSMSState(recipient);
    const char* notice;
    if (state == 1)
        notice = "\307\366\300\347 \277\302\266\363\300\316 \273\363\305\302 \300\324\264\317\264\331. \261\323\270\273\300\273 \300\314\277\353\307\317\274\274\277\344.";
    else if (state == 2)
        notice = "\307\332\265\345\306\371 \271\370\310\243\260\241 \265\356\267\317\265\307\276\356 \300\326\301\366 \276\312\275\300\264\317\264\331.";
    else if (character->GetLevel() < 20)
        notice = "\271\256\300\332 \270\336\274\274\301\366 \300\374\274\333\300\272 \267\271\272\247 20\272\316\305\315 \307\322 \274\366 \300\326\275\300\264\317\264\331.";
    else if (character->GetGold() < fee)
        notice = "\271\256\300\332 \270\336\274\274\301\366 \300\374\274\333\277\241\264\302 2000\277\370\300\314 \307\312\277\344 \307\325\264\317\264\331.";
    else {
        character->PointChange(POINT_GOLD, -fee);
        DBManager::instance().SendMoneyLog(6, 4, -fee);
        char message[51]{};
        std::memcpy(message, data + headerSize, std::min<int>(50, size - headerSize));
        char text[80];
        std::snprintf(text, sizeof(text), "%s%s:%s", g_stSMSPrefix.c_str(), character->GetName(), message);
        messenger.SendSMS(character->GetName(), recipient, text);
        notice = "\271\256\300\332 \270\336\274\274\301\366\270\246 \300\374\274\333 \307\337\275\300\264\317\264\331.";
    }

    character->ChatPacket(1, "%s", locale_find(notice, g_iUseLocale));
    return size - headerSize;
}
