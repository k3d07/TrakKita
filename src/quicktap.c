/* ============================================================
 *  quicktap.c — Quick-Tap Fare Pad
 *  CS103 - Computer Programming 2 | Group 4
 *
 *  Member: Kile Nicandro A. Roxas
 *
 *  Description:
 *    Lets the driver quickly tally fares by pressing keys.
 *    Key 1 → adds ₱11.00 (regular fare)
 *    Key 2 → adds ₱13.00 (discounted fare / extra)
 *    Key 3 → undo last tap
 *    Key 0 → done / exit pad
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "structs.h"
#include "quicktap.h"

/* ── Fare Constants ───────────────────────────────────────── */
#define FARE_REGULAR     11.00f
#define FARE_DISCOUNTED  13.00f
#define MAX_TAPS         500

/* ── Helper: get today's date as string ───────────────────── */
static void get_today(char *buf, int size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, size, "%Y-%m-%d", tm_info);
}

/* ── Helper: print the tap dashboard ─────────────────────── */
static void print_tap_screen(int reg, int disc, float total, float last) {
    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD FG_CYAN "   Quick-Tap Fare Pad   " RESET);
    print_row(DIM "  Kile Nicandro A. Roxas | Feature 5  " RESET);
    print_divider();
    print_blank_row();

    /* Fare key guide */
    print_row(FG_GREEN "  [1]  Regular Fare     +₱11.00" RESET);
    print_row(FG_YELLOW "  [2]  Discounted Fare  +₱13.00" RESET);
    print_row(FG_RED    "  [3]  Undo Last Tap" RESET);
    print_row(FG_WHITE  "  [0]  Done / Exit Pad" RESET);
    print_blank_row();
    print_divider();
    print_blank_row();

    /* Live counters */
    char buf[60];

    snprintf(buf, sizeof(buf), "  Regular  taps  : %d", reg);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Discount taps  : %d", disc);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Total taps     : %d", reg + disc);
    print_row(buf);

    print_blank_row();

    if (last > 0.0f) {
        snprintf(buf, sizeof(buf),
                 FG_GREEN "  Last added     : +P%.2f" RESET, last);
    } else if (last < 0.0f) {
        snprintf(buf, sizeof(buf),
                 FG_RED "  Last action    : Undo (P%.2f removed)" RESET,
                 -last);
    } else {
        snprintf(buf, sizeof(buf),
                 DIM "  Last action    : --" RESET);
    }
    print_row(buf);

    print_blank_row();
    print_divider();

    snprintf(buf, sizeof(buf),
             BOLD FG_CYAN "  TOTAL COLLECTED: P%.2f" RESET, total);
    print_row(buf);

    print_blank_row();
    print_bottom_border();
    printf("\n");
}

/* ── Main Feature Function ────────────────────────────────── */
void run_quicktap(void) {
    QuickTapSession session;
    memset(&session, 0, sizeof(session));
    get_today(session.date, sizeof(session.date));

    int  reg_count  = 0;   /* ₱11.00 taps */
    int  disc_count = 0;   /* ₱13.00 taps */
    float total     = 0.0f;
    float last      = 0.0f;

    /* Undo stack — stores the value of each tap so we can subtract it */
    float tap_history[MAX_TAPS];
    int   tap_top = 0;

    int choice = -1;

    while (1) {
        print_tap_screen(reg_count, disc_count, total, last);
        choice = get_int_input("  Tap a key (1 / 2 / 3 / 0): ");

        if (choice == 1) {
            /* Regular fare */
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
            /* Discounted / extra fare */
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
            /* Undo last tap */
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
            /* Exit pad */
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
    print_row(BOLD FG_CYAN "   Quick-Tap Session Summary   " RESET);
    print_divider();
    print_blank_row();

    char buf[60];

    snprintf(buf, sizeof(buf), "  Date            : %s", session.date);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Regular taps    : %d  (x P11.00)", reg_count);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Discounted taps : %d  (x P13.00)", disc_count);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Total taps      : %d", session.tapCount);
    print_row(buf);

    print_blank_row();
    print_divider();

    snprintf(buf, sizeof(buf),
             BOLD FG_GREEN "  TOTAL COLLECTED : P%.2f" RESET,
             session.totalCollected);
    print_row(buf);

    print_blank_row();
    print_bottom_border();

    printf(FG_CYAN "\n  Session recorded. Salamat!\n\n" RESET);
    sleep_ms(500);
    pause_prompt();
}
