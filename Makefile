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
       $(SRCDIR)/route.c

# Add teammate files here as they are completed, for example:
# SRCS += $(SRCDIR)/profit.c $(SRCDIR)/fuel.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS) $(INCDIR)/structs.h $(INCDIR)/utils.h $(INCDIR)/route.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRCS)

run: all
	$(TARGET)

clean:
	@if exist $(TARGET) del /Q $(TARGET)
	@if exist $(BUILDDIR)\*.o del /Q $(BUILDDIR)\*.o
