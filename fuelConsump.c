#include <stdio.h>
#include <stdlib.h>

#define MAX_LOGS 100

typedef struct {
    int tripNo;
    float distanceKm;
    float fuelLiters;
    float kmPerLiter;
} FuelLog;

void saveLogsToFile(FuelLog logs[], int count) {
    FILE *fp = fopen("fuel_log.txt", "w");
    if (fp == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    fprintf(fp, "Trip\tDistance(km)\tFuel(L)\tKm/L\n");
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d\t%.2f\t\t%.2f\t\t%.2f\n",
                logs[i].tripNo,
                logs[i].distanceKm,
                logs[i].fuelLiters,
                logs[i].kmPerLiter);
    }

    fclose(fp);
    printf("Logs saved to fuel_log.txt\n");
}

void viewLogs(FuelLog logs[], int count) {
    if (count == 0) {
        printf("No logs available.\n");
        return;
    }

    printf("\n%-8s %-15s %-15s %-15s\n", "Trip", "Distance(km)", "Fuel(L)", "Km/L");
    for (int i = 0; i < count; i++) {
        printf("%-8d %-15.2f %-15.2f %-15.2f\n",
               logs[i].tripNo,
               logs[i].distanceKm,
               logs[i].fuelLiters,
               logs[i].kmPerLiter);
    }
}

int main() {
    FuelLog logs[MAX_LOGS];
    int count = 0;
    int choice;

    do {
        printf("\n=== Fuel Consumption Log ===\n");
        printf("1. Add trip log\n");
        printf("2. View all logs\n");
        printf("3. Save logs to file\n");
        printf("4. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (count >= MAX_LOGS) {
                    printf("Log is full.\n");
                    break;
                }

                logs[count].tripNo = count + 1;

                printf("Enter distance traveled in km: ");
                scanf("%f", &logs[count].distanceKm);

                printf("Enter fuel consumed in liters: ");
                scanf("%f", &logs[count].fuelLiters);

                if (logs[count].fuelLiters > 0) {
                    logs[count].kmPerLiter = logs[count].distanceKm / logs[count].fuelLiters;
                } else {
                    logs[count].kmPerLiter = 0;
                }

                printf("Trip saved successfully.\n");
                count++;
                break;

            case 2:
                viewLogs(logs, count);
                break;

            case 3:
                saveLogsToFile(logs, count);
                break;

            case 4:
                printf("Exiting program.\n");
                break;

            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 4);

    return 0;
}