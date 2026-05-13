#ifndef BREAKEVEN_H
#define BREAKEVEN_H

/* ============================================================
 *  breakeven.h — Fare-to-Fuel Break-Even Simulator
 *  CS103 - Computer Programming 2 | Group 4
 * ============================================================ */

typedef struct {
    float fuel_price;
    float fuel_efficiency;
    float route_distance;
    int   trips_per_day;
    float boundary_fee;
    float maintenance;
    float driver_wages;
    float other_costs;
    float fare_per_pax;
    int   pax_per_trip;
} BreakevenInputs;

typedef struct {
    float fuel_cost;
    float total_cost;
    float total_passengers;
    float gross_revenue;
    float net_profit;
    int   breakeven_pax;
    int   breakeven_per_trip;
    float coverage_pct;
} BreakevenResults;

void breakeven_menu(void);

#endif /* BREAKEVEN_H */
