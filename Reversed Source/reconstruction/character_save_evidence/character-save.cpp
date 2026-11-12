#include "char.h"
#include "char_manager.h"
//08128b50: register a deferred save; skip guard precedes singleton access.
void CHARACTER::Save() {
    if(!m_bSkipSave) CHARACTER_MANAGER::instance().DelayedSave(this);
}
