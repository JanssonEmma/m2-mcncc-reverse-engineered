# Reconstruction workflow

The objective remains the complete target-backed server, matching the reference file structure and eventually supporting the requested Visual Studio build. A passing module checkpoint is not server completion.

## Work by dependencies
Recover complete old data layouts first, then connect an entire dependency group through actual application implementations. Current path: locale storage/lookup -> parser and accessory grade/time setters -> ModifyPoints/ApplyPoint/PointChange -> accessory expiry and equipment teardown -> remaining lifecycle/startup -> complete build/link. Follow binary evidence where the newer reference differs. Never fill missing production behavior with no-op substitutes.

## Delegation
The user explicitly authorizes subagents. Give each a bounded independent recovery task, exclusive output files and a compact handoff. Keep large decompilations and notes in files. Parent reviews evidence, integrates production, and owns validation; agents must not mutate overlapping source or trigger overlapping builds. Child F: permissions may differ; staging under the task workspace is supported.

## Evidence and coverage
reconstruction/target-function-inventory.json contains all7238 live Ghidra internal function entries for the verified target hash. function-coverage-overrides.json holds explicit bounded address/source/test reviews. Refresh with:
python reconstruction/tools/update_coverage.py --inventory reconstruction/target-function-inventory.json

The resulting JSON/Markdown ledger distinguishes evidence, candidate implementation, review and test association. Unreviewed does not mean unrecovered. Current24 reviewed associations and783 indexed evidence entries are not a reconstructed-function completion percentage. Preserve assembly/DWARF evidence, provenance and unresolved details separately.

## Staged validation
From any directory, run the actual Python runtime with reconstruction/tools/validate.py. Its default root is reconstruction/; --root overrides it. On this machine supply --make C:/Strawberry/c/bin/gmake.exe --cc C:/Strawberry/c/bin/gcc.exe --cxx C:/Strawberry/c/bin/g++.exe.

1. plan --suite locale_regression shows scheduled compiler outputs without building.
2. run --suite locale_regression builds and executes the focused suite. Multiple --suite arguments are supported.
3. Run suitable optimized and32-bit checks for the changed group.
4. run --full runs all runtime suites and check-abi, with a process receipt and exact executed-command verification. --log chooses a task-workspace log path.

Windows executable targets now match their .exe outputs; unsuffixed make aliases remain available. Repeating an unchanged plan schedules0 runtime compiler outputs. A simulated item.cpp modification schedules8 affected runtime outputs, not all59. Shared-header content fingerprints cover105 current project headers; a difference from the last full validated fingerprint forces rebuilding requested targets. Only a successful full run advances that baseline. --force explicitly forces requested rebuilds. Header rebuilding is conservative until finer generated dependency tracking is introduced. ABI syntax checks intentionally still run at full validation.

Each driver run writes log.receipt.json plus OPERATION_STATE.json/.md before and after the real process. Full success updates validation-status.json only if the actual whole-line runtime commands exactly match RUNTIME_TESTS. Compiler warning lines cannot enter the suite list. Focused success never claims full validation. A running receipt alone does not establish a live process; check its actual PID/handle and log before restarting work.

## Checkpoints and usage
At meaningful checkpoints check remaining usage, with more frequent checks above80% used. Refresh task outputs/METIN2_RESUME.md and reconstruction/RESUME_CHECKPOINT.md before limits prevent further writes. Include current production changes, exact validation receipts, actual running handles or completed state, staged agent files, known limitations, and the next concrete actions. Preserve the full goal as active until completion is proved. Never consume an account reset credit automatically.
