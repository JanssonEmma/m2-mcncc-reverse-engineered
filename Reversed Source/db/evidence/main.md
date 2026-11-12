# DB 2009 startup and shutdown

Evidence comes from the unpacked FreeBSD/i386 ELF with SHA-256 `85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`. The original `Main.cpp` compilation unit is DWARF DIE `0x63b21`. It names `/home/mhh/metin2/db/src` and GNU C++ 3.3.3 [FreeBSD]. The newer `Main.cpp` informed source structure.

| Routine | Address | Evidence |
| --- | --- | --- |
| `emergency_sig` | `0x0804d430` | Logs one of the two original messages, including their spelling. Calls `FlushQueryQueue(SQL_PLAYER, "dump.txt")` for either signal and aborts for SIGSEGV. |
| `main` | `0x0804d4c0` | Prints the Germany build banner and assigns FreeBSD `_malloc_options`. Constructs the service singletons on the stack. After `Start`, initializes guild and marriage and loads the country blocklist. Disables timer before quitting SQL and waits on the two player queues, sleeping 1000 microseconds for each nonzero result. Returns 1. |
| `emptybeat` | `0x0804d7b0` | No effective operation. |
| `Start` | `0x0804d7c0` | Loads `conf.txt`, reads logging, heartbeat, locale, table postfix and cache settings, connects four SQL slots, initializes poller/client manager/hot backup, installs SIGUSR1 and SIGSEGV. |
| String accessors | `0x0804e9f0`, `0x0804ea50`, `0x0804ea70`, `0x0804eb00` | Empty/default string handling for null or empty input. A nonempty player DB name gets a trailing dot. |
| Global data | `0x080d3018` through `0x080d3028`, `0x081c3f58` through `0x081c3f6c`, initializer `0x0804eb20` | Cache defaults 420, 300 and 540, logout 600, logging 1. Locale starts at euckr and name column at name. |

- `TEST_SERVER` and `LOG` branch on successful key lookup. `LOG` overwrites the parsed value with 1 when present. `CLIENT_HEART_FPS` must be configured despite the local default of 50. Log retention is written directly to the shared `log_keep_days` symbol at `0x080d3080`, defaults to 3 and clamps a configured value to 3–30. `thecore_init` return value is ignored.
- Cache-related strings are parsed with `atoi`. The recovered setter signature is `CClientManager::SetCacheFlushCountLimit(int)` at `0x08059130`.
- The four SQL calls use slot IDs 0–3 and read five fields with the original `sscanf` format. Each uses a decrementing counter initialized to 5, for up to six attempts. The code logs success to stderr even on exhaustion and continues. Missing SQL configuration returns early.
- The 2009 startup has no `WriteVersion` or `CItemIDRangeManager` calls. The observed `CMonarch` stack object lifetime is omitted under the user's scope. Other service order is preserved.
- `Main.h` exposes the observed entry and getters. The newer `End` declaration has no corresponding target definition. `stdafx.h` is an integration header assembled from the recovered shared interfaces and the current reconstructed modules. The original preprocessor text is unavailable.
- Integration still requires the reconstructed client, DB, guild, privilege and marriage manager interfaces and their dependencies. Builds and runtime verification remain deferred.
