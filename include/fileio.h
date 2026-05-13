#ifndef FILEIO_H
#define FILEIO_H

/* ============================================================
 *  fileio.h - TrakKita Persistent Record Storage
 *  CS103 - Computer Programming 2 | Group 4
 * ============================================================ */

#include "structs.h"
#include "profit.h"
#include "fuel.h"

#define RECORDS_FILE "puv_records.txt"

void fileio_save(DaySummary days[], int day_count,
                 FuelEntry  fuel[], int fuel_count);

void fileio_load(DaySummary days[], int *day_count,
                 FuelEntry  fuel[], int *fuel_count);

#endif /* FILEIO_H */
