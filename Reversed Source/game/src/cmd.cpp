#include "cmd.h"
#include "char_manager.h"
#include "messenger_manager.h"
#include "char.h"
#include "config.h"
#include "locale.hpp"
#include "locale_service.h"
#include "utils.h"
#include "../../libthecore/include/log.h"
#include <cstring>
#include <cctype>
#include <strings.h>

void do_goto(CHARACTER*, char*, int, int);
void do_cube(CHARACTER*, char*, int, int);
void do_invisibility(CHARACTER*, char*, int, int);
void do_set_skill_group(CHARACTER*, char*, int, int);
void do_setskill(CHARACTER*, char*, int, int);
void do_setskillother(CHARACTER*, char*, int, int);
void do_set_skill_point(CHARACTER*, char*, int, int);
void do_cooltime(CHARACTER*, char*, int, int);
void do_polymorph(CHARACTER*, char*, int, int);
void do_polymorph_item(CHARACTER*, char*, int, int);
void do_mount_test(CHARACTER*, char*, int, int);
void do_getqf(CHARACTER*, char*, int, int);
void do_setqf(CHARACTER*, char*, int, int);
void do_delqf(CHARACTER*, char*, int, int);
void do_affect_remove(CHARACTER*, char*, int, int);
void do_flush(CHARACTER*, char*, int, int);
void do_reset_subskill(CHARACTER*, char*, int, int);
void do_stat_plus_amount(CHARACTER*, char*, int, int);
void do_user(CHARACTER*, char*, int, int);
void do_item_purge(CHARACTER*, char*, int, int);
void do_purge(CHARACTER*, char*, int, int);
void do_mob_map(CHARACTER*, char*, int, int);
void do_mob_coward(CHARACTER*, char*, int, int);
void do_mob_aggresive(CHARACTER*, char*, int, int);
void do_group_random(CHARACTER*, char*, int, int);
void do_group(CHARACTER*, char*, int, int);
void do_mob(CHARACTER*, char*, int, int);
void do_add_socket(CHARACTER*, char*, int, int);
void do_add_attr(CHARACTER*, char*, int, int);
void do_change_attr(CHARACTER*, char*, int, int);
void do_kill(CHARACTER*, char*, int, int);
void do_level(CHARACTER*, char*, int, int);
void do_item(CHARACTER*, char*, int, int);
void do_warp(CHARACTER*, char*, int, int);
void do_shutdown(CHARACTER*, char*, int, int);
void do_inputall(CHARACTER*, char*, int, int);
void do_transfer(CHARACTER*, char*, int, int);
void do_disconnect(CHARACTER*, char*, int, int);
void do_big_notice(CHARACTER*, char*, int, int);
void do_map_notice(CHARACTER*, char*, int, int);
void do_notice(CHARACTER*, char*, int, int);
void do_who(CHARACTER*, char*, int, int);
void do_hair(CHARACTER*, char*, int, int);
void do_mobile_auth(CHARACTER*, char*, int, int);
void do_mobile(CHARACTER*, char*, int, int);
void do_emotion(CHARACTER*, char*, int, int);
void do_emotion_allow(CHARACTER*, char*, int, int);
void do_reload(CHARACTER*, char*, int, int);
void do_mount(CHARACTER*, char*, int, int);
void do_unmount(CHARACTER*, char*, int, int);
void do_user_horse_ride(CHARACTER*, char*, int, int);
void do_user_horse_back(CHARACTER*, char*, int, int);
void do_user_horse_feed(CHARACTER*, char*, int, int);
void do_safebox_close(CHARACTER*, char*, int, int);
void do_safebox_password(CHARACTER*, char*, int, int);
void do_safebox_change_password(CHARACTER*, char*, int, int);
void do_mall_password(CHARACTER*, char*, int, int);
void do_mall_close(CHARACTER*, char*, int, int);
void do_close_shop(CHARACTER*, char*, int, int);
void do_skillup(CHARACTER*, char*, int, int);
void do_guildskillup(CHARACTER*, char*, int, int);
void do_save_attribute_to_image(CHARACTER*, char*, int, int);
void do_set(CHARACTER*, char*, int, int);
void do_test(CHARACTER*, char*, int, int);
void do_detaillog(CHARACTER*, char*, int, int);
void do_monsterlog(CHARACTER*, char*, int, int);
void do_pcbang_env(CHARACTER*, char*, int, int);
void do_pcbang_info(CHARACTER*, char*, int, int);
void do_pcbang_update(CHARACTER*, char*, int, int);
void do_private(CHARACTER*, char*, int, int);
void do_inventory(CHARACTER*, char*, int, int);
void do_threeway_war_info(CHARACTER*, char*, int, int);
void do_threeway_war_myinfo(CHARACTER*, char*, int, int);
void do_siege(CHARACTER*, char*, int, int);
void do_frog(CHARACTER*, char*, int, int);
void do_reset(CHARACTER*, char*, int, int);
void do_greset(CHARACTER*, char*, int, int);
void do_advance(CHARACTER*, char*, int, int);
void do_book(CHARACTER*, char*, int, int);
void do_respawn(CHARACTER*, char*, int, int);
void do_socket_item(CHARACTER*, char*, int, int);
void do_console(CHARACTER*, char*, int, int);
void do_makeguild(CHARACTER*, char*, int, int);
void do_deleteguild(CHARACTER*, char*, int, int);
void do_safebox_size(CHARACTER*, char*, int, int);
void do_break_marriage(CHARACTER*, char*, int, int);
void do_xmas(CHARACTER*, char*, int, int);
void do_weaken(CHARACTER*, char*, int, int);
void do_stat_reset(CHARACTER*, char*, int, int);
void do_state(CHARACTER*, char*, int, int);
void do_stun(CHARACTER*, char*, int, int);
void do_slow(CHARACTER*, char*, int, int);
void do_forgetme(CHARACTER*, char*, int, int);
void do_aggregate(CHARACTER*, char*, int, int);
void do_attract_ranger(CHARACTER*, char*, int, int);
void do_pull_monster(CHARACTER*, char*, int, int);
void do_observer(CHARACTER*, char*, int, int);
void do_effect(CHARACTER*, char*, int, int);
void do_refine_rod(CHARACTER*, char*, int, int);
void do_refine_pick(CHARACTER*, char*, int, int);
void do_max_pick(CHARACTER*, char*, int, int);
void do_fishing_simul(CHARACTER*, char*, int, int);
void do_priv_empire(CHARACTER*, char*, int, int);
void do_priv_guild(CHARACTER*, char*, int, int);
void do_block_chat(CHARACTER*, char*, int, int);
void do_block_chat_list(CHARACTER*, char*, int, int);
void do_qf(CHARACTER*, char*, int, int);
void do_clear_quest(CHARACTER*, char*, int, int);
void do_set_state(CHARACTER*, char*, int, int);
void do_gwlist(CHARACTER*, char*, int, int);
void do_stop_guild_war(CHARACTER*, char*, int, int);
void do_cancel_guild_war(CHARACTER*, char*, int, int);
void do_guild_state(CHARACTER*, char*, int, int);
void do_itemlog(CHARACTER*, char*, int, int);
void do_horse_state(CHARACTER*, char*, int, int);
void do_horse_level(CHARACTER*, char*, int, int);
void do_horse_ride(CHARACTER*, char*, int, int);
void do_horse_summon(CHARACTER*, char*, int, int);
void do_horse_unsummon(CHARACTER*, char*, int, int);
void do_horse_set_stat(CHARACTER*, char*, int, int);
void do_show_arena_list(CHARACTER*, char*, int, int);
void do_end_all_duel(CHARACTER*, char*, int, int);
void do_end_duel(CHARACTER*, char*, int, int);
void do_duel(CHARACTER*, char*, int, int);
void do_build(CHARACTER*, char*, int, int);
void do_oxevent_open(CHARACTER*, char*, int, int);
void do_oxevent_door(CHARACTER*, char*, int, int);
void do_oxevent_close(CHARACTER*, char*, int, int);
void do_oxevent_end_force(CHARACTER*, char*, int, int);
void do_oxevent_quiz(CHARACTER*, char*, int, int);
void do_oxevent_show_quiz(CHARACTER*, char*, int, int);
void do_oxevent_log(CHARACTER*, char*, int, int);
void do_oxevent_get_attender(CHARACTER*, char*, int, int);
void do_oxevent_give_item(CHARACTER*, char*, int, int);
void do_war(CHARACTER*, char*, int, int);
void do_nowar(CHARACTER*, char*, int, int);
void do_party_request(CHARACTER*, char*, int, int);
void do_party_request_accept(CHARACTER*, char*, int, int);
void do_party_request_deny(CHARACTER*, char*, int, int);
void do_ungroup(CHARACTER*, char*, int, int);
void do_observer_exit(CHARACTER*, char*, int, int);
void do_view_equip(CHARACTER*, char*, int, int);
void do_cmd(CHARACTER*, char*, int, int);
void do_inputall(CHARACTER*, char*, int, int);
void do_set_walk_mode(CHARACTER*, char*, int, int);
void do_set_run_mode(CHARACTER*, char*, int, int);
void do_pkmode(CHARACTER*, char*, int, int);
void do_setblockmode(CHARACTER*, char*, int, int);
void do_restart(CHARACTER*, char*, int, int);
void do_stat(CHARACTER*, char*, int, int);
void do_stat_minus(CHARACTER*, char*, int, int);
void do_pvp(CHARACTER*, char*, int, int);
void do_messenger_auth(CHARACTER*, char*, int, int);
void do_roulette(CHARACTER*, char*, int, int);
void do_event_flag(CHARACTER*, char*, int, int);
void do_get_event_flag(CHARACTER*, char*, int, int);

