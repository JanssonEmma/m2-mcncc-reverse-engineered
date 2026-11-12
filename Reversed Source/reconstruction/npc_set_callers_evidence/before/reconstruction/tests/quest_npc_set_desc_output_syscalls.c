// Observe diagnostics while the application uses the native directory APIs.
#define sys_log npc_set_unused_sys_log
#include "desc_output_syscalls.c"
#undef sys_log

static int loaded, phase;
int npc_set_loaded_count(void) { return loaded; }

void sys_log(int level, const char* format, ...) {
    static const char* const paths[] = {
        "npc_set_fixture_a/script/alpha/q.start",
        "npc_set_fixture_b/script/alpha/q.start",
        "npc_set_fixture_a/script/beta/q.start",
        "npc_set_fixture_b/script/beta/q.start"
    };
    va_list args;
    va_start(args, format);
    if (strcmp(format, "QUEST reading %s") == 0) {
        assert(phase == 0 && level == 1);
        assert(strcmp(va_arg(args, const char*), "q.start") == 0);
        phase = 1;
    } else if (strcmp(format, "Quest reading %s") == 0) {
        assert(phase == 1 && level == 0 && loaded < 4);
        assert(strcmp(va_arg(args, const char*), paths[loaded]) == 0);
        phase = 2;
    } else if (strcmp(format, "QUEST loading %s : %s [STATE] %s") == 0) {
        assert(phase == 2 && level == 0 && loaded < 4);
        assert(strcmp(va_arg(args, const char*), paths[loaded]) == 0);
        assert(strcmp(va_arg(args, const char*), "q") == 0);
        assert(strcmp(va_arg(args, const char*), "start") == 0);
        ++loaded;
        phase = 0;
    } else {
        npc_set_unused_sys_log(level, format);
    }
    va_end(args);
}
