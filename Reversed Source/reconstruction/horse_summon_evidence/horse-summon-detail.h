#ifndef MCNCC_STAGED_HORSE_SUMMON_DETAIL_H
#define MCNCC_STAGED_HORSE_SUMMON_DETAIL_H
#include <cstring>
namespace horse_summon_detail {
inline int Signed(unsigned int bits) { int value;std::memcpy(&value,&bits,sizeof value);return value; }
inline int Add(long a,long b) { return Signed(static_cast<unsigned int>(a)+static_cast<unsigned int>(b)); }
inline int Mul(long a,long b) { return Signed(static_cast<unsigned int>(a)*static_cast<unsigned int>(b)); }
inline int Trunc32(long double x) {
    // Old x87 controlword OR0xc00 then FISTP, not nearest rounding.
    if(!(x>=-2147483648.0L && x<2147483648.0L)) return (-2147483647-1);
    return static_cast<int>(x);
}
}
#endif
