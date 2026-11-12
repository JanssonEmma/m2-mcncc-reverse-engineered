# Concrete guild level integration regression

Staged only; no production/status/Makefile edits or builds. Copy guild_level_regression.cpp, guild_level_desc_output_syscalls.c and guild_level_abi.cpp into reconstruction/tests after integrating guild.cpp, packet records and exact CHARACTER accessors. Merge Makefile.guild-level.inc, reusing the existing shared guild_lifecycle_fixture.cpp rather than introducing another guild lifecycle model.

The recipe reuses TARGET_WALKING_SOURCES/OBJECTS and adds the actual new guild.cpp exactly once plus the existing full-data test-only guild lifecycle fixture. It replaces only the external syscall/logger object. check/check-abi dependencies add execution without overriding existing recipes; do not register a second duplicate invocation. The ABI recipe reuses the current offsetof warning option and old-string-ABI target32 flags.

## Actual code exercised

Real SGuildMember constructors, map GetMember, LevelChange, CHARACTER constructor/GetJob/GetName/int GetLevel/non-const DWORD GetPlayerID, correct non-const CGuild GetID, CLIENT_DESC::DBPacket/DBPacketHeader and DESC::BufferedPacket/Packet. Full original classes and real std::map/std::set/std::string are used. Existing unrelated lifecycle fixtures remain explicit. No app-method success stubs or reduced production data models are added.

Packet-wrapper verification is in packet-wrapper-verification.json. Neither target inventory nor inspected CHAR DWARF/current/reference CHARACTER headers defines CHARACTER::Packet or CHARACTER::BufferedPacket. Original LevelChange checks the actual CHARACTER GetDesc then calls actual DESC methods directly. These complete DESC/CLIENT_DESC methods are already production code, with complete exports retained. Do not invent CHAR wrapper prototypes/bodies. CGuild::Packet is a separate helper not called by LevelChange.

## Concrete coverage

- Missing GetMember returns0 with unchanged map size. Missing LevelChange with db_clientdesc=0 returns without output/insertion or changing the existing member. Actual member lookup returns the mapped object's address.
- Both real constructor overloads run in correctly aligned placement storage prefilled0xa5/0x5a. Only raw object-representation byte12 is inspected to verify old _dummy storage preservation; the indeterminate scalar is not read. Fields and actual job/playerID/level/name initialization are verified, including copied string lifetime. Stored map fixtures explicitly initialize _dummy before copying/using records.
- Every level0..255 is requested twice: same-level changes must persist and broadcast again.
- External DB log/copy observation verifies new member level is already stored while _dummy remains0xa5. Real DB framing must make four writes1/4/4/14 and the exact23 bytes; the reset to0 is observed only later, at real client header/prefix copies.
- Exact independent golden DB23/client17 vectors preserve requested map key versus differing stored member.pid, raw grade/general/job bytes, high-bit IDs/offer and no name bytes.
- Real online pointer-set order, valid non-member recipients and null-descriptor skipping. The callback observes actual BufferedPacket4, then Packet's direct live member-pointer13 append, then complete17-byte client output. Replacing the sequence with cached or directly combined app output will fail these checks.
- Header-copy callback mutates only current mapped grade/offer fields. The DB snapshot retains original values; both immediately following first-client prefix and later recipient prefix use changed live fields. No map/set erasure, iterator mutation or application dispatch substitution occurs.
- Existing two-byte buffered prefix is retained before the combined17 bytes. A real phase0 descriptor buffers header4 but real Packet returns before prefix13/output; retained buffered bytes are checked and explicitly cleaned afterward. A phase0 actual DB client still produces its normal direct-buffer DB frame.

The C observer reads DB log varargs using actual argument types (unsigned header, DWORD/unsigned-long handle,size,buffer_size). Unexpected _sys_err aborts. No method is mocked to generate a successful outcome.

## Validation and integration requirements

No new runtime or ABI pass is claimed. Staged static checks compare golden vectors to old evidence, verify observation stages and absence of app method replacements/iterator erasure. Parent should run focused normal/O2 plus strict32 ABI and then the affected full suite. GetLevel must be int and both CHARACTER accessors non-const before this compiles; typed member-pointer assertions deliberately reject the pre-existing const GetPlayerID mismatch.

Production guild loading/creation/destruction/online-membership lifecycle and complete PointChange remain unfinished. The existing guild lifecycle test fixture is never installed as production recovery.
