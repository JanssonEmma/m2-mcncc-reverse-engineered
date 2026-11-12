# Windows test-host CRT lifetime diagnosis and guard

Observed failure: original normal child status0xC0000005 after completed real exit/atexit witness. Nondebug wrapper observations captured BOTH first-chance and unhandled EXECUTE violation parameter0=8, RIP0x7ff86e92e990; modulelookup atfaultERROR_MOD_NOT_FOUND126. Windows System32/msvcrt.dll exact exported _freefls RVA0x3e990 + runtimebase0x7ff86e8f0000 observed in process-tree module evidence yields the exact fault address. Shutdown stack slots include ntdll loader/process shutdown; rawslots are not an unwound backtrace. This supports an unloaded CRT FLS callback. The component/unload sequence responsible remains unproven.

Evidence: work/child-av-nondebug-transfer-f-crashdir/collected-native/*unhandled-av*.log and firstchance peer; receipt.json records failure and original data hashes. Native C executable could not create Cworkspace files (WinAPIerror2/CRTerrno2) but could create Fprojectfiles; capture-dirF override made startup+fault logs visible. Do not label path mechanism CFA/ACL without proof. DEBUG_PROCESS invalidhandle0xc0000008 is a separate documented debugger diagnostic; never an originalAV trace.

Controlled test-host remedy: LoadLibraryW WindowsSystemDirectory/msvcrt.dll and GetModuleHandleExW(PIN) to keep that REAL systemCRT mapped until process termination. No exception handlers, appmethod substitutions, retries, forcedstatus, or exit interception. __wrap_main immediately calls original __real_main with unchanged args; actual std::exit/atexit/globaldestructors still execute. Setup failure127 fails test expectations. POSIX test recipes unchanged. Only two Windows test recipes and new test-only C file changed, no game source or runtime port.

Diagnostic guard probe64 parentruns passed; normal testexes32parentruns passed; normalfocused2+ABI passed work/crt-lifetime-focused-retry.receipt.json. Initial helper suffix typo exited127 and is retained as failedsetup receipt, corrected with numeric0x5c separator. New locale region O2 validation passed with real guard. Full91 currently running, not yet claimed passed.

Primary platform references:
- https://learn.microsoft.com/en-us/troubleshoot/developer/visualstudio/cpp/libraries/fatal-error-thread-exit-fls-callback
- https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandleexw (PIN keeps DLL until process termination)
- https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle (debug invalidhandle exception)

This is a Windows TEST HOST compatibility boundary, not recovered2009 application behavior or nativeWindows production support.
