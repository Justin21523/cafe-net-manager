# Current Features Status

| Module | Current Status | Implemented Details | Missing / Weak Parts |
|--------|----------------|---------------------|----------------------|
| Dashboard | ✅ Complete | Revenue, orders, active seats, top items, hourly table | Data aggregation is simplified; no date range filter |
| Seat Map | ✅ Complete | `QGraphicsView`, drag edit, save layout, status colors | Lacks true floor plan objects (walls, counters) |
| Seat Edit Mode | ✅ Complete | Toggle, drag, save x/y to DB | No resize/rotate, no copy/paste, no grid snap |
| Seat Details | ✅ Complete | Info display, Start/End/Extend buttons, elapsed timer | Timer is client-side simplified; no real session sync |
| Menu | ✅ Complete | Category tabs, scrollable grid, Add button | No search, no item options/modifiers, basic cards |
| Cart | ✅ Complete | Table view, qty spinbox, remove, submit order | No discount/tax UI, no member lookup |
| Order Submit | ✅ Complete | Creates order + items in DB, status=Submitted | No draft saving, no order number generation logic |
| Kitchen Board | ✅ Complete | 3 columns, order cards, status dropdown, live timer | Timer uses local `QTimer`; status change reloads whole board |
| SQLite DB | ✅ Complete | 8 tables, FK constraints, seed data | No migration system, schema hardcoded in manager |
| Demo Data | ✅ Complete | 6 seats, 5 categories, 11 items, auto-seed | Lacks realistic order history for dashboard testing |
| Layout Save | ✅ Complete | `updateSeatPosition()` SQL, persist x/y | No undo/redo, no layout versioning |
| Session/Timer | 🟡 Prototype | Start/End/Extend, client-side elapsed display | No server sync, no expiration logic, no billing calc |
| Checkout | 🟡 Prototype | `CheckoutDialog` UI, cash/card/member radio, change calc | Not fully wired to order payment flow |
| Admin | ❌ Not Started | - | Requires full CRUD pages for seats/menu/customers |