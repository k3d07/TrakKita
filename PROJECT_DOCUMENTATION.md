# TrakKita — Project Documentation
**CS103 - Computer Programming 2 | Group 4**

| Member | Feature |
|--------|---------|
| Kim Eidrich B. Custodio | Route Efficiency Recommender |
| Cyril Gian T. Anonuevo | Fare-to-Fuel Break-Even Simulator |
| Keith Bryan O. Belmonte | Fuel Consumption Log |
| Kile Nicandro A. Roxas | Quick-Tap Fare Pad |
| Clarence James D. Gelua | Daily Profit / Loss Calculator |

---

## I. Introduction

### Purpose of the Program

TrakKita is a terminal-based C program designed for Filipino Public Utility Vehicle (PUV)
drivers and operators. Many jeepney and tricycle drivers rely on rough mental estimates to
track their daily earnings, fuel expenses, and route performance, which often leads to poor
financial decisions and unnoticed losses. TrakKita addresses this by consolidating income
tracking, fuel logging, route analysis, break-even simulation, and fare counting into one
offline, accessible system.

The program runs entirely in the terminal and requires no internet connection, making it
suitable for use on any Windows computer — including low-cost devices commonly available
to drivers and dispatchers.

### Intended Users

- **Jeepney and tricycle drivers** — to track daily income and expenses and identify their
  most profitable routes.
- **Small-scale PUV operators** — to monitor fuel consumption trends and boundary fee
  impact on net income.
- **Transportation cooperative members and dispatchers** (e.g., BITCoop) — to support
  informed decision-making for their fleets.

### Features and Functionalities

| # | Feature | Description |
|---|---------|-------------|
| 1 | Daily Profit / Loss Calculator | Records daily fares, fuel cost, and boundary fee. Computes net profit or loss and warns the driver if operating at a loss. Maintains a history of past daily entries. |
| 2 | Fuel Consumption Log (7-Day) | Logs daily fuel fill-ups (liters and price per liter). Displays a 7-day history and a summary with averages, most expensive, and cheapest day. |
| 3 | Route Efficiency Recommender | Compares 8 local PUV routes by estimated net income. Includes a setup wizard for vehicle type, ownership status, and diesel price. Scores and recommends the best route for the day. |
| 4 | Fare-to-Fuel Break-Even Simulator | Simulates the daily financial situation across 6 PUV types or custom inputs. Computes fuel cost, gross revenue, net profit/loss, break-even passenger count, and cost coverage percentage. |
| 5 | Quick-Tap Fare Pad | A fast fare counting pad for drivers mid-route. Supports regular (₱13.00) and discounted (₱11.00) fares with an undo function and a live running total. |

---

## II. Algorithm

### Overall Program Flow (Pseudocode)

```
START PROGRAM
  Set console to UTF-8 (Windows)
  Play intro animation (jeepney drives in, title appears)
  Initialize route array from seed data
  Load saved records from puv_records.txt

  DISPLAY main menu
  WHILE user has not chosen Exit (option 6):

    IF option 1: Daily Profit/Loss Calculator
      Display sub-menu (Calculate Today's P/L / View History)
      IF Calculate:
        Input totalFares, fuelCost, boundaryFee
        Compute netProfit = totalFares - fuelCost - boundaryFee
        IF netProfit < 0 → display WARNING: OPERATING AT A LOSS
        ELSE → display net profit
        Store entry in DaySummary array
      IF View History:
        Display all stored DaySummary entries in a table

    IF option 2: Fuel Consumption Log
      Display sub-menu (Add Entry / View Log / View Summary)
      IF Add Entry:
        Input liters and pricePerLiter
        Compute totalCost = liters × pricePerLiter
        Store entry in FuelEntry array
      IF View Log:
        Display all stored FuelEntry records in a table
      IF View Summary:
        Compute and display total liters, average daily cost,
        most expensive day, and cheapest day

    IF option 3: Route Efficiency Recommender
      IF first visit → run vehicle setup wizard (3 steps):
        Step 1: Select PUV type → set fuel use (L/km)
        Step 2: Select ownership → set boundary fee (0 or entered)
        Step 3: Enter today's diesel price
      Display sub-menu (View All / View Detail / Recommend / Update Settings)
      For each route:
        Compute fuel_cost = distanceKm × fuelPerKm × trafficFactor
                            × tripsPerDay × dieselPrice
        Compute net_income = (avgFarePerTrip × tripsPerDay)
                             - fuel_cost - boundaryFee
        Compute score = net_income / fuel_cost
      RECOMMEND route with highest score

    IF option 4: Fare-to-Fuel Break-Even Simulator
      Display scenario menu (6 presets or custom input)
      Compute:
        fuel_cost     = (routeDistance / fuelEfficiency) × fuelPrice
        total_cost    = fuel_cost + boundaryFee + maintenance
                        + driverWages + otherCosts
        gross_revenue = totalPassengers × farePerPax
        net_profit    = gross_revenue - total_cost
        breakeven_pax = ceil(total_cost / farePerPax)
        coverage      = (gross_revenue / total_cost) × 100%
      Display results with PROFITABLE / AT A LOSS / BREAK-EVEN status

    IF option 5: Quick-Tap Fare Pad
      Initialize session (date, counts, total = 0)
      WHILE driver is tapping:
        Key 1 → add ₱13.00 (regular), push to undo stack
        Key 2 → add ₱11.00 (discounted), push to undo stack
        Key 3 → undo last tap (pop from stack, subtract from total)
        Key 0 → exit pad
      Display session summary (total taps, total collected)

    IF option 6: Save and Exit
      Write all DaySummary and FuelEntry records to puv_records.txt
      Display farewell message
      Exit program

END PROGRAM
```

