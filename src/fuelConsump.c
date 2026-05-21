/* ============================================================
 *  fuelConsump.c - Fuel Consumption Log (7-Day)
 *  CS103 - Computer Programming 2 | Group 4
 * ============================================================ */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "structs.h"
#include "fuel.h"

/* get_today
 * ---------
 * Writes today's date as "YYYY-MM-DD" into the provided buffer using
 * the system clock. Used to stamp each fuel entry with the day it
 * was logged.
 * Params : buf  — destination char buffer
 *          size — capacity of buf in bytes
 */
static void get_today(char *buf, int size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, size, "%Y-%m-%d", tm_info);
}

/* ── Add a new fuel entry ─────────────────────────────────── */
/* fuel_add
 * --------
 * Records a new fuel-up entry. Asks the user for liters filled and
 * price per liter, then auto-computes totalCost = liters × price.
 * Uses a rolling 7-day window: when entries[] is full it shifts the
 * oldest entry out so the log always reflects the most recent week.
 * Shows a green confirmation screen with the saved values.
 * Params : entries — array of FuelEntry records
 *          count   — pointer to the current number of entries
 *                    (incremented by 1 on success)
 */
static void fuel_add(FuelEntry entries[], int *count) {
    /* Rolling window: drop oldest when full */
    if (*count >= MAX_FUEL_ENTRIES) {
        for (int i = 0; i < MAX_FUEL_ENTRIES - 1; i++)
            entries[i] = entries[i + 1];
        *count = MAX_FUEL_ENTRIES - 1;
    }

    FuelEntry *e = &entries[*count];
    get_today(e->date, sizeof(e->date));

    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD FG_YELLOW "                [2] ADD FUEL ENTRY                " RESET);
    print_divider();
    print_blank_row();

    char buf[96];
    snprintf(buf, sizeof(buf), "  Date             : %s", e->date);
    print_row(buf);
    print_blank_row();
    print_bottom_border();
    printf("\n");

    e->liters        = get_float_input("  Liters filled    : ");
    e->pricePerLiter = get_float_input("  Price per liter  : PHP ");
    e->totalCost     = e->liters * e->pricePerLiter;
    (*count)++;

    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD FG_GREEN "                 FUEL ENTRY SAVED                 " RESET);
    print_divider();
    print_blank_row();

    snprintf(buf, sizeof(buf), "  Date             : %s", e->date);
    print_row(buf);
    snprintf(buf, sizeof(buf), "  Liters filled    : %.2f L", e->liters);
    print_row(buf);
    snprintf(buf, sizeof(buf), "  Price per liter  : PHP %.2f", e->pricePerLiter);
    print_row(buf);
    print_blank_row();
    print_divider();
    snprintf(buf, sizeof(buf), BOLD FG_CYAN "  Total Cost       : PHP %.2f" RESET, e->totalCost);
    print_row(buf);
    print_blank_row();
    print_bottom_border();
    printf("\n");
    sleep_ms(400);
    pause_prompt();
}

/* ── Display 7-day log table ──────────────────────────────── */
/* fuel_display_log
 * ----------------
 * Renders the saved fuel entries as a numbered table (#, date,
 * liters, price/liter, total cost). Shows an empty-state message if
 * no entries exist yet.
 * Params : entries — array of FuelEntry records
 *          count   — number of entries currently stored
 */
static void fuel_display_log(FuelEntry entries[], int count) {
    char row[96];

    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD FG_CYAN "            7-DAY FUEL CONSUMPTION LOG            " RESET);
    print_divider();

    if (count == 0) {
        print_blank_row();
        print_row(DIM "  No fuel entries recorded yet." RESET);
        print_blank_row();
        print_bottom_border();
        pause_prompt();
        return;
    }

    print_row(BOLD "   #  Date        Liters  PHP/L     Cost" RESET);
    print_divider();

    for (int i = 0; i < count; i++) {
        snprintf(row, sizeof(row), "  %2d  %-10.10s  %6.2f  %5.2f  %7.2f",
                 i + 1,
                 entries[i].date,
                 entries[i].liters,
                 entries[i].pricePerLiter,
                 entries[i].totalCost);
        print_row(row);
    }

    print_bottom_border();
    printf("\n");
    pause_prompt();
}

