# TrakKita - PUV Daily Operations & Earnings Tracker

TrakKita is a terminal-based C program for Filipino PUV/jeepney drivers. It tracks daily operations, estimates fuel cost and income, and recommends efficient routes.

Built for CS103 - Computer Programming 2, Group 4.

## Features

| # | Feature | Status |
|---|---------|--------|
| 1 | Daily Profit / Loss Calculator | Implemented |
| 2 | Fuel Consumption Log (7-Day) | Implemented |
| 3 | Route Efficiency Recommender | Implemented |
| 4 | Fare-to-Fuel Break-Even Simulator | Implemented |
| 5 | Quick-Tap Fare Pad | Implemented |

Feature 3 uses a plain array of `RouteNode` values. It does not use linked lists.

## Requirements

Install MSYS2 first from:

```text
https://www.msys2.org
```

Then install GCC and Make from the MSYS2 UCRT64 terminal:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make
```

Make sure this folder is in your Windows PATH:

```text
C:\msys64\ucrt64\bin
```

This project uses `mingw32-make`, not `make`.

## Build And Run

From the project root:

```bash
mingw32-make
mingw32-make run
mingw32-make clean
```

Direct GCC equivalent:

```bash
gcc -Wall -Wextra -std=c11 -g -Iinclude -static -o trakkita.exe src/main.c src/utils.c src/route.c
```

If the build cannot overwrite `trakkita.exe`, close the running TrakKita window first. Windows locks `.exe` files while they are running.

## Folder Structure

```text
trakkita/
|-- src/
|   |-- main.c       Entry point, intro animation, main menu loop
|   |-- utils.c      Terminal UI helpers and input helpers
|   `-- route.c      Route seed data, computations, display, submenu
|-- include/
|   |-- structs.h    Shared structs only
|   |-- utils.h      ANSI macros and utility declarations
|   `-- route.h      Route module declarations and constants
|-- build/           Reserved for generated build artifacts
|-- Makefile         Build rules for mingw32-make
|-- setup.bat        Windows dependency setup helper
|-- README.md        Human-readable project guide
`-- CODEX.md         Technical reference for AI assistants
```

## Module Rules

- Put implementation files in `src/`.
- Put public headers in `include/`.
- Keep shared structs in `include/structs.h` only.
- Use one `.h` / `.c` pair per feature module.
- New feature modules should follow the `route.h` / `route.c` pattern.
- Module entry points should be named `<module>_menu(...)`.
- Do not use linked lists for routes. `RouteNode` is stored as `RouteNode routes[ROUTE_COUNT]` in `main()`.

## Adding A New Module

Example for `profit`:

1. Create `include/profit.h`.
2. Create `src/profit.c`.
3. Add `$(SRCDIR)/profit.c` to `SRCS` in `Makefile`.
4. Include it in `src/main.c`:

```c
#include "profit.h"
```

5. Call `profit_menu(...)` or the module's entry function from the main menu switch.

## Route Formula

```text
fuel_cost  = distanceKm * baseFuelPerKm * trafficFactor * tripsPerDay * DIESEL_PRICE_PER_LITER
gross      = avgFarePerTrip * tripsPerDay
net_income = gross - fuel_cost
score      = net_income / fuel_cost
```

Higher score means better route efficiency.

## Troubleshooting

### `make` does not work

Use:

```bash
mingw32-make
```

### Garbled boxes or colors

Use Windows Terminal or VS Code's integrated terminal. The program sets UTF-8 output on Windows, but old terminals may still render poorly.

### Build fails with linker error

Close `trakkita.exe` if it is currently running, then build again.
