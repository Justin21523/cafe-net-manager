# CafeNet Manager Project Handoff

## 1. Project Overview
- **Project Name**: CafeNet Manager
- **Purpose**: Desktop POS & management system for cafes/internet cafes (seat management, ordering, kitchen display, checkout).
- **Tech Stack**: C++17, Qt 6 (Widgets, Sql), CMake, SQLite3, VS Code.
- **Current Position**: Functional core completed. UI uses `QSplitter` + `QTabWidget`. Database schema & seed data ready. Signal/Slot flow established.
- **Completion Level**: ~60% (Core flow works, UI architecture needs refactor, several features are prototypes).
- **Next Direction**: Architecture refactor (Sidebar + QStackedWidget), Floor Plan Editor upgrade, Seat Operation Page, POS integration, Kitchen Board rewrite.

## 2. Current Tech Stack
- C++17 (`-std=c++17`)
- Qt 6.4+ (`Qt6::Widgets`, `Qt6::Sql`)
- CMake 3.16+ (AUTOMOC enabled, explicit target setup)
- SQLite 3 (via `QSqlDatabase`)
- IDE: VS Code (CMake Tools, clangd)
- OS: Linux (Ubuntu/Debian dev env)

## 3. Current Application Status
| Feature | Status | Notes |
|---------|--------|-------|
| Main Window & Layout | ✅ Complete | `QSplitter` (Left: Map, Right: Tabs) |
| Seat Visual Map | ✅ Complete | `QGraphicsView` + custom items |
| Seat Status Display | ✅ Complete | Color-coded (Available/Occupied/etc.) |
| Edit Mode / Save Layout | ✅ Complete | Drag & drop, persists x/y to DB |
| Dashboard | ✅ Complete | Basic stats, top items, hourly revenue (simplified queries) |
| Seat Details | ✅ Complete | Info display, Start/End/Extend buttons (timer simplified) |
| Menu Browser | ✅ Complete | Category tabs, grid cards, add to cart |
| Cart & Order Submit | ✅ Complete | Qty adjust, remove, submit to DB |
| Kitchen Board | ✅ Complete | 3-column layout, order cards, status dropdown, live timer |
| SQLite Database | ✅ Complete | Schema init, seed data, connection management |
| Checkout Dialog |  Prototype | UI exists, payment logic simplified |
| Customer Search | 🟡 Prototype | Basic search widget, DB table exists |
| Admin Center | ❌ Not Started | Planned for V2.5 |

## 4. How to Build and Run
```bash
cd cafe-net-manager
rm -rf build && mkdir build && cd build
cmake ..
cmake --build .
./cafe-net-manager
```
*(Database auto-creates at `data/cafenet.db` with demo data on first run.)*

## 5. Important Notes for Next Chat
- **Do not** add new features blindly. Refactor architecture first.
- Replace `QTabWidget` with `Sidebar + QStackedWidget`.
- Seat Map needs true Floor Plan Editor (walls, shapes, inspector).
- Kitchen Board cards need better readability & status flow.
- `SeatDetailsPanel` is too passive; upgrade to interactive Seat Operation Page.
- Menu & Cart should merge into a unified POS Order Page.
- Keep `AUTOMOC` & CMake structure intact.