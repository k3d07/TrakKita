/* ============================================================
 *  main.c - TrakKita Entry Point & Main Menu
 *  CS103 - Computer Programming 2 | Group 4
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "utils.h"
#include "structs.h"
#include "route.h"
#include "quicktap.h"
#include "profit.h"
#include "fuel.h"
#include "fileio.h"
#include "breakeven.h"

/* ============================================================
 *  JEEPNEY ASCII ANIMATION
 *
 *  A jeepney drives in from the left, stops at center,
 *  then the TrakKita title fades in above it.
 * ============================================================ */

/* Side-view jeepney art, kept ASCII so it renders cleanly in any terminal. */
static const char *JEEP[] = {
    "       _______________________________________________",
    "  ____/_______________________________________________\\___",
    " /  _ |   __   |   TRAKKITA JEEPNEY   |   __   |  _   \\",
    "|  | ||  |__|  |======================|  |__|  || | |   |",
    "|  |_||________|______________________|________||_| |___|",
    "|        ___                         ___              |",
    "`-------(   )-----------------------(   )-------------'",
    "         `-'                         `-'",
};
static const int JEEP_LINES = 8;
static const int JEEP_WIDTH = 62;
static const int INTRO_WIDTH = 78;

static void print_indent(int spaces) {
    for (int i = 0; i < spaces; i++) {
        putchar(' ');
    }
}

static void print_centered_text(const char *text) {
    int width = (int)strlen(text);
    int offset = (INTRO_WIDTH - width) / 2;

    if (offset < 0) {
        offset = 0;
    }

    print_indent(offset);
    printf("%s\n", text);
}

static void print_intro_title(void) {
    static const char *TITLE[] = {
        " _______        _    _  _ _ _          ",
        "|__   __|      | |  | |/ (_) |         ",
        "   | |_ __ __ _| | _| ' / _| |_ __ _   ",
        "   | | '__/ _` | |/ /  < | | __/ _` |  ",
        "  | | | | (_| |   <| . \\| | || (_| |  ",
        "  |_|_|  \\__,_|_|\\_\\_|\\_\\_|\\__\\__,_| ",
    };
    int title_lines = (int)(sizeof(TITLE) / sizeof(TITLE[0]));

    printf(FG_CYAN BOLD);
    for (int i = 0; i < title_lines; i++) {
        print_centered_text(TITLE[i]);
    }
    printf(RESET);

    printf(DIM FG_WHITE);
    print_centered_text("PUV Daily Operations & Earnings Tracker");
    print_centered_text("CS103 | Group 4 | v1.0.0");
    printf(RESET "\n");
}

static void print_clipped_line(const char *line, int offset) {
    int len = (int)strlen(line);
    int start = 0;
    int visible;

    if (offset < 0) {
        start = -offset;
        offset = 0;
    }

    if (start >= len || offset >= INTRO_WIDTH) {
        putchar('\n');
        return;
    }

    visible = len - start;
    if (offset + visible > INTRO_WIDTH) {
        visible = INTRO_WIDTH - offset;
    }

    print_indent(offset);
    printf("%.*s\n", visible, line + start);
}

static void print_jeep_at(int offset) {
    for (int i = 0; i < JEEP_LINES; i++) {
        if (i <= 1) {
            printf(FG_RED BOLD);
        } else if (i == 2 || i == 3) {
            printf(FG_YELLOW BOLD);
        } else if (i == 6 || i == 7) {
            printf(DIM FG_WHITE);
        } else {
            printf(FG_RED);
        }

        print_clipped_line(JEEP[i], offset);
        printf(RESET);
    }
}

static void print_road(void) {
    printf(DIM FG_WHITE "  ");
    for (int i = 0; i < INTRO_WIDTH - 4; i++) {
        putchar('=');
    }
    printf("\n" RESET);
}

