cmake_minimum_required(VERSION 3.16)

project(cafe-net-manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_executable(cafe-net-manager
    src/main.cpp
    
    src/widgets/MainWindow.cpp
    include/widgets/MainWindow.h
    src/widgets/SeatMapView.cpp
    include/widgets/SeatMapView.h
    src/widgets/SeatGraphicsItem.cpp
    include/widgets/SeatGraphicsItem.h
    src/widgets/SeatDetailPanel.cpp
    include/widgets/SeatDetailPanel.h
    src/widgets/MenuBrowserWidget.cpp
    include/widgets/MenuBrowserWidget.h
    src/widgets/MenuItemCard.cpp
    include/widgets/MenuItemCard.h
    src/widgets/CartWidget.cpp
    include/widgets/CartWidget.h
    src/widgets/KitchenBoardWidget.cpp
    include/widgets/KitchenBoardWidget.h
    src/widgets/OrderCardWidget.cpp
    include/widgets/OrderCardWidget.h
    src/widgets/DashboardWidget.cpp
    include/widgets/DashboardWidget.h
    src/widgets/AdminPage.cpp
    include/widgets/AdminPage.h
    src/widgets/SeatManagementWidget.cpp
    include/widgets/SeatManagementWidget.h
    src/widgets/MenuManagementWidget.cpp
    include/widgets/MenuManagementWidget.h
    src/widgets/CheckoutDialog.cpp
    include/widgets/CheckoutDialog.h
    
    src/core/AppContext.cpp
    include/core/AppContext.h
    
    src/database/DatabaseManager.cpp
    include/database/DatabaseManager.h
    src/database/SeatRepository.cpp
    include/database/SeatRepository.h
    src/database/SeatSessionRepository.cpp
    include/database/SeatSessionRepository.h
    src/database/MenuRepository.cpp
    include/database/MenuRepository.h
    src/database/OrderRepository.cpp
    include/database/OrderRepository.h
    src/database/PaymentRepository.cpp
    include/database/PaymentRepository.h
    
    src/models/Seat.cpp
    include/models/Seat.h
    src/models/SeatSession.cpp
    include/models/SeatSession.h
    src/models/MenuCategory.cpp
    include/models/MenuCategory.h
    src/models/MenuItem.cpp
    include/models/MenuItem.h
    src/models/Order.cpp
    include/models/Order.h
    src/models/OrderItem.cpp
    include/models/OrderItem.h
    src/models/Payment.cpp
    include/models/Payment.h
    
    src/services/SeatService.cpp
    include/services/SeatService.h
    src/services/SeatSessionService.cpp
    include/services/SeatSessionService.h
    src/services/MenuService.cpp
    include/services/MenuService.h
    src/services/OrderService.cpp
    include/services/OrderService.h
    src/models/Customer.cpp
    include/models/Customer.h
    src/database/CustomerRepository.cpp
    include/database/CustomerRepository.h
    src/widgets/CustomerSearchWidget.cpp
    include/widgets/CustomerSearchWidget.h

    src/utils/Logger.cpp
    include/utils/Logger.h

    # V2.0 Pages & Sidebar
    src/widgets/SidebarWidget.cpp include/widgets/SidebarWidget.h
    src/pages/DashboardPage.cpp include/pages/DashboardPage.h
    src/pages/FloorPlanPage.cpp include/pages/FloorPlanPage.h
    src/pages/PosOrderPage.cpp include/pages/PosOrderPage.h
    src/pages/KitchenPage.cpp include/pages/KitchenPage.h
    
    # V2.1 Floor Plan Editor (Graphics)
    src/graphics/FloorPlanScene.cpp include/graphics/FloorPlanScene.h
    src/graphics/FloorPlanItem.cpp include/graphics/FloorPlanItem.h
    src/graphics/SeatItem.cpp include/graphics/SeatItem.h
    src/graphics/WallItem.cpp include/graphics/WallItem.h
    src/graphics/CounterItem.cpp include/graphics/CounterItem.h
    
    # V2.1 Floor Plan Editor (Widgets)
    src/widgets/FloorPlanView.cpp include/widgets/FloorPlanView.h
    src/widgets/ToolPalette.cpp include/widgets/ToolPalette.h
    src/widgets/InspectorPanel.cpp include/widgets/InspectorPanel.h
    # V2.1.2 Undo/Redo & Line Tool
    src/utils/Command.cpp
    include/utils/Command.h
    src/graphics/LineItem.cpp
    include/graphics/LineItem.h
     # V2.2 Seat Operation Panel
    src/widgets/SeatOperationPanel.cpp
    include/widgets/SeatOperationPanel.h  
    # V2.3 POS Order Page
    src/widgets/ModifierDialog.cpp
    include/widgets/ModifierDialog.h
    src/widgets/PosMenuPanel.cpp
    include/widgets/PosMenuPanel.h
    src/widgets/PosCartPanel.cpp
    include/widgets/PosCartPanel.h     
)

target_include_directories(cafe-net-manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cafe-net-manager PRIVATE 
    Qt6::Widgets 
    Qt6::Sql
)
