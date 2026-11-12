// Partial game utility interface recovered from the original binary.
#ifndef MCNCC_GAME_UTILS_H
#define MCNCC_GAME_UTILS_H
#include <ctime>
int dice(int number, int size);
int string_dice(const char* text);
int decision(int roll, int target);
const char* get_filename_ext(const char* filename);
bool is_filename_ext(const char* filename, const char* extension);
char* first_cmd(char* argument, char* first, int limit, int* length);
char* last_cmd(char* argument, char* last);
char* text_gauge(int current, int maximum, int length);
char* get_time_string(int milliseconds);
int parse_time_str(const char* text);
int CalculateDuration(int speed,int duration);
time_t get_global_time();
void skip_spaces(char** text);
char* any_one_arg(char* argument, char* first);
void half_chop(char* argument, char* first, char* rest);
float linear_interpolate(float begin, float end, float rate);
int highest_array_element(int* array, int size);
char* one_argument(char* argument, char* first);
char* two_arguments(char* argument, char* first, char* second);
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
// Add before final guard in original game/src/utils.h. Old C++ linkage.
void str_lower(const char* src,char* dest,int len);

float gauss_random(float avg,float sigma);
double gauss_random_extended(float avg,float sigma);

#endif
