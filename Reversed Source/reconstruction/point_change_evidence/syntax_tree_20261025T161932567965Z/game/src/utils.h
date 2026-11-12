// Partial game utility interface recovered from the original binary.
#ifndef MCNCC_GAME_UTILS_H
#define MCNCC_GAME_UTILS_H
#include <ctime>
int CalculateDuration(int speed,int duration);
time_t get_global_time();
void set_global_time(time_t t);
//08051610: signed x86 arithmetic, including register wrapping at integer extremes.
inline int DISTANCE_APPROX(int dx,int dy) {
    if(dx<0) dx=static_cast<int>(0U-static_cast<unsigned int>(dx));
    if(dy<0) dy=static_cast<int>(0U-static_cast<unsigned int>(dy));
    int maximum=dx;
    if(dx<dy) { maximum=dy; dy=dx; }
    int weighted=static_cast<int>(static_cast<unsigned int>(maximum)*246U+static_cast<unsigned int>(dy)*102U);
    return weighted>>8;
}
#endif
