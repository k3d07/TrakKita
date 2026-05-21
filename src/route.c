/* ============================================================
 *  route.c - Route Efficiency Recommender
 *  CS103 - Computer Programming 2 | Group 4
 * ============================================================ */

#include <stdio.h>
#include <string.h>
#include "route.h"
#include "utils.h"

/* ── Driver session settings (set once via setup wizard) ──── */
static float g_diesel_price = DIESEL_PRICE_PER_LITER;
static float g_fuel_per_km  = BASE_FUEL_PER_KM;
static float g_boundary_fee = 500.0f;
static char  g_puv_label[32] = "Traditional Jeepney";
static int   g_setup_done   = 0;

/* ── PUV type presets ─────────────────────────────────────── */
typedef struct { const char *label; float fuel_per_km; } PuvPreset;

static const PuvPreset PUV_PRESETS[] = {
    { "Traditional Jeepney", 0.22f },
    { "Modern Jeepney",      0.11f },
    { "UV Express (AUV)",    0.08f },
    { "Tricycle",            0.03f },
};

/* ── Route seed data (no boundary fee — driver sets their own) */
typedef struct {
    int   id;
    char  name[50];
    float distanceKm;
    float trafficFactor;
    float avgFarePerTrip;
    int   tripsPerDay;
} RouteSeed;

static const RouteSeed ROUTE_SEEDS[] = {
    {  1, "Legazpi City - Daraga",          14.5f, 1.3f,  195.0f, 8 },
    {  2, "Legazpi - Sto. Domingo",         18.0f, 1.1f,  570.0f, 7 },
    {  3, "Legazpi - Tabaco City",          32.0f, 1.5f,  950.0f, 4 },
    {  4, "Daraga - Camalig",               12.0f, 1.2f,  250.0f, 9 },
    {  5, "Legazpi - Ligao City",           28.0f, 1.4f,  680.0f, 5 },
    {  6, "Sto. Domingo - Legazpi Coastal", 22.0f, 1.0f,  540.0f, 6 },
    {  7, "Legazpi - Rapu-Rapu Ferry+Jeep", 35.0f, 1.6f, 1160.0f, 3 },
    {  8, "Daraga - Guinobatan",            20.0f, 1.3f,  315.0f, 6 },
};

/* ── Initialization ───────────────────────────────────────── */

/* route_init
 * ----------
 * Fills the caller's routes[] array with our 8 hard-coded routes
 * (Legazpi area jeepney lines) by copying each field from ROUTE_SEEDS.
 * Called once at program start so the rest of the program can work
 * with RouteNode objects instead of touching the seed data directly.
 * Params : routes — array to populate (size must be >= count)
 *          count  — number of routes to initialize (use ROUTE_COUNT)
 */
void route_init(RouteNode routes[], int count) {
    for (int i = 0; i < count; i++) {
        routes[i].routeID       = ROUTE_SEEDS[i].id;
        strncpy(routes[i].name, ROUTE_SEEDS[i].name, sizeof(routes[i].name) - 1);
        routes[i].name[sizeof(routes[i].name) - 1] = '\0';
        routes[i].distanceKm    = ROUTE_SEEDS[i].distanceKm;
        routes[i].trafficFactor = ROUTE_SEEDS[i].trafficFactor;
        routes[i].baseFuelPerKm = BASE_FUEL_PER_KM;
        routes[i].avgFarePerTrip = ROUTE_SEEDS[i].avgFarePerTrip;
        routes[i].tripsPerDay   = ROUTE_SEEDS[i].tripsPerDay;
    }
}

/* ── Core calculations ────────────────────────────────────── */

/* route_estimated_fuel_cost
 * -------------------------
 * Computes the total daily fuel cost in pesos for one route.
 * Formula: liters_per_trip = distanceKm × fuelPerKm × trafficFactor
 *          cost            = liters_per_trip × tripsPerDay × dieselPrice
 * The trafficFactor multiplier (1.0–1.6) accounts for extra fuel burned
 * in stop-and-go driving on congested routes.
 * Params : node — pointer to the RouteNode being evaluated
 * Returns: estimated daily fuel cost in PHP
 */