namespace {
struct Command {
    const char* name;
    int index, subcommand, position, gmLevel;
    bool exact;
    void (*handler)(CHARACTER*, char*, int, int);
};

// The original ELF order governs prefix matching, including commands still being recovered.
Command commands[] = {
    {"who", 1, 0, 0, 5, false, do_who},
    {"war", 2, 0, 0, 0, false, do_war},
    {"warp", 3, 0, 0, 1, false, do_warp},
    {"user", 4, 0, 0, 3, false, do_user},
    {"notice", 5, 0, 0, 3, false, do_notice},
    {"notice_map", 6, 0, 0, 1, false, do_map_notice},
    {"big_notice", 7, 0, 0, 3, false, do_big_notice},
    {"nowar", 8, 0, 0, 0, false, do_nowar},
    {"purge", 9, 0, 0, 2, false, do_purge},
    {"weaken", 10, 0, 0, 4, false, do_weaken},
    {"dc", 11, 0, 0, 1, false, do_disconnect},
    {"transfer", 12, 0, 0, 1, false, do_transfer},
    {"goto", 13, 0, 0, 1, false, do_goto},
    {"level", 14, 0, 0, 1, false, do_level},
    {"eventflag", 15, 0, 0, 3, false, do_event_flag},
    {"geteventflag", 16, 0, 0, 1, false, do_get_event_flag},
    {"item", 17, 0, 0, 4, false, do_item},
    {"mob", 18, 0, 0, 3, false, do_mob},
    {"ma", 19, 0, 0, 3, false, do_mob_aggresive},
    {"mc", 20, 0, 0, 3, false, do_mob_coward},
    {"mm", 21, 0, 0, 3, false, do_mob_map},
    {"kill", 22, 0, 0, 3, false, do_kill},
    {"ipurge", 23, 0, 0, 3, false, do_item_purge},
    {"group", 24, 0, 0, 3, false, do_group},
    {"grrandom", 25, 0, 0, 3, false, do_group_random},
    {"set", 26, 0, 0, 5, false, do_set},
    {"reset", 27, 0, 0, 3, false, do_reset},
    {"greset", 28, 0, 0, 3, false, do_greset},
    {"advance", 29, 0, 0, 4, false, do_advance},
    {"book", 30, 0, 0, 5, false, do_book},
    {"console", 31, 0, 0, 1, false, do_console},
    {"shutdow", 32, 0, 0, 3, false, do_inputall},
    {"shutdown", 33, 0, 0, 3, false, do_shutdown},
    {"stat", 34, 0, 0, 0, false, do_stat},
    {"stat-", 35, 0, 0, 0, false, do_stat_minus},
    {"stat_reset", 36, 0, 0, 1, false, do_stat_reset},
    {"state", 37, 0, 0, 1, false, do_state},
    {"stun", 38, 0, 0, 1, false, do_stun},
    {"slow", 39, 0, 0, 1, false, do_slow},
    {"respawn", 40, 0, 0, 2, false, do_respawn},
    {"makeguild", 41, 0, 0, 3, false, do_makeguild},
    {"deleteguild", 42, 0, 0, 3, false, do_deleteguild},
    {"mount", 43, 0, 6, 0, false, do_mount},
    {"restart_here", 44, 1, 0, 0, false, do_restart},
    {"restart_town", 45, 0, 0, 0, false, do_restart},
    {"phase_selec", 46, 0, 0, 0, false, do_inputall},
    {"phase_select", 47, 2, 0, 0, true, do_cmd},
    {"qui", 48, 0, 0, 0, false, do_inputall},
    {"quit", 49, 1, 0, 0, true, do_cmd},
    {"logou", 50, 0, 0, 0, false, do_inputall},
    {"logout", 51, 0, 0, 0, true, do_cmd},
    {"skillup", 52, 0, 0, 0, false, do_skillup},
    {"gskillup", 53, 0, 0, 0, false, do_guildskillup},
    {"pvp", 54, 0, 0, 0, false, do_pvp},
    {"safebox", 55, 0, 0, 3, false, do_safebox_size},
    {"safebox_close", 56, 0, 0, 0, false, do_safebox_close},
    {"safebox_passwor", 57, 0, 0, 0, false, do_inputall},
    {"safebox_password", 58, 0, 0, 0, false, do_safebox_password},
    {"safebox_change_passwor", 59, 0, 0, 0, false, do_inputall},
    {"safebox_change_password", 60, 0, 0, 0, false, do_safebox_change_password},
    {"mall_passwor", 61, 0, 0, 0, false, do_inputall},
    {"mall_password", 62, 0, 0, 0, false, do_mall_password},
    {"mall_close", 63, 0, 0, 0, false, do_mall_close},
    {"ungroup", 64, 0, 0, 0, false, do_ungroup},
    {"refine_rod", 65, 0, 0, 5, false, do_refine_rod},
    {"refine_pick", 66, 0, 0, 5, false, do_refine_pick},
    {"max_pick", 67, 0, 0, 5, false, do_max_pick},
    {"fish_simul", 68, 0, 0, 5, false, do_fishing_simul},
    {"invisible", 69, 0, 0, 1, false, do_invisibility},
    {"qf", 70, 0, 0, 5, false, do_qf},
    {"clear_quest", 71, 0, 0, 3, false, do_clear_quest},
    {"test", 72, 0, 0, 5, false, do_test},
    {"close_shop", 73, 0, 0, 0, false, do_close_shop},
    {"set_walk_mode", 74, 0, 0, 0, false, do_set_walk_mode},
    {"set_run_mode", 75, 0, 0, 0, false, do_set_run_mode},
    {"setjob", 76, 0, 0, 5, false, do_set_skill_group},
    {"setskill", 77, 0, 0, 1, false, do_setskill},
    {"setskillother", 78, 0, 0, 3, false, do_setskillother},
    {"setskillpoint", 79, 0, 0, 5, false, do_set_skill_point},
    {"reload", 80, 0, 0, 5, false, do_reload},
    {"cooltime", 81, 0, 0, 3, false, do_cooltime},
    {"gwlist", 82, 0, 0, 1, false, do_gwlist},
    {"gwstop", 83, 0, 0, 1, false, do_stop_guild_war},
    {"gwcancel", 84, 0, 0, 1, false, do_cancel_guild_war},
    {"gstate", 85, 0, 0, 1, false, do_guild_state},
    {"pkmode", 86, 0, 0, 0, false, do_pkmode},
    {"mobile", 87, 0, 0, 0, false, do_mobile},
    {"mobile_auth", 88, 0, 0, 0, false, do_mobile_auth},
    {"messenger_auth", 89, 0, 0, 0, false, do_messenger_auth},
    {"getqf", 90, 0, 0, 1, false, do_getqf},
    {"setqf", 91, 0, 0, 1, false, do_setqf},
    {"delqf", 92, 0, 0, 1, false, do_delqf},
    {"set_state", 93, 0, 0, 1, false, do_set_state},
    {"\267\316\261\327\270\246\272\270\277\251\301\340", 94, 0, 0, 1, false, do_detaillog},
    {"\270\363\275\272\305\315\272\270\277\251\301\340", 95, 0, 0, 1, false, do_monsterlog},
    {"forgetme", 96, 0, 0, 1, false, do_forgetme},
    {"aggregate", 97, 0, 0, 1, false, do_aggregate},
    {"attract_ranger", 98, 0, 0, 1, false, do_attract_ranger},
    {"pull_monster", 99, 0, 0, 1, false, do_pull_monster},
    {"setblockmode", 100, 0, 0, 0, false, do_setblockmode},
    {"polymorph", 101, 0, 0, 1, false, do_polymorph},
    {"polyitem", 102, 0, 0, 3, false, do_polymorph_item},
    {"priv_empire", 103, 0, 0, 3, false, do_priv_empire},
    {"priv_guild", 104, 0, 0, 3, false, do_priv_guild},
    {"mount_test", 105, 0, 0, 5, false, do_mount_test},
    {"unmount", 106, 0, 0, 0, false, do_unmount},
    {"private", 107, 0, 0, 5, false, do_private},
    {"party_request", 108, 0, 0, 0, false, do_party_request},
    {"party_request_accept", 109, 0, 0, 0, false, do_party_request_accept},
    {"party_request_deny", 110, 0, 0, 0, false, do_party_request_deny},
    {"observer", 111, 0, 0, 5, false, do_observer},
    {"observer_exit", 112, 0, 0, 0, false, do_observer_exit},
    {"socketitem", 113, 0, 0, 5, false, do_socket_item},
    {"saveati", 114, 0, 0, 5, false, do_save_attribute_to_image},
    {"xmas_boom", 115, 0, 0, 3, false, do_xmas},
    {"xmas_snow", 116, 1, 0, 3, false, do_xmas},
    {"xmas_santa", 117, 2, 0, 3, false, do_xmas},
    {"view_equip", 118, 0, 0, 0, false, do_view_equip},
    {"jy", 119, 0, 0, 3, false, do_block_chat},
    {"block_chat", 120, 0, 0, 3, false, do_block_chat},
    {"block_chat_list", 121, 0, 0, 3, false, do_block_chat_list},
    {"build", 122, 0, 0, 0, false, do_build},
    {"affect_remove", 123, 0, 0, 1, false, do_affect_remove},
    {"horse_state", 124, 0, 0, 3, false, do_horse_state},
    {"horse_level", 125, 0, 0, 3, false, do_horse_level},
    {"horse_ride", 126, 0, 0, 3, false, do_horse_ride},
    {"horse_summon", 127, 0, 0, 3, false, do_horse_summon},
    {"horse_unsummon", 128, 0, 0, 3, false, do_horse_unsummon},
    {"horse_set_stat", 129, 0, 0, 3, false, do_horse_set_stat},
    {"phase_xhfmdmlgoa", 130, 0, 0, 0, false, do_inputall},
    {"phase_xhfmdmlgoaj", 131, 0, 0, 0, false, nullptr},
    {"pcbang_env", 132, 0, 0, 1, false, do_pcbang_env},
    {"pcbang_info", 133, 0, 0, 1, false, do_pcbang_info},
    {"pcbang_update", 134, 0, 0, 1, false, do_pcbang_update},
    {"emotion_allow", 135, 0, 5, 0, false, do_emotion_allow},
    {"kiss", 136, 0, 5, 0, false, do_emotion},
    {"slap", 137, 0, 5, 0, false, do_emotion},
    {"french_kiss", 138, 0, 5, 0, false, do_emotion},
    {"clap", 139, 0, 5, 0, false, do_emotion},
    {"cheer1", 140, 0, 5, 0, false, do_emotion},
    {"cheer2", 141, 0, 5, 0, false, do_emotion},
    {"dance1", 142, 0, 5, 0, false, do_emotion},
    {"dance2", 143, 0, 5, 0, false, do_emotion},
    {"dance3", 144, 0, 5, 0, false, do_emotion},
    {"dance4", 145, 0, 5, 0, false, do_emotion},
    {"dance5", 146, 0, 5, 0, false, do_emotion},
    {"dance1", 147, 0, 5, 0, false, do_emotion},
    {"dance2", 148, 0, 5, 0, false, do_emotion},
    {"dance3", 149, 0, 5, 0, false, do_emotion},
    {"dance4", 150, 0, 5, 0, false, do_emotion},
    {"dance5", 151, 0, 5, 0, false, do_emotion},
    {"congratulation", 152, 0, 5, 0, false, do_emotion},
    {"forgive", 153, 0, 5, 0, false, do_emotion},
    {"angry", 154, 0, 5, 0, false, do_emotion},
    {"attractive", 155, 0, 5, 0, false, do_emotion},
    {"sad", 156, 0, 5, 0, false, do_emotion},
    {"shy", 157, 0, 5, 0, false, do_emotion},
    {"cheerup", 158, 0, 5, 0, false, do_emotion},
    {"banter", 159, 0, 5, 0, false, do_emotion},
    {"joy", 160, 0, 5, 0, false, do_emotion},
    {"change_attr", 161, 0, 0, 5, false, do_change_attr},
    {"add_attr", 162, 0, 0, 5, false, do_add_attr},
    {"add_socket", 163, 0, 0, 5, false, do_add_socket},
    {"user_horse_ride", 164, 0, 0, 0, false, do_user_horse_ride},
    {"user_horse_back", 165, 0, 0, 0, false, do_user_horse_back},
    {"user_horse_feed", 166, 0, 0, 0, false, do_user_horse_feed},
    {"show_arena_list", 167, 0, 0, 1, false, do_show_arena_list},
    {"end_all_duel", 168, 0, 0, 1, false, do_end_all_duel},
    {"end_duel", 169, 0, 0, 1, false, do_end_duel},
    {"duel", 170, 0, 0, 1, false, do_duel},
    {"con+", 171, 13, 0, 1, false, do_stat_plus_amount},
    {"int+", 172, 15, 0, 1, false, do_stat_plus_amount},
    {"str+", 173, 12, 0, 1, false, do_stat_plus_amount},
    {"dex+", 174, 14, 0, 1, false, do_stat_plus_amount},
    {"break_marriage", 175, 0, 0, 1, false, do_break_marriage},
    {"roulette", 176, 0, 0, 5, false, do_roulette},
    {"itemlog", 177, 0, 0, 5, false, do_itemlog},
    {"open_oxevent", 178, 0, 0, 1, false, do_oxevent_open},
    {"close_oxevent_door", 179, 0, 0, 1, false, do_oxevent_door},
    {"end_oxevent", 180, 0, 0, 1, false, do_oxevent_close},
    {"end_oxevent_force", 181, 0, 0, 1, false, do_oxevent_end_force},
    {"start_quiz", 182, 0, 0, 1, false, do_oxevent_quiz},
    {"show_quiz", 183, 0, 0, 1, false, do_oxevent_show_quiz},
    {"log_oxevent", 184, 0, 0, 1, false, do_oxevent_log},
    {"get_oxevent_att", 185, 0, 0, 1, false, do_oxevent_get_attender},
    {"give_item_to_att", 186, 0, 0, 1, false, do_oxevent_give_item},
    {"effect", 187, 0, 0, 1, false, do_effect},
    {"test", 188, 0, 0, 1, false, do_test},
    {"threeway_info", 189, 0, 0, 1, false, do_threeway_war_info},
    {"threeway_myinfo", 190, 0, 0, 1, false, do_threeway_war_myinfo},
    {"elect", 196, 0, 0, 3, false, nullptr},
    {"rmcandidacy", 197, 0, 0, 1, false, nullptr},
    {"hair", 200, 0, 0, 0, false, do_hair},
    {"inventory", 201, 0, 0, 1, false, do_inventory},
    {"cube", 202, 0, 0, 0, false, do_cube},
    {"siege", 203, 0, 0, 1, false, do_siege},
    {"temp", 204, 0, 0, 5, false, nullptr},
    {"frog", 205, 0, 0, 3, false, do_frog},
    {"reset_subskill", 207, 0, 0, 3, false, do_reset_subskill},
    {"mhh", 208, 0, 0, 5, false, nullptr},
    {"flush", 209, 0, 0, 5, false, do_flush},
};

void double_dollar(const char* source, char* destination, int capacity)
{
    int used = 0;
    while (*source && used < capacity) {
        if (*source == '$') {
            if (capacity - used < 2)
                break;
            destination[used++] = '$';
        }
        destination[used++] = *source++;
    }
    destination[used] = 0;
}

bool CanRunCommand(const Command& command, CHARACTER& character)
{
    const bool hongKong = LC_IsHongKong();
    const bool newCibn = !hongKong && LC_IsNewCIBN();
    if (hongKong || newCibn) {
        if (!std::strcmp(command.name, "mob"))
            return character.GetGMLevel() != 0;
        if (!std::strcmp(command.name, "item") || !std::strcmp(command.name, "set")
            || !std::strcmp(command.name, "geteventflag") || (newCibn && !std::strcmp(command.name, "advance")))
            return character.GetGMLevel() > 4;
    }
    return command.gmLevel == 0 || character.GetGMLevel() >= command.gmLevel;
}
}

