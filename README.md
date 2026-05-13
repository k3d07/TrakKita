# TrakKita — PUV Daily Operations & Earnings Tracker

TrakKita is a terminal-based C program for Filipino PUV/jeepney drivers. It tracks daily
earnings, fuel consumption, and route profitability to help drivers make better financial
decisions.

Built for CS103 - Computer Programming 2, Group 4.

## Group Members

| Name | Feature |
|------|---------|
| Kim Eidrich B. Custodio | Route Efficiency Recommender |
| Cyril Gian T. Anonuevo | Daily Profit / Loss Calculator |
| Keith Bryan O. Belmonte | Fuel Consumption Log |
| Kile Nicandro A. Roxas | Quick-Tap Fare Pad |
| Clarence James D. Gelua | Fare-to-Fuel Break-Even Simulator |

## Features

| # | Feature | Description |
|---|---------|-------------|
| 1 | Daily Profit / Loss Calculator | Input fares, fuel cost, and boundary fee to compute net profit or loss for the day. Warns the driver if they are operating at a loss. Stores a running history of past entries. |
| 2 | Fuel Consumption Log (7-Day) | Log daily fuel fill-ups (liters and price per liter). View the 7-day log and a summary showing averages, most expensive day, and cheapest day. |
| 3 | Route Efficiency Recommender | Compares 8 local PUV routes by net income after fuel and boundary costs. Includes a setup wizard for vehicle type, ownership, and diesel price. Recommends the best route for the day. |
| 4 | Fare-to-Fuel Break-Even Simulator | Simulates the daily financial situation for 6 PUV types (traditional jeepney, modern jeepney, UV express, provincial bus, tricycle) or custom inputs. Shows fuel cost, gross revenue, net profit/loss, break-even passenger count, and coverage percentage. |
| 5 | Quick-Tap Fare Pad | Fast fare counting pad for drivers mid-route. Tap [1] for discounted fare (₱11.00) or [2] for regular fare (₱13.00). Displays a live running total and logs the session. |

## Data Storage

All records are saved to and loaded from `puv_records.txt` automatically on exit and startup.
The file stores daily earnings summaries and fuel log entries in a plain-text table format.

## Getting Started

### Option A — Automatic setup (recommended)

1. Install MSYS2 from `https://www.msys2.org` and use the **default install path** (`C:\msys64`).
2. Double-click `setup.bat` in the project folder. It will:
   - Check that MSYS2 is installed
   - Install GCC and `mingw32-make` via `pacman`
   - Add `C:\msys64\ucrt64\bin` to your system PATH automatically
3. **Restart your terminal or VS Code** after `setup.bat` finishes so the PATH change takes effect.
4. Open a terminal in the project folder and run:

```bash
mingw32-make run
```

### Option B — Manual setup

1. Install MSYS2 from `https://www.msys2.org`.
2. Open the **MSYS2 UCRT64** terminal and run:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make
```

3. Add `C:\msys64\ucrt64\bin` to your Windows PATH manually:
   - Open **Start** → search **"Environment Variables"**
   - Under **System variables**, select **Path** → **Edit** → **New**
   - Paste `C:\msys64\ucrt64\bin` → click OK
4. **Restart your terminal or VS Code.**
5. Open a terminal in the project folder and run:

```bash
mingw32-make run
```

> Use **Windows Terminal** or **VS Code's integrated terminal** for best results.
> Classic `cmd.exe` may render box-drawing characters incorrectly.

## Build Commands

```bash
mingw32-make        # build trakkita.exe
mingw32-make run    # build and run
mingw32-make clean  # delete trakkita.exe
```

Direct GCC equivalent (run as one line):

```bash
gcc -Wall -Wextra -std=c11 -g -Iinclude -static -o trakkita.exe src/main.c src/utils.c src/route.c src/quicktap.c src/profit.c src/fuelConsump.c src/fileio.c src/breakeven.c
```

If the build cannot overwrite `trakkita.exe`, close the running program first.
Windows locks `.exe` files while they are running.

## Folder Structure

```text
trakkita/
|-- src/
|   |-- main.c          Entry point, intro animation, main menu loop
|   |-- utils.c         Terminal UI helpers (box drawing, input, colors)
|   |-- route.c         Route Efficiency Recommender
|   |-- profit.c        Daily Profit/Loss Calculator
|   |-- fuelConsump.c   Fuel Consumption Log (7-Day)
|   |-- quicktap.c      Quick-Tap Fare Pad
|   |-- breakeven.c     Fare-to-Fuel Break-Even Simulator
|   `-- fileio.c        File save/load for puv_records.txt
|-- include/
|   |-- structs.h       Shared data structures (DaySummary, RouteNode, etc.)
|   |-- utils.h         ANSI color macros and UI utility declarations
|   |-- route.h         Route module API and constants
|   |-- profit.h        Profit module declarations
|   |-- fuel.h          Fuel module declarations
|   |-- quicktap.h      Quick-Tap module declarations
|   |-- breakeven.h     Break-Even module declarations and structs
|   `-- fileio.h        File I/O declarations and constants
|-- Makefile            Build rules for mingw32-make
|-- setup.bat           Windows dependency setup helper
|-- README.md           Project guide
`-- CODEX.md            Technical reference
```

## Data Structures

Shared structures are defined in `include/structs.h`:

```c
struct DaySummary   { date, totalFares, fuelCost, boundaryFee, netProfit }
struct RouteNode    { routeID, name, distanceKm, trafficFactor,
                      baseFuelPerKm, avgFarePerTrip, tripsPerDay }
struct FuelEntry    { date, liters, pricePerLiter, totalCost }
struct QuickTapSession { date, tapCount, totalCollected }
```

Break-even structs are defined in `include/breakeven.h`:

```c
struct BreakevenInputs  { fuel_price, fuel_efficiency, route_distance,
                          trips_per_day, boundary_fee, maintenance,
                          driver_wages, other_costs, fare_per_pax, pax_per_trip }
struct BreakevenResults { fuel_cost, total_cost, total_passengers,
                          gross_revenue, net_profit, breakeven_pax,
                          breakeven_per_trip, coverage_pct }
```

## Key Formulas

**Route Efficiency (Feature 3)**

```text
fuel_cost  = distanceKm × fuelPerKm × trafficFactor × tripsPerDay × dieselPrice
gross      = avgFarePerTrip × tripsPerDay
net_income = gross − fuel_cost − boundaryFee
score      = net_income / fuel_cost
```

`fuelPerKm`, `dieselPrice`, and `boundaryFee` are set per session via the vehicle setup wizard.
Higher score = better route efficiency.

**Break-Even Simulator (Feature 4)**

```text
fuel_cost     = (routeDistance / fuelEfficiency) × fuelPrice
total_cost    = fuel_cost + boundaryFee + maintenance + driverWages + otherCosts
gross_revenue = totalPassengers × farePerPax
net_profit    = gross_revenue − total_cost
breakeven_pax = ceil(total_cost / farePerPax)
coverage      = (gross_revenue / total_cost) × 100%
```

## Troubleshooting

**`make` does not work**
Use `mingw32-make` instead.

**Garbled boxes or colors**
Use Windows Terminal or VS Code's integrated terminal. The program sets UTF-8 output on
Windows, but old terminals (e.g. classic `cmd.exe`) may still render box characters poorly.

**Build fails with linker error**
Close `trakkita.exe` if it is currently running, then build again.
