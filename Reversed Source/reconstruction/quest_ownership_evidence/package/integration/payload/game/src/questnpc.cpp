// Whole original questnpc.cpp owning lifecycle; original complete40container record retained.
#include "questnpc.h"
namespace quest {
//C2 081adf50/C1 081ae0c0: all20+20maps defaultconstruct BEFORE m_vnum store.
NPC::NPC(){m_vnum=0;}
//D2 081ae180/D1 081ae260: original source-level userbody empty; COMPLETE array/member tear follows.
// This is not a teststub: full nativeSTL destroys all20argumentmaps then20plainmaps and ownedbyvalue records.
NPC::~NPC(){}
// Original C1copy081ab460 remains compiler memberwise-copy: m_vnum +all40containers. No fabricatedcopyctor.
}
