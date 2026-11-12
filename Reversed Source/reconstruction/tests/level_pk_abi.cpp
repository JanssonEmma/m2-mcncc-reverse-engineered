#include "../../game/src/char.h"
#include "../../game/src/config.h"
// Add to an ABI/signature check linked only when intentionally needed.
#include "../../game/src/char.h"
#include "../../game/src/config.h"
void (CHARACTER::*level_signature)(BYTE)=&CHARACTER::SetLevel;
BYTE (CHARACTER::*gm_signature)()=&CHARACTER::GetGMLevel;
void (CHARACTER::*pk_signature)(BYTE)=&CHARACTER::SetPKMode;
BYTE* mutable_pk_threshold=&PK_PROTECT_LEVEL;
typedef char pk_threshold_size[(sizeof(PK_PROTECT_LEVEL)==1)?1:-1];