---

## III. Source Code Summary

### A. Standard C Functions Used

| Function | Header File | Purpose in the Program |
|----------|------------|------------------------|
| `printf()` | `<stdio.h>` | Outputs text and formatted data to the terminal |
| `scanf()` | `<stdio.h>` | Reads user input from the terminal |
| `snprintf()` | `<stdio.h>` | Formats a string into a buffer for safe box-row printing |
| `getchar()` | `<stdio.h>` | Reads a single character; used in `pause_prompt()` and `flush_input()` |
| `putchar()` | `<stdio.h>` | Writes a single character; used for padding in UI box rows |
| `fopen()` | `<stdio.h>` | Opens `puv_records.txt` for reading or writing |
| `fclose()` | `<stdio.h>` | Closes the file after reading or writing |
| `fprintf()` | `<stdio.h>` | Writes formatted records to `puv_records.txt` |
| `fgets()` | `<stdio.h>` | Reads lines from `puv_records.txt` during load |
| `sscanf()` | `<stdio.h>` | Parses data fields from lines read out of the file |
| `fflush()` | `<stdio.h>` | Flushes the stdout buffer after animation frames |
| `strncpy()` | `<string.h>` | Safely copies route names into `RouteNode.name` with length limit |
| `strlen()` | `<string.h>` | Measures string length; used in the intro animation centering |
| `strcmp()` | `<string.h>` | Compares section tags (`[DAYS]`, `[FUEL]`) when loading the file |
| `strcat()` | `<string.h>` | Builds the score bar string from block characters |
| `memcpy()` | `<string.h>` | Copies 3-byte UTF-8 block characters into the score bar buffer |
| `memset()` | `<string.h>` | Zeroes the `QuickTapSession` struct at the start of a session |
| `strftime()` | `<time.h>` | Formats the current date as `YYYY-MM-DD` |
| `time()` | `<time.h>` | Gets the current Unix timestamp |
| `localtime()` | `<time.h>` | Converts Unix timestamp to a local `tm` struct |
| `nanosleep()` | `<time.h>` | Pauses execution for a given number of milliseconds (animations) |
| `ceilf()` | `<math.h>` | Rounds up the break-even passenger count to the nearest whole number |
| `SetConsoleOutputCP()` | `<windows.h>` | Sets the Windows console output to UTF-8 (code page 65001) |
| `SetConsoleCP()` | `<windows.h>` | Sets the Windows console input to UTF-8 |

---

### B. User-Defined Functions

---

#### `main.c`

---

**`print_indent`**
- **Purpose:** Prints a given number of space characters to indent output horizontally.
- **Parameters:** `int spaces` — number of spaces to print.
- **Return value:** `void`
- **Local variables:** `int i` — loop counter.
- **Operations:** Loops from 0 to `spaces`, calling `putchar(' ')` each iteration.

---

**`print_centered_text`**
- **Purpose:** Prints a string centered within `INTRO_WIDTH` (78 characters).
- **Parameters:** `const char *text` — the string to center.
- **Return value:** `void`
- **Local variables:** `int width` — length of text; `int offset` — left padding needed.
- **Operations:** Computes `offset = (INTRO_WIDTH - width) / 2`, calls `print_indent(offset)`, then prints the text.

---

**`print_intro_title`**
- **Purpose:** Prints the TrakKita ASCII art title and subtitle to the screen.
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** `int title_lines` — number of lines in the title array.
- **Operations:** Iterates over the `TITLE[]` array, printing each line with `FG_CYAN BOLD` color. Prints the subtitle lines in `DIM FG_WHITE`.

---

**`print_clipped_line`**
- **Purpose:** Prints a line of the jeepney ASCII art clipped to screen boundaries during the drive-in animation.
- **Parameters:** `const char *line` — one line of the jeepney art; `int offset` — horizontal position.
- **Return value:** `void`
- **Local variables:** `int len`, `int start`, `int visible` — used to compute the visible slice of the string.
- **Operations:** Calculates which portion of the line is visible given the current offset and screen width, then prints only that portion.

