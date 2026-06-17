# Architecture Summary

## 1. Folder Structure
```text
cafe-net-manager/
├── CMakeLists.txt
├── include/
│   ├── core/AppContext.h
│   ├── database/   # Repositories & DB manager
│   ├── models/     # Pure data structs
│   ├── services/   # Business logic
│   ├── utils/      # Logger
│   └── widgets/    # Qt UI components
├── src/            # Mirrors include/
├── data/           # cafenet.db
├── docs/handoff/   # This directory
└── resources/      # Icons, styles (mostly empty)
```

## 2. Layered Architecture
| Layer | Status | Notes |
|-------|--------|-------|
| `models/` | ✅ Solid | `struct`-based, no `QObject`, value objects |
| `database/` | ✅ Solid | `DatabaseManager` handles connection/schema |
| `repositories/` | ✅ Solid | CRUD only, returns `std::vector<Model>` |
| `services/` | ✅ Solid | Coordinates repos, handles cart/session logic |
| `widgets/` | 🟡 Mixed | UI + some logic coupled; `MainWindow` holds too many connections |
| `pages/` | ❌ Missing | Currently using tabs inside `MainWindow` |
| `utils/` | ✅ Basic | `Logger` wrapper around `qDebug` |

## 3. Data Flow
```text
User clicks seat in SeatMapView
  → QGraphicsScene::selectionChanged signal
  → SeatMapView emits seatSelected(Seat)
  → MainWindow connects to SeatDetailPanel::updateSeatInfo
  → User clicks "Start Session"
  → SeatDetailPanel emits startSessionRequested(seatId)
  → MainWindow calls SeatSessionService::startSession()
  → Service updates DB via Repositories
  → MainWindow calls refreshSeatMap() to update UI
```

## 4. Main Runtime Flow
```text
main.cpp
  → QApplication init
  → AppContext created (holds global services)
  → DatabaseManager::initialize() (schema + seed)
  → MainWindow created
  → Services injected via setServices()
  → Seat map loaded from DB
  → app.exec() → Event loop