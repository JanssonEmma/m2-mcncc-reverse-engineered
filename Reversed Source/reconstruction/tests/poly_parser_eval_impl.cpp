// TEST ONLY external ctype seam; full real production Poly.cc is compiled here.
#include "../../libpoly/Poly.h"
#include <cmath>
#include <cctype>
#include <cstdlib>
extern "C" int poly_test_isdigit(int);
extern "C" int poly_test_isalpha(int);
#define isdigit poly_test_isdigit
#define isalpha poly_test_isalpha
#include "../../libpoly/Poly.cc"
#undef isdigit
#undef isalpha
