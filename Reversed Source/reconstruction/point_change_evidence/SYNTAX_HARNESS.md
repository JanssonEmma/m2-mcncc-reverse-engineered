# Reproducible syntax-only harness

Prepared overlay (no compiler run): syntax_tree_20261025T161932567965Z,158 COMPLETE copied production headers,8 missing-declaration additions. Source candidate SHA/header SHA hashes and mutation list are in its SYNTAX_MANIFEST.json. Original production headers were checked unchanged during copying. No existing app method bodies or records were replaced; complete inline bodies copied from production remain original current headers. Only exact original missing declarations plus exp_table extern were added; Query public access is enforced if needed (alreadypublic in this snapshot).

To refresh after parent header changes (always chooses a NEW timestamped overlay; no recursive deletion or stale reuse):

    PYTHON work/point_change/prepare_syntax_overlay.py

Default production root is PROJECT_ROOT\Reversed Source. Optional --production-root and --out NEW_ABSOLUTE_PATH. The script NEVER invokes any compiler. Class spans are balanced from full copied headers; each declaration is compared by metadata-proven params/return/nonconst and added only if absent. An incompatible existing signature raises rather than shadowing it. Manifest records exact declaration proof paths. Existing exp_table must be mutableconstDWORD* or preparation fails. No global definition/locale initialization is created.

Parent frontend invocation (use actual existing compiler path; no compiler acquisition):

    powershell -File work/point_change/check_syntax_overlay.ps1 -OverlayRoot ABSOLUTE_OVERLAY -Compiler EXISTING_GXX_PATH

The script loads exact args from manifest, changes cwd only to overlay/reconstruction/tests, runs C++03 -m32 -fsyntax-only for copied PointChange.syntax.cpp and signature/record checks, and writes SYNTAX_RECEIPT.json. Other args match existing full character SQL/descriptor include environment: fdwatch/socket/posix/sql shims, actual copied mysql headers, sql_socket_platform.h, old COW ABI define. No object output, linking or application body is supplied. Existing free/inline declarations and external assertion seams are sufficient for frontend parsing; they are not runtime mocks. If header-copy source updates, create a fresh snapshot and keep candidate provenance/receipt matched.

A syntax exit0 establishes C++03/API/visibility/record-shape frontend validity ONLY. It does not prove runtime behavior, link closure, actual Lua/item/mount/EXP selection or the full SCC. Manifest/receipt mark functional_proof false. All missing actual function/data frontiers remain dependency-frontier.json regardless of a syntax pass. This agent prepared/copy-checked the overlay but DID NOT run check_syntax_overlay.ps1 or any compiler.
