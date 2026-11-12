#define sys_log quest_global_fallback_log
#define _sys_err quest_global_fallback_error
#include "desc_output_syscalls.c"
#undef sys_log
#undef _sys_err

extern void quest_global_lua_log(const char*, const char*, const char*);
extern void quest_global_lua_error(const char*);

void sys_log(int level, const char* format, ...)
{
    if (!strcmp(format, "from Quest %s Player %s msg : %s")) {
        va_list args;
        const char *quest, *player, *message;
        assert(level == 0);
        va_start(args, format);
        quest = va_arg(args, const char*);
        player = va_arg(args, const char*);
        message = va_arg(args, const char*);
        va_end(args);
        quest_global_lua_log(quest, player, message);
        return;
    }
    quest_global_fallback_log(level, format);
}

void _sys_err(const char* function, int line, const char* format, ...)
{
    if (!strcmp(function, "_syserr")) {
        va_list args;
        const char* message;
        assert(line == 62 && !strcmp(format, "%s"));
        va_start(args, format);
        message = va_arg(args, const char*);
        va_end(args);
        quest_global_lua_error(message);
        return;
    }
    quest_global_fallback_error(function, line, format);
}
