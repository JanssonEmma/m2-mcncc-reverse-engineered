# Run gmake from db/src on FreeBSD/i386.
DB_SERVER_ROOT := ../..
include $(DB_SERVER_ROOT)/db/db-build.mk

BINDIR ?= ..
OBJDIR ?= .obj
DBDIR ?= ../../../../server/share/bin
TARGET := $(BINDIR)/db

MYSQL8_CLIENT_LIBRARY ?= $(firstword $(wildcard \
    $(LOCALBASE)/lib/mysql/libmysqlclient.so $(LOCALBASE)/lib/libmysqlclient.so))

SRCS := Config.cpp NetBase.cpp Peer.cpp PeerBase.cpp Main.cpp Lock.cpp DBManager.cpp \
        Cache.cpp LoginData.cpp ClientManager.cpp ClientManagerPlayer.cpp ClientManagerLogin.cpp \
        ClientManagerBoot.cpp ClientManagerParty.cpp ClientManagerGuild.cpp GuildManager.cpp HB.cpp \
        PrivManager.cpp MoneyLog.cpp ItemAwardManager.cpp ClientManagerEventFlag.cpp Marriage.cpp \
        BlockCountry.cpp
OBJS := $(SRCS:%.cpp=$(OBJDIR)/db/%.o)
DEPS := $(OBJS:.o=.d)
DB_RULES := Makefile $(DB_SERVER_ROOT)/db/db-build.mk

LIB_NAMES := libsql libpoly libgame libthecore
LIBS := $(foreach lib,$(LIB_NAMES),$(DB_SERVER_ROOT)/$(lib)/.obj-db/$(lib).a)
LIB_CLEAN := $(addprefix clean-,$(LIB_NAMES))

.PHONY: all build deploy clean check-environment FORCE $(LIB_CLEAN)
all: deploy

build: $(TARGET)

deploy: $(TARGET)
	@test -d "$(DBDIR)" || { echo "DBDIR does not exist: $(DBDIR). Set DBDIR or run gmake build." >&2; exit 1; }
	@tmp=$$(mktemp "$(DBDIR)/.db.XXXXXX") && { \
	    install -m 755 "$(TARGET)" "$$tmp" && mv -f "$$tmp" "$(DBDIR)/db" || \
	    { rm -f "$$tmp"; exit 1; }; \
	}

check-environment: check-architecture
	@test -f "$(EXTERN_DIR)/include/fmt/printf.h" || { echo "Set EXTERN_DIR to the directory containing include/fmt/printf.h." >&2; exit 1; }
	@test -f "$(MYSQL8_INCLUDE_DIR)/mysql.h" || { echo "Set MYSQL8_INCLUDE_DIR to the native Oracle MySQL 8 header directory." >&2; exit 1; }
	@test -f "$(MYSQL8_CLIENT_LIBRARY)" || { echo "Set MYSQL8_CLIENT_LIBRARY to the native Oracle MySQL 8 libmysqlclient.so." >&2; exit 1; }

$(LIBS): FORCE | check-environment
	+$(MAKE) -C "$(@D)/.." all EXTERN_DIR="$(abspath $(EXTERN_DIR))" MYSQL8_INCLUDE_DIR="$(abspath $(MYSQL8_INCLUDE_DIR))"

$(TARGET): $(OBJS) $(LIBS) $(MYSQL8_CLIENT_LIBRARY) $(DB_RULES) | check-environment
	@mkdir -p "$(@D)"
	$(CXX) $(ARCH_FLAGS) $(LDFLAGS) -pthread -o "$@" $(OBJS) $(LIBS) "$(MYSQL8_CLIENT_LIBRARY)" -lz -lm $(LDLIBS)

$(OBJDIR)/db/%.o: %.cpp $(DB_RULES) | check-environment
	@mkdir -p "$(@D)"
	$(CXX) $(ARCH_FLAGS) $(CPPFLAGS) $(DB_CPPFLAGS) $(CXXFLAGS) -std=c++23 -pthread -MMD -MP -c "$<" -o "$@"

clean: $(LIB_CLEAN)
	rm -f "$(TARGET)" $(OBJS) $(DEPS)

$(LIB_CLEAN): clean-%:
	+$(MAKE) -C "$(DB_SERVER_ROOT)/$*" clean

-include $(DEPS)

