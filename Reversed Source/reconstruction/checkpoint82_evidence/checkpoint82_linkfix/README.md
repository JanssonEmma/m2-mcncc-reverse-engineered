# Checkpoint82 TEST ONLY link closure

Ready parent guard: `apply_guarded.py` (default dry-run; parent `--apply`). Changes only reconstruction/tests/Makefile plus two new test tools. No production/source logic changes and no child builds/F writes.

Seven bare DESC_PACKET_SOURCES graphs now link actual item_socket_config.o: desc_output_unit, desc_output_integration, desc_packet_integration, input_regression, desc_input_integration, desc_flush_regression, desc_control_regression. That existing object includes complete config.cpp, preserving real quest globals and defaults. Its four renamed unrelated globals avoid desc_fixture definitions. Inspection found no conflicting postfix/item range/quest global definitions in these seven fixtures or desc_fixture. Full ITEM_SOCKET_OBJECTS graphs remain untouched to avoid linking config twice.

Only unit/input/flush additionally link desc_rng_only.o and desc_rng_os_boundary.o. The other four already link complete actual desc_utils.o and its existing OS observer. The new raw object compiles COMPLETE actual libthecore/src/utils.c with function sections and no LTO; project_rng.py preserves the actual thecore_random code/unwind bytes and every retained relocation identity/type/offset. It rebuilds symbol and section indices, retaining external undefined random. Naive objcopy was rejected because it silently retargeted REL32 random to thecore_random. No reduced source body or replacement application RNG exists.

The OS boundary reuses existing utils_link_stubs.c with -UNDEBUG: an unexpected random/fork aborts. These old descriptor suites do not exercise RNG. Standalone rng_projection_smoke links the SAME projected actual function with a scripted external random boundary, asserting exactly one call per zero/one/negative/long-extreme input and exact DWORD cast. It remains outside canonical88, whose names/order are byte-for-byte unchanged.

Parent gates (from reconstruction/tests): make rng_projection_smoke.exe, run rng_projection_smoke.exe, then seven affected suites+check-abi, then full88+ABI. Source/Makefile changes are guarded against the exact current baseline. Parent should preserve standalone projection smoke exit0/proof with final validation receipt. No runtime pass claimed here.

Static selfcheck, no compiler/execution: projected parent's saved work/rng-projection-probe-full.o to probe-projected.o. probe-proof.json records equal32 code bytes, equal12-byte unwind sections, REL32 random offset9 and original pdata relocations. Independent objdump -r and nm -g confirmed U random/T thecore_random. Projection supports ordinary x86-64 COFF; unsupported naming/object layouts fail closed. This is a test linker boundary, not an ABI claim for modern STL or an original runtime RNG recovery claim.

Whole reconstruction goal remains unfinished. Checkpoint81 is the latest fully verified baseline until parent completes checkpoint82 full88; combined recorder remains pending.
