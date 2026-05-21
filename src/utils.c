/* ============================================================
 *  utils.c - TrakKita Terminal UI Utilities
 *  CS103 - Computer Programming 2 | Group 4
 * ============================================================ */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

static const char *BOX_TL = "\xE2\x95\x94"; /* ╔ U+2554 */
static const char *BOX_TR = "\xE2\x95\x97"; /* ╗ U+2557 */
static const char *BOX_BL = "\xE2\x95\x9A"; /* ╚ U+255A */
static const char *BOX_BR = "\xE2\x95\x9D"; /* ╝ U+255D */
static const char *BOX_V  = "\xE2\x95\x91"; /* ║ U+2551 */
static const char *BOX_H  = "\xE2\x95\x90"; /* ═ U+2550 */
static const char *BOX_LT = "\xE2\x95\xA0"; /* ╠ U+2560 */
static const char *BOX_RT = "\xE2\x95\xA3"; /* ╣ U+2563 */

/* sleep_ms
 * --------
 * Pauses the program for `ms` milliseconds. Used to pace the intro
 * animation and create short delays between menu transitions.
 * Params : ms — number of milliseconds to sleep
 */
void sleep_ms(int ms) {
    struct timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

/* clear_screen
 * ------------
 * Wipes the terminal using ANSI escape codes (\033[2J clears the screen,
 * \033[3J clears the scrollback, \033[H moves the cursor to top-left).
 * Called before every menu redraw so the UI stays clean.
 */
void clear_screen(void) {
    printf("\033[2J\033[3J\033[H");
    fflush(stdout);
}

/* print_horizontal_border
 * -----------------------
 * Helper used by the top/bottom/divider functions. Prints one full
 * horizontal line of the UI box using the given left and right
 * corner characters, filled in between with the horizontal char.
 * Params : left, right — UTF-8 corner glyphs (e.g. ╔ and ╗)
 */
static void print_horizontal_border(const char *left, const char *right) {
    printf(FG_CYAN "  %s", left);
    for (int i = 0; i < BOX_WIDTH; i++) {
        printf("%s", BOX_H);
    }
    printf("%s\n" RESET, right);
}

/* print_top_border
 * ----------------
 * Draws the top edge of a UI box (╔══════╗).
 */
void print_top_border(void) {
    print_horizontal_border(BOX_TL, BOX_TR);
}

/* print_bottom_border
 * -------------------
 * Draws the bottom edge of a UI box (╚══════╝).
 */
void print_bottom_border(void) {
    print_horizontal_border(BOX_BL, BOX_BR);
}

/* print_divider
 * -------------
 * Draws a horizontal divider that separates sections inside a
 * box (╠══════╣). Used between titles, data, and totals.
 */
void print_divider(void) {
    print_horizontal_border(BOX_LT, BOX_RT);
}

/* print_blank_row
 * ---------------
 * Draws an empty row inside a box (║      ║) for vertical spacing.
 */
void print_blank_row(void) {
    printf(FG_CYAN "  %s" RESET, BOX_V);
    for (int i = 0; i < BOX_WIDTH; i++) {
        putchar(' ');
    }
    printf(FG_CYAN "%s\n" RESET, BOX_V);
}

/* visible_length
 * --------------
 * Counts how many on-screen columns a string actually takes up,
 * ignoring ANSI color escape codes and treating multi-byte UTF-8
 * characters as a single column. Needed so colored/UTF-8 text still
 * lines up correctly inside the box borders.
 * Params : text — null-terminated string (may contain ANSI/UTF-8)
 * Returns: visible character width
 */
static int visible_length(const char *text) {
    int length = 0;

    for (int i = 0; text[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)text[i];

        if (text[i] == '\033' && text[i + 1] == '[') {
            i += 2;
            while (text[i] != '\0' && (text[i] < '@' || text[i] > '~')) {
                i++;
            }
        } else if ((ch & 0xC0) == 0x80) {
            continue;
        } else {
            length++;
        }
    }

    return length;
}

/* print_row
 * ---------
 * Prints one row of UI content wrapped with the left/right box borders
 * (║ content                 ║). Pads the right side with spaces so the
 * closing border always lines up, using visible_length() so color codes
 * don't throw off the alignment.
 * Params : content — the text (may include color codes) to display
 */
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

/* pause_prompt
 * ------------
 * Shows "Press ENTER to continue..." and waits for the user to press
 * Enter. Used after every result screen so output isn't wiped away
 * before the user has read it.
 */
void pause_prompt(void) {
    printf("\n" DIM "  Press ENTER to continue..." RESET);
    flush_input();
    getchar();
}

/* flush_input
 * -----------
 * Discards leftover characters in stdin up to and including the next
 * newline. Called after scanf() to clear any extra typing so the next
 * input prompt starts clean.
 */
void flush_input(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

/* get_int_input
 * -------------
 * Prompts the user and keeps re-asking until a valid integer is typed.
 * If scanf() fails (e.g., user types letters), the bad input is flushed
 * and the prompt repeats. This makes our menus crash-proof.
 * Params : prompt — text shown before the input cursor
 * Returns: the integer entered by the user
 */
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

/* get_float_input
 * ---------------
 * Same idea as get_int_input() but for decimal values like prices
 * and distances. Re-prompts on invalid input.
 * Params : prompt — text shown before the input cursor
 * Returns: the float entered by the user
 */
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
