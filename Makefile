# ============================================================
#  Makefile - TrakKita
#  CS103 - Computer Programming 2 | Group 4
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -Iinclude
LDFLAGS = -static
TARGET  = trakkita.exe

SRCDIR  = src
INCDIR  = include
BUILDDIR = build

SRCS = $(SRCDIR)/main.c \
       $(SRCDIR)/utils.c \
       $(SRCDIR)/route.c \
       $(SRCDIR)/quicktap.c \
       $(SRCDIR)/profit.c \
       $(SRCDIR)/fuelConsump.c \
       $(SRCDIR)/fileio.c \
       $(SRCDIR)/breakeven.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS) $(INCDIR)/structs.h $(INCDIR)/utils.h $(INCDIR)/route.h $(INCDIR)/quicktap.h $(INCDIR)/profit.h $(INCDIR)/fuel.h $(INCDIR)/fileio.h $(INCDIR)/breakeven.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRCS)

run: all
	$(TARGET)

clean:
	@if exist $(TARGET) del /Q $(TARGET)
	@if exist $(BUILDDIR)\*.o del /Q $(BUILDDIR)\*.o
