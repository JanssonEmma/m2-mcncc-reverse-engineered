// Recovered public logging interface (the target uses an integer threshold).
#ifndef MCNCC_LIBTHECORE_LOG_H
#define MCNCC_LIBTHECORE_LOG_H

#ifdef __cplusplus
extern "C" {
#endif
void _sys_err(const char* function, int line, const char* format, ...);
void sys_log(int level, const char* format, ...);
int log_init(void);
void log_destroy(void);
void log_rotate(void);
void log_set_level(int level);
int log_get_level(void);
void sys_log_header(const char* header);
void pt_log(const char* format, ...);
#ifdef __cplusplus
}
#endif

#endif
