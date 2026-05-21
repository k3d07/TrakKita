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

/* print_indent
 * ------------
 * Prints `spaces` blank characters. Tiny helper used by the intro
 * animation to push the jeepney art to a specific column on screen.
 */
static void print_indent(int spaces) {
    for (int i = 0; i < spaces; i++) {
        putchar(' ');
    }
}

/* print_centered_text
 * -------------------
 * Centers a single line of text within INTRO_WIDTH columns by
 * calculating the left padding before printing. Used to center the
 * TrakKita ASCII title above the jeepney during the intro.
 * Params : text — the string to center
 */
static void print_centered_text(const char *text) {
    int width = (int)strlen(text);
    int offset = (INTRO_WIDTH - width) / 2;

    if (offset < 0) {
        offset = 0;
    }

    print_indent(offset);
    printf("%s\n", text);
}

/* print_intro_title
 * -----------------
 * Renders the "TrakKita" ASCII-art logo in cyan/bold, followed by the
 * tagline and project credits in dim white. Called by animate_intro()
 * once the jeepney has settled into place.
 */
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

/* print_clipped_line
 * ------------------
 * Prints one line of the jeepney art at a given horizontal `offset`.
 * Handles two edge cases: (1) negative offsets — when the jeepney is
 * still partly off-screen to the left, we skip those characters; and
 * (2) lines that would run past INTRO_WIDTH — we truncate them.
 * Without this, the drive-in animation would either crash or wrap.
 * Params : line   — one row of the jeepney art
 *          offset — column to start drawing at (may be negative)
 */
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

/* print_jeep_at
 * -------------
 * Draws the full jeepney art at the given horizontal `offset`, coloring
 * each line by its purpose: roof in red, body/label in yellow, wheels
 * in dim white. Called repeatedly inside animate_intro() to redraw the
 * jeepney at moving positions, producing the drive-in effect.
 * Params : offset — column to draw the jeepney at
 */
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

/* print_road
 * ----------
 * Prints a dashed horizontal line in dim white directly under the
 * jeepney to represent the road. Drawn after every frame of the intro.
 */
static void print_road(void) {
    printf(DIM FG_WHITE "  ");
    for (int i = 0; i < INTRO_WIDTH - 4; i++) {
        putchar('=');
    }
    printf("\n" RESET);
}

/* animate_intro
 * -------------
 * Plays the opening jeepney animation in five phases:
 *   1. Drive in   — jeepney slides from off-screen left to center
 *   2. Brake      — slight reverse for a realistic stop
 *   3. Settle     — short pause at final position
 *   4. Title      — TrakKita logo appears above the jeepney
 *   5. Horn       — "BEEP BEEP!" blinks twice for personality
 * Each frame: clear screen → reprint jeep at new x → sleep briefly.
 * Purely cosmetic — runs once at program start, then control passes
 * to the main menu.
 */
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
/* print_main_menu
 * ---------------
 * Renders the main menu — the boxed list of all 5 features plus
 * "Save and Exit". Called at the top of every iteration of the main
 * loop so the menu re-appears after each feature exits.
 */
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


/* save_and_exit
 * -------------
 * Persists all in-memory records (daily summaries and fuel entries)
 * to puv_records.txt by calling fileio_save(), then shows a friendly
 * farewell message. Triggered when the user picks option 6 in the
 * main menu — ensures nothing entered during the session is lost.
 * Params : days, day_count   — daily P/L records to save
 *          fuel, fuel_count  — fuel log entries to save
 */
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
/* main
 * ----
 * Program entry point. Steps performed:
 *   1. Enable UTF-8 output on Windows (so box-drawing chars render).
 *   2. Play the intro animation.
 *   3. Initialize route data and allocate arrays for days/fuel logs.
 *   4. Load any previously saved records from puv_records.txt.
 *   5. Run the main menu loop until the user picks "Save and Exit".
 * Returns: 0 on normal exit.
 */
int main(void) {
    /* UTF-8 output on Windows */
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8); //Input
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