float route_estimated_fuel_cost(RouteNode *node) {
    float liters = node->distanceKm * g_fuel_per_km * node->trafficFactor;
    return liters * (float)node->tripsPerDay * g_diesel_price;
}

/* route_estimated_net_income
 * --------------------------
 * Computes the driver's projected take-home pesos for one day on a
 * given route.
 * Formula: gross = avgFarePerTrip × tripsPerDay
 *          net   = gross − fuel_cost − boundary_fee
 * The boundary_fee is the daily rent the driver pays the vehicle owner
 * (zero for owner-drivers).
 * Params : node — pointer to the RouteNode being evaluated
 * Returns: net income in PHP (may be negative if the route loses money)
 */
float route_estimated_net_income(RouteNode *node) {
    float gross = node->avgFarePerTrip * (float)node->tripsPerDay;
    return gross - route_estimated_fuel_cost(node) - g_boundary_fee;
}

/* route_efficiency_score
 * ----------------------
 * Returns "pesos earned per peso spent on fuel" — our headline metric
 * for ranking routes. A score of 2.0 means the route returns ₱2 in
 * profit for every ₱1 of fuel. Used by route_find_best().
 * Guards against divide-by-zero when fuel cost is 0.
 * Params : node — pointer to the RouteNode being evaluated
 * Returns: efficiency ratio (net_income ÷ fuel_cost)
 */
float route_efficiency_score(RouteNode *node) {
    float fuel = route_estimated_fuel_cost(node);
    if (fuel <= 0.0f) return 0.0f;
    return route_estimated_net_income(node) / fuel;
}

/* route_find_by_id
 * ----------------
 * Linear search through routes[] looking for a matching routeID.
 * Used when the user types a route number in the "View Route Details"
 * menu so we can fetch and display that specific route.
 * Params : routes — array of RouteNodes
 *          count  — number of routes in the array
 *          id     — routeID to search for (valid range: 1–8)
 * Returns: pointer to the matching route, or NULL if not found
 */
RouteNode *route_find_by_id(RouteNode routes[], int count, int id) {
    for (int i = 0; i < count; i++)
        if (routes[i].routeID == id) return &routes[i];
    return NULL;
}

/* route_find_best
 * ---------------
 * Walks the entire routes[] array, computes the efficiency score of
 * each one, and returns whichever has the highest score. This is the
 * core of the recommendation feature — it tells the driver which
 * route earns the most per peso of fuel under their current settings.
 * Params : routes — array of RouteNodes
 *          count  — number of routes in the array
 * Returns: pointer to the best route, or NULL if the array is empty
 */
RouteNode *route_find_best(RouteNode routes[], int count) {
    RouteNode *best = NULL;
    float top = -9999.0f;
    for (int i = 0; i < count; i++) {
        float score = route_efficiency_score(&routes[i]);
        if (score > top) { top = score; best = &routes[i]; }
    }
    return best;
}

/* ── Helpers ──────────────────────────────────────────────── */

/* traffic_label
 * -------------
 * Maps a numeric trafficFactor to a human-readable word so the UI
 * shows "Heavy" instead of "1.5" when displaying route details.
 * Buckets: ≤1.1 Light, ≤1.3 Moderate, ≤1.5 Heavy, otherwise Very Heavy.
 * Params : factor — the route's trafficFactor multiplier
 * Returns: a const string label (do not free)
 */
static const char *traffic_label(float factor) {
    if (factor <= 1.1f) return "Light";
    if (factor <= 1.3f) return "Moderate";
    if (factor <= 1.5f) return "Heavy";
    return "Very Heavy";
}

/* ── Setup wizard ─────────────────────────────────────────── */

/* setup_header
 * ------------
 * Draws the heading shown at the top of every step in the vehicle
 * setup wizard ("Step X of 3 — description"). Keeps the wizard
 * looking consistent across all three screens.
 * Params : step — step number (1, 2, or 3)
 *          desc — short description of what the step asks
 */
