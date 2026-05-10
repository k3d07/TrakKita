#ifndef STRUCTS_H
#define STRUCTS_H

/* ============================================================
 *  structs.h — TrakKita Shared Data Structures
 *  CS103 - Computer Programming 2 | Group 4
 * ============================================================ */

/* ── Daily Summary (used by profit.c) ─────────────────────── */
typedef struct DaySummary {
    char  date[20];
    float totalFares;
    float fuelCost;
    float boundaryFee;
    float netProfit;
} DaySummary;

/* ── Route Node (array element for route.c) ───────────────── */
typedef struct RouteNode {
    int   routeID;
    char  name[50];
    float distanceKm;
    float trafficFactor;   /* 1.0 = normal, 1.5 = heavy traffic */
    float baseFuelPerKm;   /* liters consumed per km             */
    float avgFarePerTrip;  /* average fare income per full trip  */
    int   tripsPerDay;     /* estimated trips a driver can do    */
} RouteNode;

/* ── Fuel Log Entry (used by fuel.c) ──────────────────────── */
typedef struct FuelEntry {
    char  date[20];
    float liters;
    float pricePerLiter;
    float totalCost;
} FuelEntry;

/* ── Quick-Tap Session (used by quicktap.c) ───────────────── */
typedef struct QuickTapSession {
    char  date[20];
    int   tapCount;
    float totalCollected;
} QuickTapSession;

#endif /* STRUCTS_H */
