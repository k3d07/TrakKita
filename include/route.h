#ifndef ROUTE_H
#define ROUTE_H

/* ============================================================
 *  route.h — TrakKita Route Efficiency Recommender
 *  CS103 - Computer Programming 2 | Group 4
 *  Developer: Kim Eidrich B. Custodio
 * ============================================================ */

#include "structs.h"

/* ── Fuel constants ───────────────────────────────────────── */
#define DIESEL_PRICE_PER_LITER  65.00f   /* PHP, adjustable    */
#define BASE_FUEL_PER_KM         0.08f   /* liters/km baseline */
#define ROUTE_COUNT              8       /* total routes       */

/* ── Array initialization ─────────────────────────────────── */
void route_init(RouteNode routes[], int count);   /* fill from seed data */

/* ── Display ──────────────────────────────────────────────── */
void route_display_all(RouteNode routes[], int count);
void route_display_detail(RouteNode *node);
void route_display_recommendation(RouteNode routes[], int count);

/* ── Computation ──────────────────────────────────────────── */
float route_estimated_fuel_cost(RouteNode *node);
float route_estimated_net_income(RouteNode *node);
float route_efficiency_score(RouteNode *node);    /* higher = better */

/* ── Search ───────────────────────────────────────────────── */
RouteNode *route_find_by_id(RouteNode routes[], int count, int id);
RouteNode *route_find_best(RouteNode routes[], int count);

/* ── Entry point (called from main menu) ─────────────────── */
void route_menu(RouteNode routes[], int count);

#endif /* ROUTE_H */