---

**`print_jeep_at`**
- **Purpose:** Prints all 8 lines of the jeepney ASCII art at a given horizontal offset with colors.
- **Parameters:** `int offset` — horizontal screen position of the left edge of the jeepney.
- **Return value:** `void`
- **Local variables:** `int i` — loop counter.
- **Operations:** Loops through `JEEP[]`, applies color codes per row (red for roof, yellow for windows, white for wheels), and calls `print_clipped_line()` for each line.

---

**`print_road`**
- **Purpose:** Prints a horizontal road line below the jeepney animation.
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** `int i` — loop counter.
- **Operations:** Prints `INTRO_WIDTH - 4` equal signs in `DIM FG_WHITE`.

---

**`animate_intro`**
- **Purpose:** Runs the full intro animation — jeepney drives in from the left, brakes, and the title fades in.
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** `int target` — center position; `int x` — animation frame position.
- **Operations:** Phase 1: drives jeepney from off-screen to center. Phase 2: slight reverse for brake effect. Phase 3: title appears above jeepney. Phase 4: two-blink BEEP BEEP horn effect.

---

**`print_main_menu`**
- **Purpose:** Clears the screen and renders the main menu box with all 5 features and the exit option.
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** None.
- **Operations:** Calls `clear_screen()`, then uses the box UI functions (`print_top_border`, `print_row`, `print_divider`, `print_bottom_border`) to draw the menu.

---

**`save_and_exit`**
- **Purpose:** Saves all session records to file and displays the farewell message before the program ends.
- **Parameters:** `DaySummary days[]`, `int day_count`, `FuelEntry fuel[]`, `int fuel_count`.
- **Return value:** `void`
- **Local variables:** None.
- **Operations:** Calls `fileio_save()`, sleeps briefly, then displays a farewell message in cyan.

---

**`main`**
- **Purpose:** Program entry point. Initializes all data, runs the intro, and manages the main menu loop.
- **Parameters:** None.
- **Return value:** `int` — returns 0 on normal exit.
- **Local variables:** `RouteNode routes[ROUTE_COUNT]`, `DaySummary day_summaries[MAX_DAY_SUMMARIES]`, `int day_count`, `FuelEntry fuel_entries[MAX_FUEL_ENTRIES]`, `int fuel_count`, `int choice`.
- **Operations:** Sets console to UTF-8, plays intro, initializes route array, loads saved records, then runs the main do-while loop dispatching menu choices to each module's entry function.

---

#### `utils.c`

---

**`sleep_ms`**
- **Purpose:** Pauses program execution for a specified number of milliseconds.
- **Parameters:** `int ms` — duration in milliseconds.
- **Return value:** `void`
- **Local variables:** `struct timespec ts` — holds seconds and nanoseconds.
- **Operations:** Converts `ms` to a `timespec` struct and calls `nanosleep()`.

---

**`clear_screen`**
- **Purpose:** Clears the terminal screen and moves the cursor to the top-left.
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** None.
- **Operations:** Prints the ANSI escape sequence `\033[2J\033[3J\033[H` and flushes stdout.

---

**`print_top_border`**
- **Purpose:** Prints the top border line of a UI box (╔══...══╗).
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** None.
- **Operations:** Calls the internal `print_horizontal_border()` with the top-left and top-right corner characters.

---

**`print_bottom_border`**
- **Purpose:** Prints the bottom border line of a UI box (╚══...══╝).
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** None.
- **Operations:** Calls `print_horizontal_border()` with the bottom-left and bottom-right corner characters.

---

**`print_divider`**
- **Purpose:** Prints a mid-box horizontal divider line (╠══...══╣).
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** None.
- **Operations:** Calls `print_horizontal_border()` with the left-tee and right-tee characters.

---

**`print_blank_row`**
- **Purpose:** Prints an empty row inside the box (║ spaces ║).
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** `int i` — loop counter.
- **Operations:** Prints the left border character, loops to fill `BOX_WIDTH` spaces, then prints the right border character.

---

**`visible_length`**
- **Purpose:** Returns the visible character count of a string, correctly ignoring ANSI escape codes and multi-byte UTF-8 continuation bytes.
- **Parameters:** `const char *text` — the string to measure.
- **Return value:** `int` — number of visible characters.
- **Local variables:** `int length`, `int i`, `unsigned char ch`.
- **Operations:** Iterates through the string; skips ANSI sequences (starting with `\033[`) and UTF-8 continuation bytes (`0x80`–`0xBF`); increments `length` for all other characters.

---

**`print_row`**
- **Purpose:** Prints a content string inside the box, padded to align the right border correctly even when ANSI color codes are present.
- **Parameters:** `const char *content` — the text to display inside the row.
- **Return value:** `void`
- **Local variables:** `int content_width`, `int printed_width`, `int padding`.
- **Operations:** Computes visible length via `visible_length()`, calculates required padding, prints the left border, the content, the padding spaces, and the right border.

