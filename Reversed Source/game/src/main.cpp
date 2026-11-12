#include "cube.h"
#include "config.h"
#include "ip_ban.h"
#include "char_manager.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "event.h"
#include "char.h"
#include "db.h"
#include "item_manager.h"
#include "login_sim.h"
#include "pvp.h"
#include "../../libthecore/include/utils.h"
#include <climits>
#include "input.h"
#include "locale.hpp"
#include "locale_service.h"
#include "p2p.h"
#include "text_file_loader.h"
#include "utils.h"
#include "../../libthecore/include/fdwatch.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/main.h"
#include "../../libthecore/include/signal.h"
#include "../../libthecore/include/socket.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>


// Partial original main.cpp: invalid-server state getter only.
static bool g_isInvalidServer = false;
bool Metin2Server_IsInvalid()
{
    return g_isInvalidServer;
}

void heartbeat(LPHEART heart, int pulse);
int io_loop(LPFDWATCH watcher);
#include "questlua_forked.h"
#include "regen.h"
void SendNotice(const char* notice);

extern "C" {
extern const char* _malloc_options;
extern int optreset;
}

int num_events_called = 0;
int current_bytes_written = 0;
socket_t tcp_socket = 0;
socket_t udp_socket = 0;
socket_t p2p_socket = 0;
LPFDWATCH main_fdw = 0;
bool g_bShutdown = false;
int g_shutdown_disconnect_pulse = 0;
int g_shutdown_disconnect_force_pulse = 0;
int g_shutdown_core_pulse = 0;
static std::uint32_t s_dwProfiler[4] = {0, 0, 0, 0};

// 0809ec00
void ShutdownOnFatalError()
{
    if (g_bShutdown)
        return;
    char notice[256];
    std::sprintf(notice, locale_find("\274\255\271\366\277\241 \304\241\270\355\300\373\300\316 \277\300\267\371\260\241 \271\337\273\375\307\317\277\251 \300\332\265\277\300\270\267\316 \300\347\272\316\306\303\265\313\264\317\264\331.", g_iUseLocale));
    SendNotice(notice);
    std::sprintf(notice, locale_find("10\303\312\310\304 \300\332\265\277\300\270\267\316 \301\242\274\323\300\314 \301\276\267\341\265\307\270\347,", g_iUseLocale));
    SendNotice(notice);
    std::sprintf(notice, locale_find("5\272\320 \310\304\277\241 \301\244\273\363\300\373\300\270\267\316 \301\242\274\323\307\317\275\307\274\366 \300\326\275\300\264\317\264\331.", g_iUseLocale));
    SendNotice(notice);
    g_bShutdown = true;
    g_bNoMoreClient = true;
    g_shutdown_disconnect_pulse = thecore_pulse() + passes_per_sec * 10;
    g_shutdown_disconnect_force_pulse = thecore_pulse() + passes_per_sec * 20;
    g_shutdown_core_pulse = thecore_pulse() + passes_per_sec * 30;
}

// 0809f300
void Metin2Server_Check()
{
    const socket_t connector = socket_connect("220.95.239.40", 0x1bd0);
    if (connector < 0) {
        g_isInvalidServer = true;
        return;
    }

    char response[256];
    const int bytes = socket_read(connector, response, 255);
    response[bytes] = '\0';
    std::printf("recv[%s]", response);
    if (std::strncmp(response, "OK", 2) == 0)
        g_isInvalidServer = false;
    else if (std::strncmp(response, "CK", 2) == 0)
        g_isInvalidServer = true;
    socket_close(connector);
}

// 080a1320
void usage()
{
    std::puts("Option list\n"
              "-p <port>    : bind port number (port must be over 1024)\n"
              "-l <level>   : sets log level\n"
              "-v           : log to stdout\n"
              "-r           : do not load regen tables\n"
              "-t           : traffic proflie on");
}

