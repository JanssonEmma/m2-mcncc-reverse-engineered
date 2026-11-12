# Staged descriptor output slice

Integrate common/tables.h and game/src/desc.h/.cpp only. The libthecore/include copies and event.h copy are unchanged compile dependencies, not replacements for shared files. No production stub methods are provided. Header deliberately omits unrecovered nonvirtual API while retaining every actual member and all virtual declarations in order.

Evidence is evidence-types.json, plus ../descriptor_assessment/ghidra-focused.json and assembly.json. The globals max/current/total_bytes_written are DWARF int4 at08322cd8/dc/e0. ProcessOutput is08139bb0; MIN and MAX calls081b9260/081b9280 use signed int parameters. Unsigned addition followed by conversion retains target counter wrap without signed-overflow optimization. The size_t conversion after signed MIN matches the old call sequence, including implausible oversized-buffer behavior.

Validation: desc.cpp compiles to32-bit object with GCC13.2, C++03, -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -D_USE_32BIT_TIME_T. tests/layout.cpp passes all51 member offsets and player63/account333/string4/DESC580 sizes using those flags and -Wno-invalid-offsetof. This is structural evidence only, not a linked old-runtime or full server validation. The old libstdc++ ABI selection actually works on this bundled compiler; previous feasibility concern is resolved for compile-only validation.

Include paths for tests are staged tests/shim, then destination reconstruction/tests/socket_shim and posix_shim. Staged sys/event.h adds a _TIMESPEC_DEFINED guard to the current fdwatch test header, necessary when std::string pulls in host pthread headers. Parent should apply the same guard to the shared test shim. Real production builds use FreeBSD system headers.

tests/desc_fixture.cpp provides explicitly test-only lifecycle/virtual bodies; DescOutputProbe in tests/desc_fixture.h exposes Attach for the three fields used by output. output_unit.cpp passes host unit cases for empty buffer, capacity zero/negative, failure preserving bytes/counters, partial write/rearm, full drain, signed MAX of a negative counter, and 32-bit counter wrap. Its C-module stubs are unit fixtures only; parent integration uses actual buffer/socket/fdwatch sources.

Commands run from workspace cwd (SHIM_ARGS expands to -Iwork/desc_output/tests/shim and destination reconstruction/tests/socket_shim and posix_shim):

```
g++ -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -D_USE_32BIT_TIME_T -std=c++03 -Wall -Wextra SHIM_ARGS -c work/desc_output/game/src/desc.cpp -o work/desc_output/tests/desc32.o
g++ -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -D_USE_32BIT_TIME_T -Wno-invalid-offsetof -std=c++03 SHIM_ARGS -fsyntax-only work/desc_output/tests/layout.cpp
g++ -D_GLIBCXX_USE_CXX11_ABI=0 -std=c++03 -Wall -Wextra SHIM_ARGS work/desc_output/tests/output_unit.cpp work/desc_output/tests/desc_fixture.cpp work/desc_output/game/src/desc.cpp -o work/desc_output/tests/output_unit.exe
work/desc_output/tests/output_unit.exe
```

All commands passed. Compiler is C:/Strawberry/c/bin/g++.exe. Layout fixture changes protected visibility only for offset inspection; production header retains DWARF access (all fields protected except public disconnect-event pointer). Artificial vptr is supplied by the compiler. Metadata includes exact member offsets, types, bounds, accessibility and global counter types/addresses. Promotable production list remains only common/tables.h, game/src/desc.h and game/src/desc.cpp; test fixture promotion is separately optional. Do not promote event.h or libthecore dependency snapshots.
