
/* ============================================================
 *  fileio.c - TrakKita Persistent Record Storage
 *  CS103 - Computer Programming 2 | Group 4
 * ============================================================ */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "fileio.h"

/* fileio_save
 * -----------
 * Writes all in-memory records (daily P/L summaries and fuel entries)
 * to puv_records.txt in a human-readable table format. The file opens
 * in "w" mode, so it fully overwrites the previous save — the file is
 * a snapshot of current state, not an append log. Layout:
 *   header   → project name and last-saved date
 *   [DAYS]   → count line, then a formatted table of DaySummary rows
 *   [FUEL]   → count line, then a formatted table of FuelEntry rows
 *   footer   → "End of Records"
 * If fopen() fails (e.g., disk full or no permission) the function
 * exits quietly instead of crashing the program.
 * Params : days, day_count   — daily records to save
 *          fuel, fuel_count  — fuel log entries to save
 */
void fileio_save(DaySummary days[], int day_count,
                 FuelEntry  fuel[], int fuel_count) {
    FILE *fp = fopen(RECORDS_FILE, "w");
    if (!fp) {
        return;
    }

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char date_str[20];
    strftime(date_str, sizeof(date_str), "%Y-%m-%d", tm_info);

    /* ── File header ─────────────────────────────────────────── */
    fprintf(fp, "====================================================\n");
    fprintf(fp, "  TrakKita - PUV Daily Records\n");
    fprintf(fp, "  Last saved : %s\n", date_str);
    fprintf(fp, "====================================================\n\n");

    /* ── Daily earnings table ────────────────────────────────── */
    fprintf(fp, "[DAYS]\n");
    fprintf(fp, "%d\n", day_count);
    fprintf(fp, "  #  | Date       | Total Fares |  Fuel Cost |   Boundary |  Net Profit\n");
    fprintf(fp, " ----+------------+-------------+------------+------------+-------------\n");
    for (int i = 0; i < day_count; i++) {
        fprintf(fp, " %3d | %-10s | %11.2f | %10.2f | %10.2f | %11.2f\n",
                i + 1,
                days[i].date,
                days[i].totalFares,
                days[i].fuelCost,
                days[i].boundaryFee,
                days[i].netProfit);
    }
    fprintf(fp, " ----+------------+-------------+------------+------------+-------------\n");

    /* ── Fuel log table ──────────────────────────────────────── */
    fprintf(fp, "\n[FUEL]\n");
    fprintf(fp, "%d\n", fuel_count);
    fprintf(fp, "  #  | Date       |   Liters |    PHP/L |  Total Cost\n");
    fprintf(fp, " ----+------------+----------+----------+------------\n");
    for (int i = 0; i < fuel_count; i++) {
        fprintf(fp, " %3d | %-10s | %8.2f | %8.2f | %10.2f\n",
                i + 1,
                fuel[i].date,
                fuel[i].liters,
                fuel[i].pricePerLiter,
                fuel[i].totalCost);
    }
    fprintf(fp, " ----+------------+----------+----------+------------\n");

    /* ── File footer ─────────────────────────────────────────── */
    fprintf(fp, "\n====================================================\n");
    fprintf(fp, "  End of Records\n");
    fprintf(fp, "====================================================\n");

    fclose(fp);
}

/* fileio_load
 * -----------
 * Reads puv_records.txt back into memory at program startup, refilling
 * the days[] and fuel[] arrays. Designed to tolerate the table-style
 * format produced by fileio_save():
 *   1. Section is tracked with `section` (0=none, 1=DAYS, 2=FUEL),
 *      flipped when we hit a "[DAYS]" or "[FUEL]" header line.
 *   2. The first numeric line inside a section is treated as the
 *      record count and stored in `remaining`.
 *   3. Subsequent lines are passed to sscanf() using the table's
 *      "  # | date | f | f | ..." pattern. Lines that fail to parse
 *      (column headers, separator dashes, blanks) are silently
 *      skipped — `remaining` only decrements on a successful parse.
 * Trailing \r and \n are stripped so files saved on Windows still load
 * cleanly on other systems. If the file is missing (first run) we
 * leave both counts at 0 and return — that's an expected case, not an
 * error.
 * Params : days, day_count   — destination array and out-count
 *          fuel, fuel_count  — destination array and out-count
 */
void fileio_load(DaySummary days[], int *day_count,
                 FuelEntry  fuel[], int *fuel_count) {
    *day_count  = 0;
    *fuel_count = 0;

    FILE *fp = fopen(RECORDS_FILE, "r");
    if (!fp) {
        return; /* first run — no file yet */
    }

    char line[256];
    int  section   = 0;
    int  remaining = 0;

    while (fgets(line, sizeof(line), fp)) {
        /* strip trailing newline */
        int len = (int)strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[--len] = '\0';
        }

        /* skip blank and decorator lines */
        if (line[0] == '\0' || line[0] == '=') {
            continue;
        }

        if (strcmp(line, "[DAYS]") == 0) {
            section   = 1;
            remaining = 0;
            continue;
        }
        if (strcmp(line, "[FUEL]") == 0) {
            section   = 2;
            remaining = 0;
            continue;
        }

        if (section == 1) {
            if (remaining == 0) {
                /* expect a count line; non-numeric lines (headers) are skipped */
                int n = 0;
                if (sscanf(line, "%d", &n) == 1 && n > 0) {
                    remaining = n;
                }
                continue;
            }
            /* try to parse a data row: "  1 | YYYY-MM-DD | f | f | f | f" */
            if (*day_count < MAX_DAY_SUMMARIES) {
                DaySummary *d = &days[*day_count];
                int parsed = sscanf(line,
                                    " %*d | %19s | %f | %f | %f | %f",
                                    d->date,
                                    &d->totalFares,
                                    &d->fuelCost,
                                    &d->boundaryFee,
                                    &d->netProfit);
                if (parsed == 5) {
                    (*day_count)++;
                    remaining--;
                }
                /* header / separator lines fail to parse — remaining unchanged */
            }
        } else if (section == 2) {
            if (remaining == 0) {
                int n = 0;
                if (sscanf(line, "%d", &n) == 1 && n > 0) {
                    remaining = n;
                }
                continue;
            }
            /* try to parse a data row: "  1 | YYYY-MM-DD | f | f | f" */
            if (*fuel_count < MAX_FUEL_ENTRIES) {
                FuelEntry *f = &fuel[*fuel_count];
                int parsed = sscanf(line,
                                    " %*d | %19s | %f | %f | %f",
                                    f->date,
                                    &f->liters,
                                    &f->pricePerLiter,
                                    &f->totalCost);
                if (parsed == 4) {
                    (*fuel_count)++;
                    remaining--;
                }
            }
        }
    }

    fclose(fp);
}
