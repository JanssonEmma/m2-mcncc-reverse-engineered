// Observe the loader's diagnostics at the existing test OS boundary.
#define sys_log loader_unused_sys_log
#define _sys_err loader_unused_sys_err
#include "desc_output_syscalls.c"
#undef _sys_err
#undef sys_log

static int loading_logs, index_errors, name_errors;
int loader_loading_logs(void) { return loading_logs; }
int loader_index_errors(void) { return index_errors; }
int loader_name_errors(void) { return name_errors; }

void sys_log(int level, const char* format, ...) {
    if (strcmp(format, "QUEST loading %s : %s [STATE] %s") != 0) {
        loader_unused_sys_log(level, format);
        return;
    }
    assert(level == 0);
    va_list args;
    va_start(args, format);
    assert(strcmp(va_arg(args, const char*), "quest_npc_loader_fixture.tmp") == 0);
    assert(strcmp(va_arg(args, const char*), "q") == 0);
    assert(strcmp(va_arg(args, const char*), "start") == 0);
    va_end(args);
    ++loading_logs;
}

void _sys_err(const char* function, int line, const char* format, ...) {
    assert(strcmp(function, "LoadStateScript") == 0);
    va_list args;
    va_start(args, format);
    assert(strcmp(va_arg(args, const char*), "quest_npc_loader_fixture.tmp") == 0);
    const char* name = va_arg(args, const char*);
    va_end(args);
    if (line == 138) {
        assert(strcmp(format, "invalid QUEST STATE index [%s] [%s]") == 0);
        assert(strcmp(name, "q.start.0") == 0);
        ++index_errors;
    } else {
        assert(line == 150);
        assert(strcmp(format, "invalid QUEST STATE name [%s] [%s]") == 0);
        assert(strcmp(name, "q.start.0.script.extra") == 0);
        ++name_errors;
    }
}
