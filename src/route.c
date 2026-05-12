/* ============================================================
 *  route.c - TrakKita Route Efficiency Recommender
 *  CS103 - Computer Programming 2 | Group 4
 *  Developer: Kim Edrich B. Custodio
 * ============================================================ */

#include <stdio.h>
#include <string.h>
#include "route.h"
#include "utils.h"

typedef struct {
    int   id;
    char  name[50];
    float distanceKm;
    float trafficFactor;
    float avgFarePerTrip;
    int   tripsPerDay;
} RouteSeed;

static const RouteSeed ROUTE_SEEDS[] = {
    { 1, "Legazpi City - Daraga",           14.5f, 1.3f, 180.0f, 8 },
    { 2, "Legazpi - Sto. Domingo",          18.0f, 1.1f, 210.0f, 7 },
    { 3, "Legazpi - Tabaco City",           32.0f, 1.5f, 420.0f, 4 },
    { 4, "Daraga - Camalig",                12.0f, 1.2f, 150.0f, 9 },
    { 5, "Legazpi - Ligao City",            28.0f, 1.4f, 380.0f, 5 },
    { 6, "Sto. Domingo - Legazpi Coastal",  22.0f, 1.0f, 260.0f, 6 },
    { 7, "Legazpi - Rapu-Rapu Ferry+Jeep",  35.0f, 1.6f, 500.0f, 3 },
    { 8, "Daraga - Guinobatan",             20.0f, 1.3f, 240.0f, 6 },
};

void route_init(RouteNode routes[], int count) {
    for (int i = 0; i < count; i++) {
        routes[i].routeID = ROUTE_SEEDS[i].id;
        strncpy(routes[i].name, ROUTE_SEEDS[i].name, sizeof(routes[i].name) - 1);
        routes[i].name[sizeof(routes[i].name) - 1] = '\0';
        routes[i].distanceKm = ROUTE_SEEDS[i].distanceKm;
        routes[i].trafficFactor = ROUTE_SEEDS[i].trafficFactor;
        routes[i].baseFuelPerKm = BASE_FUEL_PER_KM;
        routes[i].avgFarePerTrip = ROUTE_SEEDS[i].avgFarePerTrip;
        routes[i].tripsPerDay = ROUTE_SEEDS[i].tripsPerDay;
    }
}

float route_estimated_fuel_cost(RouteNode *node) {
    float liters_per_trip = node->distanceKm
                          * node->baseFuelPerKm
                          * node->trafficFactor;
    float total_liters = liters_per_trip * node->tripsPerDay;
    return total_liters * DIESEL_PRICE_PER_LITER;
}

float route_estimated_net_income(RouteNode *node) {
    float gross = node->avgFarePerTrip * (float)node->tripsPerDay;
    return gross - route_estimated_fuel_cost(node);
}

float route_efficiency_score(RouteNode *node) {
    float fuel = route_estimated_fuel_cost(node);
    if (fuel <= 0.0f) {
        return 0.0f;
    }
    return route_estimated_net_income(node) / fuel;
}

RouteNode *route_find_by_id(RouteNode routes[], int count, int id) {
    for (int i = 0; i < count; i++) {
        if (routes[i].routeID == id) {
            return &routes[i];
        }
    }
    return NULL;
}

RouteNode *route_find_best(RouteNode routes[], int count) {
    RouteNode *best = NULL;
    float top = -9999.0f;

    for (int i = 0; i < count; i++) {
        float score = route_efficiency_score(&routes[i]);
        if (score > top) {
            top = score;
            best = &routes[i];
        }
    }

    return best;
}

static const char *traffic_label(float factor) {
    if (factor <= 1.1f) {
        return "Light";
    }
    if (factor <= 1.3f) {
        return "Moderate";
    }
    if (factor <= 1.5f) {
        return "Heavy";
    }
    return "Very Heavy";
}

