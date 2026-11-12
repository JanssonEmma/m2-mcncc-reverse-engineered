// Insert in game/src/char.cpp; 0812a580, old non-const signature.
// Requires libthecore/include/log.h and this original cross-unit declaration.
extern bool RaceToJob(unsigned race, unsigned* ret_job);
BYTE CHARACTER::GetJob() {
    const unsigned race=static_cast<unsigned>(m_points.job);
    unsigned job;
    if(RaceToJob(race,&job)) return static_cast<BYTE>(job);
    _sys_err("GetJob",1569,"CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE",GetName(),race);
    return 0;
}