const char* GetCommandName(int index)
{
    for (const Command& entry : commands)
        if (entry.index == index)
            return entry.name;
    return nullptr;
}

// 08089970
void interpreter_set_privilege(const char* command, int privilege)
{
    for (Command& entry : commands) {
        if (strcasecmp(command, entry.name) == 0) {
            entry.gmLevel = privilege;
            sys_log(0, "Setting command privilege: %s -> %d", command, privilege);
            return;
        }
    }
}

void interpret_command(CHARACTER* character, const char* argument)
{
    if (!character || !argument || !*argument)
        return;

    char gmLine[257];
    if (character->GetGMLevel()) {
        std::strncpy(gmLine, argument, sizeof(gmLine) - 1);
        gmLine[sizeof(gmLine) - 1] = '\0';
        argument = gmLine;
    }

    char line[257];
    double_dollar(argument, line, 256);
    char word[257];
    int length = 0;
    char* input = first_cmd(line, word, 256, &length);
    if (!length)
        return;

    const Command* selected = nullptr;
    for (const Command& command : commands) {
        if (command.exact ? !std::strcmp(command.name, word) : !std::strncmp(command.name, word, length)) {
            selected = &command;
            break;
        }
    }

    if (selected && character->GetPosition() < selected->position) {
        const char* message;
        switch (character->GetPosition()) {
            case 0: message = "\276\262\267\257\301\370\040\273\363\305\302\277\241\274\255\264\302\040\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056"; break;
            case 1: message = "\262\336\274\323\277\241\274\255\040\276\356\266\273\260\324\277\344\077"; break;
            case 2:
            case 3: message = "\270\325\300\372\040\300\317\276\356\040\263\252\274\274\277\344\056"; break;
            case 6: message = "\305\272\040\273\363\305\302\277\241\274\255\264\302\040\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056"; break;
            default:
                _sys_err("interpret_command", 0x255, "unknown position %d", character->GetPosition());
                return;
        }
        character->ChatPacket(1, "%s", locale_find(message, g_iUseLocale));
        return;
    }

    if (!selected || !CanRunCommand(*selected, *character) || !selected->handler) {
        character->ChatPacket(1, "%s", locale_find("\261\327\267\261\040\270\355\267\311\276\356\264\302\040\276\370\275\300\264\317\264\331", g_iUseLocale));
        return;
    }
    if (std::strncmp("phase", selected->name, 5))
        sys_log(0, "COMMAND: %s: %s", character->GetName(), selected->name);
    selected->handler(character, input, selected->index, selected->subcommand);
}

// 080ab1a0
void do_messenger_auth(CHARACTER* character, char* argument, int, int)
{
    if (character->GetArena()) {
        character->ChatPacket(1, "%s", locale_find("\264\353\267\303\300\345\277\241\274\255 \273\347\277\353\307\317\275\307 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return;
    }

    char response[256]{};
    char companion[256]{};
    two_arguments(argument, response, companion);
    if (!response[0] || !companion[0])
        return;
    const bool deny = response[0] != 'y' && response[0] != 'Y';
    if (deny) {
        CHARACTER* requester = CHARACTER_MANAGER::instance().FindPC(companion);
        if (requester)
            requester->ChatPacket(1, locale_find("%s \264\324\300\270\267\316 \272\316\305\315 \304\243\261\270 \265\356\267\317\300\273 \260\305\272\316 \264\347\307\337\275\300\264\317\264\331.", g_iUseLocale), character->GetName());
    }
    MessengerManager::instance().AuthToAdd(character->GetName(), companion, deny);
}
