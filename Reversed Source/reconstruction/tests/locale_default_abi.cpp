// Include entire actual TU to access original file-static helper without new API.
#include "../../game/src/locale_service.cpp"
#include <set>
#include <string>
typedef char ld_pointer32[sizeof(void*)==4?1:-1];
static void(*const original_static_default)()=&__LocaleService_Init_DEFAULT;
static const std::string&(*const quest_path_api)()=&LocaleService_GetQuestPath;
static std::string*const quest_storage=&g_stQuestDir;
static std::string*const default_object_storage=&g_stDefaultQuestObjectDir;
static std::set<std::string,std::less<std::string>,std::allocator<std::string> >*const object_storage=&g_setQuestObjectDir;
void locale_default_signature_gate() {
    (void)original_static_default;(void)quest_path_api;(void)quest_storage;
    (void)default_object_storage;(void)object_storage;
}
// Original object sizes4/4/12 are evidence of old GNU STL, not assertions on modern STL.