static void animate_intro(void) {
    int target = (INTRO_WIDTH - JEEP_WIDTH) / 2;

    /* Phase 1: Drive in from the left */
    for (int x = -JEEP_WIDTH; x <= target; x += 3) {
        clear_screen();
        printf("\n\n\n");
        print_jeep_at(x);
        print_road();
        fflush(stdout);
        sleep_ms(45);
    }

    /* Phase 2: Slight reverse (brake effect) */
    for (int x = target + 2; x >= target; x--) {
        clear_screen();
        printf("\n\n\n");
        print_jeep_at(x);
        print_road();
        fflush(stdout);
        sleep_ms(60);
    }

    /* Phase 3: Settle */
    clear_screen();
    printf("\n\n\n");
    print_jeep_at(target);
    print_road();
    fflush(stdout);
    sleep_ms(300);

    /* Phase 4: Title appear */
    clear_screen();
    printf("\n\n");
    print_intro_title();
    print_jeep_at(target);
    print_road();
    fflush(stdout);
    sleep_ms(600);

    /* Phase 5: Horn honk effect (blink road) */
    for (int i = 0; i < 2; i++) {
        printf(FG_YELLOW BOLD "\t\t\t\tBEEP BEEP!\n" RESET);
        fflush(stdout);
        sleep_ms(300);
        printf("\033[1A\033[2K");
        fflush(stdout);
        sleep_ms(200);
    }

    sleep_ms(500);
}

/* ============================================================
 *  MAIN MENU
 * ============================================================ */
static void print_main_menu(void) {
    clear_screen();
    printf("\n");
    print_top_border();

    print_row(BOLD "    PUV Daily Operations & Earnings Tracker    " RESET);
    print_row(DIM "         CS103  |  Group 4  |  v1.0.0         " RESET);
    print_divider();
    print_blank_row();

    print_row(FG_GREEN "  [1]  Daily Profit / Loss Calculator" RESET);
    print_row(FG_GREEN "  [2]  Fuel Consumption Log (7-Day)" RESET);
    print_row(FG_GREEN "  [3]  Route Efficiency Recommender" RESET);
    print_row(FG_GREEN "  [4]  Fare-to-Fuel Break-Even Simulator" RESET);
    print_row(FG_GREEN "  [5]  Quick-Tap Fare Pad" RESET);

    print_blank_row();
    print_divider();

    print_row(FG_RED "  [6]  Save and Exit" RESET);

    print_blank_row();
    print_bottom_border();
}


static void save_and_exit(DaySummary days[], int day_count,
                          FuelEntry  fuel[], int fuel_count) {
    clear_screen();
    printf("\n" FG_GREEN "  Saving records to puv_records.txt...\n" RESET);
    fileio_save(days, day_count, fuel, fuel_count);
    sleep_ms(800);
    printf(FG_CYAN "  Salamat! Ingat sa daan, driver!\n\n" RESET);
    sleep_ms(600);
}

/* ============================================================
 *  MAIN
 * ============================================================ */
int main(void) {
    /* UTF-8 output on Windows */
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    /* Intro animation */
    animate_intro();

    /* Initialize data */
    RouteNode  routes[ROUTE_COUNT];
    route_init(routes, ROUTE_COUNT);

    DaySummary day_summaries[MAX_DAY_SUMMARIES];
    int        day_count = 0;

    FuelEntry  fuel_entries[MAX_FUEL_ENTRIES];
    int        fuel_count = 0;

    fileio_load(day_summaries, &day_count, fuel_entries, &fuel_count);

    /* Main loop */
    int choice = 0;
    do {
        print_main_menu();
        choice = get_int_input("Enter your choice: ");

        switch (choice) {
            case 1: profit_menu(day_summaries, &day_count); break;
            case 2: fuel_menu(fuel_entries, &fuel_count);   break;
            case 3: route_menu(routes, ROUTE_COUNT);   break;
            case 4: breakeven_menu();                  break;
            case 5: run_quicktap();                    break;
            case 6: save_and_exit(day_summaries, day_count,
                                   fuel_entries, fuel_count); break;
            default:
                printf(FG_RED "\n  Invalid choice. Please try again.\n" RESET);
                sleep_ms(700);
                break;
        }
    } while (choice != 6);

    return 0;
}
