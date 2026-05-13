/* ============================================================
 *  profit.c - Daily Profit/Loss Calculator
 *  CS103 - Computer Programming 2 | Group 4
 * ============================================================ */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "structs.h"
#include "profit.h"

static void get_today(char *buf, int size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, size, "%Y-%m-%d", tm_info);
}

/* ── Calculate and record today's P/L ────────────────────── */
static void profit_calculate(DaySummary days[], int *count) {
    /* Rolling window: drop oldest when full */
    if (*count >= MAX_DAY_SUMMARIES) {
        for (int i = 0; i < MAX_DAY_SUMMARIES - 1; i++)
            days[i] = days[i + 1];
        *count = MAX_DAY_SUMMARIES - 1;
    }

    DaySummary *d = &days[*count];
    get_today(d->date, sizeof(d->date));

    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD FG_YELLOW "            [1] ENTER TODAY'S EARNINGS            " RESET);
    print_divider();
    print_blank_row();

    char buf[96];
    snprintf(buf, sizeof(buf), "  Date             : %s", d->date);
    print_row(buf);
    print_blank_row();
    print_bottom_border();
    printf("\n");

    d->totalFares  = get_float_input("  Total Fares      : PHP ");
    d->fuelCost    = get_float_input("  Fuel Cost        : PHP ");
    d->boundaryFee = get_float_input("  Boundary Fee     : PHP ");
    d->netProfit   = d->totalFares - d->fuelCost - d->boundaryFee;
    (*count)++;

    int is_loss = (d->netProfit < 0.0f);

    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD FG_CYAN "            TODAY'S PROFIT/LOSS RESULT            " RESET);
    print_divider();
    print_blank_row();

    snprintf(buf, sizeof(buf), "  Date             : %s", d->date);
    print_row(buf);
    print_blank_row();
    print_divider();
    print_blank_row();

    snprintf(buf, sizeof(buf), "  Total Fares      : PHP %.2f", d->totalFares);
    print_row(buf);
    snprintf(buf, sizeof(buf), "  Fuel Cost        : PHP %.2f", d->fuelCost);
    print_row(buf);
    snprintf(buf, sizeof(buf), "  Boundary Fee     : PHP %.2f", d->boundaryFee);
    print_row(buf);
    print_blank_row();
    print_divider();
    print_blank_row();

    if (is_loss) {
        snprintf(buf, sizeof(buf),
                 BOLD FG_RED "  NET LOSS         : PHP %.2f" RESET, d->netProfit);
        print_row(buf);
        print_blank_row();
        print_row(BOLD FG_RED "  WARNING: YOU ARE OPERATING AT A LOSS!" RESET);
    } else {
        snprintf(buf, sizeof(buf),
                 BOLD FG_GREEN "  NET PROFIT       : PHP %.2f" RESET, d->netProfit);
        print_row(buf);
    }

    print_blank_row();
    print_bottom_border();
    printf("\n");
    sleep_ms(400);
    pause_prompt();
}

/* ── Display earnings history table ──────────────────────── */
static void profit_display_history(DaySummary days[], int count) {
    char row[96];

    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD FG_CYAN "                 EARNINGS HISTORY                 " RESET);
    print_divider();

    if (count == 0) {
        print_blank_row();
        print_row(DIM "  No records yet. Calculate today's P/L first." RESET);
        print_blank_row();
        print_bottom_border();
        pause_prompt();
        return;
    }

    print_row(BOLD "   #  Date             Fares         Net" RESET);
    print_divider();

    for (int i = 0; i < count; i++) {
        const char *net_color = (days[i].netProfit >= 0.0f) ? FG_GREEN : FG_RED;
        snprintf(row, sizeof(row), "  %2d  %-10.10s  %10.2f  %s%10.2f%s",
                 i + 1,
                 days[i].date,
                 days[i].totalFares,
                 net_color,
                 days[i].netProfit,
                 RESET);
        print_row(row);
    }

    print_bottom_border();
    printf("\n");
    pause_prompt();
}

/* ── Menu ─────────────────────────────────────────────────── */
void profit_menu(DaySummary days[], int *count) {
    int choice = 0;

    do {
        clear_screen();
        printf("\n");
        print_top_border();
        print_row(BOLD FG_YELLOW
                  "         [1] DAILY PROFIT/LOSS CALCULATOR         "
                  RESET);
        print_divider();
        print_blank_row();

        print_row(FG_WHITE "  [1]  Calculate Today's P/L" RESET);
        print_row(FG_WHITE "  [2]  View Earnings History" RESET);
        print_blank_row();
        print_row(DIM "  [0]  Back to Main Menu" RESET);
        print_blank_row();
        print_bottom_border();

        choice = get_int_input("Enter choice: ");

        switch (choice) {
            case 1: profit_calculate(days, count);           break;
            case 2: profit_display_history(days, *count);    break;
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
