// Partial original game typedef.h: target coordinates are signed long, not INT.
#ifndef MCNCC_GAME_TYPEDEF_H
#define MCNCC_GAME_TYPEDEF_H
#include <set>
#include <list>
class CEntity;
class DESC;
class SECTREE;
typedef CEntity* LPENTITY;
typedef DESC* LPDESC;
typedef SECTREE* LPSECTREE;
typedef struct pixel_position_s { long x, y, z; } PIXEL_POSITION;
typedef std::set<LPENTITY> ENTITY_SET;
typedef std::list<LPSECTREE> LPSECTREE_LIST;
#endif
