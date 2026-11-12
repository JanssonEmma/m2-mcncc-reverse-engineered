# Item constructor canary DSE: minimal test-only fix

Parent full82 stopped at item_regression.cpp41, checkpoint-79-validation.log3260. Banword focused tests passed. New legacy -O2 -flto exposed constructor lifetime dead-store elimination in the raw-byte observer; no production layout or constructor regression found.

Read-only inspection of EXISTING failing item_regression.exe (PE64) using objdump, never executed/rebuilt here:
- main item storage base rsp+0x140.
- Original canary comparison loop reads rsp+0x1b0 through rsp+0x22c inclusive (m_pProto at object+0x70).
- At140013e7e only retained seed is movl0xa5a5a5a5 to rsp+0x22c; seed writes for preceding canary region were eliminated.
- Real base Initialize call at140013e99 goes to140002590. Its stores are confined to CEntity base offsets through0x68, below derived field0x70.
- CItem vptr store at140013ead affects object+0 only; canary compare starts140013ebd. Empty CItem ctor was inlined. There is no constructor write corrupting derived scalar range.
Full snapshots main-disassembly.txt and entity-initialize-disassembly.txt preserve evidence.

Cause: ordinary pre-placement-new memset over a later object's indeterminate scalar storage was optimized away by O2/LTO lifetime analysis, while the old raw union-byte assertion still inspected physical storage. Do not initialize production scalars, alter base/derived layout, shrink canary range, remove checks, or disable optimization to conceal it.

Guarded one-file TEST fix is ready in parent_apply/payload/reconstruction/tests/item_regression.cpp. Observable volatile unsigned-byte writes seed EVERY original storage byte; const volatile unsigned-byte reads inspect the actual live item's representation. Original first/last address calculation, inclusive loop and each exact0xa5 comparison remain unchanged. No scalar value/trap representation is evaluated. No production object/type/signature or Makefile change.

apply_fix.py defaults to baseline/payload hash verification only; --apply is parent-only and backs up existing file before atomic replacement. Dry-run verified1file. Parent owns focused/full validation. No builds, binary execution or F writes by staging agent.

Stopped at parent usage87% request. Transfer package remains staged/deferred under work/locale_transfer; its guarded baselines may need re-preparation after parent source/Makefile changes. Full reconstruction remains unfinished.
