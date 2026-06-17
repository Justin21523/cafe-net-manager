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
    src/widgets/SeatMapView.cpp         # Added
    include/widgets/SeatMapView.h       # Added
    src/widgets/SeatGraphicsItem.cpp    # Added
    include/widgets/SeatGraphicsItem.h  # Added
    src/widgets/SeatDetailPanel.cpp     # Added
    include/widgets/SeatDetailPanel.h   # Added

    src/core/AppContext.cpp
    include/core/AppContext.h
    
    src/database/DatabaseManager.cpp
    include/database/DatabaseManager.h
    src/database/SeatRepository.cpp     # Added
    include/database/SeatRepository.h   # Added
    
    src/models/Seat.cpp                 # Added
    include/models/Seat.h               # Added
    
    src/services/SeatService.cpp        # Added
    include/services/SeatService.h      # Added
    
    src/models/SeatSession.cpp
    include/models/SeatSession.h
    src/database/SeatSessionRepository.cpp
    include/database/SeatSessionRepository.h
    src/services/SeatSessionService.cpp
    include/services/SeatSessionService.h

    src/utils/Logger.cpp
    include/utils/Logger.h
)

target_include_directories(cafe-net-manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cafe-net-manager PRIVATE 
    Qt6::Widgets 
    Qt6::Sql
)