// 080a1340
int start(int argc, char** argv)
{
    _malloc_options = "A";
    bool verbose = false;
    std::string localeServiceName;
    char option;
    while ((option = static_cast<char>(getopt(argc, argv, "npverlt"))) != -1) {
        char* end;
        switch (option) {
        case 'n':
            if (optind < argc) {
                localeServiceName = argv[optind++];
                optreset = 1;
            }
            break;
        case 'p':
            mother_port = static_cast<WORD>(std::strtol(argv[optind], &end, 10));
            if (mother_port < 1025) {
                usage();
                return 0;
            }
            std::printf("port %d\n", static_cast<int>(mother_port));
            ++optind;
            optreset = 1;
            break;
        case 'l':
            log_set_level(std::strtol(argv[optind], &end, 10));
            ++optind;
            optreset = 1;
            break;
        case 'r':
            g_bNoRegen = true;
            break;
        case 't':
            g_bTrafficProfileOn = true;
            break;
        case 'v':
            verbose = true;
            break;
        default:
            break;
        }
    }

    config_init(localeServiceName);
    LocaleService_LoadLocaleStringFile();
    LoadForked();
    if (!verbose)
        std::freopen("stdout", "a", stdout);
    thecore_init(25, heartbeat);
    signal_timer_disable();
    main_fdw = fdwatch_new(4096);

    tcp_socket = socket_tcp_bind(g_szPublicIP, mother_port);
    if (tcp_socket == -1) {
        std::perror("socket_tcp_bind: tcp_socket");
        return 0;
    }
    udp_socket = socket_udp_bind(g_szPublicIP, mother_port);
    if (udp_socket == -1) {
        std::perror("socket_udp_bind: udp_socket");
        return 0;
    }
    p2p_socket = socket_tcp_bind(g_szPublicIP, p2p_port);
    if (p2p_socket == -1) {
        std::perror("socket_tcp_bind: p2p_socket");
        return 0;
    }

    fdwatch_add_fd(main_fdw, tcp_socket, 0, FDW_READ, 0);
    fdwatch_add_fd(main_fdw, udp_socket, 0, FDW_READ, 0);
    fdwatch_add_fd(main_fdw, p2p_socket, 0, FDW_READ, 0);
    std::fprintf(stderr, "ConnectDB");
    db_clientdesc = DESC_MANAGER::instance().CreateConnectionDesc(main_fdw, db_addr, db_port, 8, true);
    std::fprintf(stderr, "CheckAuth");

    if (!g_bAuthServer) {
        if (teen_addr[0] && teen_port)
            g_TeenDesc = DESC_MANAGER::instance().CreateConnectionDesc(main_fdw, teen_addr, teen_port, 12, true);
    } else if (g_stAuthMasterIP.empty()) {
        const int locale = LC_GetLocalType();
        std::fprintf(stderr, "MasterAuth %d", locale);
        if (LC_IsJapan() && !test_server) {
            std::fprintf(stderr, "NETMARBLE_JP::Connect('210.174.197.180:10220')");
            g_NetmarbleDBDesc = DESC_MANAGER::instance().CreateConnectionDesc(
                main_fdw, "210.174.197.180", 10220, 11, true);
        }
    } else {
        std::fprintf(stderr, "SlaveAuth");
        g_pkAuthMasterDesc = DESC_MANAGER::instance().CreateConnectionDesc(
            main_fdw, g_stAuthMasterIP.c_str(), g_wAuthMasterPort, 9, true);
        P2P_MANAGER::instance().RegisterConnector(g_pkAuthMasterDesc);
        g_pkAuthMasterDesc->SetP2P(g_stAuthMasterIP.c_str(), g_wAuthMasterPort, g_bChannel);
    }
    signal_timer_enable(30);
    return 1;
}

// 080a1a10
void destroy()
{
    regen_free();
    socket_close(tcp_socket);
    socket_close(udp_socket);
    socket_close(p2p_socket);
    fdwatch_delete(main_fdw);
    thecore_destroy();
    event_destroy();
    CTextFileLoader::DestroySystem();
}

