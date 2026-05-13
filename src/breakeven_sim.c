/*
 * ============================================================
 *   PUV BREAK-EVEN SIMULATOR
 *
 *   PURPOSE:
 *   Simulates the daily financial situation of a Public
 *   Utility Vehicle (PUV) driver. Shows whether the driver
 *   makes a profit or loss based on fuel costs, expenses,
 *   fare, and passenger count. Designed in the context of
 *   the oil price hike caused by the conflict in the
 *   Middle East.
 *
 *   Covers the following PUV types:
 *     - Traditional Jeepney
 *     - Modern Jeepney
 *     - UV Express (AUV)
 *     - Provincial Bus
 *     - Tricycle
 * ============================================================
 */

#include <stdio.h>
#include <math.h>   /* needed for ceil() in break-even calculation */
#include <stdlib.h>


typedef struct {
    double fuel_price;       /* how much fuel costs per liter (PHP)        */
    double fuel_efficiency;  /* how many km the PUV travels per liter      */
    double route_distance;   /* total km driven in one day                 */
    int    trips_per_day;    /* how many trips the driver completes daily  */
    double boundary_fee;     /* daily rent paid to the PUV operator        */
    double maintenance;      /* daily estimate for repairs, oil, etc.      */
    double driver_wages;     /* daily pay for a hired driver (0 if owner)  */
    double other_costs;      /* food, toll fees, parking, etc.             */
    double fare_per_pax;     /* how much each passenger pays (PHP)         */
    int    pax_per_trip;     /* average number of passengers per trip      */
} Inputs;

typedef struct {
    double fuel_cost;           /* total PHP spent on fuel for the day    */
    double total_cost;          /* sum of ALL daily expenses              */
    double total_passengers;    /* trips_per_day x pax_per_trip           */
    double gross_revenue;       /* total_passengers x fare_per_pax        */
    double net_profit;          /* gross_revenue - total_cost             */
    int    breakeven_pax;       /* minimum passengers needed to not lose  */
    int    breakeven_per_trip;  /* breakeven_pax spread across all trips  */
    double coverage_pct;        /* what % of costs the revenue covers     */
} Results;

/* ─────────────────────────────────────────────── */
/*  CORE CALCULATION                               */
/*                                                 */
/*  This is the heart of the simulator.            */
/*  Takes the user's Inputs, runs the formulas,    */
/*  and returns a Results struct with everything   */
/*  already computed.                              */
/* ─────────────────────────────────────────────── */

Results calculate(const Inputs *in) {
    Results r = {0}; /* zero out all fields to avoid garbage values */

    /* Safety check: avoid division by zero crashes (aray ko nung first try ko) */
    if (in->fuel_efficiency <= 0 || in->fare_per_pax <= 0) {
        printf("Error: fuel efficiency and fare must be greater than 0.\n");
        return r;
    }

    /* How much fuel is consumed x price = daily fuel bill */
    r.fuel_cost = (in->route_distance / in->fuel_efficiency) * in->fuel_price;

    /* Add all daily expenses together */
    r.total_cost = r.fuel_cost + in->boundary_fee + in->maintenance
                   + in->driver_wages + in->other_costs;

    /* Total people served in a day */
    r.total_passengers = (double)(in->trips_per_day * in->pax_per_trip);

    /* Total money collected from passengers */
    r.gross_revenue = r.total_passengers * in->fare_per_pax;

    /* Positive = profit, Negative = loss */
    r.net_profit = r.gross_revenue - r.total_cost;

    /* Minimum passengers needed so revenue exactly covers total cost */
    r.breakeven_pax = (int)ceil(r.total_cost / in->fare_per_pax);

    /* Spread the break-even target across all trips in the day */
    r.breakeven_per_trip = (int)ceil((double)r.breakeven_pax / in->trips_per_day);

    /* How much of the total cost is covered by revenue (as a percentage) */
    r.coverage_pct = (r.total_cost > 0)
                     ? (r.gross_revenue / r.total_cost) * 100.0 : 0.0;

    return r;
}

/* ─────────────────────────────────────────────── */
/*  PRESET SCENARIOS                               */
/*                                                 */
/*  Pre-filled examples for each PUV type so the  */
/*  user doesn't have to type all 10 values.       */
/*  for demos and presentations.                   */
/* ─────────────────────────────────────────────── */