---

**`pause_prompt`**
- **Purpose:** Displays "Press ENTER to continue..." and waits for the user to press Enter.
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** None.
- **Operations:** Calls `flush_input()` to clear any leftover input, then calls `getchar()` to wait.

---

**`flush_input`**
- **Purpose:** Discards all remaining characters in the input buffer up to and including the next newline.
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** `int c` — character read from buffer.
- **Operations:** Loops calling `getchar()` until `'\n'` or `EOF` is reached.

---

**`get_int_input`**
- **Purpose:** Displays a prompt and reads a validated integer from the user.
- **Parameters:** `const char *prompt` — the prompt string to display.
- **Return value:** `int` — the validated integer value.
- **Local variables:** `int val`.
- **Operations:** Prints the prompt, calls `scanf("%d", &val)`, and loops with an error message until a valid integer is entered. Calls `flush_input()` after successful read.

---

**`get_float_input`**
- **Purpose:** Displays a prompt and reads a validated float from the user.
- **Parameters:** `const char *prompt` — the prompt string to display.
- **Return value:** `float` — the validated float value.
- **Local variables:** `float val`.
- **Operations:** Prints the prompt, calls `scanf("%f", &val)`, and loops until valid input is entered.

---

#### `route.c`

---

**`route_init`**
- **Purpose:** Populates the `RouteNode` array with data from the internal `ROUTE_SEEDS` array.
- **Parameters:** `RouteNode routes[]` — the array to fill; `int count` — number of routes.
- **Return value:** `void`
- **Local variables:** `int i` — loop counter.
- **Operations:** Loops through each seed, copies fields including safe `strncpy` for the name, and sets `baseFuelPerKm` to the default constant.

---

**`route_estimated_fuel_cost`**
- **Purpose:** Computes the estimated daily fuel cost for a route using the driver's current settings.
- **Parameters:** `RouteNode *node` — pointer to the route.
- **Return value:** `float` — total fuel cost in PHP.
- **Local variables:** `float liters` — liters consumed per day.
- **Operations:** `liters = distanceKm × g_fuel_per_km × trafficFactor`; returns `liters × tripsPerDay × g_diesel_price`.

---

**`route_estimated_net_income`**
- **Purpose:** Computes estimated daily net income for a route after fuel and boundary costs.
- **Parameters:** `RouteNode *node` — pointer to the route.
- **Return value:** `float` — net income in PHP (can be negative).
- **Local variables:** `float gross`.
- **Operations:** `gross = avgFarePerTrip × tripsPerDay`; returns `gross - fuel_cost - g_boundary_fee`.

---

**`route_efficiency_score`**
- **Purpose:** Computes the efficiency score of a route (net income per peso spent on fuel).
- **Parameters:** `RouteNode *node` — pointer to the route.
- **Return value:** `float` — score value; higher is better.
- **Local variables:** `float fuel`.
- **Operations:** Returns `net_income / fuel_cost`. Returns 0 if fuel cost is zero or negative.

---

**`route_find_by_id`**
- **Purpose:** Searches the route array for a route matching a given ID.
- **Parameters:** `RouteNode routes[]`, `int count`, `int id`.
- **Return value:** `RouteNode *` — pointer to the matching route, or `NULL` if not found.
- **Local variables:** `int i` — loop counter.
- **Operations:** Linear search through the array comparing `routeID`.

---

**`route_find_best`**
- **Purpose:** Finds the route with the highest efficiency score.
- **Parameters:** `RouteNode routes[]`, `int count`.
- **Return value:** `RouteNode *` — pointer to the best route.
- **Local variables:** `RouteNode *best`, `float top`, `float score`, `int i`.
- **Operations:** Iterates all routes, calling `route_efficiency_score()` for each and tracking the highest scorer.

---

**`traffic_label`**
- **Purpose:** Returns a human-readable traffic description for a given traffic factor.
- **Parameters:** `float factor` — the route's traffic multiplier.
- **Return value:** `const char *` — "Light", "Moderate", "Heavy", or "Very Heavy".
- **Local variables:** None.
- **Operations:** Series of threshold comparisons against the factor value.

---

**`setup_header`**
- **Purpose:** Clears the screen and prints the vehicle setup wizard header with the current step label.
- **Parameters:** `int step` — step number (1–3); `const char *desc` — step description.
- **Return value:** `void`
- **Local variables:** `char buf[128]`.
- **Operations:** Calls `clear_screen()`, draws the box header using `print_top_border()` / `print_row()`, then formats and prints the step label.

---

