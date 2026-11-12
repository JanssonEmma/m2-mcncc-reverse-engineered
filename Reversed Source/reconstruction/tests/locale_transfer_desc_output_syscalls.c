/* TEST ONLY object-specific compiler entry observation of ACTUAL CTableBySkill::Check. */
#define _sys_err locale_transfer_unused_base_sys_err
#include "desc_output_syscalls.c"
#undef _sys_err
#if defined(__GNUC__)
#define LT_NO_INSTRUMENT __attribute__((no_instrument_function))
#else
#error This test requires GCC function instrumentation
#endif
static void* locale_transfer_check_target;
static unsigned locale_transfer_check_entries;
extern void locale_transfer_observe_check(unsigned);
void LT_NO_INSTRUMENT locale_transfer_set_check_target(void* target) {
    locale_transfer_check_target=target;locale_transfer_check_entries=0;
}
void LT_NO_INSTRUMENT locale_transfer_reset_check_entries(void) {
    locale_transfer_check_entries=0;
}
unsigned LT_NO_INSTRUMENT locale_transfer_check_entry_count(void) {
    return locale_transfer_check_entries;
}
void LT_NO_INSTRUMENT __cyg_profile_func_enter(void* function,void* caller) {
    (void)caller;
    if(function==locale_transfer_check_target && function) {
        ++locale_transfer_check_entries;
        locale_transfer_observe_check(locale_transfer_check_entries);
    }
}
void LT_NO_INSTRUMENT __cyg_profile_func_exit(void* function,void* caller) {
    (void)function;(void)caller;
}
void _sys_err(const char* function,int line,const char* format,...) {
    fprintf(stderr,"unexpected locale transfer dependency error %s:%d %s\n",function,line,format);
    abort();
}
