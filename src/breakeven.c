/* ============================================================
 *  breakeven.c - Fare-to-Fuel Break-Even Simulator
 *  CS103 - Computer Programming 2 | Group 4
 * ============================================================ */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "structs.h"
#include "breakeven.h"

/* ── Core Calculation ────────────────────────────────────── */

static BreakevenResults breakeven_calculate(const BreakevenInputs *in) {
    BreakevenResults r = {0};

    if (in->fuel_efficiency <= 0.0f || in->fare_per_pax <= 0.0f) {
        printf(FG_RED "\n  Error: fuel efficiency and fare must be > 0.\n" RESET);
        return r;
    }

    r.fuel_cost          = (in->route_distance / in->fuel_efficiency) * in->fuel_price;
    r.total_cost         = r.fuel_cost + in->boundary_fee + in->maintenance
                           + in->driver_wages + in->other_costs;
    r.total_passengers   = (float)(in->trips_per_day * in->pax_per_trip);
    r.gross_revenue      = r.total_passengers * in->fare_per_pax;
    r.net_profit         = r.gross_revenue - r.total_cost;
    r.breakeven_pax      = (int)ceilf(r.total_cost / in->fare_per_pax);
    r.breakeven_per_trip = (int)ceilf((float)r.breakeven_pax / (float)in->trips_per_day);
    r.coverage_pct       = (r.total_cost > 0.0f)
                           ? (r.gross_revenue / r.total_cost) * 100.0f : 0.0f;
    return r;
}

/* ── Preset Scenarios ────────────────────────────────────── */

/* Traditional Jeepney — before the oil price hike (diesel PHP 58/L) */
static BreakevenInputs breakeven_scenario_jeepney_pre_hike(void) {
    BreakevenInputs in = {58.0f, 4.5f, 120.0f, 10, 900.0f, 120.0f, 0.0f, 70.0f, 13.0f, 20};
    return in;
}

/* Traditional Jeepney — during the oil hike (diesel PHP 85/L) */
static BreakevenInputs breakeven_scenario_jeepney_hike(void) {
    BreakevenInputs in = {85.0f, 4.5f, 120.0f, 10, 900.0f, 150.0f, 0.0f, 80.0f, 14.0f, 17};
    return in;
}

/* Modern/e-jeepney — better fuel efficiency (~9 km/L), higher boundary */
static BreakevenInputs breakeven_scenario_modern_jeepney(void) {
    BreakevenInputs in = {85.0f, 9.0f, 160.0f, 12, 2200.0f, 200.0f, 400.0f, 100.0f, 16.0f, 22};
    return in;
}

/* UV Express — longer inter-city routes, higher fares */
static BreakevenInputs breakeven_scenario_uv_express(void) {
    BreakevenInputs in = {85.0f, 12.0f, 180.0f, 8, 1500.0f, 200.0f, 0.0f, 150.0f, 50.0f, 11};
    return in;
}

/* Provincial Bus — long routes, low efficiency, hired driver */
static BreakevenInputs breakeven_scenario_provincial_bus(void) {
    BreakevenInputs in = {85.0f, 5.0f, 320.0f, 2, 4000.0f, 500.0f, 600.0f, 300.0f, 150.0f, 38};
    return in;
}

/* Tricycle — gasoline, short barangay routes, many trips */
static BreakevenInputs breakeven_scenario_tricycle(void) {
    BreakevenInputs in = {68.0f, 30.0f, 80.0f, 30, 150.0f, 50.0f, 0.0f, 50.0f, 12.0f, 3};
    return in;
}

/* ── Custom Input Collection ─────────────────────────────── */

static BreakevenInputs breakeven_get_inputs(void) {
    BreakevenInputs in;

    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD "          ENTER CUSTOM VALUES           " RESET);
    print_bottom_border();
    printf("\n");

    in.fuel_price      = get_float_input("  Fuel price (PHP/liter)        : ");
    in.fuel_efficiency = get_float_input("  Fuel efficiency (km/liter)    : ");
    in.route_distance  = get_float_input("  Daily route distance (km)     : ");
    in.trips_per_day   = get_int_input  ("  Trips per day                 : ");
    in.boundary_fee    = get_float_input("  Boundary / rent fee (PHP)     : ");
    in.maintenance     = get_float_input("  Maintenance cost (PHP)        : ");
    in.driver_wages    = get_float_input("  Driver wages (0 if none)      : ");
    in.other_costs     = get_float_input("  Other costs (PHP)             : ");
    in.fare_per_pax    = get_float_input("  Fare per passenger (PHP)      : ");
    in.pax_per_trip    = get_int_input  ("  Avg passengers per trip       : ");

    return in;
}