**`route_setup`**
- **Purpose:** Runs the 3-step vehicle setup wizard to set fuel efficiency, boundary fee, and diesel price for the session.
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** `int type`, `float manual`, `int own`, `float fee`, `float price`, `char buf[96]`.
- **Operations:** Step 1 — selects PUV type preset or manual L/km entry and stores to `g_fuel_per_km`. Step 2 — selects owner-driver (boundary = 0) or renter (enter fee) and stores to `g_boundary_fee`. Step 3 — enters diesel price into `g_diesel_price`. Sets `g_setup_done = 1` and displays a confirmation summary.

---

**`route_display_all`**
- **Purpose:** Displays a table of all routes with their distance and estimated net income per day.
- **Parameters:** `RouteNode routes[]`, `int count`.
- **Return value:** `void`
- **Local variables:** `char row[96]`, `float net`, `const char *col`, `int i`.
- **Operations:** Loops through all routes, computing net income for each and formatting a colored row (green if profitable, red if loss).

---

**`route_display_detail`**
- **Purpose:** Displays a full detail card for one route including all computed financials and a block-character score bar.
- **Parameters:** `RouteNode *node` — pointer to the route to display.
- **Return value:** `void`
- **Local variables:** `float fuel_cost`, `float gross`, `float net`, `float score`, `char row[256]`, `char bar[128]`, `char *p`, `int bar_len`, `int i`.
- **Operations:** Computes all financial values, builds a 20-block UTF-8 score bar using `█` (filled) and `░` (empty) characters, then prints the full detail box.

---

**`route_display_recommendation`**
- **Purpose:** Finds the best route and displays it with an explanatory tip message.
- **Parameters:** `RouteNode routes[]`, `int count`.
- **Return value:** `void`
- **Local variables:** `RouteNode *best`, `float net`.
- **Operations:** Calls `route_find_best()`, then `route_display_detail()` on the result. Prints a tip below the box with the projected daily take-home amount.

---

**`route_menu`**
- **Purpose:** Entry point for Feature 3. Runs the setup wizard on first visit, then manages the sub-menu loop.
- **Parameters:** `RouteNode routes[]`, `int count`.
- **Return value:** `void`
- **Local variables:** `int choice`, `int id`, `RouteNode *node`.
- **Operations:** Checks `g_setup_done`; if 0, calls `route_setup()`. Then loops the sub-menu, dispatching to view all, view detail, recommend, or update settings functions.

---

#### `profit.c`

---

**`get_today`** *(profit.c)*
- **Purpose:** Gets the current date as a formatted string.
- **Parameters:** `char *buf` — destination buffer; `int size` — buffer size.
- **Return value:** `void`
- **Local variables:** `time_t t`, `struct tm *tm_info`.
- **Operations:** Calls `time()`, `localtime()`, and `strftime()` with format `"%Y-%m-%d"`.

---

**`profit_calculate`**
- **Purpose:** Collects today's financial inputs from the user and computes and stores the net profit or loss.
- **Parameters:** `DaySummary days[]` — the history array; `int *count` — pointer to current entry count.
- **Return value:** `void`
- **Local variables:** `DaySummary *d`, `char buf[96]`, `int is_loss`.
- **Operations:** If the array is full, shifts all entries left to make room (rolling window). Gets today's date, prompts for fares, fuel cost, and boundary fee, computes `netProfit = totalFares - fuelCost - boundaryFee`, stores the entry, and displays the result with a loss warning if applicable.

---

**`profit_display_history`**
- **Purpose:** Displays all stored daily profit/loss entries in a formatted table.
- **Parameters:** `DaySummary days[]`, `int count`.
- **Return value:** `void`
- **Local variables:** `char row[96]`, `const char *net_color`, `int i`.
- **Operations:** If count is 0, shows an empty state message. Otherwise loops through all entries, coloring the net value green (profit) or red (loss).

---

**`profit_menu`**
- **Purpose:** Entry point for Feature 1. Manages the sub-menu for calculating today's P/L and viewing history.
- **Parameters:** `DaySummary days[]`, `int *count`.
- **Return value:** `void`
- **Local variables:** `int choice`.
- **Operations:** Loops the sub-menu, dispatching to `profit_calculate()` or `profit_display_history()`.

---

#### `fuelConsump.c`

---

**`get_today`** *(fuelConsump.c)*
- **Purpose:** Gets the current date as a formatted `YYYY-MM-DD` string.
- **Parameters:** `char *buf`, `int size`.
- **Return value:** `void`
- **Local variables:** `time_t t`, `struct tm *tm_info`.
- **Operations:** Same as profit.c — calls `time()`, `localtime()`, `strftime()`.

---

**`fuel_add`**
- **Purpose:** Records a new fuel fill-up entry with today's date, liters filled, and price per liter.
- **Parameters:** `FuelEntry entries[]`, `int *count`.
- **Return value:** `void`
- **Local variables:** `FuelEntry *e`, `char buf[96]`.
- **Operations:** Shifts array if full (rolling window). Gets today's date, prompts for liters and price, computes `totalCost = liters × pricePerLiter`, stores the entry, and displays a confirmation.

