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
    {  1, "Legazpi City - Daraga",          14.5f, 1.3f, 180.0f, 8 },
    {  2, "Legazpi - Sto. Domingo",         18.0f, 1.1f, 210.0f, 7 },
    {  3, "Legazpi - Tabaco City",          32.0f, 1.5f, 420.0f, 4 },
    {  4, "Daraga - Camalig",               12.0f, 1.2f, 150.0f, 9 },
    {  5, "Legazpi - Ligao City",           28.0f, 1.4f, 380.0f, 5 },
    {  6, "Sto. Domingo - Legazpi Coastal", 22.0f, 1.0f, 260.0f, 6 },
    {  7, "Legazpi - Rapu-Rapu Ferry+Jeep", 35.0f, 1.6f, 500.0f, 3 },
    {  8, "Daraga - Guinobatan",            20.0f, 1.3f, 240.0f, 6 },
};

/* ── Initialization ───────────────────────────────────────── */

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

float route_estimated_fuel_cost(RouteNode *node) {
    float liters = node->distanceKm * g_fuel_per_km * node->trafficFactor;
    return liters * (float)node->tripsPerDay * g_diesel_price;
}

float route_estimated_net_income(RouteNode *node) {
    float gross = node->avgFarePerTrip * (float)node->tripsPerDay;
    return gross - route_estimated_fuel_cost(node) - g_boundary_fee;
}

float route_efficiency_score(RouteNode *node) {
    float fuel = route_estimated_fuel_cost(node);
    if (fuel <= 0.0f) return 0.0f;
    return route_estimated_net_income(node) / fuel;
}

RouteNode *route_find_by_id(RouteNode routes[], int count, int id) {
    for (int i = 0; i < count; i++)
        if (routes[i].routeID == id) return &routes[i];
    return NULL;
}

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

static const char *traffic_label(float factor) {
    if (factor <= 1.1f) return "Light";
    if (factor <= 1.3f) return "Moderate";
    if (factor <= 1.5f) return "Heavy";
    return "Very Heavy";
}

/* ── Setup wizard ─────────────────────────────────────────── */

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
