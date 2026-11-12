// TEST ONLY: existing pointer/owning suites do not exercise RNG. Reach abort,
// not a fake application _random/my_irandom/my_frandom implementation.
#include <stdlib.h>
#ifdef __cplusplus
extern "C"
#endif
long random(void) { abort(); return 0; }
