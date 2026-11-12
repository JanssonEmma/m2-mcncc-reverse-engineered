#ifndef MCNCC_LIBTHECORE_SIGNAL_H
#define MCNCC_LIBTHECORE_SIGNAL_H
#ifdef __cplusplus
extern "C" {
#endif
void signal_setup(void);
void signal_timer_disable(void);
void signal_timer_enable(int timeout_seconds);
#ifdef __cplusplus
}
#endif
#endif