/* ── Display summary statistics ───────────────────────────── */
/* fuel_display_summary
 * --------------------
 * Computes and displays aggregate stats over all stored fuel entries:
 *   - Total liters and total cost across the window
 *   - Average liters and average cost per day
 *   - Most-expensive day (red) and cheapest day (green), when count > 1
 * Uses a single pass through the array to accumulate sums and track
 * the indexes of the max and min entries. Helps the driver spot fuel-
 * price spikes and budget for the week ahead.
 * Params : entries — array of FuelEntry records
 *          count   — number of entries currently stored
 */
static void fuel_display_summary(FuelEntry entries[], int count) {
    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD FG_CYAN "                 FUEL LOG SUMMARY                 " RESET);
    print_divider();

    if (count == 0) {
        print_blank_row();
        print_row(DIM "  No fuel entries recorded yet." RESET);
        print_blank_row();
        print_bottom_border();
        pause_prompt();
        return;
    }

    float total_liters = 0.0f;
    float total_cost   = 0.0f;
    int   max_i        = 0;
    int   min_i        = 0;

    for (int i = 0; i < count; i++) {
        total_liters += entries[i].liters;
        total_cost   += entries[i].totalCost;
        if (entries[i].totalCost > entries[max_i].totalCost) max_i = i;
        if (entries[i].totalCost < entries[min_i].totalCost) min_i = i;
    }

    float avg_liters = total_liters / (float)count;
    float avg_cost   = total_cost   / (float)count;

    char buf[96];
    print_blank_row();

    snprintf(buf, sizeof(buf), "  Entries logged   : %d / %d days", count, MAX_FUEL_ENTRIES);
    print_row(buf);
    snprintf(buf, sizeof(buf), "  Total liters     : %.2f L", total_liters);
    print_row(buf);
    snprintf(buf, sizeof(buf), "  Avg liters/day   : %.2f L", avg_liters);
    print_row(buf);
    print_blank_row();
    print_divider();
    print_blank_row();
    snprintf(buf, sizeof(buf), "  Total fuel cost  : PHP %.2f", total_cost);
    print_row(buf);
    snprintf(buf, sizeof(buf), "  Avg cost/day     : PHP %.2f", avg_cost);
    print_row(buf);

    if (count > 1) {
        print_blank_row();
        print_divider();
        print_blank_row();
        snprintf(buf, sizeof(buf), FG_RED "  Most expensive   : %s  (PHP %.2f)" RESET,
                 entries[max_i].date, entries[max_i].totalCost);
        print_row(buf);
        snprintf(buf, sizeof(buf), FG_GREEN "  Cheapest day     : %s  (PHP %.2f)" RESET,
                 entries[min_i].date, entries[min_i].totalCost);
        print_row(buf);
    }

    print_blank_row();
    print_bottom_border();
    printf("\n");
    pause_prompt();
}

/* ── Menu ─────────────────────────────────────────────────── */
/* fuel_menu
 * ---------
 * Public entry point for the Fuel Consumption Log feature. Displays
 * the sub-menu and dispatches to fuel_add(), fuel_display_log(), or
 * fuel_display_summary() based on the user's choice. Loops until the
 * user picks 0 to return to the main menu.
 * Params : entries — array of FuelEntry records
 *          count   — pointer to the current number of entries
 */
void fuel_menu(FuelEntry entries[], int *count) {
    int choice = 0;

    do {
        clear_screen();
        printf("\n");
        print_top_border();
        print_row(BOLD FG_YELLOW
                  "         [2] FUEL CONSUMPTION LOG (7-DAY)         "
                  RESET);
        print_divider();
        print_blank_row();

        print_row(FG_WHITE "  [1]  Add Today's Fuel Entry" RESET);
        print_row(FG_WHITE "  [2]  View 7-Day Log" RESET);
        print_row(FG_WHITE "  [3]  View Summary" RESET);
        print_blank_row();
        print_row(DIM "  [0]  Back to Main Menu" RESET);
        print_blank_row();
        print_bottom_border();

        choice = get_int_input("Enter choice: ");

        switch (choice) {
            case 1: fuel_add(entries, count);              break;
            case 2: fuel_display_log(entries, *count);     break;
            case 3: fuel_display_summary(entries, *count); break;
            case 0:
                printf(DIM "\n  Returning to main menu...\n" RESET);
                sleep_ms(600);
                break;
            default:
                printf(FG_RED "\n  Invalid choice. Please try again.\n" RESET);
                sleep_ms(700);
                break;
        }
    } while (choice != 0);
}