static void setup_header(int step, const char *desc) {
    char buf[128];
    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD FG_YELLOW
              "         [3] ROUTE EFFICIENCY RECOMMENDER         "
              RESET);
    print_row(DIM "                  Vehicle Setup                  " RESET);
    print_divider();
    print_blank_row();
    snprintf(buf, sizeof(buf), "  " BOLD "Step %d of 3" RESET " — %s", step, desc);
    print_row(buf);
    print_blank_row();
}

/* route_setup
 * -----------
 * 3-step wizard that personalizes the calculations for the driver:
 *   Step 1 — vehicle type (sets g_fuel_per_km from PUV_PRESETS, or
 *            lets the user enter L/km manually)
 *   Step 2 — ownership   (sets g_boundary_fee to 0 or the driver's
 *            actual daily rent)
 *   Step 3 — fuel price  (sets g_diesel_price for today)
 * Finally shows a "Settings Saved" summary and flags g_setup_done so
 * the wizard doesn't auto-run again next time route_menu() opens.
 */
static void route_setup(void) {

    /* Step 1: Vehicle type → sets L/km ─────────────────────── */
    setup_header(1, "What type of vehicle do you drive?");
    print_row(FG_CYAN "  [1]  Traditional Jeepney   (0.22 L/km)" RESET);
    print_row(FG_CYAN "  [2]  Modern Jeepney        (0.11 L/km)" RESET);
    print_row(FG_CYAN "  [3]  UV Express (AUV)      (0.08 L/km)" RESET);
    print_row(FG_CYAN "  [4]  Tricycle              (0.03 L/km)" RESET);
    print_row(FG_YELLOW "  [5]  Enter manually" RESET);
    print_blank_row();
    print_bottom_border();
    printf("\n");

    int type = get_int_input("  Select vehicle type: ");
    if (type >= 1 && type <= 4) {
        g_fuel_per_km = PUV_PRESETS[type - 1].fuel_per_km;
        strncpy(g_puv_label, PUV_PRESETS[type - 1].label, sizeof(g_puv_label) - 1);
    } else {
        float manual = get_float_input("  Enter fuel use (L/km): ");
        if (manual > 0.0f) g_fuel_per_km = manual;
        strncpy(g_puv_label, "Custom", sizeof(g_puv_label) - 1);
    }

    /* Step 2: Ownership → sets boundary fee ────────────────── */
    setup_header(2, "Do you own this vehicle?");
    print_row(FG_CYAN "  [1]  Yes, I own it  —  no boundary fee" RESET);
    print_row(FG_CYAN "  [2]  No, I pay boundary / rent" RESET);
    print_blank_row();
    print_bottom_border();
    printf("\n");

    int own = get_int_input("  Select: ");
    if (own == 1) {
        g_boundary_fee = 0.0f;
    } else {
        float fee = get_float_input("  Daily boundary fee (PHP): ");
        if (fee >= 0.0f) g_boundary_fee = fee;
    }

    /* Step 3: Diesel / gas price ───────────────────────────── */
    char buf[96];
    setup_header(3, "Today's fuel price");
    snprintf(buf, sizeof(buf), "  Current default  : PHP %.2f / L", g_diesel_price);
    print_row(buf);
    print_row(DIM "  Enter 0 to keep the current value." RESET);
    print_blank_row();
    print_bottom_border();
    printf("\n");

    float price = get_float_input("  Diesel / gas price (PHP/L): ");
    if (price > 0.0f) g_diesel_price = price;

    g_setup_done = 1;

    /* Confirmation summary ─────────────────────────────────── */
    clear_screen();
    printf("\n");
    print_top_border();
    print_row(BOLD FG_GREEN "             SETTINGS SAVED              " RESET);
    print_divider();
    print_blank_row();

    snprintf(buf, sizeof(buf), "  Vehicle type    : %s", g_puv_label);
    print_row(buf);
    snprintf(buf, sizeof(buf), "  Fuel use        : %.3f L/km", g_fuel_per_km);
    print_row(buf);

    if (g_boundary_fee == 0.0f) {
        print_row("  Boundary fee    : None  (owner-driver)");
    } else {
        snprintf(buf, sizeof(buf), "  Boundary fee    : PHP %.2f / day", g_boundary_fee);
        print_row(buf);
    }

    snprintf(buf, sizeof(buf), "  Diesel price    : PHP %.2f / L", g_diesel_price);
    print_row(buf);
    print_blank_row();
    print_bottom_border();
    sleep_ms(400);
    pause_prompt();
}

