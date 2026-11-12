# Character registry and scheduling recovery

The complete old manager data declaration follows target DWARF (172 bytes on the original ABI). Modern STL layout is not asserted equivalent. Constructor rates are all 100; registered races are 9004, 20031 and 20032. Destroy at 0818c1c0 has an evidenced empty application body.

Recovered behavior includes wrapping AllocVID, numeric VID and PID lookups, typed VID lookup, unique state insertion/removal, selected stone and race registration. Typed lookup checks both embedded ID and CRC; a numeric registry entry alone does not authenticate an identity. VID copy, assignment, equality and reset follow the two-DWORD identity. Default/value construction is a source-guided equivalent over the evidenced fields.

CHARACTER StartStateMachine and StopStateMachine now use the real manager. Start sets the next pulse only after a new insertion, preserving unsigned 32-bit wrapping; repeated starts do not reschedule.

FindPC at 0818c880 uses the first 24 input bytes and ASCII A-Z folding. The recovered body expands the old str_lower at 08146e30 locally to avoid coupling registry-only users to unrelated game clock functions. It preserves empty names, high bytes and exact map lookup semantics. Null input remains outside the original precondition. The original strlen/MIN scans the full input before copying; the reconstruction bounds the scan at 24, with equivalent results for valid NUL-terminated strings.

DestroyCharacter0818c490 is now a production implementation. The complete CHARACTER destructor itself is still unresolved, so full character resource teardown is not yet available. Tests use opaque pointers only in registry paths that never dereference them; typed lookup is separately exercised with full declared CHARACTER objects in descriptor control tests.


## Registry cleanup checkpoint

RegisterRaceNumMap0818e570 tests the current WORD-valued race against the registered DWORD set, logs every accepted call including duplicates, and inserts uniquely. UnregisterRaceNumMap0818e7d0 uses the current race and retains empty buckets. Changing a character race before unregistering leaves its previous membership untouched. GetCharacterSetByRaceNum0818e840 creates an empty bucket for any absent key and returns the actual mutable set, including races not in the registration filter.

DestroyCharacter erases the embedded VID key, removes state membership, erases the folded/truncated name key, conditionally erases nonzero PID, unregisters the current race, then invokes virtual deletion. There are no pointer-identity checks, no missing-VID return, no pending-destroy mechanism and no delayed-save/monster-log/selected-stone cleanup in this old body. Null is not a valid input. GetPlayerID's direct field accessor is corroborated by loads in this caller. The name helper expansion has the same valid-string result as the previously documented FindPC implementation.

DelayedSave0818e250 and RegisterForMonsterLog0818e310 uniquely insert pointers, including null; UnregisterForMonsterLog0818e340 erases absent or present pointers. SaveReal, delayed-save processing/flush and graceful disconnect remain pending production recovery. Their staged evidence is not an implementation.

The character-name integration suite exercises real manager cleanup with full declared characters and an explicit test-only character destructor. A derived destructor verifies that registries have already been removed before deletion; tests cover mismatched registry pointers, high IDs, PID zero, duplicate logs, changing races, WORD truncation, retained empty buckets and intentionally untouched registries. This tests manager ordering, not full CHARACTER resource destruction. The manager suite now links real character/registry dependencies; the previous unused GetName/GetRaceNum/log fixture has been removed. Remaining character destructor/virtual and quest lifecycle boundaries stay explicit.
