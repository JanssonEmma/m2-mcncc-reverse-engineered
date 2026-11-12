// Compile the real postfix helper/storage, keeping unrelated config globals private.
#define g_bTrafficProfileOn socket_unused_traffic_profile
#define g_bAuthServer socket_unused_auth_server
#define test_server socket_unused_test_server
#define passes_per_sec socket_unused_passes_per_sec
#include "../../game/src/config.cpp"