// 080a1a70
int idle()
{
    static timeval previous = {0, 0};
    static int processCount = 0;
    if (previous.tv_sec == 0)
        gettimeofday(&previous, 0);
    int pulses = thecore_idle();
    if (!pulses)
        return 0;
    assert(pulses > 0);
    while (pulses-- > 0) {
        LPHEART heart = thecore_heart;
        heart->pulse = static_cast<std::int32_t>(static_cast<std::uint32_t>(heart->pulse) + 1u);
        heartbeat(heart, heart->pulse);
    }

    DWORD started = get_dword_time();
    CHARACTER_MANAGER::instance().Update(thecore_heart->pulse);
    s_dwProfiler[1] += get_dword_time() - started;
    started = get_dword_time();
    if (!io_loop(main_fdw))
        return 0;
    s_dwProfiler[2] += get_dword_time() - started;
    DESC_MANAGER::instance().DestroyClosed();
    log_rotate();

    timeval now;
    gettimeofday(&now, 0);
    ++processCount;
    if (now.tv_sec > previous.tv_sec) {
        const int pulse = thecore_pulse();
        const int queuedEvents = event_count();
        const auto signedProfileTime = [](std::uint32_t value) {
            return static_cast<long>(static_cast<std::int32_t>(value));
        };
        pt_log("[%3d] event %5d/%-5d idle %-4ld event %-4ld heartbeat %-4ld I/O %-4ld chrUpate %-4ld | WRITE: %-7d | PULSE: %d",
               processCount, num_events_called, queuedEvents, signedProfileTime(thecore_profiler[0]),
               signedProfileTime(s_dwProfiler[0]), signedProfileTime(s_dwProfiler[3]),
               signedProfileTime(s_dwProfiler[2]), signedProfileTime(s_dwProfiler[1]), current_bytes_written, pulse);
        processCount = 0;
        num_events_called = 0;
        current_bytes_written = 0;
        gettimeofday(&previous, 0);
        thecore_profiler[0] = 0;
        thecore_profiler[1] = 0;
        std::memset(s_dwProfiler, 0, sizeof(s_dwProfiler));
    }
    return 1;
}

// 080a1cd0
int io_loop(LPFDWATCH watcher)
{
    DESC_MANAGER::instance().TryConnect();
    const int count = fdwatch(watcher, 0);
    if (count < 0)
        return 0;

    for (int index = 0; index < count; ++index) {
        DESC* desc = static_cast<DESC*>(fdwatch_get_client_data(watcher, index));
        if (!desc) {
            socket_t socket;
            if (fdwatch_check_event(watcher, tcp_socket, index) == FDW_READ) {
                DESC_MANAGER::instance().AcceptDesc(watcher, tcp_socket);
                socket = tcp_socket;
            } else if (fdwatch_check_event(watcher, p2p_socket, index) == FDW_READ) {
                DESC_MANAGER::instance().AcceptP2PDesc(watcher, p2p_socket);
                socket = p2p_socket;
            } else if (fdwatch_check_event(watcher, udp_socket, index) == FDW_READ) {
                sockaddr_in address;
                socklen_t addressSize = 16;
                char buffer[256];
                const int bytes = socket_udp_read(udp_socket, buffer, sizeof(buffer),
                    reinterpret_cast<sockaddr*>(&address), &addressSize);
                if (bytes > 0) {
                    static CInputUDP input;
                    input.SetSockAddr(address);
                    int consumed = 0;
                    input.Process(0, buffer, bytes, consumed);
                }
                socket = udp_socket;
            } else {
                continue;
            }
            fdwatch_clear_event(watcher, socket, index);
            continue;
        }

        const int type = fdwatch_check_event(watcher, desc->GetSocket(), index);
        switch (type) {
        case FDW_READ: {
            const int bytes = desc->ProcessInput();
            if (bytes && desc == db_clientdesc)
                sys_log(1, "DB_BYTES_READ: %d", bytes);
            else if (bytes && desc == g_NetmarbleDBDesc)
                sys_log(0, "NETMARBLE_JP::Read(%d)", bytes);
            if (bytes < 0)
                desc->SetPhase(0);
            break;
        }
        case FDW_WRITE: {
            int level = 0;
            const char* format;
            if (desc == db_clientdesc) {
                format = "DB_BYTES_WRITE: size %d sock_buf %d ret %d";
                level = 1;
            } else if (desc == g_NetmarbleDBDesc) {
                format = "NETMARBLE_JP::Send(size %d sock_buf %d ret %d)";
            } else {
                if (desc->ProcessOutput() < 0) {
                    desc->SetPhase(0);
                    break;
                }
                if (desc != g_TeenDesc)
                    break;
                format = "TEEN::Send(size %d sock_buf %d ret %d)";
            }
            const int size = buffer_size(desc->GetOutputBuffer());
            const int capacity = fdwatch_get_buffer_size(watcher, desc->GetSocket());
            const int result = desc->ProcessOutput();
            if (result < 0)
                desc->SetPhase(0);
            if (size)
                sys_log(level, format, size, capacity, result);
            break;
        }
        case FDW_EOF:
            desc->SetPhase(0);
            break;
        default:
            _sys_err("io_loop", 0x336, "fdwatch_check_event returned unknown %d", type);
            desc->SetPhase(0);
            break;
        }
    }
    return 1;
}

