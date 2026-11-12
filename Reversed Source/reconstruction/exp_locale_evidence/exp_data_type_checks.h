#ifndef MCNCC_EXP_DATA_TYPE_CHECKS_H
#define MCNCC_EXP_DATA_TYPE_CHECKS_H
#include "../../game/src/constants.h"
template<class A,class B> struct ExpSame { enum { value=0 }; };
template<class A> struct ExpSame<A,A> { enum { value=1 }; };
template<class T> void ExpRequireConstArray(T& array) {
    typedef char exact[ExpSame<T,const DWORD[100]>::value?1:-1];
    (void)sizeof(exact);(void)array;
}
template<class T> void ExpRequireMutablePointer(T& pointer) {
    typedef char exact[ExpSame<T,const DWORD*>::value?1:-1];
    (void)sizeof(exact);(void)pointer;
}
inline void ExpCheckAllTypes() {
    ExpRequireConstArray(exp_table_euckr);ExpRequireConstArray(exp_table_gb2312);
    ExpRequireConstArray(exp_table_sjis);ExpRequireConstArray(exp_table_hk);
    ExpRequireConstArray(exp_table_newcibn);ExpRequireMutablePointer(exp_table);
}
#endif
