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

/* breakeven_calculate
 * -------------------
 * Heart of the simulator — turns one set of operating inputs into a
 * complete set of break-even results.
 * Formulas:
 *   fuel_cost          = (distance ÷ km_per_liter) × fuel_price
 *   total_cost         = fuel + boundary + maintenance + wages + other
 *   total_passengers   = trips_per_day × pax_per_trip
 *   gross_revenue      = total_passengers × fare_per_pax
 *   net_profit         = gross_revenue − total_cost
 *   breakeven_pax      = ⌈total_cost ÷ fare_per_pax⌉      (whole pax)
 *   breakeven_per_trip = ⌈breakeven_pax ÷ trips_per_day⌉  (whole pax)
 *   coverage_pct       = (gross_revenue ÷ total_cost) × 100
 * ceilf() is used so passenger counts round up — half a passenger
 * doesn't exist, and rounding down would understate the requirement.
 * Validates inputs > 0 to avoid divide-by-zero.
 * Params : in — pointer to a populated BreakevenInputs
 * Returns: a BreakevenResults struct (all-zero if inputs are invalid)
 */
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
/* breakeven_scenario_jeepney_pre_hike
 * -----------------------------------
 * Preset for a traditional jeepney BEFORE the oil price hike, when
 * diesel was around PHP 58/L. Used as a baseline so the user can
 * compare to the "hike" scenarios and see the impact on profitability.
 * Returns: a fully populated BreakevenInputs
 */
static BreakevenInputs breakeven_scenario_jeepney_pre_hike(void) {
    BreakevenInputs in = {58.0f, 4.5f, 120.0f, 10, 900.0f, 120.0f, 0.0f, 70.0f, 13.0f, 20};
    return in;
}

/* Traditional Jeepney — during the oil hike (diesel PHP 85/L) */
/* breakeven_scenario_jeepney_hike
 * -------------------------------
 * Same traditional jeepney as the pre-hike preset, but with diesel at
 * PHP 85/L and slightly fewer trips per day (drivers cut back when
 * costs spike). Demonstrates how fuel inflation pushes the break-even
 * passenger count up.
 * Returns: a fully populated BreakevenInputs
 */
static BreakevenInputs breakeven_scenario_jeepney_hike(void) {
    BreakevenInputs in = {85.0f, 4.5f, 120.0f, 10, 900.0f, 150.0f, 0.0f, 80.0f, 14.0f, 17};
    return in;
}

/* Modern/e-jeepney — better fuel efficiency (~9 km/L), higher boundary */
/* breakeven_scenario_modern_jeepney
 * ---------------------------------
 * Preset for a modern / e-jeepney during the oil hike. Better fuel
 * efficiency (~9 km/L) softens the blow of higher diesel, but the
 * higher boundary fee (loan amortization) eats into the savings.
 * Returns: a fully populated BreakevenInputs
 */
static BreakevenInputs breakeven_scenario_modern_jeepney(void) {
    BreakevenInputs in = {85.0f, 9.0f, 160.0f, 12, 2200.0f, 200.0f, 400.0f, 100.0f, 16.0f, 22};
    return in;
}

/* UV Express — longer inter-city routes, higher fares */
/* breakeven_scenario_uv_express
 * -----------------------------
 * Preset for a UV Express AUV. Long inter-city routes mean fewer
 * trips per day but higher fares per passenger.
 * Returns: a fully populated BreakevenInputs
 */
static BreakevenInputs breakeven_scenario_uv_express(void) {
    BreakevenInputs in = {85.0f, 12.0f, 180.0f, 8, 1500.0f, 200.0f, 0.0f, 150.0f, 50.0f, 11};
    return in;
}

/* Provincial Bus — long routes, low efficiency, hired driver */
/* breakeven_scenario_provincial_bus
 * ---------------------------------
 * Preset for a provincial bus. Long routes, low fuel efficiency, and a
 * hired driver (wages > 0) — the largest cost stack of any scenario.
 * Returns: a fully populated BreakevenInputs
 */
static BreakevenInputs breakeven_scenario_provincial_bus(void) {
    BreakevenInputs in = {85.0f, 5.0f, 320.0f, 2, 4000.0f, 500.0f, 600.0f, 300.0f, 150.0f, 38};
    return in;
}

/* Tricycle — gasoline, short barangay routes, many trips */
/* breakeven_scenario_tricycle
 * ---------------------------
 * Preset for a tricycle (gasoline-powered). Very high fuel efficiency
 * and many short trips per day, but tiny passenger capacity per trip.
 * Returns: a fully populated BreakevenInputs
 */
static BreakevenInputs breakeven_scenario_tricycle(void) {
    BreakevenInputs in = {68.0f, 30.0f, 80.0f, 30, 150.0f, 50.0f, 0.0f, 50.0f, 12.0f, 3};
    return in;
}

/* ── Custom Input Collection ─────────────────────────────── */

/* breakeven_get_inputs
 * --------------------
 * Interactive form that collects all 10 BreakevenInputs fields from
 * the user one prompt at a time. Used when the driver picks the
 * "custom values" option instead of a preset scenario, so they can
 * simulate their own real-world conditions.
 * Returns: a fully populated BreakevenInputs
 */
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

/* breakeven_print_results
 * -----------------------
 * Displays all fields of a BreakevenResults in a boxed report:
 * fuel cost, total cost, passengers, revenue, net profit, break-even
 * pax (total and per trip), and cost-coverage percentage. The net
 * profit line is colored green/red based on its sign, and the bottom
 * banner reads PROFITABLE / AT A LOSS / BREAK-EVEN accordingly so the
 * verdict is unmissable.
 * Params : r — the simulation results to display
 */
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

/* breakeven_print_menu
 * --------------------
 * Renders the boxed list of scenario options (the six presets, plus
 * "custom values" and "back"). Called at the top of every loop in
 * breakeven_menu().
 */
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

/* breakeven_menu
 * --------------
 * Public entry point for the Fare-to-Fuel Break-Even Simulator. Loops:
 * show menu → grab inputs from the chosen scenario (or custom form) →
 * run breakeven_calculate() → print the results. Exits when the user
 * picks 0 to return to the main menu.
 */
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