static unsigned int save_idx = 0;

// 0809ed00
void heartbeat(LPHEART heart, int pulse)
{
    DWORD started = get_dword_time();
    num_events_called += event_process(pulse);
    s_dwProfiler[0] += get_dword_time() - started;
    started = get_dword_time();

    if (pulse % heart->passes_per_sec == 0) {
        if (!g_bAuthServer) {
            TPlayerCountPacket packet;
            packet.dwCount = DESC_MANAGER::instance().GetLocalUserCount();
            db_clientdesc->DBPacket(10, 0, &packet, sizeof(packet));
        } else {
            DESC_MANAGER::instance().ProcessExpiredLoginKey();
            DBManager::instance().FlushBilling(false);
        }

        int count = 0;
        for (std::map<DWORD, CLoginSim*>::iterator it = g_sim.begin(); it != g_sim.end(); ++it) {
            CLoginSim* simulator = it->second;
            if (simulator->bCheck)
                continue;
            simulator->bCheck = true;
            if (simulator->vecIdx < simulator->vecID.size()) {
                if (simulator->vecIdx == 0)
                    db_clientdesc->DBPacket(100, 0, &simulator->auth, sizeof(simulator->auth));
                simulator->load.player_id = simulator->vecID[simulator->vecIdx++];
                db_clientdesc->DBPacket(101, 0, &simulator->login, sizeof(simulator->login));
            }
            if (++count > 50) {
                sys_log(0, "FLUSH_SENT");
                break;
            }
        }

        if (save_idx < g_vec_save.size()) {
            const int count = MIN(100, static_cast<int>(g_vec_save.size() - save_idx));
            for (int i = 0; i < count; ++i) {
                db_clientdesc->DBPacket(4, 0, &g_vec_save[save_idx], sizeof(TPlayerTable));
                ++save_idx;
            }
            sys_log(0, "SAVE_FLUSH %d", count);
        }
    }
    if (pulse % 29 == 0)
        CHARACTER_MANAGER::instance().ProcessDelayedSave();
    if (pulse % 127 == 0) {
        ITEM_MANAGER::instance().Update();
        DESC_MANAGER::instance().UpdateLocalUserCount();
    }
    s_dwProfiler[3] += get_dword_time() - started;
    DBManager::instance().Process();
    CPVPManager::instance().Process();

    if (g_bShutdown) {
        if (thecore_pulse() > g_shutdown_disconnect_pulse) {
            const DESC_MANAGER::DESC_SET& clients = DESC_MANAGER::instance().GetClientSet();
            for (DESC_MANAGER::DESC_SET::const_iterator it = clients.begin(); it != clients.end(); ++it) {
                DESC* desc = *it;
                CHARACTER* character = desc->GetCharacter();
                if (character && character->GetGMLevel() == 0)
                    desc->GetCharacter()->ChatPacket(5, "quit");
            }
            g_shutdown_disconnect_pulse = INT_MAX;
        } else if (thecore_pulse() > g_shutdown_disconnect_force_pulse) {
            const DESC_MANAGER::DESC_SET& clients = DESC_MANAGER::instance().GetClientSet();
            for (DESC_MANAGER::DESC_SET::const_iterator it = clients.begin(); it != clients.end(); ++it) {
                DESC* desc = *it;
                if (desc->GetType() != 1 && !desc->IsPhase(9))
                    desc->SetPhase(0);
            }
        } else if (thecore_pulse() > passes_per_sec * 5 + g_shutdown_disconnect_force_pulse) {
            thecore_shutdown();
        }
    }
}

