#ifndef MCNCC_GAME_CUBE_H
#define MCNCC_GAME_CUBE_H

class CHARACTER;
void Cube_init();
void Cube_print();
void Cube_clean_item(CHARACTER* character);
void Cube_open(CHARACTER* character);
void Cube_close(CHARACTER* character);
void Cube_add_item(CHARACTER* character, int cube, int inventory);
void Cube_delete_item(CHARACTER* character, int cube);
void Cube_show_list(CHARACTER* character);
bool Cube_make(CHARACTER* character);
bool Cube_load(const char* filename);

#endif