/* ── Display: all routes ─────────────────────────────────── */

/* route_display_all
 * -----------------
 * Prints a compact table of every available route — ID, name,
 * distance, and net income for the day. The net income is colored
 * green if positive and red if negative so the driver can see
 * profitable vs. losing routes at a glance.
 * Params : routes — array of RouteNodes
 *          count  — number of routes in the array
 */
void route_display_all(RouteNode routes[], int count) {
    char row[96];

    printf("\n");
    print_top_border();
    print_row(BOLD FG_CYAN "               ALL AVAILABLE ROUTES               " RESET);
    print_divider();
    print_row(BOLD "  ID  Route Name                 Km    Net/Day" RESET);
    print_divider();

    for (int i = 0; i < count; i++) {
        float net = route_estimated_net_income(&routes[i]);
        const char *col = (net >= 0.0f) ? FG_GREEN : FG_RED;
        snprintf(row, sizeof(row),
                 "  %-2d  %-23.23s  %4.0f  %s%+8.0f%s",
                 routes[i].routeID,
                 routes[i].name,
                 routes[i].distanceKm,
                 col, net, RESET);
        print_row(row);
    }

    print_bottom_border();
}

/* ── Display: route detail ───────────────────────────────── */

/* route_display_detail
 * --------------------
 * Shows a full breakdown for a single route: distance, traffic level,
 * trips per day, fare, then the computed gross/fuel/boundary/net
 * figures. Includes a 20-segment efficiency bar built from Unicode
 * block characters (█ filled, ░ empty) so the score is visual.
 * Safely returns with an error message if `node` is NULL.
 * Params : node — the RouteNode to display (may be NULL)
 */
void route_display_detail(RouteNode *node) {
    if (!node) {
        printf(FG_RED "  Route not found.\n" RESET);
        return;
    }

    float fuel_cost = route_estimated_fuel_cost(node);
    float gross     = node->avgFarePerTrip * (float)node->tripsPerDay;
    float net       = route_estimated_net_income(node);
    float score     = route_efficiency_score(node);
    const char *net_color = (net >= 0.0f) ? FG_GREEN : FG_RED;
    char row[256];

    /* Block character score bar (█ filled, ░ empty) */
    int bar_len = (int)(score * 10.0f);
    if (bar_len > 20) bar_len = 20;
    if (bar_len < 0)  bar_len = 0;

    char bar[128];
    char *p = bar;
    for (int i = 0; i < 20; i++) {
        const char *block = (i < bar_len) ? "\xe2\x96\x88" : "\xe2\x96\x91";
        memcpy(p, block, 3);
        p += 3;
    }
    *p = '\0';

    printf("\n");
    print_top_border();

    snprintf(row, sizeof(row), BOLD FG_YELLOW "  ROUTE #%d — %.32s" RESET,
             node->routeID, node->name);
    print_row(row);
    print_divider();
    print_blank_row();

    snprintf(row, sizeof(row), "  Distance         : %.1f km", node->distanceKm);
    print_row(row);
    snprintf(row, sizeof(row), "  Traffic Level    : %s  (x%.1f)",
             traffic_label(node->trafficFactor), node->trafficFactor);
    print_row(row);
    snprintf(row, sizeof(row), "  Trips/Day        : %d trips", node->tripsPerDay);
    print_row(row);
    snprintf(row, sizeof(row), "  Avg Fare/Trip    : PHP %.2f", node->avgFarePerTrip);
    print_row(row);

    print_blank_row();
    print_divider();
    print_blank_row();

    snprintf(row, sizeof(row), "  Gross Income     : PHP %.2f", gross);
    print_row(row);
    snprintf(row, sizeof(row), "  Est. Fuel Cost   : PHP %.2f", fuel_cost);
    print_row(row);

    if (g_boundary_fee == 0.0f)
        print_row("  Boundary Fee     : None  (owner-driver)");
    else {
        snprintf(row, sizeof(row), "  Boundary Fee     : PHP %.2f / day", g_boundary_fee);
        print_row(row);
    }

    snprintf(row, sizeof(row), "%s  Net Income       : PHP %.2f" RESET, net_color, net);
    print_row(row);
    print_blank_row();
    print_divider();

    snprintf(row, sizeof(row), FG_GREEN "  Score : [%s] %.2f" RESET, bar, score);
    print_row(row);

    print_blank_row();
    print_bottom_border();
}