#include "OXEvent.h"
#include "TrafficProfiler.h"
#include "arena.h"
#include "banword.h"
#include "blend_item.h"
#include "building.h"
#include "constants.h"
#include "dungeon.h"
#include "guild_manager.h"
#include "item_addon.h"
#include "log.h"
#include "lzo_manager.h"
#include "marriage.h"
#include "matrix_for_runup.h"
#include "messenger_manager.h"
#include "mob_manager.h"
#include "motion.h"
#include "party.h"
#include "passpod.h"
#include "polymorph.h"
#include "priv_manager.h"
#include "profiler.h"
#include "questmanager.h"
#include "refine.h"
#include "sectree_manager.h"
#include "shop_manager.h"
#include "skill.h"
#include "target.h"
#include "war_map.h"
#include "wedding.h"

void WriteVersion();
void InitItemLog();
void ani_init();
void print_memory_usage();
namespace fishing { void Initialize(); }

// 0809f3d0
int main(int argc, char** argv)
{
    WriteVersion();

    SECTREE_MANAGER sectree_manager;
    DESC_MANAGER desc_manager;
    CHARACTER_MANAGER char_manager;
    ITEM_MANAGER item_manager;
    CShopManager shop_manager;
    CMobManager mob_manager;
    CMotionManager motion_manager;
    CPartyManager party_manager;
    CSkillManager skill_manager;
    CPVPManager pvp_manager;
    LZOManager lzo_manager;
    DBManager db_manager;
    LogManager log_manager;
    MessengerManager messenger_manager;
    P2P_MANAGER p2p_manager;
    CGuildManager guild_manager;
    CDungeonManager dungeon_manager;
    CRefineManager refine_manager;
    CBanwordManager banword_manager;
    CPrivManager priv_manager;
    CWarMapManager war_map_manager;
    building::CManager building_manager;
    CTargetManager target_manager;
    marriage::CManager marriage_manager;
    marriage::WeddingManager wedding_manager;
    CItemAddonManager item_addon_manager;
    CArenaManager arena_manager;
    COXEventManager OXEvent_manager;
    CAdditionalDamageByLevel additional_damage;
    MatrixDBManager matrixdb;
    InitItemLog();
    CAccountDB account_db;
    TrafficProfiler trafficProfiler;
    CTableBySkill skill_power_by_level;
    CMoneyTableBySkill money_table_by_skill;
    CPolymorphUtils polymorph_utils;
    polymorph_utils.Initialize();
    CProfiler profiler;
    CPasspod passpod;

    if (!start(argc, argv))
        return 0;

    quest::CQuestManager quest_manager;
    if (!quest_manager.Initialize())
        return 0;

    MessengerManager::instance().Initialize();
    CGuildManager::instance().Initialize();
    fishing::Initialize();
    OXEvent_manager.Initialize();
    Cube_init();
    Blend_Item_init();
    ani_init();
    Metin2Server_Check();
    if (g_bTrafficProfileOn)
        TrafficProfiler::instance().Initialize(3600, "ProfileLog");
    while (idle()) {}

    if (g_bAuthServer) {
        DBManager::instance().FlushBilling(true);
        const int limit = DBManager::instance().CountQuery() / 50;
        int retries = 0;
        DWORD count;
        do {
            do {
                count = DBManager::instance().CountQuery();
                sys_log(0, "Queries %lu", count);
                if (!count)
                    break;
                ++retries;
                usleep(500000);
            } while (retries < limit);
        } while (count && count != DBManager::instance().CountQuery());
    }

    arena_manager.Destroy();
    OXEvent_manager.Destroy();
    signal_timer_disable();
    char_manager.GracefulShutdown();
    item_manager.GracefulShutdown();
    db_clientdesc->FlushOutput();
    p2p_manager.FlushOutput();
    building_manager.Destroy();
    char_manager.Destroy();
    item_manager.Destroy();
    shop_manager.Destroy();
    desc_manager.Destroy();
    quest_manager.Destroy();
    trafficProfiler.Flush();
    destroy();
    print_memory_usage();
    return 1;
}