---

**`fuel_display_log`**
- **Purpose:** Displays all stored fuel entries in a formatted 7-day log table.
- **Parameters:** `FuelEntry entries[]`, `int count`.
- **Return value:** `void`
- **Local variables:** `char row[96]`, `int i`.
- **Operations:** Shows an empty-state message if no entries exist. Otherwise loops through all entries and formats each row with date, liters, price per liter, and total cost.

---

**`fuel_display_summary`**
- **Purpose:** Computes and displays summary statistics for all fuel log entries.
- **Parameters:** `FuelEntry entries[]`, `int count`.
- **Return value:** `void`
- **Local variables:** `float total_liters`, `float total_cost`, `float avg_liters`, `float avg_cost`, `int max_i`, `int min_i`, `char buf[96]`, `int i`.
- **Operations:** Iterates all entries to accumulate totals and find the most expensive and cheapest days. Displays averages and highlights both extremes with colors.

---

**`fuel_menu`**
- **Purpose:** Entry point for Feature 2. Manages the sub-menu for adding entries, viewing the log, and viewing the summary.
- **Parameters:** `FuelEntry entries[]`, `int *count`.
- **Return value:** `void`
- **Local variables:** `int choice`.
- **Operations:** Loops the sub-menu, dispatching to `fuel_add()`, `fuel_display_log()`, or `fuel_display_summary()`.

---

#### `quicktap.c`

---

**`get_today`** *(quicktap.c)*
- **Purpose:** Gets the current date as a `YYYY-MM-DD` string.
- **Parameters:** `char *buf`, `int size`.
- **Return value:** `void`
- **Local variables:** `time_t t`, `struct tm *tm_info`.
- **Operations:** Calls `time()`, `localtime()`, and `strftime()`.

---

**`print_tap_screen`**
- **Purpose:** Renders the live fare pad dashboard showing tap options, current counts, last action, and running total.
- **Parameters:** `int reg` — regular tap count; `int disc` — discounted tap count; `float total` — total collected; `float last` — value of last tap (negative if undo).
- **Return value:** `void`
- **Local variables:** `char buf[96]`.
- **Operations:** Clears screen, draws the box UI with all options and live counters. Colors the last-action line green for a tap and red for an undo.

---

**`run_quicktap`**
- **Purpose:** Entry point for Feature 5. Runs the interactive fare pad loop with undo support and displays a session summary on exit.
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** `QuickTapSession session`, `int reg_count`, `int disc_count`, `float total`, `float last`, `float tap_history[MAX_TAPS]`, `int tap_top`, `int choice`, `char buf[96]`.
- **Operations:** Initializes a `QuickTapSession` and an undo stack array. Loops calling `print_tap_screen()` and `get_int_input()`. Key 1 adds ₱13.00 and pushes to the stack; key 2 adds ₱11.00 and pushes; key 3 pops the stack and subtracts; key 0 exits. On exit, displays a session summary.

---

#### `breakeven.c`

---

**`breakeven_calculate`**
- **Purpose:** Computes all break-even metrics from a set of inputs.
- **Parameters:** `const BreakevenInputs *in` — pointer to the input struct.
- **Return value:** `BreakevenResults` — struct containing all computed values.
- **Local variables:** `BreakevenResults r`.
- **Operations:** Validates that fuel efficiency and fare are > 0. Computes fuel cost, total cost, total passengers, gross revenue, net profit, break-even passenger count (`ceilf(total_cost / fare_per_pax)`), break-even per trip, and coverage percentage.

---

**`breakeven_scenario_*`** *(six functions)*
- **Purpose:** Each returns a pre-filled `BreakevenInputs` struct representing a specific PUV type and scenario (traditional jeepney pre-hike, during-hike, modern jeepney, UV express, provincial bus, tricycle).
- **Parameters:** None.
- **Return value:** `BreakevenInputs`.
- **Local variables:** `BreakevenInputs in`.
- **Operations:** Initializes the struct with realistic preset values for that PUV type and returns it.

---

**`breakeven_get_inputs`**
- **Purpose:** Prompts the user to enter all 10 custom input values for the simulator.
- **Parameters:** None.
- **Return value:** `BreakevenInputs`.
- **Local variables:** `BreakevenInputs in`.
- **Operations:** Calls `get_float_input()` and `get_int_input()` for each field (fuel price, efficiency, distance, trips, boundary, maintenance, wages, other costs, fare, passengers).

---

**`breakeven_print_results`**
- **Purpose:** Displays the full simulation results box with colored net profit/loss and a status verdict.
- **Parameters:** `const BreakevenResults *r` — pointer to the results struct.
- **Return value:** `void`
- **Local variables:** `char buf[128]`.
- **Operations:** Clears screen, draws results box. Colors the net profit line green (profit) or red (loss). Shows `STATUS: PROFITABLE`, `STATUS: AT A LOSS`, or `STATUS: BREAK-EVEN` in the appropriate color.

