#ifndef FUEL_H
#define FUEL_H

/* ============================================================
 *  fuel.h - Fuel Consumption Log (7-Day)
 *  CS103 - Computer Programming 2 | Group 4
 * ============================================================ */

#include "structs.h"

#define MAX_FUEL_ENTRIES  7

void fuel_menu(FuelEntry entries[], int *count);

#endif /* FUEL_H */
