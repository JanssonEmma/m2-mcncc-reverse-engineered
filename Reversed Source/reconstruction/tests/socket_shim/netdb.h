#ifndef MCNCC_TEST_NETDB_ABI_H
#define MCNCC_TEST_NETDB_ABI_H
struct hostent {
    char* h_name;
    char** h_aliases;
    int h_addrtype, h_length;
    char** h_addr_list;
};
struct hostent* gethostbyname(const char*);
#endif
