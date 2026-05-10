/* ============================================================
 *  utils.c - TrakKita Terminal UI Utilities
 *  CS103 - Computer Programming 2 | Group 4
 * ============================================================ */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

static const char *BOX_TL = "\xE2\x95\x94";
static const char *BOX_TR = "\xE2\x95\x97";
static const char *BOX_BL = "\xE2\x95\x9A";
static const char *BOX_BR = "\xE2\x95\x9D";
static const char *BOX_V  = "\xE2\x95\x91";
static const char *BOX_H  = "\xE2\x95\x90";
static const char *BOX_LT = "\xE2\x95\xA0";
static const char *BOX_RT = "\xE2\x95\xA3";

void sleep_ms(int ms) {
    struct timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

void clear_screen(void) {
    printf("\033[2J\033[3J\033[H");
    fflush(stdout);
}

static void print_horizontal_border(const char *left, const char *right) {
    printf(FG_CYAN "  %s", left);
    for (int i = 0; i < BOX_WIDTH; i++) {
        printf("%s", BOX_H);
    }
    printf("%s\n" RESET, right);
}

void print_top_border(void) {
    print_horizontal_border(BOX_TL, BOX_TR);
}

void print_bottom_border(void) {
    print_horizontal_border(BOX_BL, BOX_BR);
}

void print_divider(void) {
    print_horizontal_border(BOX_LT, BOX_RT);
}

void print_blank_row(void) {
    printf(FG_CYAN "  %s" RESET, BOX_V);
    for (int i = 0; i < BOX_WIDTH; i++) {
        putchar(' ');
    }
    printf(FG_CYAN "%s\n" RESET, BOX_V);
}

static int visible_length(const char *text) {
    int length = 0;

    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\033' && text[i + 1] == '[') {
            i += 2;
            while (text[i] != '\0' && (text[i] < '@' || text[i] > '~')) {
                i++;
            }
        } else {
            length++;
        }
    }

    return length;
}

void print_row(const char *content) {
    int content_width = BOX_WIDTH - 2;
    int printed_width = visible_length(content);
    int padding = content_width - printed_width;

    if (padding < 0) {
        padding = 0;
    }

    printf(FG_CYAN "  %s" RESET " %s", BOX_V, content);
    for (int i = 0; i < padding; i++) {
        putchar(' ');
    }
    printf(" " FG_CYAN "%s\n" RESET, BOX_V);
}

void pause_prompt(void) {
    printf("\n" DIM "  Press ENTER to continue..." RESET);
    flush_input();
    getchar();
}

void flush_input(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

int get_int_input(const char *prompt) {
    int val;
    printf(FG_YELLOW "  >> " RESET "%s", prompt);
    while (scanf("%d", &val) != 1) {
        flush_input();
        printf(FG_RED "  Invalid input. Try again: " RESET);
    }
    flush_input();
    return val;
}

float get_float_input(const char *prompt) {
    float val;
    printf(FG_YELLOW "  >> " RESET "%s", prompt);
    while (scanf("%f", &val) != 1) {
        flush_input();
        printf(FG_RED "  Invalid input. Try again: " RESET);
    }
    flush_input();
    return val;
}
