
#include "char.h"
//08073900: original five-instruction tail jump to actual event_cancel.
void CHARACTER::StopMuyeongEvent() { event_cancel(&m_pkMuyeongEvent); }