void route_display_all(RouteNode routes[], int count) {
    char row[96];

    printf("\n");
    print_top_border();
    print_row(BOLD FG_CYAN "               ALL AVAILABLE ROUTES               " RESET);
    print_divider();
    print_row(BOLD "  ID  Route Name                  Km  Traffic" RESET);
    print_divider();

    for (int i = 0; i < count; i++) {
        snprintf(row, sizeof(row), "  %-2d  %-24.24s %5.1f  %-10.10s",
                 routes[i].routeID,
                 routes[i].name,
                 routes[i].distanceKm,
                 traffic_label(routes[i].trafficFactor));
        print_row(row);
    }

    print_bottom_border();
}

void route_display_detail(RouteNode *node) {
    if (!node) {
        printf(FG_RED "  Route not found.\n" RESET);
        return;
    }

    float fuel_cost = route_estimated_fuel_cost(node);
    float gross = node->avgFarePerTrip * (float)node->tripsPerDay;
    float net = route_estimated_net_income(node);
    float score = route_efficiency_score(node);
    const char *net_color = (net >= 0.0f) ? FG_GREEN : FG_RED;
    char row[128];
    char bar[32];

    int bar_len = (int)(score * 10.0f);
    if (bar_len > 20) {
        bar_len = 20;
    }
    if (bar_len < 0) {
        bar_len = 0;
    }

    for (int i = 0; i < 20; i++) {
        bar[i] = (i < bar_len) ? '#' : '-';
    }
    bar[20] = '\0';

    printf("\n");
    print_top_border();

    snprintf(row, sizeof(row), BOLD FG_YELLOW "  ROUTE #%d - %.32s" RESET,
             node->routeID, node->name);
    print_row(row);

    print_divider();
    print_blank_row();

    snprintf(row, sizeof(row), "  Distance       : %.1f km", node->distanceKm);
    print_row(row);

    snprintf(row, sizeof(row), "  Traffic Level  : %s (x%.1f)",
             traffic_label(node->trafficFactor), node->trafficFactor);
    print_row(row);

    snprintf(row, sizeof(row), "  Est. Trips/Day : %d trips", node->tripsPerDay);
    print_row(row);

    snprintf(row, sizeof(row), "  Avg Fare/Trip  : PHP %.2f", node->avgFarePerTrip);
    print_row(row);

    print_blank_row();
    print_divider();
    print_blank_row();

    snprintf(row, sizeof(row), "  Gross Income   : PHP %.2f", gross);
    print_row(row);

    snprintf(row, sizeof(row), "  Est. Fuel Cost : PHP %.2f", fuel_cost);
    print_row(row);

    snprintf(row, sizeof(row), "%s  Net Income    : PHP %.2f" RESET, net_color, net);
    print_row(row);

    print_blank_row();
    print_divider();

    snprintf(row, sizeof(row), FG_GREEN "  Score : [%s] %.2f" RESET, bar, score);
    print_row(row);

    print_blank_row();
    print_bottom_border();
}

void route_display_recommendation(RouteNode routes[], int count) {
    RouteNode *best = route_find_best(routes, count);
    if (!best) {
        printf(FG_RED "  No routes available.\n" RESET);
        return;
    }

    printf("\n");
    print_top_border();
    print_row(BOLD FG_GREEN "             RECOMMENDED ROUTE TODAY" RESET);
    print_bottom_border();

    route_display_detail(best);

    float net = route_estimated_net_income(best);
    printf(FG_YELLOW "\n  Tip: This route gives you the best net income\n");
    printf("       per peso spent on fuel today.\n");
    printf("       Projected daily take-home: " FG_GREEN BOLD "PHP %.2f\n" RESET, net);
}

void route_menu(RouteNode routes[], int count) {
    int choice = 0;

    do {
        clear_screen();
        printf("\n");
        print_top_border();
        print_row(BOLD FG_YELLOW
                  "        [3] ROUTE EFFICIENCY RECOMMENDER        "
                  RESET);
        print_divider();
        print_blank_row();

        print_row(FG_WHITE "  [1]  View All Routes" RESET);
        print_row(FG_WHITE "  [2]  View Route Details" RESET);
        print_row(FG_WHITE "  [3]  Get Best Route Recommendation" RESET);
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