---

**`breakeven_print_menu`**
- **Purpose:** Renders the break-even simulator menu with all 6 scenario options and the custom input option.
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** None.
- **Operations:** Calls box UI functions to draw the menu.

---

**`breakeven_menu`**
- **Purpose:** Entry point for Feature 4. Loops the simulator menu and dispatches to scenario presets or custom input.
- **Parameters:** None.
- **Return value:** `void`
- **Local variables:** `int choice`, `BreakevenInputs in`, `BreakevenResults r`.
- **Operations:** Calls `breakeven_print_menu()`, gets choice, loads the appropriate preset or calls `breakeven_get_inputs()`, calls `breakeven_calculate()`, displays results with `breakeven_print_results()`, and calls `pause_prompt()`.

---

#### `fileio.c`

---

**`fileio_save`**
- **Purpose:** Writes all daily earnings and fuel log records to `puv_records.txt` in a structured plain-text format.
- **Parameters:** `DaySummary days[]`, `int day_count`, `FuelEntry fuel[]`, `int fuel_count`.
- **Return value:** `void`
- **Local variables:** `FILE *fp`, `time_t t`, `struct tm *tm_info`, `char date_str[20]`, `int i`.
- **Operations:** Opens the file for writing. Writes a header with the save date. Writes a `[DAYS]` section with a count line and a formatted table of all `DaySummary` entries. Writes a `[FUEL]` section with the same pattern for all `FuelEntry` records. Closes the file.

---

**`fileio_load`**
- **Purpose:** Reads previously saved records from `puv_records.txt` and restores them into the in-memory arrays.
- **Parameters:** `DaySummary days[]`, `int *day_count`, `FuelEntry fuel[]`, `int *fuel_count`.
- **Return value:** `void`
- **Local variables:** `FILE *fp`, `char line[256]`, `int section`, `int remaining`.
- **Operations:** Opens the file for reading. Reads line by line, detecting `[DAYS]` and `[FUEL]` section tags to switch parsing mode. Uses `sscanf()` to parse each data row. Silently returns if the file does not exist (first run).

---

## IV. User Manual

### Step 1 — Installation

1. Install MSYS2 from `https://www.msys2.org` (use the default path `C:\msys64`).
2. Double-click `setup.bat` in the TrakKita project folder.
   - It will install GCC and `mingw32-make` automatically.
   - It will add `C:\msys64\ucrt64\bin` to your system PATH.
3. **Restart your terminal or VS Code** after `setup.bat` finishes.

### Step 2 — Building and Running

Open a terminal in the project folder and run:

```
mingw32-make run
```

The program will compile and launch automatically.

> Use **Windows Terminal** or **VS Code's integrated terminal** for correct box-drawing and colors.

---

### Step 3 — Using the Program

When the program starts, a jeepney animation plays and the main menu appears:

```
╔══════════════════════════════════════════════════════╗
║       PUV Daily Operations & Earnings Tracker        ║
║             CS103  |  Group 4  |  v1.0.0             ║
╠══════════════════════════════════════════════════════╣
║  [1]  Daily Profit / Loss Calculator                 ║
║  [2]  Fuel Consumption Log (7-Day)                   ║
║  [3]  Route Efficiency Recommender                   ║
║  [4]  Fare-to-Fuel Break-Even Simulator              ║
║  [5]  Quick-Tap Fare Pad                             ║
╠══════════════════════════════════════════════════════╣
║  [6]  Save and Exit                                  ║
╚══════════════════════════════════════════════════════╝
  >> Enter your choice:
```

Enter a number and press **Enter** to select a feature.

---

### Feature 1 — Daily Profit / Loss Calculator

**Sample input:**
```
  Total Fares      : PHP 1500.00
  Fuel Cost        : PHP 800.00
  Boundary Fee     : PHP 500.00
```

**Sample output:**
```
  NET PROFIT       : PHP 200.00
```

If fares do not cover costs:
```
  NET LOSS         : PHP -300.00
  WARNING: YOU ARE OPERATING AT A LOSS!
```

Select **[2] View Earnings History** to see all past entries in a color-coded table.

---

### Feature 2 — Fuel Consumption Log

**Sample input (Add Entry):**
```
  Liters filled    : 10.00
  Price per liter  : PHP 68.50
```

**Sample output:**
```
  Total Cost       : PHP 685.00
```

Select **[3] View Summary** to see:
- Total liters used and average per day
- Total and average fuel cost
- Most expensive and cheapest fill-up day (highlighted in red and green)

---

### Feature 3 — Route Efficiency Recommender

On first entry, the **Vehicle Setup Wizard** runs:

**Step 1** — Select your vehicle type:
```
  [1]  Traditional Jeepney   (0.22 L/km)
  [2]  Modern Jeepney        (0.11 L/km)
  [3]  UV Express (AUV)      (0.08 L/km)
  [4]  Tricycle              (0.03 L/km)
  [5]  Enter manually
```

