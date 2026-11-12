// C++03 exact API checks; existing character/manager gates retain complete logical data.
#include "../../game/src/char.h"
#include "../../game/src/char_manager.h"
void character_save_signature_checks() {
    void (CHARACTER::*save)()=&CHARACTER::Save;
    void (CHARACTER_MANAGER::*delayed)(LPCHARACTER)=&CHARACTER_MANAGER::DelayedSave;
    (void)save;(void)delayed;
}
