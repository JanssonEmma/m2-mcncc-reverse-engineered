#include "../../game/src/locale_service.cpp"
#include <string>
typedef char lr_pointer32[sizeof(void*)==4?1:-1];
typedef char lr_four_slots[PLAYER_PER_ACCOUNT==4?1:-1];
static bool(*const public_init)(const std::string&)=&LocaleService_Init;
static void(*const private_slot)(const std::string&)=&__CheckPlayerSlot;
void locale_regions_abi_gate() {
 void(*const helpers[])()={__LocaleService_Init_CIBN,__LocaleService_Init_CIBN_OLD,__LocaleService_Init_JAPAN,__LocaleService_Init_English,__LocaleService_Init_HongKong,__LocaleService_Init_NewCIBN,__LocaleService_Init_Germany,__LocaleService_Init_Korea,__LocaleService_Init_France,__LocaleService_Init_Italy,__LocaleService_Init_spain,__LocaleService_Init_UK,__LocaleService_Init_Turkey,__LocaleService_Init_Poland,__LocaleService_Init_Portugal,__LocaleService_Init_Canada,__LocaleService_Init_Brazil};
 (void)helpers;(void)public_init;(void)private_slot;
}
// Complete logical API; original GNU STL string/set layout is NOT a host layout gate.
