#ifndef MCNCC_TEST_ITEM_SOCKET_SUPPORT_H
#define MCNCC_TEST_ITEM_SOCKET_SUPPORT_H
#include "sql_connection_fixture.h"
#include <ctime>
#include <cstddef>
struct ItemDevIo {
    virtual ~ItemDevIo() {}
    virtual int Open(const char*,int,int) { std::abort(); }
    virtual int Time(timeval*,void*) { std::abort(); }
    virtual tm* Local(const time_t*,tm*) { std::abort(); }
    virtual long Write(int,const void*,std::size_t) { std::abort(); }
    virtual int Close(int) { std::abort(); }
};
extern ItemDevIo* item_dev_io;
#endif
