#include "char.h"
#include "cmd.h"
#include "locale.hpp"
#include "packet.h"
#include "buffer_manager.h"
#include "utils.h"
#include "wedding.h"
#include "../../libthecore/include/log.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>
#include <utility>

namespace {
struct Emotion {
    const char* local;
    const char* client;
    unsigned flags;
    float extra_delay;
};
const Emotion emotion_types[] = {
    {"\305\260\275\272", "french_kiss", 58, 2.0f},
    {"\273\307\273\307", "kiss", 58, 1.5f},
    {"\265\373\261\315", "slap", 18, 1.5f},
    {"\271\332\274\366", "clap", 0, 1.0f},
    {"\277\315", "cheer1", 0, 1.0f},
    {"\270\270\274\274", "cheer2", 0, 1.0f},
    {"\264\355\275\272\061", "dance1", 0, 1.0f},
    {"\264\355\275\272\062", "dance2", 0, 1.0f},
    {"\264\355\275\272\063", "dance3", 0, 1.0f},
    {"\264\355\275\272\064", "dance4", 0, 1.0f},
    {"\264\355\275\272\065", "dance5", 0, 1.0f},
    {"\303\340\307\317", "congratulation", 0, 1.0f},
    {"\277\353\274\255", "forgive", 0, 1.0f},
    {"\310\255\263\262", "angry", 0, 1.0f},
    {"\300\257\310\244", "attractive", 0, 1.0f},
    {"\275\275\307\304", "sad", 0, 1.0f},
    {"\272\352\262\364", "shy", 0, 1.0f},
    {"\300\300\277\370", "cheerup", 0, 1.0f},
    {"\301\372\305\365", "banter", 0, 1.0f},
    {"\261\342\273\335", "joy", 0, 1.0f},
    {"\n", "\n", 0, 0.0f},
};
std::set<std::pair<std::uint32_t, std::uint32_t>> allowed;

}

// 080986b0
bool CHARACTER_CanEmotion(CHARACTER& character)
{
    return marriage::WeddingManager::instance().IsWeddingMap(character.GetMapIndex()) ||
           character.IsEquipUniqueItem(0x11563) || character.IsEquipUniqueItem(0x11579);
}

namespace {
bool IsFemale(CHARACTER* character)
{
    return GET_SEX(character) == SEX_FEMALE;
}

void Notice(CHARACTER* character, const char* message)
{
    character->ChatPacket(1, "%s", locale_find(message, g_iUseLocale));
}
}

void do_emotion_allow(CHARACTER* character, char* argument, int, int)
{
    if (character->GetArena()) {
        Notice(character, "\264\353\267\303\300\345\277\241\274\255\040\273\347\277\353\307\317\275\307\040\274\366\040\276\370\275\300\264\317\264\331\056");
        return;
    }
    char target[256];
    one_argument(argument, target);
    if (target[0])
        allowed.emplace(static_cast<std::uint32_t>(character->GetVID()),
                        static_cast<std::uint32_t>(std::atoi(target)));
}

void do_emotion(CHARACTER* character, char* argument, int command, int)
{
    if (character->IsHorseRiding()) {
        Notice(character, "\270\273\300\273\040\305\272\040\273\363\305\302\277\241\274\255\040\260\250\301\244\307\245\307\366\300\273\040\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056");
        return;
    }
    const char* commandName = GetCommandName(command);
    if (!commandName)
        return;

    const Emotion* entry = emotion_types;
    while (entry->local[0] != '\n' &&
           std::strcmp(commandName, entry->local) && std::strcmp(commandName, entry->client))
        ++entry;
    if (entry->local[0] == '\n') {
        _sys_err("do_emotion", 0x95, "cannot find emotion");
        return;
    }
    const Emotion& emotion = *entry;
    if (!CHARACTER_CanEmotion(*character)) {
        Notice(character, "\277\255\301\244\300\307\040\260\241\270\351\300\273\040\302\370\277\353\275\303\277\241\270\270\040\307\322\040\274\366\040\300\326\275\300\264\317\264\331\056");
        return;
    }
    if ((emotion.flags & 4) && !IsFemale(character)) {
        Notice(character, "\277\251\300\332\270\270\040\307\322\040\274\366\040\300\326\275\300\264\317\264\331\056");
        return;
    }
    char targetName[256];
    one_argument(argument, targetName);
    CHARACTER* target = targetName[0] ? character->FindCharacterInView(targetName, (emotion.flags & 2) != 0) : nullptr;
    if (!target && (emotion.flags & 3)) {
        Notice(character, "\261\327\267\261\040\273\347\266\367\300\314\040\276\370\275\300\264\317\264\331\056");
        return;
    }
    if (target) {
        if (!target->GetDesc() || target == character)
            return;
        if (target->IsHorseRiding()) {
            Notice(character, "\270\273\300\273\040\305\272\040\273\363\264\353\277\315\040\260\250\301\244\307\245\307\366\300\273\040\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056");
            return;
        }
        const auto dy = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(character->GetY()) - static_cast<std::uint32_t>(target->GetY()));
        const auto dx = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(character->GetX()) - static_cast<std::uint32_t>(target->GetX()));
        const int distance = DISTANCE_APPROX(dx, dy);
        if (distance < 10 || distance > 500) {
            Notice(character, distance < 10 ? "\263\312\271\253\040\260\241\261\356\300\314\040\300\326\275\300\264\317\264\331\056" : "\263\312\271\253\040\270\326\270\256\040\300\326\275\300\264\317\264\331");
            return;
        }
        if ((emotion.flags & 8) && IsFemale(character) == IsFemale(target)) {
            Notice(character, "\300\314\274\272\260\243\277\241\270\270\040\307\322\040\274\366\040\300\326\275\300\264\317\264\331\056");
            return;
        }
        if (emotion.flags & 2) {
            const auto victim = static_cast<std::uint32_t>(target->GetVID());
            const auto actor = static_cast<std::uint32_t>(character->GetVID());
            if (!allowed.count({victim, actor})) {
                Notice(character, "\300\314\040\307\340\265\277\300\272\040\273\363\310\243\265\277\300\307\040\307\317\277\241\040\260\241\264\311\040\307\325\264\317\264\331\056");
                return;
            }
            allowed.emplace(actor, victim);
        }
    }
    const auto victim = target ? static_cast<std::uint32_t>(target->GetVID()) : 0U;
    const auto actor = static_cast<std::uint32_t>(character->GetVID());
    char text[256];
    const int size = std::snprintf(text, sizeof(text), "%s %lu %lu", emotion.client,
                                   static_cast<unsigned long>(actor), static_cast<unsigned long>(victim));
    if (size < 0 || size >= static_cast<int>(sizeof(text)))
        return;
    TPacketGCChat packet{};
    packet.header = 4;
    packet.size = static_cast<WORD>(sizeof(packet) + size);
    packet.type = 5;
    TEMP_BUFFER buffer;
    buffer.write(&packet, sizeof(packet));
    buffer.write(text, size);
    const int packetSize = buffer.size();
    const void* packetData = buffer.read_peek();
    character->PacketAround(packetData, packetSize);
    if (target)
        sys_log(1, "ACTION: %s TO %s", emotion.local, target->GetName());
    else
        sys_log(1, "ACTION: %s", emotion.local);
}
