# Shared flags for the reconstructed FreeBSD/i386 DB build.
.DEFAULT_GOAL := all
.DELETE_ON_ERROR:

CC = clang
CXX = clang++
ARCH_FLAGS ?= -m32
CFLAGS ?= -O2 -g -Wall -Wextra -pipe
CXXFLAGS ?= -O2 -g -Wall -Wextra -pipe

LOCALBASE ?= /usr/local
MYSQL8_INCLUDE_DIR ?= $(LOCALBASE)/include/mysql
EXTERN_DIR ?= $(patsubst %/include/fmt/printf.h,%,$(firstword $(wildcard \
    $(DB_SERVER_ROOT)/extern/include/fmt/printf.h \
    $(DB_SERVER_ROOT)/../extern/include/fmt/printf.h)))
DB_CPPFLAGS = -DMCNCC_DB_MYSQL8=1 -DFMT_HEADER_ONLY \
              -isystem "$(MYSQL8_INCLUDE_DIR)" -isystem "$(EXTERN_DIR)/include"

.PHONY: check-architecture
check-architecture:
	@printf '%s\n' '#if !defined(__FreeBSD__) || !defined(__i386__)' '#error The DB target requires a 32-bit FreeBSD toolchain' '#endif' | \
	    $(CXX) $(ARCH_FLAGS) $(CXXFLAGS) -std=c++23 -x c++ -fsyntax-only -

ifneq ($(DB_ARCHIVE),)
DB_LIB_OBJDIR := .obj-db
DB_OBJECTS := $(addprefix $(DB_LIB_OBJDIR)/,$(addsuffix .o,$(basename $(DB_LIBRARY_SRCS))))
DB_DEPS := $(DB_OBJECTS:.o=.d)
DB_LIBRARY := $(DB_LIB_OBJDIR)/$(DB_ARCHIVE)
DB_RULES := Makefile $(DB_SERVER_ROOT)/db/db-build.mk

.PHONY: all clean
all: $(DB_LIBRARY)

$(DB_LIBRARY): $(DB_OBJECTS) $(DB_RULES)
	rm -f "$@"
	$(AR) rcs "$@" $(DB_OBJECTS)

$(DB_LIB_OBJDIR)/%.o: %.cpp $(DB_RULES) | check-architecture
	@mkdir -p "$(@D)"
	$(CXX) $(ARCH_FLAGS) $(CPPFLAGS) $(DB_CPPFLAGS) $(CXXFLAGS) -std=c++23 -pthread -MMD -MP -c "$<" -o "$@"

$(DB_LIB_OBJDIR)/%.o: %.cc $(DB_RULES) | check-architecture
	@mkdir -p "$(@D)"
	$(CXX) $(ARCH_FLAGS) $(CPPFLAGS) $(DB_CPPFLAGS) $(CXXFLAGS) -std=c++23 -pthread -MMD -MP -c "$<" -o "$@"

$(DB_LIB_OBJDIR)/%.o: %.c $(DB_RULES) | check-architecture
	@mkdir -p "$(@D)"
	$(CC) $(ARCH_FLAGS) $(CPPFLAGS) $(DB_CPPFLAGS) $(CFLAGS) -std=gnu99 -pthread -MMD -MP -c "$<" -o "$@"

clean:
	rm -f "$(DB_LIBRARY)" $(DB_OBJECTS) $(DB_DEPS)

-include $(DB_DEPS)
endif

