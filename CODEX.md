# CODEX.md - TrakKita AI Reference

Read this before editing the project.

## Project Overview

TrakKita is a terminal-based C11 program for Filipino PUV/jeepney drivers. It uses a colored terminal UI and box-drawn menus.

Important constraints:

- Build command is `mingw32-make`, not `make`.
- Routes use a plain stack array: `RouteNode routes[ROUTE_COUNT]`.
- Do not suggest linked lists for `RouteNode`.
- New feature modules follow the `route.h` / `route.c` pattern.
- Keep shared structs in `include/structs.h` only.

## Folder Structure

```text
trakkita/
|-- src/
|   |-- main.c       Entry point, intro animation, main menu loop
|   |-- utils.c      Terminal UI helpers and input helpers
|   `-- route.c      Route module implementation
|-- include/
|   |-- structs.h    Shared structs
|   |-- utils.h      ANSI macros and utility declarations
|   `-- route.h      Route module API and constants
|-- build/           Generated build artifacts if needed
|-- Makefile         mingw32-make build rules
|-- setup.bat        Windows setup helper
|-- README.md        User-facing guide
`-- CODEX.md         AI technical reference
```

## Build

```bash
mingw32-make
mingw32-make run
mingw32-make clean
```

Direct GCC equivalent:

```bash
gcc -Wall -Wextra -std=c11 -g -Iinclude -static -o trakkita.exe src/main.c src/utils.c src/route.c
```

The output executable is `trakkita.exe` in the project root. If linking fails because the file cannot be opened, close the running program first.

## Makefile Rules

Current source list:

```makefile
SRCS = $(SRCDIR)/main.c \
       $(SRCDIR)/utils.c \
       $(SRCDIR)/route.c
```

To add a module, add its `.c` file under `src/` and append it to `SRCS`.

## Data Structures

Shared structs live only in `include/structs.h`.

```c
typedef struct RouteNode {
    int   routeID;
    char  name[50];
    float distanceKm;
    float trafficFactor;
    float baseFuelPerKm;
    float avgFarePerTrip;
    int   tripsPerDay;
} RouteNode;
```

No linked lists. `RouteNode` is used as a plain array.

## Route Module

Files:

- `include/route.h`
- `src/route.c`

Constants:

```c
#define DIESEL_PRICE_PER_LITER  65.00f
#define BASE_FUEL_PER_KM         0.08f
#define ROUTE_COUNT              8
```

API:

```c
void route_init(RouteNode routes[], int count);
void route_display_all(RouteNode routes[], int count);
void route_display_detail(RouteNode *node);
void route_display_recommendation(RouteNode routes[], int count);
float route_estimated_fuel_cost(RouteNode *node);
float route_estimated_net_income(RouteNode *node);
float route_efficiency_score(RouteNode *node);
RouteNode *route_find_by_id(RouteNode routes[], int count, int id);
RouteNode *route_find_best(RouteNode routes[], int count);
void route_menu(RouteNode routes[], int count);
```

Formula:

```text
fuel_cost  = distanceKm * baseFuelPerKm * trafficFactor * tripsPerDay * DIESEL_PRICE_PER_LITER
gross      = avgFarePerTrip * tripsPerDay
net_income = gross - fuel_cost
score      = net_income / fuel_cost
```

## UI Utilities

Files:

- `include/utils.h`
- `src/utils.c`

Use these helpers for boxed UI:

```c
print_top_border();
print_divider();
print_blank_row();
print_row("text");
print_bottom_border();
```

`print_row()` is ANSI-aware and can safely receive colored text. Box characters are emitted from `src/utils.c` using explicit UTF-8 byte escapes to avoid mojibake.

`clear_screen()` uses:

```c
"\033[2J\033[3J\033[H"
```

This clears the visible screen, clears scrollback where supported, and moves the cursor home.

## Main Program Flow

`src/main.c`:

1. Sets Windows console input/output to UTF-8.
2. Plays intro animation.
3. Creates `RouteNode routes[ROUTE_COUNT]`.
4. Calls `route_init(routes, ROUTE_COUNT)`.
5. Runs the main menu loop.
6. Dispatches menu option 3 to `route_menu(routes, ROUTE_COUNT)`.

## Current Feature Status

| Menu # | Feature | Files | Status |
|--------|---------|-------|--------|
| 1 | Daily Profit / Loss Calculator | planned `profit.h` / `profit.c` | Stub |
| 2 | Fuel Consumption Log (7-Day) | planned `fuel.h` / `fuel.c` | Stub |
| 3 | Route Efficiency Recommender | `route.h` / `route.c` | Complete |
| 4 | Fare-to-Fuel Break-Even | planned `breakeven.h` / `breakeven.c` | Stub |
| 5 | Quick-Tap Fare Pad | planned `quicktap.h` / `quicktap.c` | Stub |