/* ── Results Display ─────────────────────────────────────── */

static void breakeven_print_results(const BreakevenResults *r) {
    char buf[128];

    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD "           SIMULATION RESULTS           " RESET);
    print_divider();
    print_blank_row();

    snprintf(buf, sizeof(buf), "  Fuel cost          :  PHP %.2f", r->fuel_cost);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Total daily cost   :  PHP %.2f", r->total_cost);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Total passengers   :  %.0f pax", r->total_passengers);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Gross revenue      :  PHP %.2f", r->gross_revenue);
    print_row(buf);

    if (r->net_profit >= 0.0f)
        snprintf(buf, sizeof(buf), FG_GREEN "  Net profit         :  PHP %.2f" RESET, r->net_profit);
    else
        snprintf(buf, sizeof(buf), FG_RED "  Net profit/loss    :  PHP %.2f" RESET, r->net_profit);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Break-even pax     :  %d passengers", r->breakeven_pax);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Break-even/trip    :  %d per trip", r->breakeven_per_trip);
    print_row(buf);

    snprintf(buf, sizeof(buf), "  Coverage           :  %.1f%%", r->coverage_pct);
    print_row(buf);

    print_blank_row();
    print_divider();

    if (r->net_profit > 0.0f)
        print_row(FG_GREEN BOLD "  STATUS: PROFITABLE" RESET);
    else if (r->net_profit < 0.0f)
        print_row(FG_RED BOLD "  STATUS: AT A LOSS" RESET);
    else
        print_row(FG_YELLOW BOLD "  STATUS: BREAK-EVEN" RESET);

    print_blank_row();
    print_bottom_border();
}

/* ── Menu Display ────────────────────────────────────────── */

static void breakeven_print_menu(void) {
    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD FG_YELLOW
              "     [4] FARE-TO-FUEL BREAK-EVEN SIM      "
              RESET);
    print_divider();
    print_blank_row();
    print_row(FG_CYAN "  [1]  Traditional Jeepney  - Pre-hike" RESET);
    print_row(FG_CYAN "  [2]  Traditional Jeepney  - Oil hike" RESET);
    print_row(FG_CYAN "  [3]  Modern Jeepney       - Oil hike" RESET);
    print_row(FG_CYAN "  [4]  UV Express (AUV)     - Oil hike" RESET);
    print_row(FG_CYAN "  [5]  Provincial Bus       - Oil hike" RESET);
    print_row(FG_CYAN "  [6]  Tricycle             - Gasoline" RESET);
    print_row(FG_YELLOW "  [7]  Enter custom values" RESET);
    print_blank_row();
    print_divider();
    print_row(FG_RED "  [0]  Back to Main Menu" RESET);
    print_blank_row();
    print_bottom_border();
}

/* ── Public Entry Point ──────────────────────────────────── */

void breakeven_menu(void) {
    int choice;

    do {
        BreakevenInputs  in;
        BreakevenResults r;

        breakeven_print_menu();
        choice = get_int_input("  Enter choice: ");

        switch (choice) {
            case 1:
                in = breakeven_scenario_jeepney_pre_hike();
                r  = breakeven_calculate(&in);
                breakeven_print_results(&r);
                pause_prompt();
                break;
            case 2:
                in = breakeven_scenario_jeepney_hike();
                r  = breakeven_calculate(&in);
                breakeven_print_results(&r);
                pause_prompt();
                break;
            case 3:
                in = breakeven_scenario_modern_jeepney();
                r  = breakeven_calculate(&in);
                breakeven_print_results(&r);
                pause_prompt();
                break;
            case 4:
                in = breakeven_scenario_uv_express();
                r  = breakeven_calculate(&in);
                breakeven_print_results(&r);
                pause_prompt();
                break;
            case 5:
                in = breakeven_scenario_provincial_bus();
                r  = breakeven_calculate(&in);
                breakeven_print_results(&r);
                pause_prompt();
                break;
            case 6:
                in = breakeven_scenario_tricycle();
                r  = breakeven_calculate(&in);
                breakeven_print_results(&r);
                pause_prompt();
                break;
            case 7:
                in = breakeven_get_inputs();
                r  = breakeven_calculate(&in);
                breakeven_print_results(&r);
                pause_prompt();
                break;
            case 0:
                break;
            default:
                printf(FG_RED "\n  Invalid choice. Please enter 0-7.\n" RESET);
                sleep_ms(700);
                break;
        }
    } while (choice != 0);
}
