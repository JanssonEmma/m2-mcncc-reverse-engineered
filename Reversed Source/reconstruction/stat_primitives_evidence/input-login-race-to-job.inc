// Insert in game/src/input_login.cpp; 080f2d50, old input_login.cpp DWARF.
// Requires common/length.h EJobs. ret_job is a required nonnull output pointer.
bool RaceToJob(unsigned race, unsigned* ret_job) {
    *ret_job=0;
    switch(race) {
    case 0: case 4: *ret_job=JOB_WARRIOR; break;
    case 1: case 5: *ret_job=JOB_ASSASSIN; break;
    case 2: case 6: *ret_job=JOB_SURA; break;
    case 3: case 7: *ret_job=JOB_SHAMAN; break;
    default: return false;
    }
    return true;
}
