# Independent integration audit (read-only)

Reviewed CURRENT production game/src/db.h,db.cpp,char_item.cpp,char_quickslot.cpp and reconstruction/tests/autogive_closed_regression.cpp against staged original DWARF/decomp evidence. No production writes or builds performed.

## Required fixture correction
Current test line85 constructs CHARACTER ch without a quest::CQuestManager instance. Actual CHARACTER initialization in char.cpp207 calls CQuestManager::instance().GetEventFlag("arena_potion_limit_count"); ClearKillCount char.cpp114 also calls GetEventFlag("threeway_war_dead_count") from normal Initialize path. Singleton assert will fail before grid/quickslot tests. Include questmanager.h and construct quest::CQuestManager quests BEFORE CHARACTER ch; preserve quests until after ch destruction. This is a missing required REAL dependency fixture, not permission to stub GetEventFlag or CHARACTER constructor. Parent notified immediately.

## Production audit result
No additional integration mismatch found in reviewed closed method bodies:
- DBManager complete eight-field order + singleton base + virtual destructor matches dbmanager-dwarf.json. Both actual SQL members defaultconstruct/destruct; false connection init matches full0810bde0. Empty userdestructor matches full0810c210 (only field/base teardown).
- SendMoneyLog keeps zero suppression, signed int gold, actual DBPacket50/handle0/payload9, correct field assignment.
- Grid preserves raw unsigned exception+1, BYTE rowindex truncation, same45cellpage, anchor validation, size1 specialcase and size0 do-while rowcheck; GetEmpty scans90.
- Quickslot preserves invalid target/type earlyguard, duplicate deletion BEFORE item/skillposvalidation, reload inputtype aftereach actual deletion, mutable reference/livealiasing, type3unrestricted, nullguardonly onadd. Del has no descriptor guard, as original. Get invalid leaves outputuntouched.
- Packet/data declarations appear within existing packed regions; production test ABI gates should verify sizes/offsets. No extra runtimeclaim from sourceaudit.

## Test observations
Current fixture checks realDB18byteframe, actual SQL/semaphore teardown, fullCHARoccupancy/quickslots, packet-time live mutations, aliasing and invalidposition-afterdelete. Existing actual descriptor lifecycle fixture used. ch.BindDesc(0) occurs before descriptor/character teardown. sql_test_runtime cleared after DB scope (both realSQLdestructors already completed), so no additional DB teardown issue.
Host/fullclass sizes should not be asserted equal original2372-byteDBManager: current STL implementation differs. Existing dedicated field/method ABIgates are appropriate. Tests staged by thisagent were not compiled here; parent owns focused/O2/strict32/runtime verification.

