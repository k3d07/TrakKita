#ifndef UTILS_H
#define UTILS_H

/* ============================================================
 *  utils.h — TrakKita Terminal UI Utilities
 *  CS103 - Computer Programming 2 | Group 4
 * ============================================================ */

/* ── ANSI Color Codes ─────────────────────────────────────── */
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define DIM         "\033[2m"

#define FG_BLACK    "\033[30m"
#define FG_RED      "\033[31m"
#define FG_GREEN    "\033[32m"
#define FG_YELLOW   "\033[33m"
#define FG_BLUE     "\033[34m"
#define FG_MAGENTA  "\033[35m"
#define FG_CYAN     "\033[36m"
#define FG_WHITE    "\033[37m"

#define BG_BLACK    "\033[40m"
#define BG_BLUE     "\033[44m"
#define BG_CYAN     "\033[46m"

/* ── UI Constants ─────────────────────────────────────────── */
#define BOX_WIDTH   52   /* inner width of menu box (chars)    */

/* ── Function Declarations ───────────────────────────────── */
void clear_screen(void);
void sleep_ms(int ms);
void print_divider(void);
void print_top_border(void);
void print_bottom_border(void);
void print_row(const char *content);        /* centered padded row */
void print_blank_row(void);
void pause_prompt(void);
int  get_int_input(const char *prompt);
float get_float_input(const char *prompt);
void flush_input(void);

#endif /* UTILS_H */