**Step 2** — Select ownership:
```
  [1]  Yes, I own it  —  no boundary fee
  [2]  No, I pay boundary / rent
```

**Step 3** — Enter today's diesel price (or 0 to keep the default).

After setup, select **[3] Get Best Route Recommendation** to see:
```
  ROUTE #2 — Legazpi - Sto. Domingo
  ─────────────────────────────────
  Distance         : 18.0 km
  Traffic Level    : Light  (x1.1)
  Gross Income     : PHP 1470.00
  Est. Fuel Cost   : PHP 734.58
  Net Income       : PHP 735.42 (green)
  Score : [████████████░░░░░░░░] 1.00
```

---

### Feature 4 — Fare-to-Fuel Break-Even Simulator

Select a PUV scenario or enter custom values. Sample output:
```
  Fuel cost          :  PHP 1866.67
  Total daily cost   :  PHP 3066.67
  Total passengers   :  170 pax
  Gross revenue      :  PHP 2380.00
  Net profit/loss    :  PHP -686.67  (red)
  Break-even pax     :  236 passengers
  Break-even/trip    :  24 per trip
  Coverage           :  77.6%
  ─────────────────────────────────
  STATUS: AT A LOSS  (red)
```

---

### Feature 5 — Quick-Tap Fare Pad

The pad displays a live dashboard:
```
  [1]  Regular Fare     +₱13.00
  [2]  Discounted Fare  +₱11.00
  [3]  Undo Last Tap
  [0]  Done / Exit Pad
  ─────────────────────────────
  Regular taps    : 12
  Discounted taps : 3
  Total taps      : 15
  Last added     : +₱13.00
  ─────────────────────────────
  TOTAL COLLECTED : ₱189.00
```

Press **3** to undo the last tap. Press **0** when done to see the session summary.

---

### Step 4 — Saving and Exiting

Select **[6] Save and Exit** from the main menu. All daily summaries and fuel entries are
written to `puv_records.txt` in the project folder. On the next run, records are automatically
reloaded.

---

## V. Member's Participation

### Kim Eidrich B. Custodio — Feature 3: Route Efficiency Recommender

Developed the complete Route Efficiency Recommender module (`route.c`, `route.h`). Designed
the `RouteNode` data structure and the 8 pre-seeded local PUV routes based on real Bicol
region routes. Implemented the efficiency scoring formula, the route detail display with a
UTF-8 block-character score bar, and the route recommendation system. Designed and
implemented the 3-step vehicle setup wizard that handles PUV type presets, owner-driver
vs. boundary-renter distinction, and diesel price input — replacing the previous
hardcoded values with driver-specific session settings.

---

### Cyril Gian T. Anonuevo — Feature 4: Fare-to-Fuel Break-Even Simulator

Developed the Fare-to-Fuel Break-Even Simulator module (`breakeven.c`, `breakeven.h`).
Researched and implemented 6 PUV type presets with realistic operational data (fuel
efficiency, fares, route distances, boundary fees, and passenger counts). Implemented the
full financial simulation: fuel cost, total cost, gross revenue, net profit/loss,
break-even passenger count, and cost coverage percentage. Designed the results display
with color-coded profit/loss status and integrated the module into the main menu.

---

### Keith Bryan O. Belmonte — Feature 2: Fuel Consumption Log

Developed the Fuel Consumption Log module (`fuelConsump.c`, `fuel.h`). Implemented the
rolling 7-entry log using a `FuelEntry` array with automatic oldest-entry removal when
full. Built the add-entry form with automatic date stamping, the full log table view, and
the summary statistics view showing total and average fuel use, total and average cost, and
color-highlighted most expensive and cheapest days. Collaborated on the `FuelEntry` struct
definition in `structs.h`.

---

### Kile Nicandro A. Roxas — Feature 5: Quick-Tap Fare Pad

Developed the Quick-Tap Fare Pad module (`quicktap.c`, `quicktap.h`). Implemented the
real-time fare counting interface using an undo stack (`tap_history` array) to support
undoing the last tap at any time. Designed the live dashboard that updates after every tap
showing regular count, discounted count, last action, and running total. Implemented the
session summary screen displayed after the driver exits the pad.

---

### Clarence James D. Gelua — Feature 1: Daily Profit / Loss Calculator

Developed the Daily Profit / Loss Calculator module (`profit.c`, `profit.h`). Implemented
the daily entry form that collects total fares, fuel cost, and boundary fee, computes net
profit or loss, and stores the result in a `DaySummary` array with rolling window behavior.
Built the loss detection and warning system that highlights when the driver is operating
at a loss. Implemented the earnings history table view with color-coded net values.
Collaborated on the `DaySummary` struct definition in `structs.h`.