/* ── Display: best route recommendation ─────────────────── */

/* route_display_recommendation
 * ----------------------------
 * Asks route_find_best() for today's top route, then renders its
 * full detail card under a green "RECOMMENDED ROUTE TODAY" banner,
 * followed by a tip line showing the projected take-home pesos.
 * This is the headline output of the entire feature.
 * Params : routes — array of RouteNodes
 *          count  — number of routes in the array
 */
void route_display_recommendation(RouteNode routes[], int count) {
    RouteNode *best = route_find_best(routes, count);
    if (!best) {
        printf(FG_RED "  No routes available.\n" RESET);
        return;
    }

    printf("\n");
    print_top_border();
    print_row(BOLD FG_GREEN "              RECOMMENDED ROUTE TODAY              " RESET);
    print_bottom_border();

    route_display_detail(best);

    float net = route_estimated_net_income(best);
    printf(FG_YELLOW "\n  Tip: This route gives you the best net income\n");
    printf("       per peso spent on fuel today.\n");
    printf("       Projected daily take-home: " FG_GREEN BOLD "PHP %.2f\n" RESET, net);
}

/* ── Menu ─────────────────────────────────────────────────── */

/* route_menu
 * ----------
 * Public entry point for the Route Efficiency Recommender. On the
 * first call it auto-launches the setup wizard so the calculations
 * reflect the driver's actual vehicle and fuel price. Then it loops
 * the sub-menu (view all / view detail / recommend / update settings)
 * until the user picks 0 to go back to the main menu.
 * Params : routes — array of RouteNodes (populated by route_init)
 *          count  — number of routes (use ROUTE_COUNT)
 */
void route_menu(RouteNode routes[], int count) {
    if (!g_setup_done)
        route_setup();

    int choice = 0;

    do {
        clear_screen();
        printf("\n");
        print_top_border();
        print_row(BOLD FG_YELLOW
                  "         [3] ROUTE EFFICIENCY RECOMMENDER         "
                  RESET);
        print_divider();
        print_blank_row();
        print_row(FG_WHITE "  [1]  View All Routes" RESET);
        print_row(FG_WHITE "  [2]  View Route Details" RESET);
        print_row(FG_WHITE "  [3]  Get Best Route Recommendation" RESET);
        print_row(FG_WHITE "  [4]  Update Vehicle Settings" RESET);
        print_blank_row();
        print_row(DIM "  [0]  Back to Main Menu" RESET);
        print_blank_row();
        print_bottom_border();

        choice = get_int_input("Enter choice: ");

        switch (choice) {
            case 1:
                clear_screen();
                route_display_all(routes, count);
                pause_prompt();
                break;

            case 2: {
                clear_screen();
                route_display_all(routes, count);
                int id = get_int_input("Enter Route ID to view: ");
                RouteNode *node = route_find_by_id(routes, count, id);
                if (node) {
                    route_display_detail(node);
                } else {
                    printf(FG_RED "\n  Route ID %d not found.\n" RESET, id);
                }
                pause_prompt();
                break;
            }

            case 3:
                clear_screen();
                route_display_recommendation(routes, count);
                pause_prompt();
                break;

            case 4:
                route_setup();
                break;

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
