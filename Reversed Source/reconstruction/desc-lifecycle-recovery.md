# DESC lifecycle recovery

Observed evidence: desc-lifecycle-ghidra.json and desc-lifecycle-assembly.json, matching the target SHA-256 in validation-status.json. Independent static review corroborated the field boundary.

Recovered source: both constructor variants at 0x08138c70/0x08138d20 map to DESC::DESC(){Initialize();}; destructor at 0x08138dd0 calls base Destroy before automatic string destruction; Initialize at 0x08139200; GetType at 0x0813b300 returns zero. DWARF marks Initialize protected. Standard C++ string construction/destruction replaces compiler-generated COW implementation details; this is source reconstruction, not a binary equivalence claim.

Initialize clears the account table and UDP sockaddr, plus explicit fields in desc.cpp. It sets socket=-1 and pong=true. It deliberately leaves all eight strings, TCP sockaddr, handshake-sent/client times, P2P channel, both key arrays, and out/play/off times untouched. Tests seed these fields before reinitializing; fresh indeterminate fields are never read.

Production Destroy and SetPhase remain unresolved. Their test-only vtable bodies remain isolated in tests/desc_fixture.cpp. The destructor test verifies a call to that cleanup fixture, not actual server cleanup. Destroy evidence shows account-manager, character disconnect, database packet and descriptor logging dependencies; it also deletes both main buffers unconditionally. Do not copy newer idempotence guards or claim complete lifecycle behavior. No full server link or original-binary execution performed.

Validation extends desc_output_unit with all reset-field checks, retained-field checks, GetType and destructor invocation. Existing real buffer/socket/fdwatch output integration now uses the recovered constructor and destructor entry.
