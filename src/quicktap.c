/* ============================================================
 *  quicktap.c - Quick-Tap Fare Pad
 *  CS103 - Computer Programming 2 | Group 4
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "structs.h"
#include "quicktap.h"

/* ── Fare Constants ───────────────────────────────────────── */
#define FARE_REGULAR     13.00f
#define FARE_DISCOUNTED  11.00f
#define MAX_TAPS         500

/* ── Helper: get today's date as string ───────────────────── */
/* get_today
 * ---------
 * Writes today's date as "YYYY-MM-DD" into the buffer. Used to stamp
 * the QuickTap session summary.
 * Params : buf  — destination char buffer
 *          size — capacity of buf in bytes
 */
static void get_today(char *buf, int size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, size, "%Y-%m-%d", tm_info);
}

/* ── Helper: print the tap dashboard ─────────────────────── */
/* print_tap_screen
 * ----------------
 * Re-renders the entire Quick-Tap dashboard after every keypress:
 * the action keys (1/2/3/0), live counters for regular and
 * discounted taps, the last action (last added fare in green, undo in
 * red, or "--" when idle), and the running total at the bottom.
 * Called repeatedly inside run_quicktap()'s loop.
 * Params : reg   — running count of regular fare taps
 *          disc  — running count of discounted fare taps
 *          total — total pesos collected so far
 *          last  — last fare amount (positive=added, negative=undone,
 *                  zero=no recent action)
 */
static void print_tap_screen(int reg, int disc, float total, float last) {
    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD FG_YELLOW
              "              [5] QUICK-TAP FARE PAD              "
              RESET);
    print_divider();
    print_blank_row();

    print_row(FG_GREEN  "  [1]  Regular Fare     +₱13.00" RESET);
    print_row(FG_YELLOW "  [2]  Discounted Fare  +₱11.00" RESET);
    print_row(FG_RED    "  [3]  Undo Last Tap" RESET);
    print_row(FG_WHITE  "  [0]  Done / Exit Pad" RESET);
    print_blank_row();
    print_divider();
    print_blank_row();

    char buf[96];

    snprintf(buf, sizeof(buf), "  Regular taps    : %d", reg);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Discounted taps : %d", disc);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Total taps      : %d", reg + disc);
    print_row(buf);

    print_blank_row();

    if (last > 0.0f) {
        snprintf(buf, sizeof(buf),
                 FG_GREEN "  Last added     : +₱%.2f" RESET, last);
    } else if (last < 0.0f) {
        snprintf(buf, sizeof(buf),
                 FG_RED "  Last action    : Undo (₱%.2f removed)" RESET,
                 -last);
    } else {
        snprintf(buf, sizeof(buf),
                 DIM "  Last action    : --" RESET);
    }
    print_row(buf);

    print_blank_row();
    print_divider();

    snprintf(buf, sizeof(buf),
             BOLD FG_CYAN "  TOTAL COLLECTED : ₱%.2f" RESET, total);
    print_row(buf);

    print_blank_row();
    print_bottom_border();
    printf("\n");
}

/* ── Main Feature Function ────────────────────────────────── */
/* run_quicktap
 * ------------
 * Drives the Quick-Tap Fare Pad — a fast keypad-style screen the
 * driver can tap while collecting fares. Loops until the user picks 0.
 * Key handling:
 *   [1] add ₱13 (regular)         → push to undo stack, bump total
 *   [2] add ₱11 (discounted)      → push to undo stack, bump total
 *   [3] undo last tap             → pop from stack, subtract from total
 *   [0] end session and exit
 * The undo "stack" (tap_history) is a simple array indexed by tap_top;
 * each tap pushes its fare value so undo can subtract the exact amount,
 * even when regular and discounted taps are mixed. Caps at MAX_TAPS to
 * prevent overflow. When the user exits, prints a session summary with
 * date, tap counts, and total collected.
 */
void run_quicktap(void) {
    QuickTapSession session;
    memset(&session, 0, sizeof(session));
    get_today(session.date, sizeof(session.date));

    int   reg_count  = 0;   /* ₱13.00 taps */
    int   disc_count = 0;   /* ₱11.00 taps */
    float total      = 0.0f;
    float last       = 0.0f;

    /* Undo stack — stores the value of each tap so we can subtract it */
    float tap_history[MAX_TAPS];
    int   tap_top = 0;

    int choice = -1;

    while (1) {
        print_tap_screen(reg_count, disc_count, total, last);
        choice = get_int_input("  Tap a key (1 / 2 / 3 / 0): ");

        if (choice == 1) {
            if (tap_top < MAX_TAPS) {
                tap_history[tap_top++] = FARE_REGULAR;
                total      += FARE_REGULAR;
                reg_count++;
                last        = FARE_REGULAR;
            } else {
                printf(FG_RED "  Max taps reached!\n" RESET);
                sleep_ms(800);
            }

        } else if (choice == 2) {
            if (tap_top < MAX_TAPS) {
                tap_history[tap_top++] = FARE_DISCOUNTED;
                total      += FARE_DISCOUNTED;
                disc_count++;
                last        = FARE_DISCOUNTED;
            } else {
                printf(FG_RED "  Max taps reached!\n" RESET);
                sleep_ms(800);
            }

        } else if (choice == 3) {
            if (tap_top > 0) {
                float removed = tap_history[--tap_top];
                total -= removed;
                last   = -removed;   /* negative signals undo on screen */

                if (removed == FARE_REGULAR)
                    reg_count--;
                else
                    disc_count--;
            } else {
                printf(FG_YELLOW "  Nothing to undo.\n" RESET);
                sleep_ms(700);
                last = 0.0f;
            }

        } else if (choice == 0) {
            break;

        } else {
            printf(FG_RED "  Invalid key. Use 1, 2, 3, or 0.\n" RESET);
            sleep_ms(700);
            last = 0.0f;
        }
    }

    /* ── Session Summary ──────────────────────────────────── */
    session.tapCount       = reg_count + disc_count;
    session.totalCollected = total;

    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD FG_YELLOW
              "             QUICK-TAP SESSION SUMMARY            "
              RESET);
    print_divider();
    print_blank_row();

    char buf[96];

    snprintf(buf, sizeof(buf), "  Date            : %s", session.date);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Regular taps    : %d  (x ₱13.00)", reg_count);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Discounted taps : %d  (x ₱11.00)", disc_count);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Total taps      : %d", session.tapCount);
    print_row(buf);

    print_blank_row();
    print_divider();

    snprintf(buf, sizeof(buf),
             BOLD FG_GREEN "  TOTAL COLLECTED : ₱%.2f" RESET,
             session.totalCollected);
    print_row(buf);

    print_blank_row();
    print_bottom_border();

    printf(FG_CYAN "\n  Session recorded. Salamat!\n\n" RESET);
    sleep_ms(500);
    pause_prompt();
}
