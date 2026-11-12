# Profiler recovery

Original organization is header-only profiler.h. Evidence: profiler-ghidra.json, profiler-inline-assembly.json, profiler-inline-ranges.json and profiler-types.json. Full target class1072, member offsets4/8/1032/1052/1064/1068 and nested stack16/accum20 pass current32-bit old-string layout checks. GNU hash_map is retained, including the nested hash algorithm; its deprecation diagnostic is expected.

Constructor initializes containers (100 requested hash buckets),64 string records and scalar counters. Initialize resets counters without clearing accumulated entries/order. Destructor is the evidenced empty application body with normal member/base destruction. PushAccum inserts a new record once and stores its pointer in first-seen order; repeat names retain counters, initial depth and vector slot while resetting start time. The old temporary start field is indeterminate before insertion and is overwritten by the final clock read; no assertion is made about those transient bytes.

PopAccum is backed by caller-inline instructions. Missing names return without a clock read. Known names add elapsed register32 time, increment calls and decrement depth. Recursive uses of the same name overwrite the shared start time. Print emits target signed stack durations and double-precision ratios (the newer reference uses different formatting/precision). Log opens text mode w, returns on failure and closes after Print. Remaining declared historical helpers are not invented.

Tests cover exact report text, nested/repeated/recursive accumulation, first-seen order, missing names, initialization reset scope and file logging. Default/O2 and structural checks pass; original binary is never executed.
