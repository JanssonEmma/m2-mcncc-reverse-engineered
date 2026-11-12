cmake_minimum_required(VERSION 3.20)
project(Metin2Db2009 LANGUAGES C CXX)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" OR NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "The DB target requires a 32-bit FreeBSD toolchain.")
endif()

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_path(MYSQL8_INCLUDE_DIR NAMES mysql.h PATH_SUFFIXES mysql REQUIRED)
find_library(MYSQL8_CLIENT_LIBRARY NAMES mysqlclient PATH_SUFFIXES mysql REQUIRED)

include(CMakePushCheckState)
include(CheckCXXSourceCompiles)
include(CheckCXXSourceRuns)
function(check_oracle_mysql8)
    cmake_push_check_state(RESET)
    set(CMAKE_REQUIRED_INCLUDES "${MYSQL8_INCLUDE_DIR}")
    set(CMAKE_REQUIRED_LIBRARIES "${MYSQL8_CLIENT_LIBRARY}" Threads::Threads)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE EXECUTABLE)
    set(mysql8_probe [=[
#include <cstring>
#include <mysql.h>
#if !defined(MYSQL_VERSION_ID) || MYSQL_VERSION_ID < 80000 || MYSQL_VERSION_ID >= 90000
#error Oracle MySQL 8.x headers required
#endif
#if !defined(LIBMYSQL_VERSION_ID) || LIBMYSQL_VERSION_ID < 80000 || LIBMYSQL_VERSION_ID >= 90000
#error Oracle MySQL 8.x client headers required
#endif
#if defined(MARIADB_BASE_VERSION) || defined(MARIADB_PACKAGE_VERSION_ID)
#error Oracle MySQL 8.x headers required
#endif
int main()
{
    auto volatile escape = &mysql_real_escape_string_quote;
    if (!escape || mysql_library_init(0, nullptr, nullptr) != 0)
        return 1;
    const auto version = mysql_get_client_version();
    const char* description = mysql_get_client_info();
    const bool valid = version == LIBMYSQL_VERSION_ID &&
                       description && !std::strstr(description, "MariaDB");
    mysql_library_end();
    return valid ? 0 : 1;
}
]=])
    if(CMAKE_CROSSCOMPILING)
        check_cxx_source_compiles("${mysql8_probe}" MYSQL8_CLIENT_LINKS)
        set(mysql8_valid "${MYSQL8_CLIENT_LINKS}")
    else()
        check_cxx_source_runs("${mysql8_probe}" MYSQL8_CLIENT_RUNS)
        set(mysql8_valid "${MYSQL8_CLIENT_RUNS}")
    endif()
    cmake_pop_check_state()
    if(NOT mysql8_valid)
        message(FATAL_ERROR "The DB target requires matching Oracle MySQL 8.x headers and libmysqlclient.")
    endif()
endfunction()
check_oracle_mysql8()

set(SERVER_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
add_executable(db
    src/BlockCountry.cpp
    src/Cache.cpp
    src/ClientManager.cpp
    src/ClientManagerBoot.cpp
    src/ClientManagerEventFlag.cpp
    src/ClientManagerGuild.cpp
    src/ClientManagerLogin.cpp
    src/ClientManagerParty.cpp
    src/ClientManagerPlayer.cpp
    src/Config.cpp
    src/DBManager.cpp
    src/GuildManager.cpp
    src/HB.cpp
    src/ItemAwardManager.cpp
    src/Lock.cpp
    src/LoginData.cpp
    src/Main.cpp
    src/Marriage.cpp
    src/MoneyLog.cpp
    src/NetBase.cpp
    src/Peer.cpp
    src/PeerBase.cpp
    src/PrivManager.cpp
    "${SERVER_ROOT}/libgame/src/grid.cc"
    "${SERVER_ROOT}/libpoly/Poly.cc"
    "${SERVER_ROOT}/libpoly/SymTable.cc"
    "${SERVER_ROOT}/libsql/AsyncSQL.cpp"
    "${SERVER_ROOT}/libsql/Semaphore.cpp"
    "${SERVER_ROOT}/libthecore/src/buffer.c"
    "${SERVER_ROOT}/libthecore/src/fdwatch.c"
    "${SERVER_ROOT}/libthecore/src/gost.c"
    "${SERVER_ROOT}/libthecore/src/heart.c"
    "${SERVER_ROOT}/libthecore/src/log.c"
    "${SERVER_ROOT}/libthecore/src/main.c"
    "${SERVER_ROOT}/libthecore/src/memcpy.c"
    "${SERVER_ROOT}/libthecore/src/signal.c"
    "${SERVER_ROOT}/libthecore/src/socket.c"
    "${SERVER_ROOT}/libthecore/src/tea.c"
    "${SERVER_ROOT}/libthecore/src/utils.c"
)
target_compile_features(db PRIVATE c_std_99 cxx_std_23)
target_compile_definitions(db PRIVATE MCNCC_DB_MYSQL8=1)
target_include_directories(db SYSTEM PRIVATE "${MYSQL8_INCLUDE_DIR}")
target_link_libraries(db PRIVATE "${MYSQL8_CLIENT_LIBRARY}" fmt::fmt ZLIB::ZLIB Threads::Threads m)
