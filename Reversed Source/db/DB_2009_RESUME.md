# Metin2 DB 2009 reconstruction

Source target: PROJECT_ROOT\Reversed Source\db.
Reference: REFERENCE_PROJECT\Source\srv\server\db\src.

The unpacked input is PROJECT_ROOT\Original Binaries\db-2009-unpacked.elf.
SHA-256 85E673ABA60DA66B68A6844C9E29DE5090769249718A9046F868CC5E128FF111.
The packed original SHA-256 is 626C8EE89515F457811C64BA9DF5B4CA75F34115D4FAF229539419FB9729D878.

## Current source and validation

- The ELF symbol table lists 24 application translation units. The source tree contains all 23 in scope. Monarch.cpp and 11 Monarch-related ClientManager methods are excluded. Ten Monarch dispatch cases consume the packet without calling a handler. Boot v5 keeps the original framing with one zero-filled 304-byte legacy record and an empty legacy candidate section.
- A name-level sweep found definitions for all 165 non-Monarch global ClientManager entries, counting compiler constructor/destructor variants. This is a coverage check. It does not establish semantic equivalence.
- Binary evidence includes DWARF, GNU objdump and 227 Ghidra pseudocode exports from an isolated headless project. The shared CodeBrowser session was not switched. Per-module notes are in evidence/.
- All 23 translation units passed a 32-bit GNU C++17 syntax check and compiled to i386 COFF objects with the shared reconstruction's test-only POSIX and MySQL-header shims. The relocatable merge succeeded. Comparing defined and undefined object symbols found no outstanding references to the recovered DB classes. The POSIX, CRT, and shared library symbols require the target toolchain.
- The separate 32-bit layout check passed for peer headers, the account and player tables, item/quest rows, and representative billing, privilege, party, guild, price list, and safebox packets. Host STL layouts and native runtime behavior were not checked.
- Shared libsql/AsyncSQL.h and AsyncSQL.cpp gained CAsyncSQL2::SetLocale from db 0x080bfab0. The common guild reserve and guild bet records are reused. DBProtocol.h temporarily contains DB-side declarations missing from shared common.
- Source uses modern ownership wrappers where the original GCC 3.3 code used auto_ptr. The recovery notes list behavioral approximations, including bounded copies in some packet handlers. Verify target behavior with data-driven tests after the native link.

## C++23 and MySQL 8 build

The FreeBSD/i386 CMake target requires a C++23-capable toolchain, Oracle
MySQL 8.x headers and libmysqlclient, fmtlib's CMake package, zlib, and
POSIX threads. Use MYSQL8_INCLUDE_DIR for the directory containing mysql.h
and MYSQL8_CLIENT_LIBRARY for the exact library path when automatic
discovery picks up another installation. An example on the target:

```sh
cmake -S db -B build-db \
  -DMYSQL8_INCLUDE_DIR=/usr/local/include/mysql \
  -DMYSQL8_CLIENT_LIBRARY=/usr/local/lib/mysql/libmysqlclient.so
cmake --build build-db
```

The DB target checks the MySQL client at configuration and verifies the
client library version at startup, requiring an exact match with the
headers because the wrapper stores MYSQL by value. The server must be
MySQL 8.x. The four SQL slots use the
configured optional port, and startup stops after an exhausted retry
rather than proceeding with a missing connection. The game target
continues to use the legacy libsql compilation branch. MySQL 8 applies
a locale change on each connection before its next query, uses
mysql_set_character_set to keep escaping in sync, and reports query
failures without replaying writes after a lost connection.

Dynamically generated SQL is formatted with fmtlib. The buffer helpers
retain printf width, numeric conversions, and snprintf truncation rules.
Existing string literals are passed directly to MySQL. MySQL 8 reserved
identifiers window and rank are quoted, and the block_exception deletion
uses a WHERE login predicate. Escaping uses
mysql_real_escape_string_quote for single-quoted values.

The modernized tree compiled to 25 i386 objects under GNU C++23
(23 DB units plus both libsql sources), using Oracle MySQL 8.0.39 headers,
fmt 10.2.1, and host-only POSIX shims. All 25 original layout assertions
and the SQL formatter smoke check passed. The legacy libsql branch also
passed its separate syntax check.

## DB peer configuration

The DB listener defaults to IPv4 loopback. If game or auth servers run
on different hosts, set BIND_IP to the DB host's private interface and
list the source addresses of the remote peers in DB_PEER_ALLOWLIST.
Set DB_AUTH_PEER_IP to the source address allowed to register the auth
role. Loopback remains available on the allowlist.

    BIND_IP = 192.168.10.10
    DB_PEER_ALLOWLIST = "192.168.10.11 192.168.10.12"
    DB_AUTH_PEER_IP = 192.168.10.11

The legacy TCP protocol checks source addresses and carries no
cryptographic peer authentication.

A native FreeBSD link and live query checks still require the target
environment and database schema. The old ELF statically embeds MySQL
4.1.22, with ABI note 502010, and was built with GNU C++ 3.3.3 for FreeBSD.
These details describe the reconstruction input.
