#include "input.h"
#include "char.h"
#include "desc.h"
#include "desc_manager.h"
#include "locale.hpp"
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {
int PacketLength(BYTE header)
{
    switch (header) {
    case 0x12: return 37;
    case 0x13: return 33;
    case 0x14: return 41;
    default: return 0;
    }
}

void TeenNotice(CHARACTER& character, int hour)
{
    switch (hour) {
    case 0:
        character.SetOverTime(OT_NONE);
        break;
    case 1:
    case 2:
        character.ChatPacket(2, LC_TEXT("\264\347\275\305\300\307 \264\251\300\373 \277\302\266\363\300\316 \275\303\260\243\300\314 \300\314\271\314 %d\275\303\260\243\300\314 \301\366\263\265\275\300\264\317\264\331."), hour);
        character.SetOverTime(OT_NONE);
        break;
    case 3:
        character.ChatPacket(2, LC_TEXT("\264\347\275\305\300\307 \264\251\300\373 \277\302\266\363\300\316 \275\303\260\243\300\314 \300\314\271\314 %d\275\303\260\243\300\314 \265\307\276\372\275\300\264\317\264\331,"), hour);
        character.ChatPacket(2, "%s", LC_TEXT("\301\266\274\323\310\367 \301\242\274\323\300\273 \301\276\267\341\307\317\275\305 \310\304 \260\307\260\255\300\273 \300\247\307\330 \310\336\275\304\300\273 \303\353\307\330\301\326\275\303\261\342\271\331\266\370\264\317\264\331."));
        character.SetOverTime(OT_3HOUR);
        break;
    case 4:
        character.ChatPacket(2, "%s", LC_TEXT("\264\347\275\305\300\272 \300\314\271\314 \260\324\300\323 \307\307\267\316 \273\363\305\302\277\241 \265\351\276\356\274\271\300\270\270\347,"));
        character.ChatPacket(2, "%s", LC_TEXT("\260\324\300\323 \263\273\300\307 \274\366\300\315\300\314 \301\244\273\363\304\241\300\307 50%\267\316 \307\317\307\342\265\313\264\317\264\331."));
        character.ChatPacket(2, "%s", LC_TEXT("\264\347\275\305\300\307 \260\307\260\255\300\273 \300\247\307\330 \301\266\274\323\310\367 \301\242\274\323\300\273 \301\276\267\341\307\317\275\303\260\355"));
        character.ChatPacket(2, "%s", LC_TEXT("\310\336\275\304 \271\327 \307\320\276\367\277\241 \277\255\301\337\307\330\301\326\275\312\275\303\277\300."));
        character.SetOverTime(OT_3HOUR);
        break;
    default:
        character.ChatPacket(2, "%s", LC_TEXT("\264\347\275\305\300\272 \300\314\271\314 \277\302\300\374\307\317\301\366 \270\370\307\321 \260\324\300\323 \275\303\260\243\277\241 \265\351\276\356\274\271\275\300\264\317\264\331."));
        character.ChatPacket(2, "%s", LC_TEXT("\260\307\260\255\300\273 \300\247\307\330 \301\266\274\323\310\367 \301\242\274\323\300\273 \301\276\267\341\307\317\275\305 \310\304 \310\336\275\304\300\273 \303\353\307\330\301\326\275\312\275\303\277\300,"));
        character.ChatPacket(2, "%s", LC_TEXT("\261\327\267\270\301\366 \276\312\300\270\270\351 \260\307\260\255 \273\363\277\241 \307\307\307\330\270\246 \300\324\300\273 \274\366 \300\326\300\270\270\347 \260\324\300\323 \263\273\300\307 \274\366\304\241\264\302 0\300\314 \265\313\264\317\264\331."));
        character.ChatPacket(2, "%s", LC_TEXT("\264\251\300\373 \277\300\307\301\266\363\300\316 \275\303\260\243\300\314 5\275\303\260\243\300\314 \265\307\270\351 \301\244\273\363\300\270\267\316 \265\271\276\306\277\311\264\317\264\331."));
        character.SetOverTime(OT_5HOUR);
        break;
    }
}

void FormatTime(int seconds, char (&buffer)[64])
{
    const int hours = seconds / 3600;
    if (hours < 1)
        std::snprintf(buffer, sizeof(buffer), LC_TEXT("%d\272\320"), seconds / 60);
    else
        std::snprintf(buffer, sizeof(buffer), LC_TEXT("%d\275\303\260\243"), hours);
}

void LoginNotice(CHARACTER& character, int onTime, int offTime)
{
    char onText[64], offText[64];
    FormatTime(onTime, onText);
    FormatTime(offTime, offText);
    if (onTime == 0) {
        character.ChatPacket(2, LC_TEXT("\264\347\275\305\300\307 \264\251\300\373 \277\300\307\301\266\363\300\316 \275\303\260\243\300\272 %s\300\324\264\317\264\331."), offText);
        character.ChatPacket(2, "%s", LC_TEXT("\264\347\275\305\300\307 \264\251\300\373 \277\302\266\363\300\316 \275\303\260\243\300\272 0\300\314 \265\307\276\372\275\300\264\317\264\331. 100% \310\277\260\372\270\246 \276\362\300\270\275\307 \274\366 \300\326\275\300\264\317\264\331."));
    } else {
        character.ChatPacket(2, LC_TEXT("\264\347\275\305\300\307 \264\251\300\373 \277\300\307\301\266\363\300\316 \275\303\260\243\300\272 %s\300\314\270\347, \264\251\300\373 \277\302\266\363\300\316 \275\303\260\243\300\272 %s\300\324\264\317\264\331."), offText, onText);
    }
}

void DispatchTeen(BYTE header, const char* body)
{
    char login[31]{};
    std::memcpy(login, body, 30);
    DESC* descriptor = DESC_MANAGER::instance().FindByLoginName(login);
    if (!descriptor)
        return;
    if (header == 0x13) {
        descriptor->SetPhase(0);
        return;
    }
    CHARACTER* character = descriptor->GetCharacter();
    if (!character)
        return;

    std::int32_t first;
    std::memcpy(&first, body + 30, sizeof(first));
    if (header == 0x12) {
        TeenNotice(*character, first);
    } else {
        std::int32_t second;
        std::memcpy(&second, body + 34, sizeof(second));
        LoginNotice(*character, first, second);
    }
}
}

