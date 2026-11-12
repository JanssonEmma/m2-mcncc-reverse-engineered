// Observe diagnostics while the application uses the native directory APIs.
#define sys_log npc_set_unused_sys_log
#include "desc_output_syscalls.c"
#undef sys_log

static int loaded, phase;
int npc_set_loaded_count(void) { return loaded; }

static const char* tree = "script";
void npc_set_expect_tree(const char* name) {
    assert(phase == 0);
    tree = name;
    loaded = 0;
}

void sys_log(int level, const char* format, ...) {
    const int slot = loaded % 4;
    char path[100];
    sprintf(path, "npc_set_fixture_%c/%s/%s/q.start", 'a' + slot % 2,
            tree, slot < 2 ? "alpha" : "beta");
    va_list args;
    va_start(args, format);
    if (strcmp(format, "QUEST reading %s") == 0) {
        assert(phase == 0 && level == 1);
        assert(strcmp(va_arg(args, const char*), "q.start") == 0);
        phase = 1;
    } else if (strcmp(format, "Quest reading %s") == 0) {
        assert(phase == 1 && level == 0);
        assert(strcmp(va_arg(args, const char*), path) == 0);
        phase = 2;
    } else if (strcmp(format, "QUEST loading %s : %s [STATE] %s") == 0) {
        assert(phase == 2 && level == 0);
        assert(strcmp(va_arg(args, const char*), path) == 0);
        assert(strcmp(va_arg(args, const char*), "q") == 0);
        assert(strcmp(va_arg(args, const char*), "start") == 0);
        ++loaded;
        phase = 0;
    } else {
        npc_set_unused_sys_log(level, format);
    }
    va_end(args);
}
