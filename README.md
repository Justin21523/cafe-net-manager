
# CafeNet Manager ☕🖥️

A comprehensive desktop management system for cafes and internet cafes, built with C++ and Qt 6.

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Qt](https://img.shields.io/badge/Qt-6.4+-green.svg)
![C++](https://img.shields.io/badge/C++-17-blue.svg)

## ✨ Features

### 🪑 Seat Management
- Visual seat map with drag-and-drop editing
- Real-time seat status (Available, Occupied, Reserved, Cleaning, Disabled)
- Color-coded status indicators
- Seat capacity and amenities tracking

### ⏱️ Session Management
- Cafe mode: Minimum charge / dining
- Internet Cafe mode: Hourly packages, time tracking, extensions
- Real-time elapsed time display
- Automatic timeout warnings
- Quick time extension (30min, 1hr)

### 📋 Order System
- Complete menu management with categories
- Shopping cart with quantity adjustment
- Order status tracking (Draft → Submitted → Preparing → Ready → Served)
- Table-side ordering
- Multiple payment methods (Cash, Credit Card, Member Balance)

### 👨‍🍳 Kitchen Display System
- Real-time order display
- Three-column layout (Pending, Preparing, Ready)
- Live wait time calculation
- Drag-and-drop status updates
- Order priority management

### 💳 Checkout System
- Automatic bill calculation
- Service charge (10%)
- Discount support
- Cash change calculation
- Receipt generation

### 📊 Dashboard & Reports
- Today's revenue statistics
- Total order count
- Active seat count
- Top 5 menu items
- Hourly revenue breakdown
- Export to CSV

### 👤 Customer Management
- Customer database with search
- Visit history tracking
- Total spending calculation
- Customer notes and preferences
- Quick customer lookup

###  User Interface
- Modern dark theme
- Responsive layout
- Intuitive drag-and-drop seat editor
- Multi-tab interface
- Real-time updates

## 🛠️ Tech Stack

- **Language**: C++17
- **GUI Framework**: Qt 6 (Widgets)
- **Database**: SQLite 3
- **Build System**: CMake 3.16+
- **IDE**: Visual Studio Code
- **Version Control**: Git

## 📐 Architecture

```
┌─────────────────────────────────────────────────────────┐
│                     Presentation Layer                   │
│  (MainWindow, SeatMapView, CartWidget, KitchenBoard)    │
├─────────────────────────────────────────────────────────┤
│                    Business Logic Layer                  │
│    (SeatService, OrderService, SessionService)          │
├─────────────────────────────────────────────────────────┤
│                      Data Access Layer                   │
│  (SeatRepository, OrderRepository, MenuRepository)      │
├─────────────────────────────────────────────────────────┤
│                       Database Layer                     │
│                    (SQLite Database)                     │
└─────────────────────────────────────────────────────────┘
```

## 📁 Project Structure

```
cafe-net-manager/
├── CMakeLists.txt              # Build configuration
├── README.md                   # Project documentation
├── data/
│   └── cafenet.db             # SQLite database
├── docs/
│   ├── database-schema.md     # Database documentation
│   ├── roadmap.md             # Development roadmap
│   └── screenshots/           # Application screenshots
├── include/
│   ├── core/
│   │   └── AppContext.h       # Application context
│   ├── database/
│   │   ├── DatabaseManager.h  # Database connection manager
│   │   ├── SeatRepository.h   # Seat data access
│   │   ├── MenuRepository.h   # Menu data access
│   │   └── OrderRepository.h  # Order data access
│   ├── models/
│   │   ├── Seat.h             # Seat model
│   │   ├── SeatSession.h      # Session model
│   │   ├── MenuItem.h         # Menu item model
│   │   ├── Order.h            # Order model
│   │   └── Customer.h         # Customer model
│   ├── services/
│   │   ├── SeatService.h      # Seat business logic
│   │   ├── SeatSessionService.h # Session logic
│   │   ├── MenuService.h      # Menu logic
│   │   └── OrderService.h     # Order & cart logic
│   ├── utils/
│   │   └── Logger.h           # Logging utility
│   └── widgets/
│       ├── MainWindow.h       # Main window
│       ├── SeatMapView.h      # Seat map visualization
│       ├── SeatDetailPanel.h  # Seat details panel
│       ├── MenuBrowserWidget.h # Menu browser
│       ├── CartWidget.h       # Shopping cart
│       ├── KitchenBoardWidget.h # Kitchen display
│       ├── DashboardWidget.h  # Dashboard
│       └── CheckoutDialog.h   # Checkout dialog
├── resources/
│   ├── icons/                 # Application icons
│   └── styles/                # QSS stylesheets
├── src/
│   ├── main.cpp               # Application entry point
│   ├── core/
│   ├── database/
│   ├── models/
│   ├── services/
│   ├── utils/
│   └── widgets/
└── tests/                     # Unit tests (future)
```

## 🚀 Quick Start

### Prerequisites

- **Operating System**: Linux (Ubuntu/Debian recommended), Windows, macOS
- **Compiler**: g++ 11+ or clang++ 12+
- **Qt**: 6.4 or later
- **CMake**: 3.16 or later
- **SQLite**: 3.x

### Installation

#### Ubuntu/Debian

```bash
# Update package list
sudo apt update

# Install build dependencies
sudo apt install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    qt6-base-dev \
    libsqlite3-dev \
    sqlite3 \
    pkg-config

# Verify installation
g++ --version
cmake --version
qmake6 --version
```

#### Fedora

```bash
sudo dnf install gcc-c++ cmake ninja-build git qt6-qtbase-devel sqlite-devel
```

#### Arch Linux

```bash
sudo pacman -S base-devel cmake ninja git qt6-base sqlite
```

### Build from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/cafe-net-manager.git
cd cafe-net-manager

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# Run
./cafe-net-manager
```

### Build with VS Code

1. Open the project folder in VS Code
2. Install extensions:
   - C/C++ (Microsoft)
   - CMake Tools (Microsoft)
   - clangd (LLVM)
3. Press `Ctrl+Shift+B` to build
4. Press `F5` to run with debugger

## 📖 Usage Guide

### First Launch

1. The application will automatically create the database at `data/cafenet.db`
2. Demo data (6 seats, sample menu items) will be seeded automatically
3. You'll see the seat map on the left and details panel on the right

### Managing Seats

1. **View Seats**: Click on any seat to view details
2. **Start Session**: Select an available seat → Click "Start Session"
3. **End Session**: Select an occupied seat → Click "End Session"
4. **Edit Layout**: 
   - Click "Enable Edit Mode"
   - Drag seats to reposition
   - Click "Save Layout" to persist changes

### Placing Orders

1. Select a seat from the map
2. Click "Start Session" (required before ordering)
3. Switch to "Menu" tab
4. Browse categories (Coffee, Tea, Dessert, etc.)
5. Click "Add" on desired items
6. Switch to "Cart" tab
7. Adjust quantities if needed
8. Click "Submit Order"

### Kitchen Operations

1. Switch to "Kitchen Board" tab
2. View orders in three columns:
   - **Pending**: New orders awaiting attention
   - **Preparing**: Orders being prepared
   - **Ready**: Orders ready for serving
3. Use dropdown on each card to change status
4. Cards automatically move between columns

### Checkout

1. Select the occupied seat
2. Review all orders in the cart
3. Click "Checkout"
4. Choose payment method:
   - **Cash**: Enter amount received, calculate change
   - **Credit Card**: Direct payment
   - **Member Balance**: Deduct from member account
5. Confirm payment
6. Receipt will be generated

### Dashboard

1. Switch to "Dashboard" tab
2. View key metrics:
   - Today's total revenue
   - Total order count
   - Active seats count
3. Review top 5 selling items
4. Analyze hourly revenue distribution

## 🗄️ Database Schema

For detailed database documentation, see [docs/database-schema.md](docs/database-schema.md).

### Key Tables

- **seats**: Store seat information and status
- **seat_sessions**: Track customer sessions at seats
- **menu_items**: Menu catalog with pricing
- **orders**: Order headers with totals
- **order_items**: Order line items
- **customers**: Customer database
- **payments**: Payment records

## 🎨 Customization

### Themes

Edit `resources/styles/default.qss` to customize the appearance:

```css
QMainWindow {
    background-color: #2b2b2b;
}

QPushButton {
    background-color: #4CAF50;
    color: white;
    border-radius: 5px;
    padding: 8px;
}
```

### Menu Items

Add your own menu items via the Admin Panel or directly in the database:

```sql
INSERT INTO menu_items (category_id, name, price, is_active)
VALUES (1, 'Your Coffee', 12000);
```

## 🧪 Testing

```bash
# Run tests (when available)
cd build
ctest
```

## 📝 Development Roadmap

### Phase 1: Core Features ✅
- [x] Seat management
- [x] Order system
- [x] Kitchen display
- [x] Checkout
- [x] Basic reports

### Phase 2: Advanced Features 🚧
- [x] Customer management
- [x] Seat map editor
- [x] Internet cafe timer
- [ ] Inventory management
- [ ] Employee management
- [ ] Shift reports

### Phase 3: Cloud ☁️
- [ ] Multi-store support
- [ ] Cloud synchronization
- [ ] Web admin panel
- [ ] RESTful API

### Phase 4: Mobile 📱
- [ ] iOS app
- [ ] Android app
- [ ] Customer ordering app
- [ ] Online reservation

## 🐛 Troubleshooting

### Common Issues

**Problem**: "Qt6 not found" error during CMake configuration
```bash
# Solution: Install Qt6 development packages
sudo apt install qt6-base-dev
```

**Problem**: "SQLite driver not loaded"
```bash
# Solution: Install SQLite driver
sudo apt install libqt6sql6-sqlite
```

**Problem**: Application crashes on startup
```bash
# Solution: Check database permissions
chmod 755 data/
chmod 644 data/cafenet.db
```

**Problem**: Seats not displaying correctly
```bash
# Solution: Reset database
rm data/cafenet.db
./cafe-net-manager  # Will recreate with demo data
```

### Getting Help

- Check the [issues page](https://github.com/yourusername/cafe-net-manager/issues)
- Read the [documentation](docs/)
- Contact: your.email@example.com

## 🤝 Contributing

We welcome contributions! Here's how you can help:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Development Guidelines

- Follow C++17 standard
- Use Qt coding conventions
- Write meaningful comments
- Update documentation for new features
- Add tests for new functionality

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2024 Your Name

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 👨‍ Author

**Your Name**
- GitHub: [Justin21523](https://github.com/Justin21523)
- Email: your.email@example.com
- LinkedIn: [Your Profile](https://linkedin.com/in/yourprofile)

## 🙏 Acknowledgments

- Qt Company for the excellent Qt framework
- SQLite team for the lightweight database
- All open-source contributors
- My family and friends for support

## 📊 Project Statistics

- **Lines of Code**: ~8,000+
- **Files**: 80+
- **Classes**: 40+
- **Development Time**: 6 months
- **Last Updated**: June 2026

## 🌟 Show Your Support

If you like this project, please ⭐ star this repository!

---