// 0813b360
void CInputTeen::SetStep(int step)
{
    m_step = step;
}

// 0813b370
bool CInputTeen::Process(DESC* desc, const void* raw, int bytes, int& consumed)
{
    const char* data = static_cast<const char*>(raw);
    m_iBufferLeft = bytes;
    if (m_step == 0) {
        if (!ProcessHandshake(desc, data, bytes, consumed))
            return true;
        data += 4;
    }
    if (m_step == 1)
        return ProcessMain(desc, data, m_iBufferLeft, consumed);
    return true;
}

// 0813b3e0
bool CInputTeen::ProcessHandshake(DESC* desc, const void* data, int bytes, int& consumed)
{
    if (bytes < 4)
        return false;

    std::uint32_t challenge;
    std::memcpy(&challenge, data, sizeof(challenge));
    const std::uint32_t response = challenge ^ 0x6ab3d224u;
    m_iBufferLeft -= 4;
    consumed += 4;
    SetStep(1);
    desc->Packet(&response, sizeof(response));
    return true;
}

// 0813bb70
bool CInputTeen::ProcessMain(DESC* desc, const void* raw, int bytes, int& consumed)
{
    const char* data = static_cast<const char*>(raw);
    m_iBufferLeft = bytes;
    while (m_iBufferLeft >= 3) {
        const BYTE header = static_cast<BYTE>(data[0]);
        const int length = PacketLength(header);
        if (!length) {
            desc->SetPhase(0);
            return true;
        }
        if (m_iBufferLeft < length)
            break;
        DispatchTeen(header, data + 3);
        data += length;
        consumed += length;
        m_iBufferLeft -= length;
    }
    return true;
}

BYTE CInputTeen::GetType()
{
    return 10;
}

int CInputTeen::Analyze(DESC*, BYTE, const char*)
{
    return 0;
}

