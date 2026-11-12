# DB-specific archive for libthecore.
DB_SERVER_ROOT := ..
DB_ARCHIVE := libthecore.a
DB_LIBRARY_SRCS := $(addprefix src/,buffer.c fdwatch.c gost.c heart.c log.c main.c \
                   memcpy.c signal.c socket.c tea.c utils.c)
include $(DB_SERVER_ROOT)/db/db-build.mk

