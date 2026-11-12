#ifndef MCNCC_GAME_XMAS_EVENT_H
#define MCNCC_GAME_XMAS_EVENT_H
#include <string>
namespace xmas {
void ProcessEventFlag(const std::string& name, int previous, int value);
void UpdateFireworkSellers(bool spawn, const char* function, int line);
void SpawnSanta(long map, int delay);
}
#endif
