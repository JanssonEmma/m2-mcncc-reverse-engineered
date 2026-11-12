// Partial original STL helpers: stringhash evidenced inline at08158d20..08158d63.
#ifndef MCNCC_COMMON_STL_H
#define MCNCC_COMMON_STL_H
#include <string>
#include <cstddef>
#include <cctype>
// Inlined in old LoadGroup: normalize key/group bytes with the active C locale.
inline void stl_lowers(std::string& value) {
    for(std::size_t i=0;i<value.size();++i) value[i]=static_cast<char>(std::tolower(value[i]));
}
struct stringhash {
    std::size_t operator()(const std::string& value) const {
        unsigned int hash=0; // target size_t was32 bits, even on a64-bit host
        for(std::size_t i=0;i<value.size();++i)
            hash=hash*16777619U ^ static_cast<unsigned char>(value[i]);
        return hash;
    }
};
#endif