/*
 * TRADITIONAL JEEPNEY — Pre-hike baseline
 * Diesel at PHP 58/L. Represents the normal situation
 * BEFORE the Middle East conflict drove oil prices up.
 * Fuel efficiency is low (~4.5 km/L) due to old engines.
 */
Inputs scenario_jeepney_pre_hike(void) {
    Inputs in = {58.0, 4.5, 120.0, 10, 900.0, 120.0, 0.0, 70.0, 13.0, 20};
    return in;
}

/*
 * TRADITIONAL JEEPNEY — During oil price hike
 * Diesel jumps to PHP 85/L. Same route and driver but
 * fuel is now far more expensive due to the oil crisis.
 * Passenger count drops slightly as higher fares push
 * some riders to other modes of transport.
 */
Inputs scenario_jeepney_hike(void) {
    Inputs in = {85.0, 4.5, 120.0, 10, 900.0, 150.0, 0.0, 80.0, 14.0, 17};
    return in;
}

/*
 * MODERN JEEPNEY (e-jeepney / Euro 4 compliant)
 * Still at PHP 85/L but with much better fuel efficiency
 * (~9 km/L) due to a modern engine. Carries more passengers
 * per trip and charges a slightly higher fare. However,
 * the boundary fee and wages are also higher because modern
 * units cost more to operate and finance.
 */
Inputs scenario_modern_jeepney(void) {
    Inputs in = {85.0, 9.0, 160.0, 12, 2200.0, 200.0, 400.0, 100.0, 16.0, 22};
    return in;
}

/*
 * UV EXPRESS (Asian Utility Vehicle / AUV)
 * Typically serves longer routes between cities or
 * key terminals. Better fuel efficiency than jeepneys
 * (~12 km/L) and higher fares due to longer distances.
 * Carries fewer passengers (10-12) but at higher rates.
 * Boundary fee varies widely by route and operator.
 */
Inputs scenario_uv_express(void) {
    /* diesel=85, efficiency=12, distance=180, trips=8,
       boundary=1500, maintenance=200, wages=0, other=150,
       fare=50, pax=11                                      */
    Inputs in = {85.0, 12.0, 180.0, 8, 1500.0, 200.0, 0.0, 150.0, 50.0, 11};
    return in;
}

/*
 * PROVINCIAL BUS
 * Covers long inter-city or inter-province routes.
 * Low fuel efficiency (~5 km/L) due to large engine and
 * heavy load. Routes are much longer (300+ km/day).
 * Higher fares offset the high fuel cost somewhat, but
 * the boundary fee and hired driver wages are also higher.
 * Passenger capacity is large (~40 per trip).
 */
Inputs scenario_provincial_bus(void) {
    /* diesel=85, efficiency=5, distance=320, trips=2,
       boundary=4000, maintenance=500, wages=600, other=300,
       fare=150, pax=38                                      */
    Inputs in = {85.0, 5.0, 320.0, 2, 4000.0, 500.0, 600.0, 300.0, 150.0, 38};
    return in;
}

/*
 * TRICYCLE
 * Uses gasoline instead of diesel, so more fuel efficient
 * per liter (~30 km/L). Operates on short local routes
 * within barangays or municipalities. Very low fare and
 * low boundary fee. Carries only 2-4 passengers per trip
 * but completes many short trips throughout the day.
 * Note: fuel_price here reflects gasoline, not diesel.
 */
Inputs scenario_tricycle(void) {
    /* gasoline=68, efficiency=30, distance=80, trips=30,
       boundary=150, maintenance=50, wages=0, other=50,
       fare=12, pax=3                                       */
    Inputs in = {68.0, 30.0, 80.0, 30, 150.0, 50.0, 0.0, 50.0, 12.0, 3};
    return in;
}

/* ─────────────────────────────────────────────── */
/*  GET INPUTS                                     */
/*                                                 */
/*  Prompts the user to type in all 10 values      */
/*  one by one. Used by menu option 7.             */
/* ─────────────────────────────────────────────── */

