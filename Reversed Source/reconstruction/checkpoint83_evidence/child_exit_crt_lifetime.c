/* TEST HOST ONLY. No recovered application body or exit status is replaced.
 * Captured Windows teardown executed an unmapped msvcrt!_freefls callback.
 * Keep the SYSTEM CRT mapped for the test process lifetime. The original main
 * and its real std::exit/atexit/global teardown continue normally.
 * No exception handler, retries, exit interception, or forced success.
 */
#define _WIN32_WINNT 0x0601
#include <windows.h>
extern int __real_main(int,char**);
int __wrap_main(int argc,char** argv)
{
    WCHAR path[MAX_PATH];
    const WCHAR suffix[]={0x5c,'m','s','v','c','r','t','.','d','l','l',0};
    UINT n=GetSystemDirectoryW(path,MAX_PATH),i;
    HMODULE loaded=0,pinned=0;
    if(!n || n+(sizeof suffix/sizeof suffix[0])>MAX_PATH) return 127;
    for(i=0;i<sizeof suffix/sizeof suffix[0];++i) path[n+i]=suffix[i];
    loaded=LoadLibraryW(path);
    if(!loaded || !GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN,path,&pinned) || loaded!=pinned) return 127;
    /* Pin is deliberate process-lifetime ownership, not a production library. */
    return __real_main(argc,argv);
}