Inputs get_inputs(void) {
    Inputs in;

    printf("\n--- ENTER VALUES ---\n");
    printf("Fuel price (PHP/liter)         : "); scanf("%lf", &in.fuel_price);
    printf("Fuel efficiency (km/liter)     : "); scanf("%lf", &in.fuel_efficiency);
    printf("Daily route distance (km)      : "); scanf("%lf", &in.route_distance);
    printf("Trips per day                  : "); scanf("%d",  &in.trips_per_day);
    printf("Boundary / rent fee (PHP)      : "); scanf("%lf", &in.boundary_fee);
    printf("Maintenance cost (PHP)         : "); scanf("%lf", &in.maintenance);
    printf("Driver wages (PHP, 0 if none)  : "); scanf("%lf", &in.driver_wages);
    printf("Other costs (PHP)              : "); scanf("%lf", &in.other_costs);
    printf("Fare per passenger (PHP)       : "); scanf("%lf", &in.fare_per_pax);
    printf("Avg passengers per trip        : "); scanf("%d",  &in.pax_per_trip);

    return in;
}

/* ─────────────────────────────────────────────── */
/*  PRINT RESULTS                                  */
/*                                                 */
/*  Displays the computed Results to the terminal. */
/*  Called after every scenario or custom input.   */
/* ─────────────────────────────────────────────── */

void print_results(const Results *r) {
    printf("\n========================================\n");
    printf("           SIMULATION RESULTS\n");
    printf("========================================\n");
    printf("Fuel cost           : PHP %.2f\n",  r->fuel_cost);
    printf("Total daily cost    : PHP %.2f\n",  r->total_cost);
    printf("Total passengers    : %.0f pax\n",  r->total_passengers);
    printf("Gross revenue       : PHP %.2f\n",  r->gross_revenue);
    printf("Net profit / loss   : PHP %.2f\n",  r->net_profit);
    printf("Break-even pax      : %d passengers\n", r->breakeven_pax);
    printf("Break-even per trip : %d per trip\n",   r->breakeven_per_trip);
    printf("Coverage            : %.1f%%\n",        r->coverage_pct);
    printf("----------------------------------------\n");

    /* Status verdict based on net profit */
    if (r->net_profit > 0)
        printf("STATUS: PROFITABLE\n");
    else if (r->net_profit < 0)
        printf("STATUS: AT A LOSS\n");
    else
        printf("STATUS: BREAK-EVEN\n");

    printf("========================================\n");
}

/* ─────────────────────────────────────────────── */
/*  MAIN                                           */
/* ─────────────────────────────────────────────── */

int main(void) {
    int choice;

    printf("========================================\n");
    printf("      PUV BREAK-EVEN SIMULATOR\n");
    printf("   Oil Price Hike Impact Analyzer\n");
    printf("========================================\n");

    /* Keep looping until the user chooses to exit */
    do {
        printf("\n--- MENU ---\n");
        printf("[1] Traditional Jeepney  - Pre-hike baseline\n");
        printf("[2] Traditional Jeepney  - During oil hike\n");
        printf("[3] Modern Jeepney       - During oil hike\n");
        printf("[4] UV Express (AUV)     - During oil hike\n");
        printf("[5] Provincial Bus       - During oil hike\n");
        printf("[6] Tricycle             - Current gasoline price\n");
        printf("[7] Enter custom values\n");
        printf("[0] Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        Inputs  in;
        Results r;

        switch (choice) {
            case 1:
                /* Traditional jeepney before the oil price hike */
                in = scenario_jeepney_pre_hike();
                r  = calculate(&in);
                print_results(&r);
                break;
            case 2:
                /* Traditional jeepney during the oil price hike */
                in = scenario_jeepney_hike();
                r  = calculate(&in);
                print_results(&r);
                break;
            case 3:
                /* Modern/e-jeepney with better fuel efficiency */
                in = scenario_modern_jeepney();
                r  = calculate(&in);
                print_results(&r);
                break;
            case 4:
                /* UV Express serving longer inter-city routes */
                in = scenario_uv_express();
                r  = calculate(&in);
                print_results(&r);
                break;
            case 5:
                /* Provincial bus on long inter-city routes */
                in = scenario_provincial_bus();
                r  = calculate(&in);
                print_results(&r);
                break;
            case 6:
                /* Tricycle on short local barangay routes */
                in = scenario_tricycle();
                r  = calculate(&in);
                print_results(&r);
                break;
            case 7:
                /* User manually enters all 10 values */
                in = get_inputs();
                r  = calculate(&in);
                print_results(&r);
                break;
            case 0:
                printf("\nExiting simulator. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please enter 0-7.\n");
        }

    } while (choice != 0); /* loop ends when user picks 0 */

    return 0;
}